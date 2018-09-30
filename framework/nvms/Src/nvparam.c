/******************************************************************************
 Copyright (c) 2018 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		nvparam.c
 [Date]:
 		2018-07-09
 [Author]:
 		Tim Lin
 [Reversion History]:
 		v1.0
*******************************************************************************/

/*========================================================================================================
										I N C L U D E
========================================================================================================*/
// external
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// internal
#include "flex_drv_m24cxx.h"
#include "nvms.h"
#include "nvparam.h"



/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME					"[nvparam]--"
#endif

#if defined(__FLEX_TRACE_ENABLE__)
#define TRACE_F(fmt, arg...)				DBG_MSG(MOD_NAME "[F]-%s.\r\n", __func__)
#define TRACE_I(fmt, arg...)				DBG_MSG(MOD_NAME "[I]-" fmt, ##arg)
#define TRACE_W(fmt, arg...)			DBG_MSG(MOD_NAME "[W]-" fmt, ##arg)
#else
#define TRACE_F()
#define TRACE_I(fmt, arg...)
#define TRACE_W(fmt, arg...)
#endif
#define TRACE_E(fmt, arg...)				DBG_MSG(MOD_NAME "[E]-" fmt, ##arg)

/*========================================================================================================
										E X T E R N
========================================================================================================*/





/*========================================================================================================
										D E F I N E
========================================================================================================*/


/*========================================================================================================
										T Y P E D E F
========================================================================================================*/

enum
{
	FLAG_VARIABLE_LEN = 0x01,		// parameter has variable length
};

typedef struct
{
	uint8_t tag;					// unique parameter tag, user has to ensure the unique value
	uint8_t flags;					// parameter flags
	uint16_t length;				// parameter max length
	uint32_t offset;				// parameter offset inside area
} parameter_t;

typedef struct
{
	const char *name;				// unique area name, user has to ensure the unique name
	nvms_partition_id_t partition;	// area partition
	uint32_t offset;				// area offset inside partition
	const parameter_t *parameters;	// list of area parameters
	size_t num_parameters;			// number of area parameters
} area_t;

typedef struct
{
	const area_t *area; 			// attached area configuration
	nvms_t nvms_h;					// NVMS handle
} nvparam_data_t;


/* Create nvparam configuration from ad_nvparam_defs.h */
#define IN_NVPARAM_C
#include <nvparam_defs.h>


/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/



/*========================================================================================================
										F U N C T I O N
========================================================================================================*/

static const parameter_t *find_parameter(const area_t *area, uint8_t tag)
{
	int i;

	for (i = 0; i < area->num_parameters; i++)
	{
		if (area->parameters[i].tag == tag)
		{
			return &area->parameters[i];
		}
	}

	return NULL;
}

nvparam_t nvparam_open(const char *area_name)
{
	int i;
	nvms_t *nvms_h;
	const area_t *area = NULL;
	nvparam_data_t *nv_data;

	for (i = 0; i < num_areas; i++)
	{
		if (!strcmp(areas[i].name, area_name))
		{
			area = &areas[i];
			break;
		}
	}

	if (!area)
	{
		return NULL;
	}

	nvms_h = nvms_open(area->partition);

	if (!nvms_h)
	{
		return NULL;
	}

	nv_data = pf_malloc(sizeof(*nv_data));
	assert_param(nv_data);

	nv_data->area = area;
	nv_data->nvms_h = nvms_h;

	return (nvparam_t) nv_data;
}


void nvparam_close(nvparam_t nvparam)
{
	if (nvparam)
	{
		pf_free(nvparam);
	}
}

void nvparam_erase_all(nvparam_t nvparam)
{
	nvparam_data_t *nv_data = nvparam;
	size_t size;
	uint8_t *write_buf;
	int i;

	if (!nvparam)
	{
		return;
	}

	for (i = 0; i < nv_data->area->num_parameters; i++)
	{
		uint32_t addr = nv_data->area->offset + nv_data->area->parameters[i].offset;
		size = nv_data->area->parameters[i].length;
		write_buf = pf_malloc(size * sizeof(uint8_t));
		assert_param(write_buf);
		
		pf_memset(write_buf, 0xFF, size);

		nvms_write(nv_data->nvms_h, addr, write_buf, size);

		pf_free(write_buf);
	}
}

void nvparam_erase(nvparam_t nvparam, uint8_t tag)
{
	nvparam_data_t *nv_data = nvparam;
	const parameter_t *param;
	size_t size;
	uint8_t *write_buf;

	if (!nvparam)
	{
		return;
	}

	param = find_parameter(nv_data->area, tag);

	if (!param)
	{
		return;
	}

	size = param->length;
	write_buf = pf_malloc(size * sizeof(uint8_t));
	assert_param(write_buf);
	
	pf_memset(write_buf, 0xFF, size);

	nvms_write(nv_data->nvms_h, nv_data->area->offset + param->offset, write_buf, size);

	pf_free(write_buf);
}

uint16_t nvparam_read_offset(nvparam_t nvparam, uint8_t tag, uint16_t offset,uint16_t length, void *data)
{
	nvparam_data_t *nv_data = nvparam;
	const parameter_t *param;
	uint32_t param_offset;
	uint16_t max_length;

	if (!nvparam)
	{
		return 0;
	}

	param = find_parameter(nv_data->area, tag);

	if (!param)
	{
		return 0;
	}

	param_offset = nv_data->area->offset + param->offset;
	max_length = param->length;

	if (param->flags & FLAG_VARIABLE_LEN)
	{
		uint16_t stored_len;
		int read_len;

		/* read current parameter length first */
		read_len = nvms_read(nv_data->nvms_h, param_offset, (uint8_t *) &stored_len, 2);

		/*
		 * 1) check if read was successful (2 bytes were read)
		 * 2) in case of 0xFFFF is read as parameter length, we assume this is due to clean
		 *	  flash and treat it as 0x0000
		 */
		if (read_len != 2 || stored_len == 0xffff)
		{
			return 0;
		}

		param_offset += read_len;
		max_length = stored_len;
	}

	/* Do not allow reading past max_length */
	if (offset >= max_length)
	{
		return 0;
	}

	/* truncate read to maximum length, including offset */
	max_length -= offset;

	if (length > max_length)
	{
		length = max_length;
	}

	param_offset += offset;
	return nvms_read(nv_data->nvms_h, param_offset, (uint8_t *) data, length);
}

uint16_t nvparam_write(nvparam_t nvparam, uint8_t tag, uint16_t length, const void *data)
{
	nvparam_data_t *nv_data = nvparam;
	const parameter_t *param;
	uint32_t offset;

	if (!nvparam)
	{
		return 0;
	}

	param = find_parameter(nv_data->area, tag);

	if (!param)
	{
		return 0;
	}

	offset = nv_data->area->offset + param->offset;

	/* truncate write to maximum length */
	if (length > param->length)
	{
		length = param->length;
	}

	if (param->flags & FLAG_VARIABLE_LEN)
	{
		int written;

		/* maximum length must be subtracted by 2 which are occupied by length parameter */
		if (length > param->length - 2)
		{
			length = param->length - 2;
		}

		/* write parameter length first */
		written = nvms_write(nv_data->nvms_h, offset, (uint8_t *) &length, 2);

		if (written != 2)
		{
			return 0;
		}

		offset += 2;
	}

	return nvms_write(nv_data->nvms_h, offset, data, length);
}

uint16_t nvparam_get_length(nvparam_t nvparam, uint8_t tag, uint16_t *max_length)
{
	nvparam_data_t *nv_data = nvparam;
	const parameter_t *param;
	uint32_t offset;
	uint16_t length;
	int read;

	if (!nvparam)
	{
		return 0;
	}

	param = find_parameter(nv_data->area, tag);

	if (!param)
	{
		return 0;
	}

	if (max_length)
	{
		*max_length = param->length;

		/*
		 * for variable length parameters max_length should be shorter about 2 bytes which
		 * are intend for parameter length
		 */
		if (param->flags & FLAG_VARIABLE_LEN)
		{
			*max_length -= 2;
		}
	}

	/* for non-variable length parameters length is just the same as max length */
	if (!(param->flags & FLAG_VARIABLE_LEN))
	{
		return param->length;
	}

	offset = nv_data->area->offset + param->offset;

	read = nvms_read(nv_data->nvms_h, offset, (uint8_t *) &length, 2);

	/*
	 * 1) check if read was successful (2 bytes were read)
	 * 2) in case of 0xFFFF is read as parameter length, we assume this is due to clean flash
	 *	  and treat it as 0x0000
	 */
	if (read != 2 || length == 0xFFFF)
	{
		return 0;
	}

	/*
	 * check if read length is grater than parameter length defined by user reduced by 2 bytes
	 * which are intended for its length
	 */
	if (length > param->length - 2)
	{
		assert_param(0);
		/* in Release mode return 0 to avoid reading garbage */
		return 0;
	}

	return length;
}

/*========================================================================================================
										End
========================================================================================================*/

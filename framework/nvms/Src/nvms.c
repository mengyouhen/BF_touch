/******************************************************************************
 Copyright (c) 2018 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		nvms.c
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
										E X T E R N
========================================================================================================*/
static int m24cxx_direct_read(struct simple_partition_t *part, uint32_t addr, uint8_t *buf, uint32_t size);
static int m24cxx_direct_write(struct simple_partition_t *part, uint32_t addr, const uint8_t *buf, uint32_t size);



/*========================================================================================================
										D E F I N E
========================================================================================================*/


/*========================================================================================================
										T Y P E D E F
========================================================================================================*/




/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/

const simple_partition_driver_t m24cxx_direct_driver =
{
	.read     = m24cxx_direct_read,
	.write    = m24cxx_direct_write,
	.erase    = NULL,
};

static const simple_partition_t simple_part_init[] =
{
	{
		.type = EEPROM_PARAM_PART,
		.start = 0x0,
		.size = M24CXX_SIZE,
		.driver = &m24cxx_direct_driver,
	},
};

/*========================================================================================================
										F U N C T I O N
========================================================================================================*/

static int m24cxx_direct_read(struct simple_partition_t *part, uint32_t addr, uint8_t *buf, uint32_t size)
{
	M24CXX_STATUS status;

	if (!part || !buf || (0 == size))
	{
		return 0;
	}

	//check if address out of device boundary
	if ((addr + size) > (part->start + part->size))
	{
		return 0;
	}

	status = M24CXX_Read((uint16_t) addr, buf, (uint16_t)size);

	if (M24CXX_OK  == status)
	{
		return size;
	}

	return 0;
}

static int m24cxx_direct_write(struct simple_partition_t *part, uint32_t addr, const uint8_t *buf, uint32_t size)
{
	M24CXX_STATUS status;

	if (!part || !buf || (0 == size))
	{
		return 0;
	}

	//check if address out of device boundary
	if ((addr + size) > (part->start + part->size))
	{
		return 0;
	}

	status = M24CXX_Write((uint16_t) addr, (uint8_t *)buf, (uint16_t)size);

	if (M24CXX_OK  == status)
	{
		return size;
	}

	return 0;
}


/**
 * \brief Initialize NVMS.
 */
void nvms_init(void)
{
	M24CXX_Init();
}

/**
 * \brief Open partition to read/write access.
 *
 * There can be only one partition with given id.
 *
 * \param [in] id partition to get access to
 *
 * \returns handle to use for partition access, NULL if partition does not exists
 */
nvms_t nvms_open(nvms_partition_id_t id)
{
	simple_partition_t *part = (simple_partition_t *)simple_part_init;

	for (int i = 0; i < sizeof(simple_part_init) / sizeof(simple_partition_t); i++)
	{
		if (part[i].type == id)
		{
			if (part[i].driver)
			{
				return (nvms_t) part;
			}
		}
	}

	return NULL;
}

/**
 * \brief Get partition size.
 *
 * \return size in bytes of partition
 */
size_t nvms_get_size(nvms_t handle)
{
	simple_partition_t *part = (simple_partition_t *) handle;

	if (part == NULL)
	{
		return 0;
	}

	return part->size;
}

/**
 * \brief Read partition data.
 *
 * \param [in] handle partition handle
 * \param [in] addr starting offset
 * \param [out] buf buffer to read data to
 * \param [in] len number of bytes to read
 *
 * \returns number of bytes read, < 0 in case of error
 */
int nvms_read(nvms_t handle, uint32_t addr, uint8_t *buf, uint32_t len)
{
	simple_partition_t *part = (simple_partition_t *) handle;

	if (part == NULL)
	{
		return -1;
	}

	return part->driver->read(part, addr, buf, len);
}

/**
 * \brief Writes data to partition.
 *
 * This function allows to write any number of bytes to partition.
 *
 * \param [in] handle partition handle
 * \param [in] addr offset in partition to write data to
 * \param [in] buf pointer to data to write
 * \param [in] size number of bytes to write
 *
 * return number of bytes written, < 0 if case of error
 *
 */
int nvms_write(nvms_t handle, uint32_t addr, const uint8_t *buf, uint32_t size)
{
	simple_partition_t *part = (simple_partition_t *) handle;

	if (part == NULL)
	{
		return -1;
	}

	return part->driver->write(part, addr, buf, size);
}


/*========================================================================================================
										End
========================================================================================================*/

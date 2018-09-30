/******************************************************************************
 Copyright (c) 2018 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		flex_app_nvparam.c
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


// internal
#include "flex_app_nvparam.h"


/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME					""
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
#define TIME_ZONE_DEFAULT	((int8_t)8)
#define BRIGHTNESS_DEFAULT	((uint8_t)100)


/*========================================================================================================
										T Y P E D E F
========================================================================================================*/



/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/



/*========================================================================================================
										F U N C T I O N
========================================================================================================*/


/*!
*\brief read timezone from nvram
*\retval timezone
*/
int8_t GetTimeZoneFromNvram(void)
{
	uint16_t length;
	uint8_t buffer[2];
	int8_t timezone;

	length = ReadParamFromNvram(MISC_PARAM_AREA_NAME, TAG_TIMEZONE, 2, buffer);
	timezone = (2 == length) ? ((buffer[0] == 0x5A) ? buffer[1] : TIME_ZONE_DEFAULT) : TIME_ZONE_DEFAULT;

	return timezone;
}

/*!
*\brief set timezone to nvram
*\param [in] timezone - timezone
*/
void SetTimeZoneFromNvram(int8_t timezone)
{
	uint16_t length;
	uint8_t buffer[2];

	//validity byte
	buffer[0] = 0x5A;
	buffer[1] = (uint8_t)timezone;

	length = WriteParamToNvram(MISC_PARAM_AREA_NAME, TAG_TIMEZONE, 2, buffer);

	if (2 != length)
	{
		DBG_MSG("%s failed:%d\n", __FUNCTION__, timezone);
	}
}

/*!
*\brief get date format from nvram
*\retval date format
*/
DATE_FORMAT GetDateFormatFromNvram(void)
{
	uint16_t length;
	uint8_t buffer[2];
	DATE_FORMAT format;

	length = ReadParamFromNvram(MISC_PARAM_AREA_NAME, TAG_DATE_FORMAT, 2, buffer);
	format = (2 == length) ? ((buffer[0] == 0x5A) ? (DATE_FORMAT)buffer[1] : DATE_FORMAT_MMDDYY) : DATE_FORMAT_MMDDYY;

	return format;
}

/*!
*\brief set date format to nvram
*\param [in] timezone - timezone
*/
void SetDateFormatToNvram(DATE_FORMAT format)
{
	uint16_t length;
	uint8_t buffer[2];

	//validity byte
	buffer[0] = 0x5A;
	buffer[1] = (uint8_t)format;

	length = WriteParamToNvram(MISC_PARAM_AREA_NAME, TAG_DATE_FORMAT, 2, buffer);

	if (2 != length)
	{
		DBG_MSG("%s failed:%d\n", __FUNCTION__, format);
	}
}

/*!
*\brief get brightness from nvram
*\retval brightness
*/
uint8_t GetBrightnessFromNvram(void)
{
	uint16_t length;
	uint8_t buffer[2];
	uint8_t brightness;

	length = ReadParamFromNvram(MISC_PARAM_AREA_NAME, TAG_BRIGHTNESS, 2, buffer);
	brightness = (2 == length) ? ((buffer[0] == 0x5A) ? buffer[1] : BRIGHTNESS_DEFAULT) : BRIGHTNESS_DEFAULT;

	return brightness;
}

/*!
*\brief set display brightness to nvram
*\param [in] brightness - brighness
*/
void SetBrighnessToNvram(uint8_t brightness)
{
	uint16_t length;
	uint8_t buffer[2];

	//validity byte
	buffer[0] = 0x5A;
	buffer[1] = brightness;

	length = WriteParamToNvram(MISC_PARAM_AREA_NAME, TAG_BRIGHTNESS, 2, buffer);

	if (2 != length)
	{
		DBG_MSG("%s failed:%d\n", __FUNCTION__, brightness);
	}
}


/*!
*\brief read parameter from nvram
*\param [in] name - area name
*\param [in] tag - tag id
*\param [in] length -data length
*\param [out] data -buffer to hold data
*\retval 0 - not found or error
*\retal  !0 - length of data read back
*/
uint16_t ReadParamFromNvram(const char *name, uint8_t tag, uint16_t length, uint8_t *data)
{
	nvparam_t param;
	uint16_t len = 0;

	if (!name || !data || !length)
	{
		return 0;
	}

	param = nvparam_open(name);

	if (param)
	{
		len = nvparam_read(param, tag, length, data);
		nvparam_close(param);
	}

	return len;
}


/*!
*\brief write parameter to nvram
*\param [in] name - area name
*\param [in] tag - tag id
*\param [in] length -data buffer
*\param [out] data -data buffer
*\retval 0 - tag not found or error
*\retal  !0 - data length be written
*/

uint16_t WriteParamToNvram(const char *name, uint8_t tag, uint16_t length, uint8_t *data)
{
	nvparam_t param;
	uint16_t len = 0;

	if (!name || !data || !length)
	{
		return 0;
	}

	param = nvparam_open(name);

	if (param)
	{
		len = nvparam_write(param, tag, length, data);
		nvparam_close(param);
	}

	return len;
}



/*========================================================================================================
										End
========================================================================================================*/

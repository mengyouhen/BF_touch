/******************************************************************************
 Copyright (c) 2018 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		flex_app_nvparam.h
 [Date]:
 		2018-07-09
 [Author]:
 		Tim Lin
 [Reversion History]:
 		v1.0
*******************************************************************************/

#ifndef _FLEX_APP_NVPARAM_H_
#define _FLEX_APP_NVPARAM_H_
/*========================================================================================================
										I N C L U D E
========================================================================================================*/
// external

// internal
#include "flex_common.h"
#include "nvms.h"
#include "nvparam.h"
#include "platform_nvparam.h"

/*========================================================================================================
										D E F I N E
========================================================================================================*/



/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
typedef enum
{
	DATE_FORMAT_MMDDYY,
	DATE_FORMAT_DDMMYY,
	DATE_FORMAT_YYMMDD,
	DATE_FORMAT_MAX,
} DATE_FORMAT;


/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/


/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
int8_t GetTimeZoneFromNvram(void);
void SetTimeZoneFromNvram(int8_t timezone);
DATE_FORMAT GetDateFormatFromNvram(void);
void SetDateFormatToNvram(DATE_FORMAT format);
uint8_t GetBrightnessFromNvram(void);
void SetBrighnessToNvram(uint8_t brightness);

uint16_t ReadParamFromNvram(const char *name, uint8_t tag, uint16_t length, uint8_t *data);
uint16_t WriteParamToNvram(const char *name, uint8_t tag, uint16_t length, uint8_t *data);


#endif /*_FLEX_APP_NVPARAM_H_*/


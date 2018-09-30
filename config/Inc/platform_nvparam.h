/******************************************************************************
 Copyright (c) 2018 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		platform_nvparam.h
 [Date]:
 		2018-07-09
 [Author]:
 		Tim Lin
 [Reversion History]:
 		v1.0
*******************************************************************************/


#ifndef PLATFORM_NVPARAM_H_
#define PLATFORM_NVPARAM_H_

#include <nvparam_defs.h>

/**
 * Tags definition for 'misc_cfg' area.
 *
 * \note These are the same as tags for RW NVDS to make translation easier.
 */
#define MISC_PARAM_AREA_NAME			"misc_para"
#define TAG_BLE_STATE					0x01 // 1 bytes array
#define TAG_TIMEZONE						0x02 // 1 bytes array
#define TAG_DATE_FORMAT					0x03 // 1 bytes array
#define TAG_BRIGHTNESS					0x04 // 1 bytes array



/**
 * 'misc_cfg' area definition
 *
 * Each parameter value includes additional byte for validity data, i.e. 1st byte of value shall be
 * set to 0x00 for parameter value to be considered valid.
 */
NVPARAM_AREA(misc_para, EEPROM_PARAM_PART, 0x0000)
//				tag id				start offset	     length
NVPARAM_PARAM(TAG_BLE_STATE,        			0, 		2) // uint8[1]
NVPARAM_PARAM(TAG_TIMEZONE, 			 	2, 		2) // uint8[1]
NVPARAM_PARAM(TAG_DATE_FORMAT, 			4,		2) // uint8[1]
NVPARAM_PARAM(TAG_BRIGHTNESS, 			 	6, 		2) // uint8[1]
NVPARAM_AREA_END()

/**
 * Tags definition for 'varparam_cfg' area.
 */
#define VARA_PARAM_AREA_NAME		"var_param"
#define TAG_WIFI_AP_SSID			0x01  // up to 32 bytes
#define TAG_WIFI_AP_PWD			0x02  //up to 64 bytes

/**
 * 'varparam_cfg' area definition
 *
 * Parameters length need to include 2 extra bytes for parameter header.
 *
 */
NVPARAM_AREA(var_param, EEPROM_PARAM_PART, 0x0040)
//					tag id				start offset	length
NVPARAM_VARPARAM(TAG_WIFI_AP_SSID,			0, 		34) 	// uint8[32]
NVPARAM_VARPARAM(TAG_WIFI_AP_PWD,			34, 		66) 	// uint8[64]
NVPARAM_AREA_END()


#endif /* PLATFORM_NVPARAM_H_ */


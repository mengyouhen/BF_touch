/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_app_osd_string.h
 [Date]:       
 		2018-05-23
 [Author]:   	
 		carson.qian 
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_APP_OSD_STRING_H_
#define _FLEX_APP_OSD_STRING_H_

#ifdef __cplusplus 
extern "C" {
#endif


/*========================================================================================================
										I N C L U D E
========================================================================================================*/
#include "flex_common.h"



/*========================================================================================================
										D E F I N E
========================================================================================================*/
#define STRING_DUMMY   "" // "DUMMY"



/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
//===============================================================
// STRING
//===============================================================
typedef enum
{
//--------------------------------------
// Fixed Text Index
//--------------------------------------
	str_null,
	str_bracket_L,		
	str_bracket_R,	
	str_bracket_LR7,
	str_bracket_LR8,
	str_Title,
	str_sys_cfg,
	str_wifi_cfg,
	str_bt_cfg,
	str_sys_info,
	str_sys_status,
	str_credits,
	str_timezone,
	str_dateformat,
	str_wifi_status,
	str_wifi_ssid,
	str_wifi_rssi,
	str_wifi_mac,
	str_wifi_ip,
	str_bt_status,
	str_bt_mac,
	str_sw_version,
	str_sw_version_value,
	str_sdk_version,
	str_at_version,
	str_serial_number,
	str_serial_number_value,
	str_build_info,
	str_build_info_date,
	str_build_info_time,
	str_brightness,
	str_fanspeed,
	str_help_colon,
	str_help_select,
	str_help_enter,
	str_help_adjust,
	str_help_return,
	str_off,
	str_on,
	str_low,
	str_medium,
	str_high,
	str_weak,
	str_normal,
	str_strong,
	str_disconnect,
	str_connect,
	str_yymmdd,
	str_mmddyy,
	str_ddmmyy,
	str_max,
//--------------------------------------
// ListBox Text index
//--------------------------------------
	fstr_null,
	fstr_onoff_select,
	fstr_fan_speed_select,
	fstr_date_format_select,
	fstr_wifi_status_select,
	fstr_wifi_mac_content,
	fstr_wifi_ssid_content,
	fstr_wifi_rssi_select,
	fstr_wifi_ip_content,
	fstr_bt_status_select,
	fstr_bt_mac_content,
	fstr_sdk_ver_content,
	fstr_at_ver_content,
	fstr_help_lr_select,
	fstr_max,
//--------------------------------------
// Fixed Icon Index
//--------------------------------------
	icon_null,
	icon_wifi_off,
	icon_wifi_disconnect,
	icon_wifi_low,
	icon_wifi_middle,
	icon_wifi_high,
	icon_bt_off,
	icon_bt_disconnect,
	icon_bt_connect,
	icon_arrow_up,
	icon_arrow_down,
	icon_arrow_left,
	icon_arrow_right,
	icon_slide,
	icon_return,
	icon_max,
//--------------------------------------
// ListBox Icon index
//--------------------------------------
	ficon_null,
	ficon_wifi_select,
	ficon_bt_select,
	ficon_max,
}OSD_DISP_ENUM;



/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/







/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
const uint8_t* Osd_Resource_Get_Disp_Data(OSD_DISP_ENUM disp_id);
OSD_DISP_ENUM Osd_Resource_Get_ListBox_Index(OSD_DISP_ENUM disp_id, uint8_t value);

/*========================================================================================================
										End
========================================================================================================*/
#ifdef __cplusplus 
}
#endif

#endif//_FLEX_APP_OSD_STRING_H_



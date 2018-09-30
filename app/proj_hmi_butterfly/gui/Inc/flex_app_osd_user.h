/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_app_osd_user.h
 [Date]:       
 		2018-06-26
 [Author]:   	
 		carson.qian 
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_APP_OSD_USER_H_
#define _FLEX_APP_OSD_USER_H_

#ifdef __cplusplus 
extern "C" {
#endif


/*========================================================================================================
										I N C L U D E
========================================================================================================*/
#include "flex_common.h"
#include "flex_app_nvparam.h"



/*========================================================================================================
										D E F I N E
========================================================================================================*/





/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
//--------------------------------------------------
typedef enum
{
	LIST_NVRAM_TAG_BRIGHTNESS,
	LIST_NVRAM_TAG_TIMEZONE,
	LIST_NVRAM_TAG_DATEFORMAT,
	LIST_NVRAM_TAG_MAX
}USER_NVRAM_TAG_ENUM;

//--------------------------------------------------
typedef enum
{
	LIST_WIFI_ICON_ST_OFF,
	LIST_WIFI_ICON_ST_DICONN,
	LIST_WIFI_ICON_ST_LOW,
	LIST_WIFI_ICON_ST_MIDDLE,
	LIST_WIFI_ICON_ST_HIGH,
	LIST_WIFI_ICON_ST_MAX
}USER_WIFI_ICON_ST_ENUM;

typedef enum
{
	LIST_BT_ICON_ST_OFF,
	LIST_BT_ICON_ST_DISCONN,
	LIST_BT_ICON_ST_CONN,
	LIST_BT_ICON_ST_MAX
}USER_BT_ICON_ST_ENUM;

//--------------------------------------------------
typedef enum
{
	LIST_WIFI_ST_OFF,
	LIST_WIFI_ST_DISCONN,
	LIST_WIFI_ST_CONN,
	LIST_WIFI_ST_MAX
}USER_WIFI_ST_ENUM;


typedef enum
{
	LIST_BT_ST_OFF,
	LIST_BT_ST_DISCONN,
	LIST_BT_ST_CONN,
	LIST_BT_ST_MAX
}USER_BT_ST_ENUM;
	
//--------------------------------------------------

typedef enum
{
	LIST_STRENGTH_LOW,
	LIST_STRENGTH_MEDIUM,
	LIST_STRENGTH_HIGH,
	LIST_STRENGTH_MAX,
}USER_STRENGTH_ENUM;

//--------------------------------------------------
typedef struct
{
	int8_t nTimezone;
	DATE_FORMAT nDateformat;
	uint8_t nBrightness;
}OSD_USER_DATA_ST;






/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
extern OSD_USER_DATA_ST g_osd_user_data;







/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
void Osd_User_Data_Load(USER_NVRAM_TAG_ENUM tag);
void Osd_User_Data_Save(USER_NVRAM_TAG_ENUM tag);





/*========================================================================================================
										End
========================================================================================================*/
#ifdef __cplusplus 
}
#endif

#endif//_FLEX_APP_OSD_USER_H_



/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_app_osd_func.c
 [Date]:       
 		2018-05-23
 [Author]:   	
 		carson.qian 
 [Reversion History]:
 		v1.0
*******************************************************************************/

/*========================================================================================================
										I N C L U D E
========================================================================================================*/
// external
#include "time.h"
#include "rtc.h"
#include "flex_drv_panel.h"
#include "flex_framework_esp32.h"
#include "flex_app_osd_user.h"
#include "flex_app_osd_menu.h"


// internal
#include "flex_app_osd_func.h"







/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME								"[OSD FUNC]--"
#endif

#if defined(__FLEX_TRACE_ENABLE__)
#define TRACE_F(fmt, arg...)				DBG_MSG(MOD_NAME "[F]--%s.\r\n", __func__)
#define TRACE_I(fmt, arg...)				DBG_MSG(MOD_NAME "[I]--" fmt, ##arg)
#define TRACE_W(fmt, arg...)				DBG_MSG(MOD_NAME "[W]--" fmt, ##arg)
#else
#define TRACE_F()										//do{}while(0)
#define TRACE_I(fmt, arg...)				//do{}while(0)	
#define TRACE_W(fmt, arg...)				//do{}while(0)	
#endif
#define TRACE_E(fmt, arg...)				DBG_MSG(MOD_NAME "[E]--" fmt, ##arg)





/*========================================================================================================
										E X T E R N 
========================================================================================================*/







/*========================================================================================================
										D E F I N E
========================================================================================================*/








/*========================================================================================================
										T Y P E D E F
========================================================================================================*/








/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/




/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
//=======================================================
// funtion for adjust
//=======================================================
#if 1
int16_t DecIncValue(OSD_MIA_ENUM action, int16_t Value, int16_t MinValue, int16_t MaxValue, uint8_t step)
{
	if(action == LIST_MIA_INCVALUE)
	{
		if(Value >= MaxValue)
		{
			return MaxValue;
		}
		
		Value += step;
		
		if(Value > MaxValue)
		{
			Value = MaxValue;
		}
	}
	else if(action==LIST_MIA_DECVALUE)
	{
		if(Value <= MinValue)
		{
			return MinValue;
		}
	/*		
		if(Value < step)
		{
			Value = 0;
		}
		else
	*/
		{
			Value -= step;
		}
		
		if(Value < MinValue)
		{
			Value = MinValue;
		}
	}
	return Value;
}

int16_t DecIncValue_Cycle(OSD_MIA_ENUM action, int16_t Value, int16_t MinValue, int16_t MaxValue, uint8_t step)
{
	if(action == LIST_MIA_INCVALUE)
	{
		if(Value >= MaxValue)
		{
			return MinValue;
		}
		
		Value += step;
		
		if(Value > MaxValue)
		{
			Value = MinValue;
		}
	}
	else if(action==LIST_MIA_DECVALUE)
	{
		if(Value <= MinValue)
		{
			return MaxValue;
		}

		/*
		if(Value < step)
		{
			Value = 0;
		}
		else
		*/
		{
			Value -= step;
		}
		
		if(Value < MinValue)
		{
			Value = MaxValue;
		}
	}
	return Value;
}

#else
uint16_t DecIncValue(OSD_MIA_ENUM action, uint16_t Value, uint16_t MinValue, uint16_t MaxValue, uint8_t step)
{
	if(action == LIST_MIA_INCVALUE)
	{
		if(Value >= MaxValue)
		{
			return MaxValue;
		}
		
		Value += step;
		
		if(Value > MaxValue)
		{
			Value = MaxValue;
		}
	}
	else if(action==LIST_MIA_DECVALUE)
	{
		if(Value <= MinValue)
		{
			return MinValue;
		}
		
		if(Value < step)
		{
			Value = 0;
		}
		else
		{
			Value -= step;
		}
		
		if(Value < MinValue)
		{
			Value = MinValue;
		}
	}
	return Value;
}

uint16_t DecIncValue_Cycle(OSD_MIA_ENUM action, uint16_t Value, uint16_t MinValue, uint16_t MaxValue, uint8_t step)
{
	if(action == LIST_MIA_INCVALUE)
	{
		if(Value >= MaxValue)
		{
			return MinValue;
		}
		
		Value += step;
		
		if(Value > MaxValue)
		{
			Value = MinValue;
		}
	}
	else if(action==LIST_MIA_DECVALUE)
	{
		if(Value <= MinValue)
		{
			return MaxValue;
		}
		
		if(Value < step)
		{
			Value = 0;
		}
		else
		{
			Value -= step;
		}
		
		if(Value < MinValue)
		{
			Value = MaxValue;
		}
	}
	return Value;
}
#endif
//=======================================================
// STATUS BAR: 
//=======================================================
static USER_WIFI_ICON_ST_ENUM Osd_Func_Get_WiFi_Icon_Status(void)
{
	USER_WIFI_ICON_ST_ENUM st=LIST_WIFI_ICON_ST_OFF;
	ESP32_WIFI_STATE wifiState = ESP32_GetWifiState();

/*
	switch(wifiState)
	{
		case ESP32_WIFI_OFF:						TRACE_I("[Func]:ESP32_WIFI_OFF.\r\n");					break;	
		case ESP32_WIFI_DISCONN:				TRACE_I("[Func]:ESP32_WIFI_DISCONN.\r\n"); 			break;	
		case ESP32_WIFI_CONN: 					TRACE_I("[Func]:ESP32_WIFI_CONN.\r\n");					break;	
		case ESP32_WIFI_GOTIP:					TRACE_I("[Func]:ESP32_WIFI_GOTIP.\r\n"); 				break;	
		case ESP32_WIFI_SMARTCONFIG:		TRACE_I("[Func]:ESP32_WIFI_SMARTCONFIG.\r\n"); 	break;	
		default:
			TRACE_W("[Func]:wifi status goto default.\r\n");
			break;
	}
*/

	if(ESP32_WIFI_OFF ==wifiState)
	{
		st = LIST_WIFI_ICON_ST_OFF;
	}
	else 
	{
		if((ESP32_WIFI_DISCONN == wifiState) || (ESP32_WIFI_SMARTCONFIG == wifiState))
		{
			// disconnected
			st = LIST_WIFI_ICON_ST_DICONN;
		}
		else if((ESP32_WIFI_CONN == wifiState) || (ESP32_WIFI_GOTIP == wifiState))
		{
			// connected, show strngth
			ESP32_SS_LEVEL level = ESP32_GetAPSS();
			if(ESP32_SS_HIGH == level)
			{
				st = LIST_WIFI_ICON_ST_HIGH;
			}
			else if(ESP32_SS_MIDDLE == level)
			{
				st = LIST_WIFI_ICON_ST_MIDDLE;
			}
			else
			{
				st = LIST_WIFI_ICON_ST_LOW;
			}
		}
	}

	//TRACE_I("ICON:wifi status: %d(raw:%d).\r\n", st,ESP32_GetWifiState());
	return st;
}

//----------------------------------------
static USER_BT_ICON_ST_ENUM Osd_Func_Get_Bt_Icon_Status(void)
{
	USER_BT_ICON_ST_ENUM st=LIST_BT_ICON_ST_OFF;

	switch(ESP32_GetBleState())
	{
		case ESP32_BLE_OFF:
			st=LIST_BT_ICON_ST_OFF;
			break;
			
		case ESP32_BLE_DISCONN:
			st=LIST_BT_ICON_ST_DISCONN;
			break;
			
		case ESP32_BLE_CONN:
			st=LIST_BT_ICON_ST_CONN;
			break;
			
		default:
			st=LIST_BT_ICON_ST_OFF;
			break;
	}
	//TRACE_I("ICON:bt status: %d(raw:%d).\r\n", st,ESP32_GetBleState());

	return st;
}

//=======================================================
// SUB MENU: SYS CONFIG
//=======================================================
static int8_t Osd_Func_Get_Timezone(void)
{
	Osd_User_Data_Load(LIST_NVRAM_TAG_TIMEZONE);
	return g_osd_user_data.nTimezone;
}

static bool Osd_Func_Adjust_Timezone(OSD_MIA_ENUM action)
{
	time_t utc;
	struct tm mtime;
	struct tm * ptime;
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;
	int8_t timezone = g_osd_user_data.nTimezone;
	Osd_User_Data_Load(LIST_NVRAM_TAG_TIMEZONE);
	g_osd_user_data.nTimezone = DecIncValue(action, g_osd_user_data.nTimezone, -11, 13, 1);

	if(timezone != g_osd_user_data.nTimezone)
	{
		//save new time zone to nvram
		Osd_User_Data_Save(LIST_NVRAM_TAG_TIMEZONE);
		//get current date & time
		pf_memset(&time,0,sizeof(RTC_TimeTypeDef));
		RTC_GetDateTime(&date,&time);
		//convert tu UTC time
		mtime.tm_hour = time.Hours;
		mtime.tm_min = time.Minutes;
		mtime.tm_sec = time.Seconds;
		//RTC year is based on 2000, and utc year is based on 1900
		mtime.tm_year= date.Year + (2000-1900);
		mtime.tm_mon = date.Month;
		mtime.tm_mday = date.Date;
		mtime.tm_wday = date.WeekDay - 1;
		mtime.tm_isdst = 0;
		utc = mktime(&mtime);
		//adjust utc time according to time zone changes
		utc +=(time_t)(((int8_t)(g_osd_user_data.nTimezone - timezone) )* 3600);
		//convert new utc time
		 ptime = localtime(&utc);
		//convert new utc time to date & time
		time.Hours = ptime->tm_hour;
		time.Minutes = ptime->tm_min;
		time.Seconds = ptime->tm_sec;
		//RTC year is based on 2000, and utc year is based on 1900
		date.Year = ptime->tm_year - (2000 - 1900);
		date.Month = ptime->tm_mon;
		date.Date = ptime->tm_mday;
		date.WeekDay = ptime->tm_wday + 1;
		RTC_SetDateTime(&date,&time);

		//DBG_MSG ("new local time and date: %s\n", asctime(ptime));
	}
	return true;
}
//----------------------------------------
static DATE_FORMAT Osd_Func_Get_DateFormat(void)
{
	Osd_User_Data_Load(LIST_NVRAM_TAG_DATEFORMAT);
	return g_osd_user_data.nDateformat;
}

static bool Osd_Func_Adjust_DateFormat(OSD_MIA_ENUM action)
{
// adjsut
	Osd_User_Data_Load(LIST_NVRAM_TAG_DATEFORMAT);
	g_osd_user_data.nDateformat = (DATE_FORMAT)DecIncValue_Cycle(action, g_osd_user_data.nDateformat, (DATE_FORMAT)0, (DATE_FORMAT_MAX-1), 1);
	Osd_User_Data_Save(LIST_NVRAM_TAG_DATEFORMAT);
// action
	Osd_Menu_Draw_Page(MENU_STATUS_BAR,0);
	return true;
}

//=======================================================
// SUB MENU: WIFI CONFIG
//=======================================================
static USER_WIFI_ST_ENUM Osd_Func_Get_WiFi_Status(void)
{
	USER_WIFI_ST_ENUM st;

	// OFF-->ON-->CONNECT-->DHCP ASSIGN IP
	switch(ESP32_GetWifiState())
	{	
		case ESP32_WIFI_OFF:
			st = LIST_WIFI_ST_OFF;	
			break;
	
		case ESP32_WIFI_DISCONN:
		case ESP32_WIFI_SMARTCONFIG:
			st = LIST_WIFI_ST_DISCONN;	
			break;
			
		case ESP32_WIFI_CONN:
		case ESP32_WIFI_GOTIP:
			st = LIST_WIFI_ST_CONN;	
			break;

		default:
			st = LIST_WIFI_ST_OFF;	
			break;
	}

	//TRACE_I("wifi status: %d(raw:%d).\r\n", st,ESP32_GetWifiState());
	
	return st;
}


static USER_STRENGTH_ENUM Osd_Func_Get_WiFi_Rssi(void)
{
	USER_STRENGTH_ENUM rssi;

	switch(ESP32_GetAPSS())
	{
		case ESP32_SS_LOW:
			rssi=LIST_STRENGTH_LOW;
			break;
			
		case ESP32_SS_MIDDLE:
			rssi=LIST_STRENGTH_MEDIUM;
			break;
			
		case ESP32_SS_HIGH:
			rssi=LIST_STRENGTH_HIGH;
			break;

		default:
			break;
	}
	
	return rssi;
}

//=======================================================
// SUB MENU: BT CONFIG
//=======================================================
static USER_BT_ST_ENUM Osd_Func_Get_Bt_Status(void)
{
	USER_BT_ST_ENUM st;

	switch(ESP32_GetBleState())
	{
		case ESP32_BLE_OFF:
			st = LIST_BT_ST_OFF;	
			break;
			
		case ESP32_BLE_DISCONN:
			st = LIST_BT_ST_DISCONN;	
			break;
			
		case ESP32_BLE_CONN:
			st = LIST_BT_ST_CONN;	
			break;

		default:
			st = LIST_BT_ST_OFF;	
			break;		
	}
	
	//TRACE_I("bt status: %d(raw:%d).\r\n", st,ESP32_GetBleState());
	
	return st;
}

//=======================================================
// SUB MENU: SYS INFO
//=======================================================




//=======================================================
// SUB MENU: SYS STATUS
//=======================================================
static uint8_t Osd_Func_Get_Brightness(void)
{
	Osd_User_Data_Load(LIST_NVRAM_TAG_BRIGHTNESS);
	return g_osd_user_data.nBrightness;
}

static bool Osd_Func_Adjust_Brightness(OSD_MIA_ENUM action)
{
// adjsut
	Osd_User_Data_Load(LIST_NVRAM_TAG_BRIGHTNESS);
#if 1
/*
	for STN LCD's Backlight will take effect on TSL jumpping.only pwm=0 or 100 will ok.
*/
	g_osd_user_data.nBrightness = DecIncValue(action, g_osd_user_data.nBrightness, 0, 100, 100);
#else
	g_osd_user_data.nBrightness = DecIncValue(action, g_osd_user_data.nBrightness, 0, 100, 10);
#endif
	Osd_User_Data_Save(LIST_NVRAM_TAG_BRIGHTNESS);
// action
	Drv_Panel_Set_Brightness(LIST_PANEL_INDEX_MAIN, g_osd_user_data.nBrightness);
	return true;
}
//----------------------------------------
static uint8_t Osd_Func_Get_FanSpeed(void)
{
	//return g_osd_user_data.nFanSpeed;
	return LIST_STRENGTH_MEDIUM;
}

static bool Osd_Func_Adjust_FanSpeed(OSD_MIA_ENUM action)
{
	//g_osd_user_data.nFanSpeed = (USER_STRENGTH_ENUM)DecIncValue_Cycle(action, g_osd_user_data.nFanSpeed, (USER_STRENGTH_ENUM)0, (LIST_STRENGTH_MAX-1), 1);
	return true;
}

//=======================================================
// Ditector
//=======================================================
int16_t Osd_Value_Director(bool dynamic, OSD_KEY_FUNCTION_ENUM KeyFuncIdx, OSD_MIA_ENUM action)
{
	int16_t ret = 0;
	
	if (!dynamic)
	{
		ret = KeyFuncIdx;
	}
	else
	{
		switch (KeyFuncIdx)
		{
		//------------------------- 
		// STATUS BAR
		//------------------------- 
			case Func_Get_StatusBar_WiFi:   
				ret = Osd_Func_Get_WiFi_Icon_Status();
				break;
				
			case Func_Get_StatusBar_Bt:
				ret = Osd_Func_Get_Bt_Icon_Status();
				break;

		//------------------------- 
		// MAIN MENU
		//------------------------- 



		//------------------------- 
		// SUB MENH: WIFI CFG
		//------------------------- 
			case Func_Get_TimeZone:
				ret=Osd_Func_Get_Timezone();
				break;
				
			case Func_Set_TimeZone:
				ret=Osd_Func_Adjust_Timezone(action);
				break;
		
			case Func_Get_DateFormat:
				ret=Osd_Func_Get_DateFormat();
				break;
				
			case Func_Set_DateFormat:
				ret=Osd_Func_Adjust_DateFormat(action);
				break;

		//------------------------- 
		// SUB MENH: WIFI CFG
		//------------------------- 
			case Func_Get_Wifi_Status:
				ret=Osd_Func_Get_WiFi_Status();
				break;
			
			case Func_Get_Wifi_Rssi:
				ret=Osd_Func_Get_WiFi_Rssi();
				break;
			
		//------------------------- 
		// SUB MENH: BT CFG
		//------------------------- 
			case Func_Get_Bt_Status:
				ret=Osd_Func_Get_Bt_Status();
				break;
				
		//------------------------- 
		// SUB MENH: SYS INFO
		//------------------------- 
			case Func_Get_SdkVersion:
				break;
				
			case Func_Get_AtVersion:
				break;
				
			case Func_Get_SerialNo:
				break;
				
			case Func_Get_BuildInfo:
				break;
					
		//------------------------- 
		// SUB MENH: SYS STATUS
		//------------------------- 
		case Func_Get_Brightness: 	
			ret=Osd_Func_Get_Brightness();
			break;
			
		case Func_Set_Brightness:
			ret=Osd_Func_Adjust_Brightness(action);
			break;
		//------- 	
		case Func_Get_FanSpeed: 	
			ret=Osd_Func_Get_FanSpeed();
			break;
			
		case Func_Set_FanSpeed: 
			ret=Osd_Func_Adjust_FanSpeed(action);
			break;
		
		case Func_Get_Temp:
			break;
			
		case Func_Set_Temp:
			break;
		
		case Func_Get_Humidity:
			break;

		case Func_Get_Help_LR_label:
			switch(g_osd_menu_data.nPage)
			{
				case MENU_SYS_CFG:
				case MENU_POPUP_BRIGHTNESS:
					ret=1;  // 1:ADJ
					break;

				default:
					ret=0;	// 0:ENTER, 
					break;
			}
			break;
		//------------------------- 
			default:
				break;
		}
	}
	return ret;
}

//=======================================================
// Date and Time for status bar
//=======================================================
RTC_DateTypeDef Osd_Func_Get_Date(void)
{
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;

	RTC_GetDateTime(&date, &time);
	return date;
}

RTC_TimeTypeDef Osd_Func_Get_Time(void)
{
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;

	RTC_GetDateTime(&date, &time);
	return time;
}



/*========================================================================================================
										End
========================================================================================================*/




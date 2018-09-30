/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		flex_app.c
 [Date]:
 		2017-12-20
 [Author]:
 		carson.qian
 [Reversion History]:
 		v1.0
*******************************************************************************/

/*========================================================================================================
										I N C L U D E
========================================================================================================*/
// external
#include "flex_hal.h"

#include "rtc.h"
#include "tsc.h"
#include "tsl_user.h"
#include "flex_drv_tsc.h"
#include "flex_drv_captouch.h"


// internal
#include "flex_app.h"
#include "flex_app_blotter.h"
#include "flex_app_osd_application.h"
#include "flex_framework_input.h"
#include "flex_framework_esp32.h"
#include "flex_app_nvparam.h"

/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME					"[APP]--"
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

//========================================
// DEBUG
//========================================
#if defined(__FLEX_TRACE_ENABLE__)
#define DEBUG_TRACE				
#endif
/*========================================================================================================
										E X T E R N
========================================================================================================*/






/*========================================================================================================
										D E F I N E
========================================================================================================*/
#define MISC_TASK_SIGNAL_CAPTOUCH_POLLING_MASK    _BIT0_


#define BLOTTER_WIFI_CONN_MSG "connect:"



//========================================
// Led delay off
//========================================
#define ENABLE_DIRECTKEYLED_DELAY_OFF  true
	
#if ENABLE_DIRECTKEYLED_DELAY_OFF
#define LED_DELAY_OFF_TIME		500
#endif

/*========================================================================================================
										T Y P E D E F
========================================================================================================*/







/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
// Task
static osThreadId g_app_task_id_misc = NULL;
static osThreadId g_app_task_id_key = NULL;

// Queue
static osMessageQId g_app_queue_id_key = NULL;

// Timer
static osTimerId g_TimerId_CapTouch;
#if ENABLE_DIRECTKEYLED_DELAY_OFF
static osTimerId g_TimerId_KeyUpLed=NULL;
static osTimerId g_TimerId_KeyDownLed=NULL;
static osTimerId g_TimerId_KeyLeftLed=NULL;
static osTimerId g_TimerId_KeyRightLed=NULL;
#endif

/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
//========================================
// CapTouch
//========================================
static void App_CapTouch_Polling_Timer_Expired(void const *argument)
{
	int32_t ret = 0;

	//if (g_app_task_id_misc != NULL)
	{
		ret = osSignalSet(g_app_task_id_misc, MISC_TASK_SIGNAL_CAPTOUCH_POLLING_MASK);

		if (0x80000000 == ret)
		{
			TRACE_E("[TIMER_CAP1214]-ret(%d).\r\n", ret);
		}
	}
}

//========================================
// Key
//========================================
static void App_Input_Key_Callback(FRM_INPUT_KEY_ST key_info)
{
	switch(key_info.nKey)
	{
		case LIST_KEY_0:      TRACE_I("KEY_0---%s \r\n", key_info.nAction?"ON":"OFF");		break;					
		case LIST_KEY_1:      TRACE_I("KEY_1---%s \r\n", key_info.nAction?"ON":"OFF");		break;				
		case LIST_KEY_2:      TRACE_I("KEY_2---%s \r\n", key_info.nAction?"ON":"OFF");		break;						
		case LIST_KEY_3:      TRACE_I("KEY_3---%s \r\n", key_info.nAction?"ON":"OFF");		break;					 				
		case LIST_KEY_4:      TRACE_I("KEY_4---%s \r\n", key_info.nAction?"ON":"OFF");		break;					 				
		case LIST_KEY_5:      TRACE_I("KEY_5---%s \r\n", key_info.nAction?"ON":"OFF");		break;					 				
		case LIST_KEY_6:      TRACE_I("KEY_6---%s \r\n", key_info.nAction?"ON":"OFF");		break;					 				
		case LIST_KEY_7:      TRACE_I("KEY_7---%s \r\n", key_info.nAction?"ON":"OFF");		break;					 				
		case LIST_KEY_8:      TRACE_I("KEY_8---%s \r\n", key_info.nAction?"ON":"OFF");		break;					 				
		case LIST_KEY_9:      TRACE_I("KEY_9---%s \r\n", key_info.nAction?"ON":"OFF");		break;					
		case LIST_KEY_OK:     TRACE_I("KEY_OK---%s \r\n", key_info.nAction?"ON":"OFF");	break;			
		case LIST_KEY_DEL:    TRACE_I("KEY_DEL---%s \r\n", key_info.nAction?"ON":"OFF");	break;		
	//----------------
		case LIST_KEY_UP:     TRACE_I("KEY_UP---%s \r\n", key_info.nAction?"ON":"OFF");	break;	
		case LIST_KEY_DOWN:   TRACE_I("KEY_DOWN---%s \r\n", key_info.nAction?"ON":"OFF");		break;	
		case LIST_KEY_LEFT:   TRACE_I("KEY_LEFT---%s \r\n", key_info.nAction?"ON":"OFF");		break;	
		case LIST_KEY_RIGHT:  TRACE_I("KEY_RIGHT---%s \r\n", key_info.nAction?"ON":"OFF");		break;	
	//----------------
	/*
		case LIST_KEY_GEAR_1: TRACE_I("KEY_GEAR_0---%s \r\n", key_info.nAction?"ON":"OFF");		break;	
		case LIST_KEY_GEAR_2: TRACE_I("KEY_GEAR_0---%s \r\n", key_info.nAction?"ON":"OFF");		break;	
		case LIST_KEY_GEAR_3: TRACE_I("KEY_GEAR_0---%s \r\n", key_info.nAction?"ON":"OFF");		break;	
		case LIST_KEY_GEAR_4: TRACE_I("KEY_GEAR_0---%s \r\n", key_info.nAction?"ON":"OFF");		break;	
		case LIST_KEY_GEAR_5: TRACE_I("KEY_GEAR_0---%s \r\n", key_info.nAction?"ON":"OFF");		break;	
		case LIST_KEY_GEAR_6: TRACE_I("KEY_GEAR_0---%s \r\n", key_info.nAction?"ON":"OFF");		break;	
		case LIST_KEY_GEAR_7: TRACE_I("KEY_GEAR_0---%s \r\n", key_info.nAction?"ON":"OFF");		break;	
		case LIST_KEY_GEAR_8: TRACE_I("KEY_GEAR_0---%s \r\n", key_info.nAction?"ON":"OFF");		break;	
	//----------------
		case LIST_KEY_WIFI:   TRACE_I("KEY_WIFI---%s \r\n", key_info.nAction?"ON":"OFF");		break;				
		case LIST_KEY_BT:     TRACE_I("KEY_BT---%s \r\n", key_info.nAction?"ON":"OFF");		break;				
	//----------------
		case LIST_KEY_PLAY:   TRACE_I("KEY_PLAY---%s \r\n", key_info.nAction?"ON":"OFF");		break;				
		case LIST_KEY_PAUSE:  TRACE_I("KEY_PAUSE---%s \r\n", key_info.nAction?"ON":"OFF");		break;	 		
	*/
	//----------------
		case LIST_KEY_PAGE_UP:   TRACE_I("KEY_PAGE_UP---%s \r\n", key_info.nAction?"ON":"OFF");		break;	 	
		case LIST_KEY_PAGE_DOWN: TRACE_I("KEY_PAGE_DOWN---%s \r\n", key_info.nAction?"ON":"OFF");	break;	 
	//----------------
		case LIST_KEY_INC:       TRACE_I("KEY_INC---%s \r\n", key_info.nAction?"ON":"OFF");		break;	 			
		case LIST_KEY_DEC:       TRACE_I("KEY_DEC---%s \r\n", key_info.nAction?"ON":"OFF");		break;	 
	//----------------
		default:
			break;
	}
	Queue_MsgPut(g_app_queue_id_key, &key_info, 50);
}

#if ENABLE_DIRECTKEYLED_DELAY_OFF
static void App_KeyUp_Led_DelayOff_Callback(void const *argument)
{
	Drv_Led_Ctrl(LIST_LED_UP, false);	
}

static void App_KeyDown_Led_DelayOff_Callback(void const *argument)
{
	Drv_Led_Ctrl(LIST_LED_DOWN, false);	
}

static void App_KeyLeft_Led_DelayOff_Callback(void const *argument)
{
	Drv_Led_Ctrl(LIST_LED_LEFT, false);	
}

static void App_KeyRight_Led_DelayOff_Callback(void const *argument)
{
	Drv_Led_Ctrl(LIST_LED_RIGHT, false);	
}
#endif

static void App_Input_Key_Led_Handler(FRM_KEY_ENUM key, bool pressed)
{
	switch(key)
	{
		case LIST_KEY_0:				
		case LIST_KEY_1: 				
		case LIST_KEY_2: 				
		case LIST_KEY_3: 				
		case LIST_KEY_4: 				
		case LIST_KEY_5: 				
		case LIST_KEY_6: 				
		case LIST_KEY_7: 				
		case LIST_KEY_8: 				
		case LIST_KEY_9:
			Drv_Led_Ctrl((flex_led_name_enum)(LIST_LED_KEY0+(key-LIST_KEY_0)), pressed?true:false); 
			break;
		case LIST_KEY_OK:		
			Drv_Led_Ctrl(LIST_LED_OK, pressed?true:false);	
			break;
		case LIST_KEY_DEL:			
			Drv_Led_Ctrl(LIST_LED_DEL, pressed?true:false);	
			break;
	//----------------
		case LIST_KEY_UP: 
			#if ENABLE_DIRECTKEYLED_DELAY_OFF
			if(pressed)
			{
				osTimerStop(g_TimerId_KeyUpLed);
				Drv_Led_Ctrl(LIST_LED_UP, true);	
			}
			else
			{
				osTimerStart(g_TimerId_KeyUpLed, LED_DELAY_OFF_TIME);
			}
			#else
			Drv_Led_Ctrl(LIST_LED_UP, pressed?true:false);	
			#endif
			break;
			
		case LIST_KEY_DOWN: 
			#if ENABLE_DIRECTKEYLED_DELAY_OFF
			if(pressed)
			{
				osTimerStop(g_TimerId_KeyDownLed);
				Drv_Led_Ctrl(LIST_LED_DOWN, true);	
			}
			else
			{
				osTimerStart(g_TimerId_KeyDownLed, LED_DELAY_OFF_TIME);
			}
			#else
			Drv_Led_Ctrl(LIST_LED_DOWN, pressed?true:false);	
			#endif
			break;
			
		case LIST_KEY_LEFT: 
			#if ENABLE_DIRECTKEYLED_DELAY_OFF
			if(pressed)
			{
				osTimerStop(g_TimerId_KeyLeftLed);
				Drv_Led_Ctrl(LIST_LED_LEFT, true);	
			}
			else
			{
				osTimerStart(g_TimerId_KeyLeftLed, LED_DELAY_OFF_TIME);
			}
			#else
			Drv_Led_Ctrl(LIST_LED_LEFT, pressed?true:false);	
			#endif
			break;
			
		case LIST_KEY_RIGHT:	
			#if ENABLE_DIRECTKEYLED_DELAY_OFF
			if(pressed)
			{
				osTimerStop(g_TimerId_KeyRightLed);
				Drv_Led_Ctrl(LIST_LED_RIGHT, true); 
			}
			else
			{
				osTimerStart(g_TimerId_KeyRightLed, LED_DELAY_OFF_TIME);
			}
			#else
			Drv_Led_Ctrl(LIST_LED_RIGHT, pressed?true:false); 
			#endif
			break;
	//----------------
		case LIST_KEY_GEAR_1:		
		case LIST_KEY_GEAR_2:		
		case LIST_KEY_GEAR_3:		
		case LIST_KEY_GEAR_4:		
		case LIST_KEY_GEAR_5:	
		case LIST_KEY_GEAR_6:	 
		case LIST_KEY_GEAR_7:	
		case LIST_KEY_GEAR_8:		
			break;
	//----------------
		case LIST_KEY_WIFI:			
			break;
		case LIST_KEY_BT:			
			break;
	//----------------
		case LIST_KEY_PLAY:			
			break;
		case LIST_KEY_PAUSE:		
			break;
	//----------------
		case LIST_KEY_PAGE_UP: 	
			break;
		case LIST_KEY_PAGE_DOWN:
			break;
	//----------------
		case LIST_KEY_INC: 			
			break;
		case LIST_KEY_DEC:
			break;
	//----------------
		default:
			break;			
	}
}

static void App_Input_Key_Process_Led(FRM_INPUT_KEY_ST KeyInfo)
{
	if (LIST_KEY_ACT_PRESS== KeyInfo.nAction)
	{
		//TRACE_I("APP: KEY PRESS.\r\n");
		App_Input_Key_Led_Handler(KeyInfo.nKey, true);	
	}
	else
	{
		//TRACE_I("APP: KEY RELEASE.\r\n");
		App_Input_Key_Led_Handler(KeyInfo.nKey, false);	
	}
}
//========================================
//ESP32 EVENT callback
//========================================
static void App_ESP32_Callback(ESP32_NOTIFICATION *notification)
{
	//UNUSED(notification);
	char *p;
	uint16_t length;
#ifdef ESP32_AP_LIST_SUPPORT
	ESP32_AP_INFO_CHAIN *chain;
#endif
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;
	static ESP32_WIFI_STATE lastWifiState = ESP32_WIFI_MAX_STATE;
	static ESP32_BLE_STATE lastBleState = ESP32_BLE_MAX_STATE;

	//notify APP OSD
	App_Osd_Add_EspEvent(notification->event);

	switch (notification->event)
	{
		case ESP32_EVENT_WIFI_OFF:
			//DBG_MSG("WIFI off\n");
			App_Blotter_Add_Event("WIFI off");
			break;

		case ESP32_EVENT_WIFI_CONN:
			//DBG_MSG("WIFI Connect to AP:%s,%d\n",notification->data.apInfo->ssid,notification->data.apInfo->rssi);
			length = strlen(notification->data.apInfo->ssid) + strlen(BLOTTER_WIFI_CONN_MSG) + 8;
			p = pf_malloc(length);

			if (p)
			{
				snprintf(p, length, "%s%s,%d", BLOTTER_WIFI_CONN_MSG,
						 notification->data.apInfo->ssid,
						 notification->data.apInfo->rssi);
				App_Blotter_Add_Event(p);
				pf_free(p);
			}

			break;

		case  ESP32_EVENT_WIFI_SS:
			//connected AP wifi signal strength event
			if (ESP32_SS_HIGH == *notification->data.apSSLevel)
			{
				App_Blotter_Add_Event("WIFI signal good");
			}
			else if (ESP32_SS_MIDDLE == *notification->data.apSSLevel)
			{
				App_Blotter_Add_Event("WIFI signal medium");
			}
			else
			{
				App_Blotter_Add_Event("WIFI signal weak");
			}

			break;

		case ESP32_EVENT_WIFI_GOTIP:
			//DBG_MSG("IP:%s\n", notification->data.ipAddr);
			App_Blotter_Add_Event(notification->data.ipAddr);

			//request to synchronize SNTP time
			ESP32_RequestSntpTime(GetTimeZoneFromNvram());
			break;

		case ESP32_EVENT_WIFI_DISCONN:
			if ( (ESP32_WIFI_CONN == lastWifiState) || (ESP32_WIFI_GOTIP == lastWifiState) )
			{
				App_Blotter_Add_Event("WIFI disconnect");
			}
			else
			{
				//DBG_MSG("WIFI on\n");
				App_Blotter_Add_Event("WIFI on");
			}

			break;

		case ESP32_EVENT_WIFI_SMARTCONFIG:
			//DBG_MSG("WIFI Smart config is going\n");
			App_Blotter_Add_Event("WIFI Smart config is going");
			break;

		case ESP32_EVENT_SNTP_TIME:

			//DBG_MSG("%d/%d/%d %02d:%02d:%02d\n", notification->data.dateTime->month,
			//		notification->data.dateTime->day, notification->data.dateTime->year,
			//		notification->data.dateTime->hour, notification->data.dateTime->minute,
			//		notification->data.dateTime->second);

			//date will be 1970 if SNTP is failed
			if (notification->data.dateTime->year >= 2018)
			{
				date.Year = notification->data.dateTime->year - 2000;
				date.Month =  notification->data.dateTime->month;
				date.Date = notification->data.dateTime->day;
				date.WeekDay =  notification->data.dateTime->weekday;
				time.Hours = notification->data.dateTime->hour;
				time.Minutes = notification->data.dateTime->minute;
				time.Seconds =  notification->data.dateTime->second;
				time.SecondFraction = 0;
				time.SubSeconds = 0;
				time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
				time.TimeFormat = RTC_HOURFORMAT12_AM;
				time.StoreOperation = RTC_STOREOPERATION_RESET;
				RTC_SetDateTime(&date, &time);
			}

			//add it to blotter whatever
			p = pf_malloc(32);

			if (p)
			{
				snprintf(p, 32, "%d/%d/%d %02d:%02d:%02d", notification->data.dateTime->month,
						 notification->data.dateTime->day, notification->data.dateTime->year,
						 notification->data.dateTime->hour, notification->data.dateTime->minute,
						 notification->data.dateTime->second);
				App_Blotter_Add_Event(p);
				pf_free(p);
			}

			break;

#ifdef ESP32_AP_LIST_SUPPORT
		case ESP32_EVENT_WIFI_AP_LIST:
			chain = notification->data.apList;

			do
			{
				//DBG_MSG("%d,%s,%d,%s,%d\n", chain->apInfo.ecn, chain->apInfo.ssid, chain->apInfo.rssi,
				//		chain->apInfo.macAddr, chain->apInfo.channel);
				chain = chain->next;
			}
			while (chain);

			break;
#endif

		case ESP32_EVENT_BT_OFF:
			App_Blotter_Add_Event("BT off");
			break;

		case ESP32_EVENT_BT_DISCONN:
			if ( ESP32_BLE_CONN == lastBleState)
			{
				App_Blotter_Add_Event("BT disconnect");
			}
			else
			{
				App_Blotter_Add_Event("BT on");
			}

			break;

		case ESP32_EVENT_BT_CONN:
			App_Blotter_Add_Event("BT connect");
			break;

		default:
			break;
	}

	//save last wifi/bt state
	lastWifiState = ESP32_GetWifiState();
	lastBleState = ESP32_GetBleState();
}

//========================================
// Task----Misc
//========================================
static void App_Misc_Task_Init(void)
{
	TRACE_F();

// set key callback
	Frm_Input_Key_Register_Callback(App_Input_Key_Callback);
// Timer---For Led Delay to Off 
#if ENABLE_DIRECTKEYLED_DELAY_OFF
	osTimerDef(KeyUpLedTimer, App_KeyUp_Led_DelayOff_Callback);
	g_TimerId_KeyUpLed= osTimerCreate(osTimer(KeyUpLedTimer), osTimerOnce, NULL);
	assert_param(g_TimerId_KeyUpLed != NULL);

	osTimerDef(KeyDownLedTimer, App_KeyDown_Led_DelayOff_Callback);
	g_TimerId_KeyDownLed= osTimerCreate(osTimer(KeyDownLedTimer), osTimerOnce, NULL);
	assert_param(g_TimerId_KeyDownLed != NULL);

	osTimerDef(KeyLeftLedTimer, App_KeyLeft_Led_DelayOff_Callback);
	g_TimerId_KeyLeftLed= osTimerCreate(osTimer(KeyLeftLedTimer), osTimerOnce, NULL);
	assert_param(g_TimerId_KeyLeftLed != NULL);

	osTimerDef(KeyRightLedTimer, App_KeyRight_Led_DelayOff_Callback);
	g_TimerId_KeyRightLed= osTimerCreate(osTimer(KeyRightLedTimer), osTimerOnce, NULL);
	assert_param(g_TimerId_KeyRightLed != NULL);
#endif

// Timer---CapTouch Polling
	if(Drv_CapTouch_Support_Polling())
	{
		osTimerDef(ButterflyTscCapTimer, App_CapTouch_Polling_Timer_Expired);
		g_TimerId_CapTouch = osTimerCreate(osTimer(ButterflyTscCapTimer), osTimerPeriodic, NULL);
		assert_param(g_TimerId_CapTouch != NULL);
		osTimerStart(g_TimerId_CapTouch, 50);	
	}
}

void App_Misc_Task(void const *argument)
{
	osEvent event;

	TRACE_F();

	g_app_task_id_misc = osThreadGetId();

	// Init
	App_Misc_Task_Init();

	//register ESP32 APP notification
	ESP32_AppRegisterCallback(App_ESP32_Callback);

	/* Infinite loop */
	for (;;)
	{
		event = osSignalWait(0x7FFFFFFF, osWaitForever);

		if (event.status == osEventSignal)
		{
		//---------------------------------------------------
			if ((event.value.signals) & MISC_TASK_SIGNAL_CAPTOUCH_POLLING_MASK)
			{
				Drv_CapTouch_Polling();
			}
		//---------------------------------------------------
		}
	}
}

//========================================
// Task----Kye
//========================================
void App_Key_Task(void const * argument)
{
	osEvent event;
	
	g_app_task_id_key = osThreadGetId();

	osMessageQDef(AppKeyQueue, 16, FRM_INPUT_KEY_ST);
	g_app_queue_id_key = osMessageCreate(osMessageQ(AppKeyQueue), g_app_task_id_key);
	assert_param(g_app_queue_id_key != NULL);

	for(;;)
	{
		event = osMessageGet(g_app_queue_id_key, osWaitForever);

		if(event.status==osEventMessage)  
		{
			FRM_INPUT_KEY_ST KeyInfo;
			pf_memcpy(&KeyInfo, &event.value.p, sizeof(FRM_INPUT_KEY_ST));
			TRACE_I("RECV:BTN=%d, ACT=%d.\r\n", KeyInfo.nKey,  KeyInfo.nAction);
			App_Input_Key_Process_Led(KeyInfo);
		}
	}
}


//========================================
// Task----TSC
//========================================
void App_Tsc_Task(void const *argument)
{
	tsl_user_status_t tsl_status;

	TRACE_F();

	for (;;)
	{
		osDelay(1);
		tsl_status = tsl_user_Exec();

		//TRACE_I("tsl_status=%d.\r\n", tsl_status);
		if (tsl_status != TSL_USER_STATUS_BUSY)
		{
			Drv_Tsc_Process_Sensors(tsl_status);
		}
	}
}

/*========================================================================================================
										End
========================================================================================================*/




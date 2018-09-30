/******************************************************************************
 Copyright (c) 2018 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		flex_framework_esp32.h
 [Date]:
 		2018-06-26
 [Author]:
 		Tim Lin
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_FRAMEWORK_ESP32_H_
#define _FLEX_FRAMEWORK_ESP32_H_
/*========================================================================================================
										I N C L U D E
========================================================================================================*/
// FLEX
#include "flex_common.h"

#include "stdint.h"
#include "string.h"
#include "gpio.h"
/*========================================================================================================
										D E F I N E
========================================================================================================*/
//Enable ESP32 BLE as server role
#define ESP32_BLE_SERVER_SUPPORT
//switch to support AP list query service
//#define ESP32_AP_LIST_SUPPORT


/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
typedef enum
{
	ESP32_STATE_IDLE,
	ESP32_STATE_BUSY,
	ESP32_STATE_MAX_STATE,
} ESP32_STATE;

typedef enum
{
	ESP32_STATUS_OK,
	ESP32_STATUS_ERROR,
	ESP32_STATUS_TIMEOUT,
	ESP32_STATUS_BUSY,
	ESP32_STATUS_BUFFER_TOO_SMALL,
	ESP32_STATUS_UNAVALIABLE,
	ESP32_STATUS_MAX_STATUS,
} ESP32_STATUS;

typedef enum
{
	ESP32_WIFI_OFF,
	ESP32_WIFI_DISCONN,
	ESP32_WIFI_CONN,
	ESP32_WIFI_GOTIP,
	ESP32_WIFI_SMARTCONFIG,
	ESP32_WIFI_MAX_STATE,
} ESP32_WIFI_STATE;

typedef enum
{
	ESP32_BLE_OFF,
	ESP32_BLE_DISCONN, //BT on, but not connected
	ESP32_BLE_CONN,
	ESP32_BLE_MAX_STATE,
} ESP32_BLE_STATE;

typedef enum
{
	//ESP32_EVENT_AT_VER,
	//ESP32_EVENT_SDK_VER,
	//ESP32_EVENT_MAC_ADDR,
	ESP32_EVENT_WIFI_OFF,
	ESP32_EVENT_WIFI_CONN,
	ESP32_EVENT_WIFI_GOTIP,
	ESP32_EVENT_WIFI_DISCONN,
	ESP32_EVENT_WIFI_SMARTCONFIG,
#ifdef ESP32_AP_LIST_SUPPORT
	ESP32_EVENT_WIFI_AP_LIST,
#endif
	ESP32_EVENT_WIFI_SS,
	ESP32_EVENT_SNTP_TIME,
	ESP32_EVENT_SMARTCFG_INFO,
	ESP32_EVENT_BT_OFF,
	ESP32_EVENT_BT_CONN,
	ESP32_EVENT_BT_DISCONN,	
	ESP32_EVENT_MAX,
} ESP32_EVENT;


//AT typedef
typedef char *ESP32_AT_VERSION;
typedef char *ESP32_SDK_VERSION;
typedef char *ESP32_MAC_ADDR;
typedef char *ESP32_AP_SSID;
typedef char *ESP32_AP_PWD;
typedef int8_t ESP32_AP_RSSI;
typedef uint8_t ESP32_CHANNEL;
typedef char *ESP32_IP_ADDR;

//SNTP
typedef enum
{
	ESP32_WEEK_INVALID,
	ESP32_WEEK_MONDAY,
	ESP32_WEEK_TUESDAY,
	ESP32_WEEK_WEDNESDAY,
	ESP32_WEEK_THURSDAY,
	ESP32_WEEK_FRIDAY,
	ESP32_WEEK_SATURDAY,
	ESP32_WEEK_SUNDAY,
} ESP32_WEEKDAY;

typedef enum
{
	ESP32_MONTH_INVALID,
	ESP32_MONTH_JANUARY,
	ESP32_MONTH_FEBURARY,
	ESP32_MONTH_MARCH,
	ESP32_MONTH_APRIL,
	ESP32_MONTH_MAY,
	ESP32_MONTH_JUNE,
	ESP32_MONTH_JULY,
	ESP32_MONTH_AUGUST,
	ESP32_MONTH_SETEMPBER,
	ESP32_MONTH_OCTOBER,
	ESP32_MONTH_NOVEMBER,
	ESP32_MONTH_DECEMBER,
} ESP32_MONTH;

typedef struct
{
	uint16_t year;
	ESP32_MONTH month;
	uint8_t day;
	ESP32_WEEKDAY weekday;
	uint8_t hour; 		//in binary format
	uint8_t minute;	//in binary format
	uint8_t second;
} ESP32_DATE_TIME;

typedef int8_t TIMEZONE;
typedef void (*SNTP_TIME_CB)(ESP32_STATUS status,ESP32_DATE_TIME *date);

typedef struct
{
	TIMEZONE timezone;
	SNTP_TIME_CB callback;
} SNTP_TIME_PARAMETER;

//SMART CONFIG AP information
typedef struct
{
	ESP32_AP_SSID ssid;
	ESP32_AP_PWD pwd;
}ESP32_SMARTCONFIG_INFO;


//Connected AP information
#define ESP32_INVALID_RSSI	((ESP32_AP_RSSI)127)
typedef struct
{
	ESP32_AP_SSID ssid;
	ESP32_MAC_ADDR macAddr;
	ESP32_AP_RSSI rssi;
}ESP32_CONN_AP_INFO;

//signal strength level
typedef enum
{
	ESP32_SS_LOW,
	ESP32_SS_MIDDLE,
	ESP32_SS_HIGH,
}ESP32_SS_LEVEL;

//encryption method
typedef enum
{
	ESP32_ECN_OPEN,
	ESP32_ECN_WEP,		
	ESP32_ECN_WPA_PSK,
	ESP32_ECN_WPA2_PSK,
	ESP32_ECN_WPA2_ENTERPRISE, // (AT can NOT connect to WPA2_Enterprise AP for now.)
}ESP32_ECN;

//AP Information
typedef struct
{
	ESP32_ECN ecn;//encryption method
	ESP32_AP_SSID ssid;
	ESP32_AP_RSSI rssi;
	ESP32_MAC_ADDR macAddr;
	ESP32_CHANNEL channel;
}ESP32_AP_INFO;

typedef struct _ESP32_AP_INFO_CHAIN
{
	ESP32_AP_INFO apInfo;
	struct _ESP32_AP_INFO_CHAIN *next; //NULL as last one
} ESP32_AP_INFO_CHAIN;

//ESP32 EVENT CALLBACK
typedef struct
{
	ESP32_EVENT event;
	ESP32_STATUS status;
	union
	{
		void	*p;
		//ESP32_AT_VERSION atVersion;
		//ESP32_SDK_VERSION sdkVersion;
		//ESP32_MAC_ADDR macAddr;
		ESP32_CONN_AP_INFO *apInfo;
		//connected AP signal strength level
		ESP32_SS_LEVEL *apSSLevel;
		ESP32_IP_ADDR ipAddr;
#ifdef ESP32_AP_LIST_SUPPORT		
		ESP32_AP_INFO_CHAIN *apList;
#endif
		ESP32_DATE_TIME *dateTime;

	} data;
} ESP32_NOTIFICATION;
typedef void (*ESP32_AP_CALLBACK)(ESP32_NOTIFICATION *notification);


/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/


/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
void ESP32_Init(void);
void ESP32_SetWifiState(ESP32_WIFI_STATE state);
void ESP32_SetBleState(ESP32_BLE_STATE state);
void ESP32_AppRegisterCallback(ESP32_AP_CALLBACK cb);
void ESP32_UartRxIdleHandler(void);
ESP32_WIFI_STATE ESP32_GetWifiState(void);
ESP32_BLE_STATE ESP32_GetBleState(void);
ESP32_MAC_ADDR ESP32_GetBleMac(void);
ESP32_AT_VERSION ESP32_GetATVersion(void);
ESP32_SDK_VERSION ESP32_GetSDKVersion(void);
ESP32_MAC_ADDR ESP32_GetWifiMac(void);
ESP32_IP_ADDR ESP32_GetWifiIP(void);
ESP32_CONN_AP_INFO * ESP32_GetAPInfo(void);
ESP32_SS_LEVEL ESP32_GetAPSS(void);
ESP32_STATUS ESP32_RequestSntpTime(TIMEZONE timezone);
ESP32_STATUS ESP32_RequestSntpWithCB(TIMEZONE timezone, SNTP_TIME_CB cb);
#ifdef ESP32_AP_LIST_SUPPORT
ESP32_STATUS ESP32_RequestAPList(void);
#endif


//WIFI/BT Button Input Handler
void ESP32_InputHandler(uint16_t nGPIO);

#endif

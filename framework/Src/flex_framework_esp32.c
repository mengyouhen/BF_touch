/******************************************************************************
 Copyright (c) 2018 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		flex_framework_esp32.c
 [Date]:
 		2018-06-26
 [Author]:
 		Tim Lin
 [Reversion History]:
 		v1.0
*******************************************************************************/
/*========================================================================================================
										I N C L U D E
========================================================================================================*/
#include <stdio.h>
#include <string.h>
#include "usart.h"
#include "flex_app_nvparam.h"
#include "flex_framework_esp32.h"
#include "flex_framework_privtocol.h"

/*========================================================================================================
										D E F I N E
========================================================================================================*/
#define NEW_LINE		"\r\n"

#define ESP32_RX_BUFFERSIZE		1024
#define ESP32_TX_TIMEOUT		(1000)
#define ESP32_CMD_RSP_TIMEOUT	(4000)
#define ESPE2_TX_MSG_Q_SIZE		10
#define ESPE2_MAX_DATA_LEN		128

//debounce time in ms
#define DEBOUNCE_TIME	(50)
//wifi button hold time in ms to activate smart configuration
#define SMARTCONFIG_ACTIVATION_TIME	3000

//advertising payload 0~31 bytes in hex
#define BLE_ADV_DATA_MAX_LEN	64

//WIFI AP poll interval in second
#define WIFI_AP_POLL_INTERVAL	10

//WIFI SIGNAL STRENGTH LEVEL
// HIGH:RSSI > WIFI_SS_HIGH_THRESHOLD
#define WIFI_SS_HIGH_THRESHOLD		((ESP32_AP_RSSI)-70)
//MIDDLE: [WIFI_SS_MIDDLE_THRESHOLD,WIFI_SS_HIGH_THRESHOLD]
#define WIFI_SS_MIDDLE_THRESHOLD	((ESP32_AP_RSSI)-80)


//rx task event notification bit field
#define RX_IDLE_EVENT				_BIT0_
#define RX_BUFFER_FULL_EVENT		_BIT1_

//timezone is inverted in ESP32 AT verson 1.0,e.g Beijing is -8 instead of +8
#define ESP32_TIMEZONE_INVERT

/*========================================================================================================
										E X T E R N
========================================================================================================*/
extern DMA_HandleTypeDef hdma_usart1_rx;

/*========================================================================================================
										T Y P E D E F
========================================================================================================*/

typedef struct
{
	const char *message;
	bool ( * handler) (char *rx);
} ESP32_RX_MSG_HANDLER;

typedef void (*ESP32TxFunc)(void *);
typedef struct
{
	ESP32TxFunc function;
	void *parameter;//parameter
} ESP32_TX_MSG;

typedef enum
{
	ESP32_WIFI_MODE_DISABLE,			//0: Null mode, WiFi RF will be disabled
	ESP32_WIFI_MODE_STATION,			//1: Station mode
	ESP32_WIFI_MODE_SOFTAP,			//2: SoftAP mode
	ESP32_WIFI_MODE_SOFTAP_STATION,	//3: SoftAP+Station mode
} ESP32_CWMODE;

typedef enum
{
	ESP32_BLE_ROLE_OFF,			//0: off
	ESP32_BLE_ROLE_CLIENT,			//1: client role
	ESP32_BLE_ROLE_SERVER,			//2: server role
} ESP32_BLE_ROLE;


typedef struct
{
	char *str; //abbreviation of week day
	ESP32_WEEKDAY weekday;
} ESP32_WEEKDAY_MAP;

typedef struct
{
	char *str; //abbreviation of month
	ESP32_MONTH month;
} ESP32_MONTH_MAP;

typedef struct
{
	bool ready;
	ESP32_STATE txState;
	ESP32_STATUS cmdStatus;
	//error code if cmdStatus is not equal to 0
	uint32_t errorCode;
	ESP32_WIFI_STATE wifiState;
	ESP32_CWMODE cwMode;
	ESP32_AT_VERSION atVersion;
	ESP32_SDK_VERSION sdkVersion;
	//AP SSID from smartconfig to be connected
	ESP32_AP_SSID apSSID;
	//AP Passwrod from smartconfig alone with APP SSID
	ESP32_AP_PWD apPwd;
	//AP to which ESP32 station is already connected
	ESP32_CONN_AP_INFO connApInfo;
	//station mac address
	ESP32_MAC_ADDR macAddr;
	//station ip address
	ESP32_IP_ADDR ipAddr;
	//date time for SNTP
	ESP32_DATE_TIME dateTime;

	//bluetooth low energy
	ESP32_BLE_ROLE bleRole;
	ESP32_BLE_STATE bleState;
	ESP32_MAC_ADDR bleMacAddr;
} ESP32_PRIVATE_DATA;

/*========================================================================================================
						F U N C T I ON Declaration
========================================================================================================*/
static void ESP32_UartInit(void);
static void ESP32_NotifyWifiStatus(ESP32_WIFI_STATE state);
static void ESP32_SetWifiMode(void *parameter);
static  void ESP32_NotifyApp(ESP32_NOTIFICATION *notification);
static void ESP32_WifiLedTimerCallback(void const *argument);
static bool ESP32_SaveStrContent(char **dest, const char *source, uint16_t length);
static void ESP32_SetBleMode(void *parameter);
static void ESP32_NotifyBleStatus(ESP32_BLE_STATE state);
static void ESP32_BleLedTimerCallback(void const *argument);
static osStatus ESP32_TxMsgPut (osMessageQId queue_id, void *message, uint32_t millisec);

//esp32 at command rx handler
static void ESP32_RxMsgDispatcher(void);
static bool ESP32_RxCmdEcho(char *buffer);
static bool ESP32_RxReady(char *buffer);
static bool ESP32_RxCmdOk(char *buffer);
static bool ESP32_RxCmdError(char *buffer);
static bool ESP32_RxErrorCode(char *buffer);
static bool ESP32_RxCmdBusy(char *buffer);
static bool ESP32_RxWifiMode(char *buffer);
static bool ESP32_RxWifiAutoConnInfo(char *buffer);
static bool ESP32_RxAtVersion(char *buffer);
static bool ESP32_RxSdkVersion(char *buffer);
static bool ESP32_RxSmartCfgApSsid(char *buffer);
static bool ESP32_RxSmartCfgApPwd(char *buffer);
static bool ESP32_RxConnApInfo(char *buffer);
static bool ESP32_RxAPList(char *buffer);
static bool ESP32_RxMacAddr(char *buffer);
static bool ESP32_RxIpInfo(char *buffer);
static bool ESP32_RxGotIpMsg(char *buffer);
static bool ESP32_RxWifiConn(char *buffer);
static bool ESP32_RxWifiDisconn(char *buffer);
static bool ESP32_RxSmartCfgDone(char *buffer);
static bool ESP32_RxSntpTime(char *buffer);
static bool ESP32_RxBleMacAddr(char *buffer);
static bool ESP32_RxBleConn(char *buffer);
static bool ESP32_RxBleDisConn(char *buffer);
static bool ESP32_RxBleRole(char *buffer);
static void ESP32_StartBleAdvertising(void *parameter);
static void ESP32_StopBleAdvertising(void *parameter);



//esp32 at command tx function
static ESP32_STATUS ESP32_UartTxCmd(const char *atCmd);
static ESP32_STATUS ESP32_SetCmd(const char *command, const char *value);
static ESP32_STATUS ESP32_ExecuteCmd(const char *command);
static ESP32_STATUS ESP32_QueryCmd(const char *command);
static void ESP32_EnableWifiAutoConn(void);
static void ESP32_TryToConnectAP(void *parameter);
static void ESP32_QueryIP(void *parameter);
static void ESP32_QueryConnAP(void *parameter);
static void ESP32_StartSmartConfig(void *parameter);
static void ESP32_StopSmartConfig(void *parameter);
static void ESP32_SmartConfigDone(void *parameter);
static void ESP32_QuerySntpTime(void *parameter);
static ESP32_BLE_STATE GetBleStateFromNvram(void);
static void SetBleStateToNvram(ESP32_BLE_STATE state);
static uint16_t GetApSsidFromNvram(char *buffer, uint16_t length);
static uint16_t SetApSsidToNvram(char *buffer, uint16_t length);
static uint16_t GetApPwdFromNvram(char *buffer, uint16_t length);
static uint16_t SetApPwdToNvram(char *buffer, uint16_t length);


static void SetApNameSpecial(char *buffer);
/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
static osThreadId rxTaskHandle;
static osThreadId txTaskHandle;
static bool atCmdTxDone;
static osTimerId pollTimer;
static osTimerId wifiLedTimer;
static osTimerId  btLedTimer;
static osMessageQId txMsgQId;
static osSemaphoreId cmdRspSemaphore;
static ESP32_PRIVATE_DATA esp32Data;
static ESP32_AP_CALLBACK esp32AppCallback = NULL;
static uint8_t rxBuffer[ESP32_RX_BUFFERSIZE];
//AP connection status poll inteval in second
static uint16_t wifiApPollCount = 0;

static ESP32_AP_INFO_CHAIN *apListHead = NULL;
//connected AP signal strength level
static ESP32_SS_LEVEL connApSSLevel = ESP32_SS_HIGH;

//bluetooth advertising prefix name, it is contenated by bt mac address
static const char esp32BleAdvNamePrefix[] = "Butterfly";
static const char esp32BleName[] = "\"Butterfly\"";

/*!
*\brief  conversion table for weekday abbreviaton string to digital weekday
*/
static const ESP32_WEEKDAY_MAP weekDayMapTable[] =
{
	{"Mon", ESP32_WEEK_MONDAY},
	{"Tue", ESP32_WEEK_TUESDAY},
	{"Wed", ESP32_WEEK_WEDNESDAY},
	{"Thu", ESP32_WEEK_THURSDAY},
	{"Fri", ESP32_WEEK_FRIDAY},
	{"Sat", ESP32_WEEK_SATURDAY},
	{"Sun", ESP32_WEEK_SUNDAY},
};

/*!
*\brief  conversion table for month abbreviaton string to digital month
*/
static const ESP32_MONTH_MAP monthMapTable[] =
{
	{"Jan", ESP32_MONTH_JANUARY},
	{"Feb", ESP32_MONTH_FEBURARY},
	{"Mar", ESP32_MONTH_MARCH},
	{"Apr", ESP32_MONTH_APRIL},
	{"May", ESP32_MONTH_MAY},
	{"Jun", ESP32_MONTH_JUNE},
	{"Jul", ESP32_MONTH_JULY},
	{"Aug", ESP32_MONTH_AUGUST},
	{"Sep", ESP32_MONTH_SETEMPBER},
	{"Oct", ESP32_MONTH_OCTOBER},
	{"Nov", ESP32_MONTH_NOVEMBER},
	{"Dec", ESP32_MONTH_DECEMBER},
};


/*!
*\brief  UART RX message handler table for messages with prefix '+'
*/
static  ESP32_RX_MSG_HANDLER const rxMsgHandlerTbl1[] =
{
	{"+CWMODE:", ESP32_RxWifiMode},
	{"+CWAUTOCONN:", ESP32_RxWifiAutoConnInfo},
	{"+CIPSTAMAC:", ESP32_RxMacAddr},
	{"+CIFSR:STAIP", ESP32_RxIpInfo},
	{"+CIPSNTPTIME:", ESP32_RxSntpTime},
	{"+CWJAP:", ESP32_RxConnApInfo},
	{"+BLEADDR:", ESP32_RxBleMacAddr},
	{"+BLECONN:", ESP32_RxBleConn},
	{"+BLEDISCONN:", ESP32_RxBleDisConn},
	{"+BLEINIT:", ESP32_RxBleRole},
	{"+CWLAP:", ESP32_RxAPList},
};


/*!
*\brief  UART RX message handler table for other messages
*/
static  ESP32_RX_MSG_HANDLER const rxMsgHandlerTbl2[] =
{
	{"AT+", ESP32_RxCmdEcho},
	{"OK", ESP32_RxCmdOk},
	{"WIFI CONNECTED", ESP32_RxWifiConn},
	{"WIFI GOT IP", ESP32_RxGotIpMsg},
	{"WIFI DISCONNECT", ESP32_RxWifiDisconn},
	{"AT version:", ESP32_RxAtVersion},
	{"SDK version:", ESP32_RxSdkVersion},
	{"ssid:", ESP32_RxSmartCfgApSsid},
	{"password:", ESP32_RxSmartCfgApPwd},
	{"smartconfig connected", ESP32_RxSmartCfgDone},
	{"ready", ESP32_RxReady},
	{"busy p...", ESP32_RxCmdBusy},
	{"ERR CODE", ESP32_RxErrorCode},
	{"ERROR", ESP32_RxCmdError},
};


/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
/**
  * @brief Set Ap Name Special charactor.
  * @param Ap name.
  * @retval None
  */
static void SetApNameSpecial(char *buffer)
{
	char s[3] = {0xE2,0x80};
	uint8_t len = strlen(buffer);
	
	for(uint8_t i=0;i<len;i++)	
	{
		//DBG_MSG("esp32Data.connApInfo.ssid:%x\n", esp32Data.connApInfo.ssid[i]);		
		if((buffer[i]==s[0]) && (buffer[i+1]==s[1]))
		{
			esp32Data.connApInfo.ssid[i] = '\'';
			strncpy(&buffer[i+1],&buffer[i+3],len - i);
			break;
		}			
	}	
}
/**
  * @brief Tx Transfer completed callback.
  * @param huart UART handle.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(huart);
	atCmdTxDone = true;
}


/**
  * @brief Rx Transfer completed callback.
  * @param huart UART handle.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart1)
	{
		if (rxTaskHandle )
		{
			osSignalSet(rxTaskHandle, RX_BUFFER_FULL_EVENT);
		}
	}
	else if (huart == &huart2)
	{
		Privtocol_UartRxCpltHandler(huart);
	}
}

/*!
*\brief  UART RX line idle interrupt handler
*/
void ESP32_UartRxIdleHandler(void)
{
	__HAL_UART_CLEAR_IDLEFLAG(&huart1);

	if (rxTaskHandle)
	{
		uint16_t size =  ESP32_RX_BUFFERSIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);

		if (size)
		{
			osSignalSet(rxTaskHandle, RX_IDLE_EVENT);
		}
	}
}

/*!
*\brief  UART received message dispatcher
*/
static void ESP32_RxMsgDispatcher(void)
{
	uint16_t i;
	char *start;
	char *end;
	uint16_t remain;
	uint16_t rxSize;
	uint16_t items;
	const ESP32_RX_MSG_HANDLER *handlerPtr;

	//stop UART RX DMA
	HAL_UART_DMAStop(&huart1);
	//recieved size
	rxSize =  ESP32_RX_BUFFERSIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);

	if (ESP32_RX_BUFFERSIZE <= rxSize)
	{
		DBG_MSG("Warning:ESP32 UART DMA Rx Buffer Full:%d\n", ESP32_RX_BUFFERSIZE);
		//Todo: need to handle DMA Rx buffer full situation further
	}

	start = (char *)rxBuffer;

	do
	{
		//
		//Process message line by line (line with "\r\n")
		//

		//find new line by searching "\r\n"
		end = strstr(start, NEW_LINE);

		if (end)
		{
			//replace with terminator '\0' to construct a string
			*end = '\0';

			//DBG_MSG("%s\r\n", start);

			//skip blank line ("\r\n")
			if (strlen(start))
			{

				if ('+' == start[0])
				{
					//handlers for message with prefix '+'
					handlerPtr = rxMsgHandlerTbl1;
					items = sizeof(rxMsgHandlerTbl1) / sizeof(ESP32_RX_MSG_HANDLER);
				}
				else
				{
					handlerPtr = rxMsgHandlerTbl2;
					items = sizeof(rxMsgHandlerTbl2) / sizeof(ESP32_RX_MSG_HANDLER);
				}

				//walk through table to search message handler
				for ( i = 0 ; i < items; i++)
				{
					if (strstr((const char * )start, handlerPtr[i].message))
					{
						if (handlerPtr[i].handler((char *)start))
						{
							//message has been proceeded, move forward
							break;
						}
					}
				}
			}

			//move to next line
			start = end + strlen(NEW_LINE);
		}
	}
	while (end);


	/*Tim.Lin: copy remaining characters to buffer head.for instance:
	   RX: WIFI CONNECTED\r\nWIFI GOT -- rx line idle event
	   RX: IP\r\n - rx line idle event
	   After proceeding "WIFI CONNECTED\r\n", WIFI GOT shall be kept instead of discard,
	   move "WIFI GOT" to start of rx buffer and DMA Rx from end of "WIFI GOT"
	*/

	//check remaining characters
	remain = strlen(start);

	if (remain)
	{
		//skip copy if start is in begining of rx buffer
		if (start != (char *)rxBuffer)
		{
			strncpy((char *)rxBuffer, start, remain);
		}
	}

	//check if clear is needed
	if (rxSize - remain)
	{
		pf_memset(&rxBuffer[remain], 0, rxSize - remain);
	}

	//start UART recived DMA
	HAL_UART_Receive_DMA(&huart1, (uint8_t *)&rxBuffer[remain], ESP32_RX_BUFFERSIZE - remain);
}


/*!
*\brief  UART initialization for ESP32 AT communication
*/
static void ESP32_UartInit(void)
{
	//UART1 has been intialized outside as 115200, 8,0, CTS & RTS enabled, RX DMA
	//MX_USART1_UART_Init();

	//Enable UART RX DMA
	pf_memset(rxBuffer, 0, ESP32_RX_BUFFERSIZE);
	HAL_UART_Receive_DMA(&huart1, rxBuffer, ESP32_RX_BUFFERSIZE);

	//enable RX line idle interrupt
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
}

/*!
*\brief  transimit AT command  on UART
*\param [in] atCmd -at command string
*
*\retval ESP32_STATUS_OK - command successfully
*\retval ESP32_STATUS_ERROR - command failed
*\retval ESP32_STATUS_TIMEOUT - UART busy or no command response is received
*/
static ESP32_STATUS ESP32_UartTxCmd(const char *atCmd)
{
	ESP32_STATUS status = ESP32_STATUS_OK ;
	HAL_StatusTypeDef halStatus;
	int32_t semaphoreStatus;
	uint8_t retry = 1;

	while (1)
	{
		//gain semaphore
		semaphoreStatus =  osSemaphoreWait(cmdRspSemaphore, 100);

		if (osOK != semaphoreStatus)
		{
			DBG_MSG("%s:failed to get semaphore %x\n", __FUNCTION__, semaphoreStatus);
			return ESP32_STATUS_TIMEOUT;
		}

		//DBG_MSG("sending %s\n", atCmd);
		atCmdTxDone = false;
		halStatus = HAL_UART_Transmit_IT(&huart1, (uint8_t *)atCmd, strlen(atCmd));

		if (HAL_OK != halStatus)
		{
			DBG_MSG("%s:failed to send %s", __FUNCTION__, atCmd);
			status = ESP32_STATUS_TIMEOUT;
		}
		else
		{
			//wait for sempahore to be released in received handler
			semaphoreStatus =  osSemaphoreWait(cmdRspSemaphore, ESP32_CMD_RSP_TIMEOUT);

			if (osOK != semaphoreStatus)
			{
				if (!atCmdTxDone)
				{
					DBG_MSG("%s:failed to send %s", __FUNCTION__, atCmd);
				}
				else
				{
					DBG_MSG("%s:rsp timeout:%s", __FUNCTION__, atCmd);
				}

				if ((&huart1)->TxXferCount != 0)
				{
					//abort ongoing UART transimit
					HAL_UART_AbortTransmit_IT(&huart1);
				}

				status = ESP32_STATUS_TIMEOUT;
			}
			else
			{
				status = esp32Data.cmdStatus;
			}
		}

		//release semphore whatever
		osSemaphoreRelease(cmdRspSemaphore);

		if ((ESP32_STATUS_OK != status) && (retry--))
		{
			osDelay(2000);
			//retry
			continue;
		}

		break;
	}

	return status;
}

/*!
*\brief  send AT set command
*\param [in] command -command
*
*\retval ESP32_STATUS_OK - command successfully
*\retval ESP32_STATUS_ERROR - command failed
*\retval ESP32_STATE_BUSY - busy, previous command is undergoing
*/
static ESP32_STATUS ESP32_SetCmd(const char *command, const char *value)
{
	ESP32_STATUS status = ESP32_STATUS_OK;
	char *cmdBuffer;
	uint16_t cmdLen;

	if (ESP32_STATE_IDLE == esp32Data.txState)
	{
		esp32Data.txState = ESP32_STATE_BUSY;
		cmdLen = strlen(command) + strlen(value);
		//extra length of "AT+", "=","\r\n", '\0'
		cmdLen = cmdLen + 8;

		//plus 1 for terminating null character  '\0'
		cmdBuffer = (char *)pf_malloc(cmdLen);

		if (!cmdBuffer)
		{
			esp32Data.txState = ESP32_STATE_IDLE;
			return ESP32_STATUS_ERROR;
		}

		snprintf(cmdBuffer, cmdLen, "AT+%s=%s%s", command, value, NEW_LINE);
		//DBG_MSG("%s\n",cmdBuffer);

		status = ESP32_UartTxCmd((const char *)cmdBuffer);

		pf_free(cmdBuffer);
		esp32Data.txState = ESP32_STATE_IDLE;
	}
	else
	{
		status = ESP32_STATUS_BUSY;
	}

	return status;

}

/*!
*\brief  send AT execute command
*\param [in] command -command
*
*\retval ESP32_STATUS_OK - command successfully
*\retval ESP32_STATUS_ERROR - command failed
*\retval ESP32_STATE_BUSY - busy, previous command is undergoing
*/
static ESP32_STATUS ESP32_ExecuteCmd(const char *command)
{
	ESP32_STATUS status = ESP32_STATUS_OK;
	char *cmdBuffer;
	uint16_t cmdLen;

	if (ESP32_STATE_IDLE == esp32Data.txState)
	{
		esp32Data.txState = ESP32_STATE_BUSY;
		cmdLen = strlen(command);
		//extra length of "AT+","\r\n", '\0'
		cmdLen = cmdLen + 8;

		//plus 1 for terminating null character  '\0'
		cmdBuffer = (char *)pf_malloc(cmdLen);

		if (!cmdBuffer)
		{
			esp32Data.txState = ESP32_STATE_IDLE;
			return ESP32_STATUS_ERROR;
		}

		if (0 == strlen(command))
		{
			//blank command
			snprintf(cmdBuffer, cmdLen, "AT"  NEW_LINE);
		}
		else
		{
			snprintf(cmdBuffer, cmdLen, "AT+%s%s", command, NEW_LINE);
		}

		//DBG_MSG("%s\n",cmdBuffer);

		status = ESP32_UartTxCmd((const char *)cmdBuffer);

		pf_free(cmdBuffer);
		esp32Data.txState = ESP32_STATE_IDLE;
	}
	else
	{
		status = ESP32_STATUS_BUSY;
	}

	return status;

}

/*!
*\brief  send AT query command
*\param [in] command -command
*
*\retval ESP32_STATUS_OK - command successfully
*\retval ESP32_STATUS_ERROR - command failed
*\retval ESP32_STATE_BUSY - busy, previous command is undergoing
*/
static ESP32_STATUS ESP32_QueryCmd(const char *command)
{
	ESP32_STATUS status = ESP32_STATUS_OK;
	char *cmdBuffer;
	uint16_t cmdLen;

	if (ESP32_STATE_IDLE == esp32Data. txState)
	{
		esp32Data.txState = ESP32_STATE_BUSY;
		cmdLen = strlen(command);
		//extra length of "AT+","?","\r\n", '\0'
		cmdLen = cmdLen + 8;

		//plus 1 for terminating null character  '\0'
		cmdBuffer = (char *)pf_malloc(cmdLen);

		if (!cmdBuffer)
		{
			esp32Data.txState = ESP32_STATE_IDLE;
			return ESP32_STATUS_ERROR;
		}

		snprintf(cmdBuffer, cmdLen, "AT+%s?%s", command, NEW_LINE);

		//DBG_MSG("%s\n",cmdBuffer);

		status = ESP32_UartTxCmd((const char *)cmdBuffer);

		pf_free(cmdBuffer);
		esp32Data.txState = ESP32_STATE_IDLE;
	}
	else
	{
		status = ESP32_STATUS_BUSY;
	}

	return status;

}


#if 0
static bool ESP32_RxDummy(char *buffer)
{
	DBG_MSG("ESP32 rx:%s\r\n", buffer);
	return true;
}
#endif

/*!
*\brief  handler for "AT+" echo message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxCmdEcho(char *buffer)
{
	UNUSED(buffer);
	return true;
}


/*!
*\brief  handler for "ready" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxReady(char *buffer)
{
	UNUSED(buffer);
	esp32Data.ready = true;
	return true;
}

/*!
*\brief  handler for "OK" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxCmdOk(char *buffer)
{
	UNUSED(buffer);
	esp32Data.cmdStatus = ESP32_STATUS_OK;
	osSemaphoreRelease(cmdRspSemaphore);
	return true;
}

/*!
*\brief  handler for "ERROR" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/

static bool ESP32_RxCmdError(char *buffer)
{
	UNUSED(buffer);
	esp32Data.cmdStatus = ESP32_STATUS_ERROR;
	osSemaphoreRelease(cmdRspSemaphore);
	return true;
}

/*!
*\brief  handler for "ERR CODE" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/

static bool ESP32_RxErrorCode(char *buffer)
{
	//e.g. ERR CODE:0x01090000
	uint32_t value;
	char *p = strchr(buffer, ':');

	if (p)
	{
		p++;

		if (atoulEx(p, &value, 16))
		{
			esp32Data.errorCode = value;
		}
	}

	return true;
}

/*!
*\brief  handler for "busy p..." message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxCmdBusy(char *buffer)
{
	UNUSED(buffer);
	esp32Data.cmdStatus = ESP32_STATUS_BUSY;
	osSemaphoreRelease(cmdRspSemaphore);
	return true;
}

/*!
*\brief  handler for "+CWMODE:" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxWifiMode(char *buffer)
{
	uint32_t value;
	char *p = strchr(buffer, ':');

	if (p)
	{
		if (!atoulEx(++p, &value, 0))
		{
			DBG_MSG("Wrong CWMODE:%s\n", p);
		}
		else
		{
			esp32Data.cwMode = (ESP32_CWMODE)value;

			if (ESP32_WIFI_MODE_DISABLE == esp32Data.cwMode)
			{
				ESP32_SetWifiState(ESP32_WIFI_OFF);
			}
			else
			{
				if (ESP32_WIFI_OFF == esp32Data.wifiState)
				{
					ESP32_SetWifiState(ESP32_WIFI_DISCONN);
				}
			}
		}
	}

	return true;
}

/*!
*\brief  handler for "+CWAUTOCONN:" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxWifiAutoConnInfo(char *buffer)
{
	uint32_t value;
	//+CWAUTOCONN:1
	char *p = strchr(buffer, ':');

	if (p)
	{
		if (!atoulEx(++p, &value, 0))
		{
			DBG_MSG("Wrong CWAUTOCONN:%s\n", p);
		}
		else
		{
			//enable auto connect if disabled
			if (0 == value)
			{
				osMessagePut(txMsgQId, (uint32_t)ESP32_EnableWifiAutoConn, 100);
			}
		}
	}

	return true;
}

/*!
*\brief  free previous memory if present, allocate new memory and save string content.
*\param [in] dest -destination
*\param [in] source -source
*\param [in] length -maximum length
*\retval true - successfully
*\retval false -failed
*/
static bool ESP32_SaveStrContent(char **dest, const char *source, uint16_t length)
{
	//in case somthing wrong
	length = FLEX_MIN(length, ESPE2_MAX_DATA_LEN);

	if (*dest)
	{
		//release prevous allocated buffer
		pf_free(*dest);
	}

	//allocate memory to string content
	*dest = (char *)pf_malloc(length);

	if (*dest)
	{
		strncpy(*dest, source, length - 1);
		//for circumstance of string length > ESPE2_MAX_DATA_LEN
		(*dest)[length - 1] = '\0';
		return true;
	}
	else
	{
		return false;
	}
}

/*!
*\brief  handler for "AT version:" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxAtVersion(char *buffer)
{
	uint16_t length;

	//e.g. AT version:1.0.0.0(870285c - Nov 14 2017 07:26:38)
	char *p = strchr(buffer, ':');

	if (p)
	{
		//walk over ':'
		p++;
		//plus null terminator
		length = strlen(p) + 1;

		if (ESP32_SaveStrContent(&esp32Data.atVersion, p, length))
		{
			//DBG_MSG("at ver:%s\n", esp32Data.atVersion);
		}
	}

	return true;

}


/*!
*\brief  handler for "SDK version:" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxSdkVersion(char *buffer)
{
	uint16_t length;
	char *p = strchr(buffer, ':');

	if (p)
	{
		//walk over ':'
		p++;
		//plus null terminator
		length = strlen(p) + 1;

		if (ESP32_SaveStrContent(&esp32Data.sdkVersion, p, length))
		{
			//DBG_MSG("sdk ver:%s\n", esp32Data.sdkVersion);
		}
	}

	return true;
}


/*!
*\brief  handler for "ssid:" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxSmartCfgApSsid(char *buffer)
{
	uint16_t length;
	char *p = strchr(buffer, ':');

	if (p)
	{
		//walk over ':'
		p++;
		//plus null terminator
		length = strlen(p) + 1;

		if (ESP32_SaveStrContent(&esp32Data.apSSID, p, length))
		{
			//DBG_MSG("AP SSID:%s\n", esp32Data.apSSID);
		}
	}

	return true;
}

/*!
*\brief  handler for "password:" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxSmartCfgApPwd(char *buffer)
{
	uint16_t length;
	char *p = strchr(buffer, ':');

	if (p)
	{
		//walk over ':'
		p++;
		//plus null terminator
		length = strlen(p) + 1;

		if (ESP32_SaveStrContent(&esp32Data.apPwd, p, length))
		{
			//DBG_MSG("AP password:%s\n", esp32Data.apPwd);
		}
	}

	return true;
}

/*!
*\brief  handler for "+CWJAP:" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxConnApInfo(char *buffer)
{
	//+CWJAP:<ssid>,<bssid>,<channel>,<rssi>
	//e.g. +CWJAP:"ssid","d4:ee:07:09:37:de",3,-58

	int32_t rssi;
	uint16_t length;

	char *p = strchr(buffer, '\"');

	if (p)
	{
		//walk over first "
		p++;
		char *p2 = strchr(p, '\"');

		if (p2)
		{
			//plus null terminator
			length = p2 - p + 1;

			if (ESP32_SaveStrContent(&esp32Data.connApInfo.ssid, p, length))
			{
				SetApNameSpecial(esp32Data.connApInfo.ssid);				
				//DBG_MSG("Connected AP:%s\n", esp32Data.connApInfo.ssid);
			}

			//point to start of mac address
			p = p2 + 3;
			//12 bytes mac + 5 ':' + '\0'
			length = 18;

			if (ESP32_SaveStrContent(&esp32Data.connApInfo.macAddr, p, length))
			{
				//DBG_MSG("Connected AP mac:%s\n",esp32Data.connApInfo.macAddr);
			}

			//locate rssi in last
			do
			{
				p = p2 + 1;
				p2 =  strchr(p, ',');
			}
			while (p2 != NULL);

			if (atolEx(p, &rssi, 0))
			{
				esp32Data.connApInfo.rssi = (ESP32_AP_RSSI)rssi;
			}
		}
	}

	return true;
}


/*!
*\brief  handler for "+CIFSR:STAMAC" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxMacAddr(char *buffer)
{
	//e.g. +CIFSR:STAMAC,"24:0a:c4:85:a7:78"
	uint16_t length;
	char *p = strchr(buffer, '\"');

	if (p)
	{
		//walk over '\"'
		p++;

		//location for last '\'" for null terminator
		length = strlen(p);

		if (ESP32_SaveStrContent(&esp32Data.macAddr, p, length))
		{
			//DBG_MSG("mac addr:%s\n", esp32Data.macAddr);
		}
	}

	return true;
}

/*!
*\brief  handler for "+CIFSR:STAIP" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxIpInfo(char *buffer)
{
	//e.g. +CIFSR:STAIP,"192.168.0.172"
	uint16_t length;
	char *p = strchr(buffer, '\"');

	if (p)
	{
		//walk over '\"'
		p++;
		//location for last '\'" for null terminator
		length = strlen(p);

		if (ESP32_SaveStrContent(&esp32Data.ipAddr, p, length))
		{
			//DBG_MSG("ip addr:%s\n", esp32Data.ipAddr);
		}
	}

	return true;
}

/*!
*\brief  handler for "WIFI GOT IP" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxGotIpMsg(char *buffer)
{
	//request to query ip
	ESP32_TX_MSG txMsg;
	txMsg.function = ESP32_QueryIP;
	txMsg.parameter = NULL;
	ESP32_TxMsgPut(txMsgQId, &txMsg, 0);
	return true;
}

/*!
*\brief  handler for "WIFI CONNECT" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxWifiConn(char *buffer)
{
	//request to query AP info
	ESP32_TX_MSG txMsg;
	txMsg.function = ESP32_QueryConnAP;
	//delay some time in milliseconds before issue out AT+CWJAP?
	txMsg.parameter = (void *)2000;
	ESP32_TxMsgPut(txMsgQId, &txMsg, 100);
	return true;
}

/*!
*\brief  handler for "WIFI DISCONNECT" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxWifiDisconn(char *buffer)
{
	//release stored AP information and wifi IP address
	if (esp32Data.ipAddr)
	{
		pf_free(esp32Data.ipAddr);
		esp32Data.ipAddr = NULL;
	}

	if (esp32Data.connApInfo.ssid)
	{
		pf_free(esp32Data.connApInfo.ssid);
		esp32Data.connApInfo.ssid = NULL;
	}

	if (esp32Data.connApInfo.macAddr)
	{
		pf_free(esp32Data.connApInfo.macAddr);
		esp32Data.connApInfo.macAddr = NULL;
	}

	ESP32_SetWifiState(ESP32_WIFI_DISCONN);

	return true;
}

/*!
*\brief  handler for "smartconfig connected" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxSmartCfgDone(char *buffer)
{
	ESP32_TX_MSG txMsg;
	txMsg.function = ESP32_SmartConfigDone;
	txMsg.parameter = NULL;
	ESP32_TxMsgPut(txMsgQId, &txMsg, 0);
	return true;
}

/*!
*\brief  translate SNTP calendar string to date time
*\param [in] calendar -calendar string, e.g. Thu Jun 28 03:52:18 2018
*\param [in] dateTime -date time pointer
*\retval true - successfully
 *\retval false - failed
 */
static bool ESP32_TranslateSntpTime(char *calendar, ESP32_DATE_TIME *dateTime)
{
	uint8_t i;
	uint16_t length;
	char *p;
	char *p2;
	uint32_t value;

	//e.g. Calendar string:Thu Jun  28 03:52:18 2018
	p = calendar;

	//search weekday abbreviation
	p2 = strchr(p, ' ');

	if (!p)
	{
		goto ERROR;
	}

	*p2 = '\0';

	length = sizeof(weekDayMapTable) / sizeof(ESP32_WEEKDAY_MAP);

	for (i = 0; i < length; i++)
	{
		if (strstr(weekDayMapTable[i].str, p))
		{
			dateTime->weekday = weekDayMapTable[i].weekday;
			break;
		}
	}

	if (i == length)
	{
		goto ERROR;
	}

	//find next non space character
	p = strnchr(++p2, ' ');

	if (!p)
	{
		goto ERROR;
	}

	p2 = strchr(p, ' ');

	if (!p2)
	{
		goto ERROR;
	}

	*p2 = '\0';

	length = sizeof(monthMapTable) / sizeof(ESP32_WEEKDAY_MAP);

	for (i = 0; i < length; i++)
	{
		if (strstr(monthMapTable[i].str, p))
		{
			dateTime->month = monthMapTable[i].month;
			break;
		}
	}

	if (i == length)
	{
		goto ERROR;
	}

	//find next non space character
	p = strnchr(++p2, ' ');

	if (!p)
	{
		goto ERROR;
	}

	p2 = strchr(p, ' ');

	if (!p2)
	{
		goto ERROR;
	}

	*p2 = '\0';

	if (!atoulEx(p, &value, 10))
	{
		goto ERROR;
	}

	dateTime->day = (uint8_t)value;

	//get hour
	//find next non space character
	p = strnchr(++p2, ' ');

	if (!p)
	{
		goto ERROR;
	}

	//find next non '0' character
	p = strnchr(p, '0');

	if (!p)
	{
		goto ERROR;
	}

	p2 = strchr(p, ':');

	if (!p2)
	{
		goto ERROR;
	}

	*p2 = '\0';

	if (!atoulEx(p, &value, 0))
	{
		goto ERROR;
	}

	dateTime->hour = (uint8_t)value;

	//get minute
	//find next non '0' character
	p = strnchr(++p2, '0');

	if (!p)
	{
		goto ERROR;
	}

	p2 = strchr(p, ':');

	if (!p2)
	{
		goto ERROR;
	}

	*p2 = '\0';

	if (!atoulEx(p, &value, 10))
	{
		goto ERROR;
	}

	dateTime->minute = (uint8_t)value;

	//get second
	//find next non '0' character
	p = strnchr(++p2, '0');

	if (!p)
	{
		goto ERROR;
	}

	p2 = strchr(p, ' ');

	if (!p2)
	{
		goto ERROR;
	}

	*p2 = '\0';

	if (!atoulEx(p, &value, 10))
	{
		goto ERROR;
	}

	dateTime->second = (uint8_t)value;

	//find next non space character
	p = strnchr(++p2, ' ');

	if (!p)
	{
		goto ERROR;
	}

	//get year
	if (!atoulEx(p, &value, 10))
	{
		goto ERROR;
	}

	dateTime->year = (uint16_t)value;
	return true;
ERROR:
	return false;
}

/*!
*\brief  handler for "+CIPSNTPTIME:" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxSntpTime(char *buffer)
{
	//e.g. +CIPSNTPTIME:Thu Jun 28 03:52:18 2018

	/* Calendar string:
	0:00:00 Coordinated Universal Time (UTC), January 1, 1970.
	a 24-hour clock format.
	The days are abbreviated to: Sun, Mon, Tue, Wed, Thu, Fri, and Sat.
	The months are abbreviated to: Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, and Dec.
	*/

	char *p;
	uint16_t length;
	ESP32_DATE_TIME	dateTime;
	ESP32_NOTIFICATION notification;

	//
	//in ESP32 Wroom32 AT version:1.0.0.0(870285c - Nov 14 2017 07:26:38),there is an issue that '\n' (0x0a) is in the end of calendar string,
	//e.g. +CIPSNTPTIME:Sun Jul  1 09:51:12 2018,the data is following:
	//2b 43 49 50 53 4e 54 50 54 49 4d 45 3a 53 75 6e 20 4a 75 6c 20 20 31 20 30 39 3a 35 31 3a 31 32 20 32 30 31 38 0a
	//so we need get rid of '\n' when present
	//
	length = strlen(buffer);

	if ('\n' == buffer[length - 1])
	{
		buffer[length - 1] = '\0';
	}

	p = strchr(buffer, ':');

	if (p)
	{
		//walk over ':'
		p++;

		if (ESP32_TranslateSntpTime(p, &dateTime))
		{
			//copy date time to esp32Data.dateTime
			pf_memcpy((void *)&esp32Data.dateTime, (void *)&dateTime, sizeof(ESP32_DATE_TIME));
			//notify APP
			notification.status = ESP32_STATUS_OK;
			notification.data.dateTime = &esp32Data.dateTime;
			notification.event = ESP32_EVENT_SNTP_TIME;
			ESP32_NotifyApp(&notification);
		}
	}

	return true;
}

/*!
*\brief  handler for "+BLEINIT:" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxBleRole(char *buffer)
{
	//e.g. +BLEINIT:0
	uint32_t value;
	char *p = strchr(buffer, ':');

	if (p)
	{
		if (!atoulEx(++p, &value, 0))
		{
			DBG_MSG("Wrong BLEINIT:%s\n", p);
		}
		else
		{
			esp32Data.bleRole = (ESP32_BLE_ROLE)value;
		}
	}

	return true;
}



/*!
*\brief  handler for "+BLEADDR" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxBleMacAddr(char *buffer)
{
	//e.g. +BLEADDR:24:0a:c4:85:a7:7a
	uint16_t length;
	char *p = strchr(buffer, ':');

	if (p)
	{
		//walk over '\"'
		p++;

		//plus null terminator
		length = strlen(p) + 1;

		if (ESP32_SaveStrContent(&esp32Data.bleMacAddr, p, length))
		{
			//DBG_MSG("bt mac addr:%s\n", esp32Data.bleMacAddr);
		}
	}

	return true;
}


/*!
*\brief  handler for "+BLECONN" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxBleConn(char *buffer)
{
	//+BLECONN:<conn_index>,<remote_address>
	//e.g. +BLECONN:0,"68:70:cf:e6:34:3c"
	//<conn_index>: index of BLE connection; only 0 is supported for the single connection right now.

	//DBG_MSG("%s\n", buffer);
	ESP32_SetBleState(ESP32_BLE_CONN);
	return true;
}

/*!
*\brief  handler for "+BLEDISCONN" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxBleDisConn(char *buffer)
{
	//e.g. +BLEDISCONN:0, "68:70:cf:e6:34:3c"
	UNUSED(buffer);
	ESP32_TX_MSG txMsg;

	//DBG_MSG("%s\n", buffer);
	ESP32_SetBleState(ESP32_BLE_DISCONN);
	//request to start advertising
	txMsg.function = ESP32_StartBleAdvertising;
	txMsg.parameter = NULL;
	ESP32_TxMsgPut(txMsgQId, &txMsg, 0);

	return true;
}

/*!
*\brief  handler for "+CWJAP:" message
*\param [in] buffer -message
*\retval true - handled
*\retval false - not handled
*/
static bool ESP32_RxAPList(char *buffer)
{
	//+CWLAP:<ecn>,<ssid>,<rssi>,<mac>,<channel>
	//e.g.+CWLAP:(0,"Flex-Visitor",-57,"20:a6:cd:01:f2:82",11)

	char *p;
	char *p2;
	int32_t value;
	uint16_t length;
	ESP32_AP_INFO_CHAIN *chain = NULL;
	ESP32_AP_INFO_CHAIN *list = NULL;

	chain = (ESP32_AP_INFO_CHAIN *)pf_malloc(sizeof(ESP32_AP_INFO_CHAIN));

	if (!chain)
	{
		goto ERROR;
	}

	pf_memset(&chain->apInfo, 0, sizeof(ESP32_AP_INFO));

	p = strchr(buffer, '(');

	if (!p)
	{
		goto ERROR;
	}

	//get ecn
	p++;
	p2 =  strchr(p, ',');

	if (!p2)
	{
		goto ERROR;
	}

	*p2 = '\0';

	if (atolEx(p, &value, 0))
	{
		chain->apInfo.ecn = (ESP32_ECN)value;
	}

	//get ssid
	p = p2 + 2; //move afward to \" of ssid;
	p2 = strchr(p, '\"');

	if (!p2)
	{
		goto ERROR;
	}

	*p2 = '\0';

	//plus null terminator
	length = p2 - p + 1;

	if (!ESP32_SaveStrContent(&chain->apInfo.ssid, p, length))
	{
		goto ERROR;
	}

	//get rssi
	p = p2 + 2; //move afward to rssi;
	p2 =  strchr(p, ',');

	if (!p2)
	{
		goto ERROR;
	}

	*p2 = '\0';

	if (atolEx(p, &value, 0))
	{
		chain->apInfo.rssi = (ESP32_AP_RSSI)value;
	}

	//get mac
	p = p2 + 2; //move afward to " of mac;
	p2 = strchr(p, '\"');

	if (!p2)
	{
		goto ERROR;
	}

	*p2 = '\0';

	//plus null terminator
	length = p2 - p + 1;

	if (!ESP32_SaveStrContent(&chain->apInfo.macAddr, p, length))
	{
		goto ERROR;
	}

	//get channel
	p = p2 + 2; //move afward to channel;
	p2 = strchr(p, ')');

	if (!p2)
	{
		goto ERROR;
	}

	*p2 = '\0';

	if (atolEx(p, &value, 0))
	{
		chain->apInfo.channel = (ESP32_AP_RSSI)value;
	}

	//chain next point to NULL
	chain->next = NULL;

	if (NULL == apListHead)
	{
		//first in list
		apListHead = chain;
	}
	else
	{
		//search last one in list
		list = apListHead;

		while (list->next)
		{
			list = list->next;
		}

		list->next = chain;
	}

	goto EXIT;

ERROR:

	if (chain)
	{
		//release allocated memory if present
		if (chain->apInfo.ssid)
		{
			pf_free(chain->apInfo.ssid);
		}

		if (chain->apInfo.macAddr)
		{
			pf_free(chain->apInfo.macAddr);
		}

		pf_free(chain);
	}

EXIT:

	return true;
}


/*!
*\brief release previous allocated memory resoure which restored AP list
*
*/
static void ESP32_ReleaseApListResource(void)
{
	ESP32_AP_INFO_CHAIN *chain;
	ESP32_AP_INFO_CHAIN *list;

	if (apListHead)
	{
		//release resource
		chain = apListHead;

		do
		{
			list = chain->next;

			if (chain->apInfo.ssid)
			{
				pf_free(chain->apInfo.ssid);
			}

			if (chain->apInfo.macAddr)
			{
				pf_free(chain->apInfo.macAddr);
			}

			pf_free(chain);
			chain = list;
		}
		while (chain);

		apListHead = NULL;
	}

}

#ifdef ESP32_AP_LIST_SUPPORT
/*!
*\brief query AP list
*\param [in] parameter -parameter pointer
*
*/
static void ESP32_QueryAPList(void *parameter)
{
	UNUSED(parameter);
	ESP32_NOTIFICATION notification;

	//CWLAP！list all available APs.
	if (ESP32_STATUS_OK == ESP32_ExecuteCmd("CWLAP"))
	{
		if (apListHead)
		{
			//notify APP
			notification.status = ESP32_STATUS_OK;
			notification.data.apList = apListHead;
			notification.event = ESP32_EVENT_WIFI_AP_LIST;
			ESP32_NotifyApp(&notification);

			ESP32_ReleaseApListResource();
		}

	}
}
#endif

/*!
*\brief convert RSSI to signal strength level
*\param [in] rssi -rssi value
*\retval ESP32_SS_LEVEL
*
*/
static ESP32_SS_LEVEL ESP32_ConvertRssiToLevel(ESP32_AP_RSSI rssi)
{
	ESP32_SS_LEVEL level;

	if (rssi > WIFI_SS_HIGH_THRESHOLD)
	{
		level = ESP32_SS_HIGH;
	}
	else if (rssi < WIFI_SS_MIDDLE_THRESHOLD)
	{
		level = ESP32_SS_LOW;
	}
	else
	{
		level = ESP32_SS_MIDDLE;
	}

	return level;
}

/*!
*\brief query information of AP to which ESP32 is already connected.
*\param [in] parameter -parameter pointer
*
*/
static void ESP32_QueryConnAP(void *parameter)
{
	UNUSED(parameter);
	uint32_t delayMs = (uint32_t)parameter;

	if ( delayMs)
	{
		//do some wait, or AT+CWJAP? might only return OK
		osDelay(delayMs);
	}

	//invalidate rssi value to trace if "+CWJAP:" is received
	esp32Data.connApInfo.rssi = ESP32_INVALID_RSSI;

	//AT + CWJAP?！to query the AP to which the ESP32 Station is already connected.
	if (ESP32_STATUS_OK == ESP32_QueryCmd("CWJAP"))
	{
		//check if "+CWJAP:" is received
		if (ESP32_INVALID_RSSI != esp32Data.connApInfo.rssi)
		{
			//yes, ESP32 connected to AP
			if ((ESP32_WIFI_CONN != esp32Data.wifiState) && (ESP32_WIFI_GOTIP != esp32Data.wifiState))
			{
				ESP32_SetWifiState(ESP32_WIFI_CONN);
			}

			ESP32_SS_LEVEL level = ESP32_ConvertRssiToLevel(esp32Data.connApInfo.rssi);

			if (  level != connApSSLevel)
			{
				//update connect AP signal strength levle
				connApSSLevel = level;
				//notify APP
				ESP32_NOTIFICATION notification;
				notification.status = ESP32_STATUS_OK;
				notification.data.apSSLevel = &connApSSLevel;
				notification.event = ESP32_EVENT_WIFI_SS;
				ESP32_NotifyApp(&notification);
			}
		}
		else
		{
			//no, disconnect
			ESP32_SetWifiState(ESP32_WIFI_DISCONN);
		}
	}
}

/*!
*\brief query station IP
*\param [in] parameter -parameter pointer
*
*/
static void ESP32_QueryIP(void *parameter)
{
	UNUSED(parameter);

	//CIFSR！Gets the Local IP Address
	if (ESP32_STATUS_OK == ESP32_ExecuteCmd("CIFSR"))
	{
		ESP32_SetWifiState(ESP32_WIFI_GOTIP);
	}
}

/*!
*\brief smart config process is done successfuly
*\param [in] parameter -parameter pointer
*
*/
static void ESP32_SmartConfigDone(void *parameter)
{
	UNUSED(parameter);

	ESP32_StopSmartConfig(NULL);
	//save smart config info including null terminator to nvram
	SetApSsidToNvram(esp32Data.apSSID, strlen(esp32Data.apSSID) + 1);
	SetApPwdToNvram(esp32Data.apPwd, strlen(esp32Data.apPwd) + 1);
}


/*!
*\brief start smart config process
*\param [in] parameter -parameter pointer
*
*/
static void ESP32_StartSmartConfig(void *parameter)
{
	UNUSED(parameter);
	ESP32_STATUS status;

	//AT+CWSTARTSMART！Starts SmartConfig
	status = ESP32_SetCmd("CWSTARTSMART", "1");

	if (ESP32_STATUS_OK == status)
	{
		//update wifi state
		ESP32_SetWifiState(ESP32_WIFI_SMARTCONFIG);
	}
	else
	{
		DBG_MSG("Failed to start SmartConfig\n");
	}
}

/*!
*\brief stop smart config process
*\param [in] parameter -parameter pointer
*
*/
static void ESP32_StopSmartConfig(void *parameter)
{
	UNUSED(parameter);
	ESP32_STATUS status;

	//delay some time to let EspTouch APP knows smart config status
	//osDelay(2000);

	//AT+CWSTOPSMART！Stops SmartConfig
	status = ESP32_ExecuteCmd("CWSTOPSMART");

	if (ESP32_STATUS_OK  == status )
	{
		if (esp32Data.wifiState == ESP32_WIFI_SMARTCONFIG)
		{
			//update wifi state if it is still in smart config state
			ESP32_SetWifiState(ESP32_WIFI_DISCONN);
		}
	}
	else
	{
		DBG_MSG("Failed to stop SmartConfig\n");
	}
}

/*!
*\brief Start to query SNTP timer
*\param [in] parameter -parameter pointer
*
*/
static void ESP32_QuerySntpTime(void *parameter)
{
	ESP32_STATUS status;
	char buff[6];
	SNTP_TIME_PARAMETER *para = (SNTP_TIME_PARAMETER *)parameter;

	snprintf(buff, 6, "1,%d", para->timezone);

	//ESP32_QueryCmd("CIPSNTPCFG");

	//use default  SNTP server: "cn.ntp.org.cn","ntp.sjtu.edu.cn","us.pool.ntp.org"
	status = ESP32_SetCmd("CIPSNTPCFG", buff);

	if (ESP32_STATUS_OK  == status )
	{
		//do some delay for network connection stable, or CIPSNTPTIME might get "Wed Dec 31 16:00:08 1969"
		//because of network issue
		osDelay(1000);

		status = ESP32_QueryCmd("CIPSNTPTIME");

		//disable SNTP
		snprintf(buff, 6, "0,%d", para->timezone);
		status = ESP32_SetCmd("CIPSNTPCFG", buff);
	}

	if (para->callback)
	{
		para->callback(status, &esp32Data.dateTime);
	}
}

/*!
*\brief enable ESP32 Connects to the AP automatically on power-up.
*\note auto connection will only be at ESP32 module power on.
*/
static void ESP32_EnableWifiAutoConn(void)
{
	ESP32_SetCmd("CWAUTOCONN", "1");
}

/*!
*\brief set ESP32 module to desired mode
*\param [in] parameter -parameter pointer
*
*/
static void ESP32_SetWifiMode(void *parameter)
{
	ESP32_STATUS status;
	ESP32_WIFI_STATE desireState = (ESP32_WIFI_STATE)((uint32_t)parameter);

	switch (desireState)
	{
		case ESP32_WIFI_OFF:
			if (ESP32_WIFI_SMARTCONFIG == esp32Data.wifiState)
			{
				//stop smart config if it's going on
				ESP32_StopSmartConfig(NULL);
			}
			else if ((ESP32_WIFI_GOTIP == esp32Data.wifiState) ||
					 (ESP32_WIFI_CONN == esp32Data.wifiState))
			{
				ESP32_ExecuteCmd("CWQAP");
			}

			//turn off WIFI RF
			status = ESP32_SetCmd("CWMODE", "0");

			if (ESP32_STATUS_OK == status)
			{
				esp32Data.cwMode = ESP32_WIFI_MODE_DISABLE;
				ESP32_SetWifiState(ESP32_WIFI_OFF);
			}

			break;

		case ESP32_WIFI_DISCONN:
			//set WIFI Station mode
			status = ESP32_SetCmd("CWMODE", "1");

			if (ESP32_STATUS_OK == status)
			{
				esp32Data.cwMode = ESP32_WIFI_MODE_STATION;
				ESP32_SetWifiState(ESP32_WIFI_DISCONN);
			}

			break;

		case ESP32_WIFI_SMARTCONFIG:
			if (ESP32_WIFI_MODE_DISABLE == esp32Data.cwMode)
			{
				//set WIFI Station mode
				status = ESP32_SetCmd("CWMODE", "1");

				if (ESP32_STATUS_OK == status)
				{
					esp32Data.cwMode = ESP32_WIFI_MODE_STATION;
					ESP32_SetWifiState(ESP32_WIFI_DISCONN);
					//do some delay after wifi mode change
					osDelay(1000);
				}
			}

			if (ESP32_WIFI_MODE_STATION == esp32Data.cwMode)
			{
				if (ESP32_WIFI_SMARTCONFIG == esp32Data.wifiState)
				{
					//stop smart config if it is on going
					ESP32_StopSmartConfig(NULL);
				}

				//start smart config
				ESP32_StartSmartConfig(NULL);
			}

			break;

		default:
			break;

	}
}


/*!
*\brief configure and start BLE advertising
*\param [in] parameter -parameter pointer
*
*/
static ESP32_STATUS ESP32_ConfigBleAdvData(void *parameter)
{
	UNUSED(parameter);
	uint16_t length;
	uint16_t i;
	uint16_t j;
	ESP32_STATUS status;
	char payload[BLE_ADV_DATA_MAX_LEN];
	char bleName[BLE_ADV_DATA_MAX_LEN];//for Complete Local Name


	//get BT mac address if not availabe yet
	if (!esp32Data.bleMacAddr)
	{
		status = ESP32_QueryCmd("BLEADDR");
	}

	//plus AD type (09-Complete Local Name) and "_AABBCCDD"
	length = strlen(esp32BleAdvNamePrefix) + 6;

	//
	//construct Complete Local Name: Prefix_AABBCCDD where
	//AABBCCDD is last 4 byte of BLE MAC address.
	//
	sprintf(bleName, "%02X%02X", length, 9);

	length = strlen(esp32BleAdvNamePrefix);

	for (i = 0; i < length ; i++)
	{
		j = strlen(bleName);
		sprintf(&bleName[j], "%02X", esp32BleAdvNamePrefix[i]);
	}

	j = strlen(bleName);

	if (esp32Data.bleMacAddr)
	{
		i = strlen(esp32Data.bleMacAddr);
		sprintf(&bleName[j], "%02X%02X%02X%02X%02X",
				'_',
				esp32Data.bleMacAddr[i - 5],
				esp32Data.bleMacAddr[i - 4],
				esp32Data.bleMacAddr[i - 2],
				esp32Data.bleMacAddr[i - 1]);
	}
	else
	{
		sprintf(&bleName[j], "%02X%02X%02X%02X%02X", '_', 0, 0, 0, 0);
	}

	//DBG_MSG("Ble name:%s,%d\n", bleName,strlen(bleName));

	//construct Advertising Data (AD) elements
	strcpy(&payload[0], "\"020106030302A0");

	length = BLE_ADV_DATA_MAX_LEN - strlen(payload) - 2;
	snprintf(&payload[strlen(payload)], length, "%s\"", bleName);

	//DBG_MSG("ble adv data:%s,len = %d\n", payload, strlen(payload) / 2);

	if (strlen(payload) > BLE_ADV_DATA_MAX_LEN)
	{
		DBG_MSG("Error BLE ADV data too long:%d\n", strlen(payload));
	}

	//Generic Attributes Server (GATTS) creates services
	status = ESP32_ExecuteCmd("BLEGATTSSRVCRE");
	//GATTS starts services
	status = ESP32_ExecuteCmd("BLEGATTSSRVSTART");
	//Sets BLE advertising data
	status = ESP32_SetCmd("BLEADVDATA", payload);
	//Sets BLE device's name
	status = ESP32_SetCmd("BLENAME", esp32BleName);

	return status;
}


/*!
*\brief start BLE advertising
*\param [in] parameter -parameter pointer
*
*/
static void ESP32_StartBleAdvertising(void *parameter)
{
	UNUSED(parameter);

	if (ESP32_BLE_ROLE_SERVER  == esp32Data.bleRole)
	{
		//AT+BLEADVSTART Starts Advertising
		ESP32_ExecuteCmd("BLEADVSTART");
	}
}

/*!
*\brief stop BLE advertising
*\param [in] parameter -parameter pointer
*
*/
static void ESP32_StopBleAdvertising(void *parameter)
{
	UNUSED(parameter);

	//AT+BLEADVSTOP Stops Advertising
	ESP32_ExecuteCmd("BLEADVSTOP");
}

/*!
*\brief set ESP32 bluetooth to desired mode
*\param [in] parameter -parameter pointer
*
*/
static void ESP32_SetBleMode(void *parameter)
{
	ESP32_STATUS status;
	ESP32_BLE_STATE desireState = (ESP32_BLE_STATE)((uint32_t)parameter);

	//skip if current wifi state is equivalent to desire one
	if (esp32Data.bleState != desireState)
	{
		switch (desireState)
		{
			case ESP32_BLE_OFF://turn off bt
				if (esp32Data.bleState == ESP32_BLE_DISCONN)
				{
					//stop advertising
					ESP32_StopBleAdvertising(NULL);
				}

				//GATTS stops services
				status = ESP32_ExecuteCmd("BLEGATTSSRVSTOP");

				//turn off BT RF
				status = ESP32_SetCmd("BLEINIT", "0");

				if (ESP32_STATUS_OK == status)
				{
					esp32Data.bleRole = ESP32_BLE_ROLE_OFF;
					ESP32_SetBleState(ESP32_BLE_OFF);
					//save ble state to nvram
					SetBleStateToNvram(ESP32_BLE_OFF);
				}

				break;

			case ESP32_BLE_DISCONN: //turn on BT
				//configure BT server mode
				status = ESP32_SetCmd("BLEINIT", "2");

				if (ESP32_STATUS_OK == status)
				{
					esp32Data.bleRole = ESP32_BLE_ROLE_SERVER;
					//configure Ble advertising data
					status = ESP32_ConfigBleAdvData(NULL);

					if (ESP32_STATUS_OK == status)
					{
						//start advertising
						ESP32_StartBleAdvertising(NULL);
						ESP32_SetBleState(ESP32_BLE_DISCONN);
						//save ble state to nvram
						SetBleStateToNvram(ESP32_BLE_DISCONN);
					}
				}

				break;

			default:
				break;

		}
	}
}


/*!
*\brief Initialize ESP32 module
*\param [in] parameter -parameter pointer
*
*/
static void ESP32_InitDev(void *parameter)
{
	UNUSED(parameter);
	ESP32_STATUS status;
	uint16_t count100ms;

	//try to get AP SSID & Password from nvram
	uint16_t length;
	char *ptr = (char *) pf_malloc(32);

	if (ptr)
	{
		pf_memset(ptr, 0, 32);
		length = GetApSsidFromNvram(ptr, 32);

		if (length)
		{
			esp32Data.apSSID = ptr;
			//DBG_MSG("ap ssid:%s\n", esp32Data.apSSID);
		}

		char *ptr = (char *) pf_malloc(64);

		if (ptr)
		{
			pf_memset(ptr, 0, 64);
			length = GetApPwdFromNvram(ptr, 64);

			if (length)
			{
				esp32Data.apPwd = ptr;
				//DBG_MSG("ap pwd:%s\n", esp32Data.apPwd);
			}
		}
	}

	//get last ble state from nvram
	ESP32_BLE_STATE bleState = GetBleStateFromNvram();

	count100ms = 10;

	while (count100ms--)
	{
		if (esp32Data.ready)
		{
			break;
		}

		//wait for some time to let ESP32 ready
		osDelay(100);
	}

	if (!esp32Data.ready)
	{
		//perform AT+RST if there is no ready message,e.g. under STM32 MCU reset
		status = ESP32_ExecuteCmd("RST");

		if (ESP32_STATUS_OK != status )
		{
			DBG_MSG("Error:ESP32 reset failed\n");
			return;
		}

		count100ms = 20;

		while (count100ms--)
		{
			if (esp32Data.ready)
			{
				break;
			}

			osDelay(100);
		}
	}

	//configure current UART
	status = ESP32_SetCmd("UART_CUR", "115200,8,1,0,3");

	if (ESP32_STATUS_OK != status)
	{
		DBG_MSG("Failed to set ESP32 UART mode\n");
	}

	esp32Data.ready = true;
	status = ESP32_QueryCmd("CWMODE");
	status = ESP32_ExecuteCmd("GMR");
	status = ESP32_QueryCmd("CIPSTAMAC");
	status = ESP32_QueryCmd("CWAUTOCONN");

	//configure BT server mode firstly
	esp32Data.bleRole = ESP32_BLE_ROLE_SERVER;
	status = ESP32_SetCmd("BLEINIT", "2");
	//query ble mac address
	status = ESP32_QueryCmd("BLEADDR");

	if (ESP32_BLE_OFF == bleState)
	{
		//turn ble off
		status = ESP32_SetCmd("BLEINIT", "0");
		ESP32_SetBleState(ESP32_BLE_OFF);
	}
	else
	{
		//configure Ble advertising data
		status = ESP32_ConfigBleAdvData(NULL);

		if (ESP32_STATUS_OK == status)
		{
			//start advertising
			ESP32_StartBleAdvertising(NULL);
			ESP32_SetBleState(ESP32_BLE_DISCONN);
		}
	}

	//discovery BLE init status
	status = ESP32_QueryCmd("BLEINIT");
}


/*!
*\brief Notify APP with ESP32 event
*\param [in] notification -notification pointer
*/
static  void ESP32_NotifyApp(ESP32_NOTIFICATION *notification)
{
	if (esp32AppCallback)
	{
		esp32AppCallback(notification);
	}
}

/*!
*\brief Get ESP32 WIFI state
*
*\retval ESP32_WIFI_STATE
*/
ESP32_WIFI_STATE ESP32_GetWifiState(void)
{
	return esp32Data.wifiState;
}


/*!
*\brief Get ESP32 BLE state
*
*\retval ESP32_BLE_STATE
*/
ESP32_BLE_STATE ESP32_GetBleState(void)
{
	return esp32Data.bleState;
}


/*!
*\brief Get ESP32 AT command version
*
*\retval ESP32_AT_VERSION
*/
ESP32_AT_VERSION ESP32_GetATVersion(void)
{
	return esp32Data.atVersion;
}

/*!
*\brief Get ESP32 SDK version
*
*\retval ESP32_SDK_VERSION
*/
ESP32_SDK_VERSION ESP32_GetSDKVersion(void)
{
	return esp32Data.sdkVersion;
}

/*!
*\brief Get WIFI MAC address
*
*\retval ESP32_MAC_ADDR
*/
ESP32_MAC_ADDR ESP32_GetWifiMac(void)
{
	return esp32Data.macAddr;
}

/*!
*\brief Get WIFI IP address
*
*\retval ESP32_IP_ADDR
*/
ESP32_IP_ADDR ESP32_GetWifiIP(void)
{
	return esp32Data.ipAddr;
}

/*!
*\brief Get AP information
*
*\retval ESP32_CONN_AP_INFO pointer
*/
ESP32_CONN_AP_INFO *ESP32_GetAPInfo(void)
{
	SetApNameSpecial(esp32Data.connApInfo.ssid);
	return &esp32Data.connApInfo;
}

/*!
*\brief Get connected AP signal strength level
*
*\retval ESP32_SS_LEVEL
*/
ESP32_SS_LEVEL ESP32_GetAPSS(void)
{
	return connApSSLevel;
}

/*!
*\brief Get BLE MAC address
*
*\retval ESP32_MAC_ADDR
*/
ESP32_MAC_ADDR ESP32_GetBleMac(void)
{
	return esp32Data.bleMacAddr;
}

/*!
*\brief register an APP callback for ESP32 event notification
*\param [in] callback - APP callback function
*
*/
void ESP32_AppRegisterCallback(ESP32_AP_CALLBACK callback)
{
	if (callback)
	{
		esp32AppCallback = callback;
	}
}

/*!
*\brief Request a SNTP timer synchronization with designated callback
*\param [in] timezone - timezone in range of -11 ~ 13
*
*\retval ESP32_STATUS
*\note  sntp time will be supplied by callback directly
*/
ESP32_STATUS ESP32_RequestSntpWithCB(TIMEZONE timezone, SNTP_TIME_CB cb)
{
	ESP32_STATUS status = ESP32_STATUS_OK;
	ESP32_TX_MSG txMsg;
	static SNTP_TIME_PARAMETER parameter;

	if ((timezone < -11) && (timezone > 13))
	{
		status = ESP32_STATUS_ERROR;
	}
	else
	{
		if (esp32Data.wifiState == ESP32_WIFI_GOTIP)
		{
#ifdef ESP32_TIMEZONE_INVERT
			parameter.timezone = -timezone;
#else
			parameter.timezone = timezone;
#endif
			parameter.callback = cb;

			txMsg.function = ESP32_QuerySntpTime;
			txMsg.parameter = &parameter;
			ESP32_TxMsgPut(txMsgQId, &txMsg, 0);
		}
		else
		{
			status = ESP32_STATUS_UNAVALIABLE;
		}
	}

	return status;
}

/*!
*\brief Request a SNTP timer synchronization
*\param [in] timezone - timezone in range of -11 ~ 13
*
*\retval ESP32_STATUS
*\note  sntp time will be supplied by ESP32 APP callback with ESP32_EVENT_SNTP_TIME event
*/
ESP32_STATUS ESP32_RequestSntpTime(TIMEZONE timezone)
{
	return ESP32_RequestSntpWithCB(timezone, NULL);
}

#ifdef ESP32_AP_LIST_SUPPORT
/*!
*\brief request to get AP list
*
*\retval ESP32_STATUS
*\note  sntp time will be supplied by ESP32 APP callback with ESP32_EVENT_SNTP_TIME event
*/
ESP32_STATUS ESP32_RequestAPList(void)
{
	ESP32_STATUS status = ESP32_STATUS_OK;

	if (ESP32_WIFI_OFF  != esp32Data.wifiState)
	{
		//request to query AP list
		ESP32_TX_MSG txMsg;
		txMsg.function = ESP32_QueryAPList;
		//delay some time in milliseconds before issue out AT+CWJAP?
		txMsg.parameter = NULL;
		ESP32_TxMsgPut(txMsgQId, &txMsg, 0);
	}
	else
	{
		status = ESP32_STATUS_UNAVALIABLE;
	}

	return status;
}
#endif

/*!
*\brief try to connect AP with ssid & password
*/
static void ESP32_TryToConnectAP(void *parameter)
{
	UNUSED(parameter);
	char *pBuffer;
	uint16_t length;

	//try to reconnect AP with AP SSID & Password from smart config
	if (esp32Data.apSSID)
	{
		//e.g. AT+CWJAP="abc","0123456789"
		length = strlen(esp32Data.apSSID) + strlen(esp32Data.apPwd) + 6;
		pBuffer = (char *) pf_malloc(length);

		if (pBuffer)
		{
			ESP32_STATUS status;
			snprintf(pBuffer, length, "\"%s\"", esp32Data.apSSID);
			//check if specific AP is here
			ESP32_ReleaseApListResource();
			status = ESP32_SetCmd("CWLAP", pBuffer);

			if (ESP32_STATUS_OK == status)
			{
				//there is no need to do strcmp(apListHead->apInfo.ssid,pBuffer)
				if (apListHead)
				{
					ESP32_ReleaseApListResource();
					//delay some time
					osDelay(2000);

					//AT+CWJAP="abc","0123456789"
					snprintf(pBuffer, length, "\"%s\",\"%s\"", esp32Data.apSSID, esp32Data.apPwd);
					status = ESP32_SetCmd("CWJAP", pBuffer);
				}
			}
			pf_free(pBuffer);
		}
	}
}

/*!
*\brief poll timer call back function
*\param [in] argument -argument pointer
*/
static void ESP32_PollTimerCallback(void const *argument)
{
	switch ( esp32Data.wifiState)
	{
		case ESP32_WIFI_DISCONN:
			if ((++wifiApPollCount) >= WIFI_AP_POLL_INTERVAL)
			{
				wifiApPollCount = 0;
				//request to query AP info
				ESP32_TX_MSG txMsg;
				txMsg.function = ESP32_TryToConnectAP;
				//delay some time in milliseconds before issue out AT+CWJAP?
				txMsg.parameter = NULL;
				ESP32_TxMsgPut(txMsgQId, &txMsg, 0);
			}

			break;

		case ESP32_WIFI_CONN:
		case ESP32_WIFI_GOTIP:
			if ((++wifiApPollCount) >= WIFI_AP_POLL_INTERVAL)
			{
				wifiApPollCount = 0;
				//request to query AP info
				ESP32_TX_MSG txMsg;
				txMsg.function = ESP32_QueryConnAP;
				//delay some time in milliseconds before issue out AT+CWJAP?
				txMsg.parameter = NULL;
				ESP32_TxMsgPut(txMsgQId, &txMsg, 0);
			}

			break;

		default:
			break;
	}
}

/*!
* \brief Determine whether we are in thread mode or handler mode.
* \retval true - in interrupt handler mode
* \retval false - not in interrupt handler mode
*/
static int inHandlerMode (void)
{
	return __get_IPSR() != 0;
}

/*!
* \brief Put a message to a queue.
* \param [in] queue_id  - message queue ID obtained with \ref osMessageCreate.
* \param [in] message  -  point to  message.
* \param [in] millisec -  timeout value or 0 in case of no time-out.
* \retval status code that indicates the execution status of the function.
*/
static osStatus ESP32_TxMsgPut (osMessageQId queue_id, void *message, uint32_t millisec)
{
	portBASE_TYPE taskWoken = pdFALSE;
	TickType_t ticks;

	ticks = millisec / portTICK_PERIOD_MS;

	if (inHandlerMode())
	{
		if (xQueueSendFromISR(queue_id, message, &taskWoken) != pdTRUE)
		{
			return osErrorOS;
		}

		portEND_SWITCHING_ISR(taskWoken);
	}
	else
	{
		if (xQueueSend(queue_id, message, ticks) != pdTRUE)
		{
			return osErrorOS;
		}
	}

	return osOK;
}

/*!
*\brief put ESP32 module AT init into message queue
*/
static void ESP32_InitReq(void)
{
	ESP32_TX_MSG txMsg;
	//put ESP32 device init to queue
	txMsg.function = ESP32_InitDev;
	txMsg.parameter = NULL;
	ESP32_TxMsgPut(txMsgQId, &txMsg, 0);
}


/*!
*\brief ESP32 receiver task
*\param [in] argument - argument pointer
*/
void ESP32_RxTask(void const *argument)
{
	osEvent event;
	rxTaskHandle = osThreadGetId();

	/* Infinite loop */
	for (;;)
	{
		event = osSignalWait(0x7FFFFFFF, osWaitForever);

		if (event.status == osEventSignal)
		{
			if (event.value.signals & RX_IDLE_EVENT)
			{
				ESP32_RxMsgDispatcher();
			}

			if (event.value.signals & RX_BUFFER_FULL_EVENT)
			{
				DBG_MSG("Warning: ESP32 UART DMA Rx Buffer Full:%d\n", ESP32_RX_BUFFERSIZE);
				//Todo: need to handle DMA Rx buffer full situation
			}
		}
	}
}


/*!
*\brief ESP32 transmit task
*\param [in] argument - argument pointer
*/
void ESP32_TxTask(void const *argument)
{
	osEvent event;
	ESP32_TX_MSG *pTxMsg;

	txTaskHandle = osThreadGetId();

	osSemaphoreDef(esp32CmdRspSem);
	cmdRspSemaphore = osSemaphoreCreate (osSemaphore(esp32CmdRspSem), 1);
	assert_param(cmdRspSemaphore != NULL);

	osMessageQDef(esp32TxMsgQ, ESPE2_TX_MSG_Q_SIZE, ESP32_TX_MSG);
	txMsgQId = osMessageCreate (osMessageQ(esp32TxMsgQ), txTaskHandle);
	assert_param(txMsgQId != NULL);

	osTimerDef(esp32WifiLedTimer, ESP32_WifiLedTimerCallback);
	wifiLedTimer = osTimerCreate(osTimer(esp32WifiLedTimer), osTimerPeriodic, NULL);
	assert_param(wifiLedTimer != NULL);

	osTimerDef(esp32BtLedTimer, ESP32_BleLedTimerCallback);
	btLedTimer = osTimerCreate(osTimer(esp32BtLedTimer), osTimerPeriodic, NULL);
	assert_param(btLedTimer != NULL);

	osTimerDef(esp32pollTimer, ESP32_PollTimerCallback);
	pollTimer = osTimerCreate(osTimer(esp32pollTimer), osTimerPeriodic, NULL);
	assert_param(pollTimer != NULL);


	//clear private data
	pf_memset(&esp32Data, 0, sizeof(ESP32_PRIVATE_DATA));

	//intialize UART
	ESP32_UartInit();

	//request ESP32 devce init
	ESP32_InitReq();

	/* Infinite loop */
	for (;;)
	{
		event = osMessageGet(txMsgQId, portMAX_DELAY);

		if (event.status == osEventMessage)
		{
			pTxMsg = (ESP32_TX_MSG *)&event.value;

			if (pTxMsg->function)
			{
				pTxMsg->function(pTxMsg->parameter);
			}
		}
	}
}


/*!
*\brief ESP32 initialization function
*/
void ESP32_Init(void)
{
	osThreadDef(EspRxTask, ESP32_RxTask, osPriorityBelowNormal, 0, 256);
	osThreadCreate(osThread(EspRxTask), NULL);

	osThreadDef(EspTxTask, ESP32_TxTask, osPriorityLow, 0, 128);
	osThreadCreate(osThread(EspTxTask), NULL);
}

/*!
*\brief wifi led indicator timer call back function
*\param [in] argument -argument pointer
*/
static void ESP32_WifiLedTimerCallback(void const *argument)
{
	HAL_GPIO_TogglePin(MS_LED2_GPIO_Port, MS_LED2_Pin);
}


/*!
*\brief set wifi state
*\param [in] state - wifi state to be set
*/
void ESP32_SetWifiState(ESP32_WIFI_STATE state)
{
	esp32Data.wifiState = state;
	//update wifi led
	ESP32_NotifyWifiStatus(state);
}

/*!
*\brief notify wifi LED indicator and upper callback if available upon wifi status change
*\param [in] state - wifi state to be set
*/
static void ESP32_NotifyWifiStatus(ESP32_WIFI_STATE state)
{
	ESP32_NOTIFICATION notification;
	notification.status = ESP32_STATUS_OK;
	notification.data.p = NULL;

	switch (state)
	{
		case ESP32_WIFI_OFF:
			//led notification
			osTimerStop(wifiLedTimer);
			HAL_GPIO_WritePin(MS_LED2_GPIO_Port, MS_LED2_Pin, GPIO_PIN_SET);
			//app notification
			notification.event = ESP32_EVENT_WIFI_OFF;
			ESP32_NotifyApp(&notification);
			//stop status poll Timer
			osTimerStop(pollTimer);
			break;

		case ESP32_WIFI_DISCONN:
			osTimerStart(wifiLedTimer, 1000);
			//app notification
			notification.event = ESP32_EVENT_WIFI_DISCONN;
			ESP32_NotifyApp(&notification);
			//start status poll Timer
			wifiApPollCount = 0;
			osTimerStart(pollTimer, 1000);
			break;

		case ESP32_WIFI_CONN:
			osTimerStop(wifiLedTimer);
			HAL_GPIO_WritePin(MS_LED2_GPIO_Port, MS_LED2_Pin, GPIO_PIN_RESET);
			//start status poll Timer
			wifiApPollCount = 0;
			osTimerStart(pollTimer, 1000);
			//app notification
			notification.event = ESP32_EVENT_WIFI_CONN;
			notification.data.apInfo = &esp32Data.connApInfo;
			ESP32_NotifyApp(&notification);
			break;

		case ESP32_WIFI_GOTIP:
			//no need to notify led as it must walk through ESP32_WIFI_CONN state
			//app notification
			notification.event = ESP32_EVENT_WIFI_GOTIP;
			notification.data.ipAddr = esp32Data.ipAddr;
			ESP32_NotifyApp(&notification);
			break;

		case ESP32_WIFI_SMARTCONFIG:
			osTimerStart(wifiLedTimer, 200);
			//app notification
			notification.event = ESP32_EVENT_WIFI_SMARTCONFIG;
			ESP32_NotifyApp(&notification);
			break;

		default:
			break;

	}

}

/*!
*\brief bluetooth led indicator timer call back function
*\param [in] argument -argument pointer
*/
static void ESP32_BleLedTimerCallback(void const *argument)
{
	HAL_GPIO_TogglePin(MS_LED1_GPIO_Port, MS_LED1_Pin);
}

/*!
*\brief set bluetooth state
*\param [in] state - bluetool state to be set
*/
void ESP32_SetBleState(ESP32_BLE_STATE state)
{
	esp32Data.bleState = state;
	//update wifi led
	ESP32_NotifyBleStatus(state);
}

/*!
*\brief notify bluetooth LED indicator and upper callback if available upon wifi status change
*\param [in] state - bluetooth state to be set
*/
static void ESP32_NotifyBleStatus(ESP32_BLE_STATE state)
{
	ESP32_NOTIFICATION notification;
	notification.status = ESP32_STATUS_OK;
	notification.data.p = NULL;

	switch (state)
	{
		case ESP32_BLE_OFF:
			//led notification
			osTimerStop(btLedTimer);
			HAL_GPIO_WritePin(MS_LED1_GPIO_Port, MS_LED1_Pin, GPIO_PIN_SET);
			//app notification
			notification.event = ESP32_EVENT_BT_OFF;
			ESP32_NotifyApp(&notification);
			break;

		case ESP32_BLE_DISCONN:
			osTimerStart(btLedTimer, 1000);
			//app notification
			notification.event = ESP32_EVENT_BT_DISCONN;
			ESP32_NotifyApp(&notification);
			break;

		case ESP32_BLE_CONN:
			osTimerStop(btLedTimer);
			HAL_GPIO_WritePin(MS_LED1_GPIO_Port, MS_LED1_Pin, GPIO_PIN_RESET);
			//app notification
			notification.event = ESP32_EVENT_BT_CONN;
			//notification.data.apInfo = &esp32Data.connApInfo;
			ESP32_NotifyApp(&notification);
			break;

		default:
			break;

	}

}


/*!
*\brief WIFI & Bluetooth button interrupt handler
*\param [in] nGPIO - GPIO interrupt source (number)
*/
void ESP32_InputHandler(uint16_t nGPIO)
{
	static uint32_t btBtnTick = 0;
	static uint32_t wifiBtnTick = 0;
	ESP32_TX_MSG txMsg;

	switch (nGPIO)
	{
		case MS_BTN1_Pin://WIFI Switch

			//debounce
			if (CheckTimeElapsed(wifiBtnTick, DEBOUNCE_TIME))
			{
				//rising edeg?
				if (HAL_GPIO_ReadPin(MS_BTN1_GPIO_Port, MS_BTN1_Pin))
				{
					if (txMsgQId)
					{
						//hold on > 3S to activate wifi smart config
						if ((osKernelSysTick() - wifiBtnTick) >= SMARTCONFIG_ACTIVATION_TIME)
						{
							txMsg.parameter = (void *)ESP32_WIFI_SMARTCONFIG;
							//send to tx queue
							txMsg.function = ESP32_SetWifiMode;
							ESP32_TxMsgPut(txMsgQId, &txMsg, 0);
						}
						else
						{
							//toggle wifi on/off
							if (ESP32_WIFI_OFF == esp32Data.wifiState)
							{
								txMsg.parameter = (void *) ESP32_WIFI_DISCONN;
							}
							else
							{
								txMsg.parameter = (void *) ESP32_WIFI_OFF;
							}

							//put to tx queue
							txMsg.function = ESP32_SetWifiMode;
							ESP32_TxMsgPut(txMsgQId, &txMsg, 0);
						}
					}
				}
				else
				{
					//indicates user button is pressing down, led will be updated later
					if (wifiLedTimer)
					{
						osTimerStop(wifiLedTimer);
						HAL_GPIO_WritePin(MS_LED2_GPIO_Port, MS_LED2_Pin, GPIO_PIN_RESET);
					}
				}
			}

			wifiBtnTick = osKernelSysTick();
			break;

		case MS_BTN2_Pin:	//BT Switch

			//debounce
			if (CheckTimeElapsed(btBtnTick, DEBOUNCE_TIME))
			{
				//rising edeg?
				if (HAL_GPIO_ReadPin(MS_BTN2_GPIO_Port, MS_BTN2_Pin))
				{
					if (ESP32_BLE_OFF == esp32Data.bleState)
					{
						//Turn on BT
						txMsg.parameter = (void *)ESP32_BLE_DISCONN;
					}
					else
					{
						//turn off BT
						txMsg.parameter = (void *)ESP32_BLE_OFF;
					}

					//put to tx queue
					txMsg.function = ESP32_SetBleMode;
					ESP32_TxMsgPut(txMsgQId, &txMsg, 0);

				}
				else
				{
					//indicates user button is pressing down, led will be updated later
					if (btLedTimer)
					{
						osTimerStop(btLedTimer);
						HAL_GPIO_WritePin(MS_LED1_GPIO_Port, MS_LED1_Pin, GPIO_PIN_RESET);
					}
				}
			}

			btBtnTick = osKernelSysTick();
			break;

		default:
			break;
	}
}

/*!
*\brief get ble on/off state from nvram
*\retval ESP32_BLE_OFF - ble is off
*\retval ESP32_BLE_DISCONN - ble is on
*/
static ESP32_BLE_STATE GetBleStateFromNvram(void)
{
	uint16_t length;
	uint8_t buffer[2];
	ESP32_BLE_STATE state = ESP32_BLE_OFF;

	length = ReadParamFromNvram(MISC_PARAM_AREA_NAME, TAG_BLE_STATE, 2, buffer);

	if (2 == length)
	{
		//check validity flag
		if (buffer[0] == 0x5A)
		{
			state = (ESP32_BLE_OFF != buffer[1]) ? ESP32_BLE_DISCONN : ESP32_BLE_OFF;
		}
	}

	return state;
}

/*!
*\brief set ble on/off state to nvram
*\param [in] state - ble off/on state
*/
static void SetBleStateToNvram(ESP32_BLE_STATE state)
{
	uint16_t length;
	uint8_t buffer[2];

	//validity byte
	buffer[0] = 0x5A;
	buffer[1] = (uint8_t)state;

	length = WriteParamToNvram(MISC_PARAM_AREA_NAME, TAG_BLE_STATE, 2, buffer);

	if (2 != length)
	{
		DBG_MSG("%s failed:%d\n", __FUNCTION__, length);
	}
}

/*!
*\brief read smart config AP SSID
*\param [out] buffer - buffer to read to
*\retval 0 -read failed
*\retval !0 -length read back
*/
static uint16_t GetApSsidFromNvram(char *buffer, uint16_t length)
{
	return ReadParamFromNvram(VARA_PARAM_AREA_NAME, TAG_WIFI_AP_SSID, length, (uint8_t *)buffer);
}

/*!
*\brief write smart config AP SSID
*\param [in] buffer - buffer
*\retval 0 -write failed
*\retval !0 -length written
*/
static uint16_t SetApSsidToNvram(char *buffer, uint16_t length)
{
	return WriteParamToNvram(VARA_PARAM_AREA_NAME, TAG_WIFI_AP_SSID, length, (uint8_t *)buffer);
}

/*!
*\brief read smart config AP password from nvram
*\param [out] buffer - buffer to read to
*\retval 0 -read failed
*\retval !0 -length read back
*/
static uint16_t GetApPwdFromNvram(char *buffer, uint16_t length)
{
	return ReadParamFromNvram(VARA_PARAM_AREA_NAME, TAG_WIFI_AP_PWD, length, (uint8_t *)buffer);
}


/*!
*\brief write smart config AP password to nvram
*\param [in] buffer - buffer
*\retval 0 -write failed
*\retval !0 -length written
*/
static uint16_t SetApPwdToNvram(char *buffer, uint16_t length)
{
	return WriteParamToNvram(VARA_PARAM_AREA_NAME, TAG_WIFI_AP_PWD, length, (uint8_t *)buffer);
}


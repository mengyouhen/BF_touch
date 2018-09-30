/******************************************************************************
 Copyright (c) 2018 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		flex_framework_privtocol.c
 [Date]:
 		2018-08-14
 [Author]:
 		joanna Wang
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
#include "flex_framework_privtocol.h"
#include "flex_app_segdisp.h"
#include "cmsis_armcc.h"
#include "flex_drv_led.h"
#include "flex_drv_panel.h"
#include "flex_framework_esp32.h"
#include "rtc.h"
/*========================================================================================================
										D E F I N E
========================================================================================================*/
#define PRIVTOCOL_PRINT(arg...)   	printf(arg)
#define RX_FROM_FIX_POS		0
#define FREAME_HEADER		0x55AA
#define CMD_POS		2
#define SUBCMD_POS	 3
#define CMD_DATALEN_POS	 4
#define CMD_DATAARRAY_POS	 5

#define PRIVTOCOL_RX_BUFFERSIZE		288
#define PRIVTOCOL_TX_MSG_Q_SIZE	10

//rx task event notification bit field
#define RX_CMD_DONE_EVENT			_BIT0_

typedef enum
{
	RX_IDLE_STATE = 0,
	RX_CMD_HEADER_STATE,
	RX_CMD_DATA_STATE,
	RX_CMD_MAX_STATE,
} PRIVTOCOL_RX_STATE;

typedef enum
{
	PRIVTOCOL_ESP_WIFI = 0,
	PRIVTOCOL_ESP_BLE,
	PRIVTOCOL_ESP_MAX,
} PRIVTOCOL_ESP_MODULE;
/*========================================================================================================
						F U N C T I O N Declaration
========================================================================================================*/
static void Privtocol_UartInit(void);
static uint8_t CRC_High_First(uint8_t *ptr, uint8_t len);
static void Privtocol_RestartCmdRx(void);


/*========================================================================================================
										E X T E R N
========================================================================================================*/
extern DMA_HandleTypeDef hdma_usart2_rx;
static osThreadId rxTaskHandle;
static uint16_t rxCmdLength;
static PRIVTOCOL_RX_STATE rxState = RX_IDLE_STATE;
static uint8_t rxBuffer[PRIVTOCOL_RX_BUFFERSIZE];
uint8_t GetESPState(uint8_t module);
void SetESPState(uint8_t module,uint8_t state);
/*========================================================================================================
										T Y P E D E F
========================================================================================================*/

typedef struct
{	
	const PRIVTOCOL_SUBCMD subcmd ;	
	PRIVTOCOL_STATUS_LIST ( * handler) (char *rx);
}PRIVTOCOL_RX_MSG_HANDLER;

typedef void (*Privtocol_TxFunc)(void *);
typedef struct
{
	Privtocol_TxFunc function;
	void *parameter;//parameter
}PRIVTOCOL_TX_MSG;

/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
static  PRIVTOCOL_RX_MSG_HANDLER const Info_HandlerTbl[] =
{ 
	{SUBCMD_GET_PROT_VER,Privtocol_Get_Communication_Protocol_Version},
	{SUBCMD_GET_DEVICE_ID,Privtocol_Get_Device_ID},
	{SUBCMD_SET_DEVICE_ID,Privtocol_Set_Device_ID}
};
static  PRIVTOCOL_RX_MSG_HANDLER const Ctl_HandlerTbl[] =
{ 
	{SUBCMD_SET_LCD,Privtocol_Control_Lcd},
	{SUBCMD_SET_WIRELESS,Privtocol_Control_Wireless},
	{SUBCMD_SET_BEEPER,Privtocol_Control_Beeper},
	{SUBCMD_SET_TIME,Privtocol_Control_Time},
	{SUBCMD_SET_LED,Privtocol_Control_Led},	
};
static  PRIVTOCOL_RX_MSG_HANDLER const Status_HandlerTbl[] =
{ 
	{SUBCMD_GET_LCD,Privtocol_Get_Lcd_Status},
	{SUBCMD_GET_WIRELESS,Privtocol_Get_Wireless_Status},
	{SUBCMD_GET_BEEPER,Privtocol_Get_Beeper_Status},
	{SUBCMD_GET_TIME,Privtocol_Get_Time_Status},
	{SUBCMD_GET_LED,Privtocol_Get_Led_Status},
	{SUBCMD_GET_BTN,Privtocol_Get_Btn_Status},	
};
static  PRIVTOCOL_RX_MSG_HANDLER const Sys_HandlerTbl[] =
{
	{SUBCMD_SYS_SOFTRST, Privtocol_Sys_Softrst},
};
/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
uint8_t GetESPState(uint8_t module)
{
	uint8_t ret=0;
	switch(module)
	{
		case PRIVTOCOL_ESP_WIFI:
			ret = ESP32_GetWifiState();
		  break;
		case PRIVTOCOL_ESP_BLE:
			ret = ESP32_GetBleState();
		  break;
		default:
		  break;
	}
	return ret;
}
void SetESPState(uint8_t module,uint8_t state)
{
	switch(module)
	{
		case PRIVTOCOL_ESP_WIFI:
			ESP32_SetWifiState((ESP32_WIFI_STATE)state);
		  break;
		case PRIVTOCOL_ESP_BLE:
			ESP32_SetBleState((ESP32_BLE_STATE)state);
		  break;
		default:
		  break;
	}
	 
}

PRIVTOCOL_STATUS_LIST Privtocol_Get_Communication_Protocol_Version(char *buffer)
{
	PRIVTOCOL_RSP_FRAME cmdframe;
	
	cmdframe.header = FREAME_HEADER;
	cmdframe.cmd = CMD_DEVICE_INFO | 0x80;
	cmdframe.subcmd = SUBCMD_GET_PROT_VER;
	cmdframe.retdata = (PRIVTOCOL_DATA)"Butterfly Private Protocol V1.0";
	cmdframe.length = strlen((char *)cmdframe.retdata);
	cmdframe.crc8code = CRC_High_First((uint8_t *)&cmdframe, CMD_DATALEN_POS + cmdframe.length);
	PRIVTOCOL_PRINT("%c%c%c%c%c", cmdframe.header & 0xFF, (cmdframe.header & 0xFF00) >> 8, cmdframe.cmd, cmdframe.subcmd, cmdframe.length);

	for (uint8_t i = 0; i < strlen(buffer) + 1; i++)
	{
		PRIVTOCOL_PRINT("%c", cmdframe.retdata[i]);
	}

	PRIVTOCOL_PRINT("%c\r\n", cmdframe.crc8code);
	//PRIVTOCOL_PRINT("Privtocol_Get_Communication_Protocol_Version\r\n");
	return PRIVTOCOL_STATUS_OK;
}
PRIVTOCOL_STATUS_LIST Privtocol_Get_Device_ID(char *buffer)
{
	PRIVTOCOL_RSP_FRAME cmdframe;
	
	cmdframe.header = FREAME_HEADER;
	cmdframe.cmd = CMD_DEVICE_INFO | 0x80;
	cmdframe.subcmd = SUBCMD_GET_DEVICE_ID;	
	cmdframe.retdata = (PRIVTOCOL_DATA)"Butterfly Firmware V1.0";
	cmdframe.length = strlen((char *)cmdframe.retdata);
	cmdframe.crc8code = CRC_High_First((uint8_t *)&cmdframe, CMD_DATALEN_POS + cmdframe.length);
	PRIVTOCOL_PRINT("%c%c%c%c%c", cmdframe.header & 0xFF, (cmdframe.header & 0xFF00) >> 8, cmdframe.cmd, cmdframe.subcmd, cmdframe.length);

	for (uint8_t i = 0; i < strlen(buffer) + 1; i++)
	{
		PRIVTOCOL_PRINT("%c", cmdframe.retdata[i]);
	}

	PRIVTOCOL_PRINT("%c\r\n", cmdframe.crc8code);
	//PRIVTOCOL_PRINT("Privtocol_Get_Device_ID\r\n");
	return PRIVTOCOL_STATUS_OK;
}
PRIVTOCOL_STATUS_LIST Privtocol_Set_Device_ID(char *buffer)
{
	PRIVTOCOL_RSP_FRAME cmdframe;
	
	cmdframe.header = FREAME_HEADER;
	cmdframe.cmd = CMD_DEVICE_INFO | 0x80;
	cmdframe.subcmd = SUBCMD_SET_DEVICE_ID;
	cmdframe.length = strlen(buffer)+1;	
	cmdframe.retdata = (PRIVTOCOL_DATA)pf_malloc(cmdframe.length);
  memcpy(cmdframe.retdata,buffer,strlen(buffer)-1);
	cmdframe.crc8code = CRC_High_First((uint8_t *)&cmdframe, CMD_DATALEN_POS + cmdframe.length);
	PRIVTOCOL_PRINT("%c%c%c%c%c", cmdframe.header & 0xFF, (cmdframe.header & 0xFF00) >> 8, cmdframe.cmd, cmdframe.subcmd, cmdframe.length);

	for (uint8_t i = 0; i < strlen(buffer) + 1; i++)
	{
		PRIVTOCOL_PRINT("%c", cmdframe.retdata[i]);
	}

	PRIVTOCOL_PRINT("%c\r\n", cmdframe.crc8code);
	//PRIVTOCOL_PRINT("Privtocol_Set_Device_ID\r\n");
	return PRIVTOCOL_STATUS_OK;
}
PRIVTOCOL_STATUS_LIST Privtocol_Control_Lcd(char *buffer)
{
	PRIVTOCOL_RSP_FRAME cmdframe;
	PRIVTOCOL_LCDCTL_DEF lcd_ctl;
	
	cmdframe.header = FREAME_HEADER;
	cmdframe.cmd = CMD_SET_CTL | 0x80;
	cmdframe.subcmd = SUBCMD_SET_LCD;
	cmdframe.length = strlen(buffer)+1;

	lcd_ctl.bOn = buffer[CMD_DATAARRAY_POS+1];
	lcd_ctl.index_number = buffer[CMD_DATAARRAY_POS];
	lcd_ctl.payload = (char *)pf_malloc(strlen(buffer)-1);
	Drv_Panel_Power((DRV_PANEL_NAME_ENUM)lcd_ctl.index_number, (DRV_PANEL_POWER_ENUM)lcd_ctl.bOn);
	//Display string TBD	
	memcpy(lcd_ctl.payload, buffer, strlen(buffer) - 1);
	cmdframe.retdata = (PRIVTOCOL_DATA)pf_malloc(sizeof(lcd_ctl));
	sprintf((char *)cmdframe.retdata, "%c%c%s", lcd_ctl.bOn,lcd_ctl.index_number,lcd_ctl.payload);
	Drv_Panel_Clear_All(LIST_PANEL_INDEX_MAIN);
	Drv_Panel_Draw_Str(LIST_PANEL_INDEX_MAIN, LIST_FONT_6X8, 10, 10, cmdframe.retdata);	

	cmdframe.crc8code = CRC_High_First((uint8_t *)&cmdframe, CMD_DATALEN_POS + cmdframe.length);
	PRIVTOCOL_PRINT("%c%c%c%c%c", cmdframe.header & 0xFF, (cmdframe.header & 0xFF00) >> 8, cmdframe.cmd, cmdframe.subcmd, cmdframe.length);

	for (uint8_t i = 0; i < strlen(buffer) + 1; i++)
	{
		PRIVTOCOL_PRINT("%c", cmdframe.retdata[i]);
	}

	PRIVTOCOL_PRINT("%c\r\n", cmdframe.crc8code);
	//PRIVTOCOL_PRINT("Privtocol_Control_Lcd\r\n");
	return PRIVTOCOL_STATUS_OK;
}
PRIVTOCOL_STATUS_LIST Privtocol_Control_Wireless(char *buffer)
{
	PRIVTOCOL_RSP_FRAME cmdframe;
	PRIVTOCOL_WIRELESSCTL_DEF wireless_ctl;	
	
	cmdframe.header = FREAME_HEADER;
	cmdframe.cmd = CMD_SET_CTL | 0x80;
	cmdframe.subcmd = SUBCMD_SET_WIRELESS;
	cmdframe.length = sizeof(PRIVTOCOL_WIRELESSCTL_DEF);
	wireless_ctl.bOn = buffer[CMD_DATALEN_POS+2];
	wireless_ctl.index_number = buffer[CMD_DATALEN_POS+1];
  SetESPState(buffer[CMD_DATALEN_POS+1],buffer[CMD_DATALEN_POS+2]);	
	cmdframe.retdata = (PRIVTOCOL_DATA)pf_malloc(sizeof(wireless_ctl));
	sprintf((char *)cmdframe.retdata, "%c%c", wireless_ctl.bOn,wireless_ctl.index_number);
	cmdframe.crc8code = CRC_High_First((uint8_t *)&cmdframe, CMD_DATALEN_POS + cmdframe.length);
	PRIVTOCOL_PRINT("%c%c%c%c%c", cmdframe.header & 0xFF, (cmdframe.header & 0xFF00) >> 8, cmdframe.cmd, cmdframe.subcmd, cmdframe.length);

	for (uint8_t i = 0; i < sizeof(wireless_ctl); i++)
	{
		PRIVTOCOL_PRINT("%c", cmdframe.retdata[i]);
	}

	PRIVTOCOL_PRINT("%c\r\n", cmdframe.crc8code);
	//PRIVTOCOL_PRINT("Privtocol_Control_Wireless\r\n");
	return PRIVTOCOL_STATUS_OK;
}
PRIVTOCOL_STATUS_LIST Privtocol_Control_Beeper(char *buffer)
{
	PRIVTOCOL_RSP_FRAME cmdframe;
	PRIVTOCOL_BEEPERCTL_DEF beeper_ctl;	
	
	cmdframe.header = FREAME_HEADER;
	cmdframe.cmd = CMD_SET_CTL | 0x80;
	cmdframe.subcmd = SUBCMD_SET_BEEPER;
	cmdframe.length = sizeof(PRIVTOCOL_BEEPERCTL_DEF);

	beeper_ctl.bOn = true;
	beeper_ctl.pwm_freq = 0x1234;

	cmdframe.retdata = (PRIVTOCOL_DATA)pf_malloc(sizeof(beeper_ctl));
	sprintf((char *)cmdframe.retdata, "%c%c", beeper_ctl.bOn,beeper_ctl.pwm_freq);
	cmdframe.crc8code = CRC_High_First((uint8_t *)&cmdframe, CMD_DATALEN_POS + cmdframe.length);
	PRIVTOCOL_PRINT("%c%c%c%c%c", cmdframe.header & 0xFF, (cmdframe.header & 0xFF00) >> 8, cmdframe.cmd, cmdframe.subcmd, cmdframe.length);

	for (uint8_t i = 0; i < sizeof(beeper_ctl); i++)
	{
		PRIVTOCOL_PRINT("%c", cmdframe.retdata[i]);
	}

	PRIVTOCOL_PRINT("%c\r\n", cmdframe.crc8code);
	//PRIVTOCOL_PRINT("Privtocol_Control_Beeper\r\n");
	return PRIVTOCOL_STATUS_OK;
}
PRIVTOCOL_STATUS_LIST Privtocol_Control_Time(char *buffer)
{	
	PRIVTOCOL_RSP_FRAME cmdframe;

	cmdframe.header = FREAME_HEADER;
	cmdframe.cmd = CMD_SET_CTL | 0x80;
	cmdframe.subcmd = SUBCMD_SET_TIME;

	cmdframe.retdata = (PRIVTOCOL_DATA)pf_malloc(sizeof(RTC_DateTypeDef) + sizeof(RTC_TimeTypeDef));
	sprintf((char *)cmdframe.retdata, "%x", buffer[CMD_DATALEN_POS+1]);
  RTC_SetDateTime((RTC_DateTypeDef *)(buffer + CMD_DATALEN_POS + 1), (RTC_TimeTypeDef *)(buffer+CMD_DATALEN_POS + 1 + sizeof(RTC_DateTypeDef)));	

	cmdframe.crc8code = CRC_High_First((uint8_t *)&cmdframe, CMD_DATALEN_POS + cmdframe.length);
	PRIVTOCOL_PRINT("%c%c%c%c%c", cmdframe.header & 0xFF, (cmdframe.header & 0xFF00) >> 8, cmdframe.cmd, cmdframe.subcmd, cmdframe.length);

	for (uint8_t i = 0; i < 4; i++)
	{
		PRIVTOCOL_PRINT("%c", cmdframe.retdata[i]);
	}

	PRIVTOCOL_PRINT("%c\r\n", cmdframe.crc8code);
	//PRIVTOCOL_PRINT("Privtocol_Control_Time\r\n");
	return PRIVTOCOL_STATUS_OK;
}
PRIVTOCOL_STATUS_LIST Privtocol_Control_Led(char *buffer)
{
	PRIVTOCOL_RSP_FRAME cmdframe;
	PRIVTOCOL_LEDCTL_DEF led_ctl;

	cmdframe.header = FREAME_HEADER;
	cmdframe.cmd = CMD_SET_CTL | 0x80;
	cmdframe.subcmd = SUBCMD_SET_LED;	
	cmdframe.length = sizeof(PRIVTOCOL_LEDCTL_DEF);
	
	led_ctl.bOn = buffer[CMD_DATAARRAY_POS+1];
	led_ctl.index_number = buffer[CMD_DATAARRAY_POS];
	Drv_Led_Ctrl((flex_led_name_enum)led_ctl.index_number, led_ctl.bOn);	
	
	cmdframe.retdata = (PRIVTOCOL_DATA)pf_malloc(sizeof(led_ctl));
	memcpy(cmdframe.retdata,&led_ctl,sizeof(led_ctl));
	cmdframe.crc8code = CRC_High_First((uint8_t *)&cmdframe, CMD_DATALEN_POS + cmdframe.length);
	PRIVTOCOL_PRINT("%c%c%c%c%c", cmdframe.header & 0xFF, (cmdframe.header & 0xFF00) >> 8, cmdframe.cmd, cmdframe.subcmd, cmdframe.length);

	for (uint8_t i = 0; i < sizeof(led_ctl); i++)
	{
		PRIVTOCOL_PRINT("%c", cmdframe.retdata[i]);
	}

	PRIVTOCOL_PRINT("%c\r\n", cmdframe.crc8code);
	//PRIVTOCOL_PRINT("Privtocol_Control_Led\r\n");
	return PRIVTOCOL_STATUS_OK;
}
PRIVTOCOL_STATUS_LIST Privtocol_Get_Lcd_Status(char * buffer)
{
	PRIVTOCOL_RSP_FRAME cmdframe;
	PRIVTOCOL_LCD_STATUS lcd_status;

	cmdframe.header = FREAME_HEADER;
	cmdframe.cmd = CMD_GET_STATUS | 0x80;
	cmdframe.subcmd = SUBCMD_GET_LCD;	
	cmdframe.length = sizeof(PRIVTOCOL_LCD_STATUS);	
	
	lcd_status.ret_code = RET_OK;
	lcd_status.bOn = Drv_Panel_Get_PwrState((DRV_PANEL_NAME_ENUM)buffer[CMD_DATAARRAY_POS]);
	lcd_status.index_number = buffer[CMD_DATAARRAY_POS];
	
	cmdframe.retdata = (PRIVTOCOL_DATA)pf_malloc(sizeof(lcd_status));
	memcpy(cmdframe.retdata,&lcd_status,sizeof(lcd_status));
	cmdframe.crc8code = CRC_High_First((uint8_t *)&cmdframe, CMD_DATALEN_POS + cmdframe.length);
	PRIVTOCOL_PRINT("%c%c%c%c%c", cmdframe.header & 0xFF, (cmdframe.header & 0xFF00) >> 8, cmdframe.cmd, cmdframe.subcmd, cmdframe.length);

	for (uint8_t i = 0; i < sizeof(lcd_status); i++)
	{
		PRIVTOCOL_PRINT("%c", cmdframe.retdata[i]);
	}

	PRIVTOCOL_PRINT("%c\r\n", cmdframe.crc8code);
	return PRIVTOCOL_STATUS_OK;
}
PRIVTOCOL_STATUS_LIST Privtocol_Get_Wireless_Status(char * buffer)
{	
	PRIVTOCOL_RSP_FRAME cmdframe;
	PRIVTOCOL_WIRELESS_STATUS wireless_status;

	cmdframe.header = FREAME_HEADER;
	cmdframe.cmd = CMD_GET_STATUS | 0x80;
	cmdframe.subcmd = SUBCMD_GET_WIRELESS;
	cmdframe.length = sizeof(PRIVTOCOL_WIRELESS_STATUS);
	
	wireless_status.ret_code = RET_OK;
	wireless_status.bOn = GetESPState(buffer[CMD_DATALEN_POS +1]);
	wireless_status.index_number = buffer[CMD_DATALEN_POS +1];
	
	cmdframe.retdata = (PRIVTOCOL_DATA)pf_malloc(sizeof(wireless_status));
	memcpy(cmdframe.retdata,&wireless_status,sizeof(wireless_status));
	cmdframe.crc8code = CRC_High_First((uint8_t *)&cmdframe, CMD_DATALEN_POS + cmdframe.length);
	PRIVTOCOL_PRINT("%c%c%c%c%c", cmdframe.header & 0xFF, (cmdframe.header & 0xFF00) >> 8, cmdframe.cmd, cmdframe.subcmd, cmdframe.length);

	for (uint8_t i = 0; i < sizeof(wireless_status); i++)
	{
		PRIVTOCOL_PRINT("%c", cmdframe.retdata[i]);
	}

	PRIVTOCOL_PRINT("%c\r\n", cmdframe.crc8code);
	return PRIVTOCOL_STATUS_OK;
}
PRIVTOCOL_STATUS_LIST Privtocol_Get_Beeper_Status(char *buffer)
{		
	static PRIVTOCOL_RSP_FRAME cmdframe;
	static PRIVTOCOL_BEEPER_STATUS beeper_status;		

	cmdframe.header = FREAME_HEADER;
	cmdframe.cmd = CMD_GET_STATUS | 0x80;
	cmdframe.subcmd = SUBCMD_GET_BEEPER;
	cmdframe.length = sizeof(PRIVTOCOL_BEEPER_STATUS);
	
	beeper_status.ret_code = RET_OK;
	beeper_status.bOn = true;
	beeper_status.payload = 0x1010;
	
	cmdframe.retdata = (PRIVTOCOL_DATA)pf_malloc(sizeof(beeper_status));
	memcpy(cmdframe.retdata,&beeper_status,sizeof(beeper_status));
	cmdframe.crc8code = CRC_High_First((uint8_t *)&cmdframe, CMD_DATALEN_POS + cmdframe.length);
	PRIVTOCOL_PRINT("%c%c%c%c%c", cmdframe.header & 0xFF, (cmdframe.header & 0xFF00) >> 8, cmdframe.cmd, cmdframe.subcmd, cmdframe.length);

	for (uint8_t i = 0; i < sizeof(beeper_status); i++)
	{
		PRIVTOCOL_PRINT("%c", cmdframe.retdata[i]);
	}

	PRIVTOCOL_PRINT("%c\r\n", cmdframe.crc8code);
	return PRIVTOCOL_STATUS_OK;
}
PRIVTOCOL_STATUS_LIST Privtocol_Get_Time_Status(char *buffer)
{	
	PRIVTOCOL_RSP_FRAME cmdframe;
	PRIVTOCOL_TIME_STATUS time_status;	
	
	
	cmdframe.header = FREAME_HEADER;
	cmdframe.cmd = CMD_GET_STATUS | 0x80;
	cmdframe.subcmd = SUBCMD_GET_TIME;
	cmdframe.length = sizeof(PRIVTOCOL_TIME_STATUS);
	
  time_status.ret_code = RET_OK;	
	RTC_GetDateTime(&time_status.Date, &time_status.Time);
	
	cmdframe.retdata = (PRIVTOCOL_DATA)pf_malloc(sizeof(RTC_DateTypeDef) + sizeof(RTC_TimeTypeDef)+1);	
	memcpy(cmdframe.retdata,&time_status,(sizeof(RTC_DateTypeDef) + sizeof(RTC_TimeTypeDef) + 1));
	cmdframe.crc8code = CRC_High_First((uint8_t *)&cmdframe, CMD_DATALEN_POS + cmdframe.length);
	PRIVTOCOL_PRINT("%c%c%c%c%c", cmdframe.header & 0xFF, (cmdframe.header & 0xFF00) >> 8, cmdframe.cmd, cmdframe.subcmd, cmdframe.length);

	for (uint8_t i = 0; i < sizeof(time_status); i++)
	{
		PRIVTOCOL_PRINT("%c", cmdframe.retdata[i]);
	}

	PRIVTOCOL_PRINT("%c\r\n", cmdframe.crc8code);
	return PRIVTOCOL_STATUS_OK;
}
PRIVTOCOL_STATUS_LIST Privtocol_Get_Led_Status(char *buffer)
{	
	PRIVTOCOL_RSP_FRAME cmdframe;
	PRIVTOCOL_LED_STATUS led_status;
	
	cmdframe.header = FREAME_HEADER;
	cmdframe.cmd = CMD_GET_STATUS | 0x80;
	cmdframe.subcmd = SUBCMD_GET_LED;
	cmdframe.length = sizeof(PRIVTOCOL_LED_STATUS);
	
	led_status.ret_code = RET_OK;
	led_status.index_number = buffer[CMD_DATAARRAY_POS];
	led_status.bOn = Drv_Led_Get_Status((flex_led_name_enum)buffer[CMD_DATAARRAY_POS]);	
	
	cmdframe.retdata = (PRIVTOCOL_DATA)pf_malloc(sizeof(led_status));	
	memcpy(cmdframe.retdata,&led_status,sizeof(led_status));
	cmdframe.crc8code = CRC_High_First((uint8_t *)&cmdframe, CMD_DATALEN_POS + cmdframe.length);
	PRIVTOCOL_PRINT("%c%c%c%c%c", cmdframe.header & 0xFF, (cmdframe.header & 0xFF00) >> 8, cmdframe.cmd, cmdframe.subcmd, cmdframe.length);

	for (uint8_t i = 0; i < sizeof(led_status); i++)
	{
		PRIVTOCOL_PRINT("%c", cmdframe.retdata[i]);
	}

	PRIVTOCOL_PRINT("%c\r\n", cmdframe.crc8code);
	return PRIVTOCOL_STATUS_OK;
}
PRIVTOCOL_STATUS_LIST Privtocol_Get_Btn_Status(char *buffer)
{		
	PRIVTOCOL_RSP_FRAME cmdframe;
	PRIVTOCOL_BTN_STATUS btn_status;
	
	cmdframe.header = FREAME_HEADER;
	cmdframe.cmd = CMD_GET_STATUS | 0x80;
	cmdframe.subcmd = SUBCMD_GET_BTN;
	cmdframe.length = sizeof(PRIVTOCOL_BTN_STATUS);
	
	btn_status.ret_code = RET_OK;
	btn_status.index_number = 0;
	btn_status.bOn = true;
	
	cmdframe.retdata = (PRIVTOCOL_DATA)pf_malloc(sizeof(btn_status));	
	memcpy(cmdframe.retdata,&btn_status,sizeof(btn_status));
	cmdframe.crc8code = CRC_High_First((uint8_t *)&cmdframe, CMD_DATALEN_POS + cmdframe.length);
	PRIVTOCOL_PRINT("%c%c%c%c%c", cmdframe.header & 0xFF, (cmdframe.header & 0xFF00) >> 8, cmdframe.cmd, cmdframe.subcmd, cmdframe.length);

	for (uint8_t i = 0; i < sizeof(btn_status); i++)
	{
		PRIVTOCOL_PRINT("%c", cmdframe.retdata[i]);
	}

	PRIVTOCOL_PRINT("%c\r\n", cmdframe.crc8code);
	return PRIVTOCOL_STATUS_OK;
}

PRIVTOCOL_STATUS_LIST Privtocol_Sys_Softrst(char *buffer)
{
	PRIVTOCOL_RSP_FRAME cmdframe;

	cmdframe.header = FREAME_HEADER;
	cmdframe.cmd = CMD_SYS | 0x80;
	cmdframe.subcmd = SUBCMD_SYS_SOFTRST;
	cmdframe.length = 0;
	cmdframe.crc8code = CRC_High_First((uint8_t *)&cmdframe, CMD_DATALEN_POS + cmdframe.length);
	PRIVTOCOL_PRINT("%c%c%c%c%c", cmdframe.header & 0xFF, (cmdframe.header & 0xFF00) >> 8, cmdframe.cmd, cmdframe.subcmd, cmdframe.length);
	PRIVTOCOL_PRINT("%c\r\n", cmdframe.crc8code);
	__set_FAULTMASK(1);//Why warning? But compile successfully
	NVIC_SystemReset();
	PRIVTOCOL_PRINT("Reset\r\n");		
	return PRIVTOCOL_STATUS_OK;
}
/*!
*\brief  Command Message extract
*/
static uint8_t * Privtocol_cmdextract(uint8_t *message)
{
	uint8_t len=0;
	len = strlen((char *)message);
	uint8_t * ret_cmd;
	
	for(uint8_t i=0;i<len; i++)	
	{
		if((message[i] == 0xaa) && (message[i+1] == 0x55))
		{	
			ret_cmd = (uint8_t *)pf_malloc(i);
			memcpy(ret_cmd,message,i);
		}
	}
	return ret_cmd;
}
/*!
*\brief  UART received message processor
*/
static void Privtocol_RxMsgProcessor(uint8_t *message)
{
	uint16_t items;
	const PRIVTOCOL_RX_MSG_HANDLER *handlerPtr;	

	if(message[CMD_POS] == CMD_DEVICE_INFO)
	{
		handlerPtr = Info_HandlerTbl;
		items = sizeof(Info_HandlerTbl) / sizeof(PRIVTOCOL_RX_MSG_HANDLER);
	}
	if(message[CMD_POS] == CMD_SET_CTL)
	{
		handlerPtr = Ctl_HandlerTbl;
		items = sizeof(Ctl_HandlerTbl) / sizeof(PRIVTOCOL_RX_MSG_HANDLER);
	}
	if(message[CMD_POS] == CMD_GET_STATUS)
	{
		handlerPtr = Status_HandlerTbl;
		items = sizeof(Status_HandlerTbl) / sizeof(PRIVTOCOL_RX_MSG_HANDLER);
	}
	if(message[CMD_POS] == CMD_SYS)
	{
		handlerPtr = Sys_HandlerTbl;
		items = sizeof(Sys_HandlerTbl) / sizeof(PRIVTOCOL_RX_MSG_HANDLER);
	}	
	//walk through table to search message handler
	for (uint8_t i = 0 ; i < items; i++)
	{
		if (message[SUBCMD_POS] == handlerPtr[i].subcmd)
		{
			if (!handlerPtr[i].handler((char *)(Privtocol_cmdextract(message) + CMD_DATAARRAY_POS)))
			{
				//message has been proceeded, move forward
				//PRIVTOCOL_PRINT("i=%d,message[CMD_POS]:0x%x,message[SUBCMD_POS]:0x%x\r\n",i,message[CMD_POS],message[SUBCMD_POS]);
				break;
			}
		}
	}	
}
/*!
*\brief  CRC8 algrithm
*/
static uint8_t CRC_High_First(uint8_t *ptr, uint8_t len)
{
    uint8_t i=0; 
    uint8_t crc=0x00;  

    while(len--)
    {
        crc ^= *ptr++;    
        for (i=8; i>0; --i)  
        { 
            if (crc & 0x80)
			{
                crc = (crc << 1) ^ 0x31;
			}
            else
			{
                crc = (crc << 1);
        }
    }
	}

    return (crc); 
}

/*!
*\brief  restart protocol command receive
*/
static void Privtocol_RestartCmdRx(void)
{
	//flush UART RX
	HAL_UART_AbortReceive(&huart2);
	rxCmdLength = 0;
	rxState = RX_CMD_HEADER_STATE;
	HAL_UART_Receive_DMA(&huart2, rxBuffer, sizeof(PRIVTOCOL_CMD_HEADER));
}

/*!
*\brief  UART initialization for private protocol communication
*/
static void Privtocol_UartInit(void)
{
	Privtocol_RestartCmdRx();
}

/*!
*\brief  UART receiver completion callback handler
*\param [in] huart - uart handle
*/
void Privtocol_UartRxCpltHandler(UART_HandleTypeDef *huart)
{
	if (RX_CMD_HEADER_STATE == rxState)
	{
		if (FREAME_HEADER == (rxBuffer[0] + (rxBuffer[1] << 8)))
		{
			//continue to receive data array + crc8
			rxCmdLength = sizeof(PRIVTOCOL_CMD_HEADER);
			rxState = RX_CMD_DATA_STATE;
			HAL_UART_Receive_DMA(&huart2, rxBuffer + sizeof(PRIVTOCOL_CMD_HEADER), rxBuffer[sizeof(PRIVTOCOL_CMD_HEADER) - 1] + 1);
		}
		else
		{
			if (rxTaskHandle)
			{
				rxState = RX_IDLE_STATE;
				rxCmdLength = sizeof(PRIVTOCOL_CMD_HEADER);
				osSignalSet(rxTaskHandle, RX_CMD_DONE_EVENT);
			}
			else
			{
				Privtocol_RestartCmdRx();
			}
		}
	}
	else if (RX_CMD_DATA_STATE == rxState)
	{
		rxState = RX_IDLE_STATE;
		rxCmdLength += rxBuffer[sizeof(PRIVTOCOL_CMD_HEADER) - 1] + 1;
		osSignalSet(rxTaskHandle, RX_CMD_DONE_EVENT);
	}
}

/*!
*\brief  UART received message Analysis
*/
static void Privtocol_RxMsgAnalysis(uint8_t *message)
{
	uint8_t datalen = 0;
	uint8_t crc8_value = 0;
	uint8_t i = 0;
	
	do
	{
		if((message[i] == 0xaa) && (message[i+1] == 0x55))
		{
			datalen = message[i + CMD_DATALEN_POS];
			crc8_value = message[i + CMD_DATALEN_POS + datalen + 1];
			//PRIVTOCOL_PRINT("datalen:0x%x  crc8_rec:0x%x crc8_cal:0x%x\r\n", datalen,crc8_value, CRC_High_First((unsigned char *)&rxBuffer[i], CMD_DATALEN_POS + datalen));
			if(CRC_High_First(&message[i], CMD_DATALEN_POS + datalen) == crc8_value)
			{
				Privtocol_RxMsgProcessor(&message[i]);//TBD call function
			}
			else
			{
				PRIVTOCOL_PRINT("CMD is not correct:CRC8 ERROR\r\n");
				//break;
			}
		}
		i++;
	}while(message[i] != '\0');
}

/*!
*\brief  UART received message dispatcher
*/
static void Privtocol_RxMsgDispatcher(void)
{
#if 0
	PRIVTOCOL_PRINT("Rx Length: %d\n", rxCmdLength);

	for (uint16_t i = 0; i < rxCmdLength; i++)
	{
		PRIVTOCOL_PRINT("%c", rxBuffer[i]);
	}

	PRIVTOCOL_PRINT("\n");
#endif


	if (FREAME_HEADER == (rxBuffer[0] + (rxBuffer[1] << 8)))
	{
		//do crc check if neccessary and cmd dispatch
		Privtocol_RxMsgAnalysis(rxBuffer);
	}
	else
	{
		PRIVTOCOL_PRINT("Invalid Command Header\n");
	}

	//restart UART receiver
	Privtocol_RestartCmdRx();
}

/*!
*\brief Private protocol receiver task
*\param [in] argument - argument pointer
*/
void Privtocol_RxTask(void const *argument)
{
	osEvent event;
	rxTaskHandle = osThreadGetId();

	//intialize UART
	Privtocol_UartInit();
	/* Infinite loop */
	for (;;)
	{
		event = osSignalWait(0x7FFFFFFF, osWaitForever);

		if (event.status == osEventSignal)
		{
			if (event.value.signals & RX_CMD_DONE_EVENT)
			{
				Privtocol_RxMsgDispatcher();
			}
		}
	}
}
/*!
*\brief Private protocol initialization function
*/
void Privtocol_Init(void)
{
	osThreadDef(PrivtocolRxTask, Privtocol_RxTask, osPriorityBelowNormal, 0, 128);
	osThreadCreate(osThread(PrivtocolRxTask), NULL);
}

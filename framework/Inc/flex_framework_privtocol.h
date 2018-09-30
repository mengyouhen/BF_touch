/******************************************************************************
 Copyright (c) 2018 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		flex_framework_privtocol.h
 [Date]:
 		2018-08-14
 [Author]:
 		Joanna Wang
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_FRAMEWORK_PRIVTOCOL_H_
#define _FLEX_FRAMEWORK_PRIVTOCOL_H_
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
//Enable private protocol as server role
#define PRIVTOCOL_SERVER_SUPPORT


/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
typedef enum
{
	CMD_DEVICE_INFO,
	CMD_SET_CTL,
	CMD_GET_STATUS,
	CMD_SYS,
} PRIVTOCOL_CMD_LIST;
typedef enum
{
	SUBCMD_GET_PROT_VER = 0x1,
	SUBCMD_GET_DEVICE_ID,
	SUBCMD_SET_DEVICE_ID,
} PRIVTOCOL_SUBCMD_DEVINFO_LIST;
typedef enum
{
	SUBCMD_SET_LCD = 0x1,
	SUBCMD_SET_WIRELESS,
	SUBCMD_SET_BEEPER,
	SUBCMD_SET_TIME,
	SUBCMD_SET_LED,
} PRIVTOCOL_SUBCMD_SETCTL_LIST;
typedef enum
{
	SUBCMD_GET_LCD = 0x1,
	SUBCMD_GET_WIRELESS,
	SUBCMD_GET_BEEPER,
	SUBCMD_GET_TIME,
	SUBCMD_GET_LED,
	SUBCMD_GET_BTN,
} PRIVTOCOL_SUBCMD_GETSTUS_LIST;
typedef enum
{
	SUBCMD_SYS_SOFTRST = 0x1,
} PRIVTOCOL_SUBCMD_SYS_LIST;
typedef enum
{
	RET_OK,
	RET_FAILURE,
	RET_WRONG_CMD,
	RET_INVALID_HEADER,
	RET_WRONG_DEVINDEX,
	RET_WRONG_CHECKSUM,
} PRIVTOCOL_RET_LIST;
typedef enum
{
	DIRECT_FROM_HOST,
	DIRECT_FROM_DEVICE,
} PRIVTOCOL_CMD_DIRECTION;
typedef enum
{	
	PRIVTOCOL_STATUS_OK,
	PRIVTOCOL_STATUS_ERROR,	
	PRIVTOCOL_STATUS_TIMEOUT,
	PRIVTOCOL_STATUS_BUSY,
	PRIVTOCOL_STATUS_BUFFER_TOO_SMALL,
	PRIVTOCOL_STATUS_UNAVALIABLE,
	PRIVTOCOL_STATUS_MAX_STATUS,
} PRIVTOCOL_STATUS_LIST;

//typedef
typedef uint16_t PRIVTOCOL_FRAME_HEADER;
typedef uint8_t PRIVTOCOL_CMD;
typedef uint8_t PRIVTOCOL_SUBCMD;
typedef uint8_t PRIVTOCOL_LENGTH;
typedef uint8_t *PRIVTOCOL_DATA;
typedef uint8_t PRIVTOCOL_CRC8;
typedef char *PRIVTOCOL_COMMUNICATION_VERSION;
typedef char *PRIVTOCOL_DEVICE_ID;
typedef uint8_t PRIVTOCOL_RET_CODE;
typedef uint8_t PRIVTOCOL_INDEX;
typedef bool PRIVTOCOL_STATUS;
//CMD frame:  |frame header  |   CMD  |  Sub CMD  |  length  |      data array     |  CRC8  |
//RSP frame:  |frame header  |   CMD  |  Sub CMD  |  length  |  return code + data |  CRC8  |


#pragma push
#pragma pack(1)

typedef struct
{
	PRIVTOCOL_FRAME_HEADER header;
	PRIVTOCOL_CMD cmd;
	PRIVTOCOL_SUBCMD subcmd;
	PRIVTOCOL_LENGTH length;
} PRIVTOCOL_CMD_HEADER;


typedef struct
{
	PRIVTOCOL_FRAME_HEADER header;
	PRIVTOCOL_CMD cmd;
	PRIVTOCOL_SUBCMD subcmd;
	PRIVTOCOL_LENGTH length;
	PRIVTOCOL_DATA data;
	PRIVTOCOL_CRC8 crc8code;
}PRIVTOCOL_CMD_FRAME;
typedef struct
{
	PRIVTOCOL_FRAME_HEADER header;
	PRIVTOCOL_CMD cmd;
	PRIVTOCOL_SUBCMD subcmd;
	PRIVTOCOL_LENGTH length;
	PRIVTOCOL_DATA retdata;
	PRIVTOCOL_CRC8 crc8code;
}PRIVTOCOL_RSP_FRAME;
//Set Controls to device's peripherals
typedef struct
{
  PRIVTOCOL_INDEX index_number;  
  PRIVTOCOL_STATUS  bOn;         
  char* payload;      
}PRIVTOCOL_LCDCTL_DEF;

typedef struct
{
	PRIVTOCOL_INDEX index_number;         
	PRIVTOCOL_STATUS  bOn;      
}PRIVTOCOL_WIRELESSCTL_DEF;
typedef struct
{
	uint16_t pwm_freq;         
	PRIVTOCOL_STATUS  bOn;      
}PRIVTOCOL_BEEPERCTL_DEF;
typedef struct
{
	PRIVTOCOL_INDEX index_number;         
	PRIVTOCOL_STATUS  bOn;      
}PRIVTOCOL_LEDCTL_DEF;
//Get status from device's peripherals
typedef struct
{
  PRIVTOCOL_RET_CODE ret_code;
  PRIVTOCOL_INDEX index_number;  
	PRIVTOCOL_STATUS  bOn;         
}PRIVTOCOL_LCD_STATUS;
typedef struct
{
  PRIVTOCOL_RET_CODE ret_code;
  PRIVTOCOL_INDEX index_number;  
  PRIVTOCOL_STATUS  bOn;         
}PRIVTOCOL_WIRELESS_STATUS;
typedef struct
{
  PRIVTOCOL_RET_CODE ret_code;
  PRIVTOCOL_STATUS  bOn;         
  uint16_t  payload;      
}PRIVTOCOL_BEEPER_STATUS;
typedef struct
{
  PRIVTOCOL_RET_CODE ret_code;
	RTC_DateTypeDef Date;
	RTC_TimeTypeDef Time;      
}PRIVTOCOL_TIME_STATUS;
typedef struct
{
  PRIVTOCOL_RET_CODE ret_code;
  PRIVTOCOL_INDEX index_number;  
  PRIVTOCOL_STATUS  bOn;         
}PRIVTOCOL_LED_STATUS;
typedef struct
{
  PRIVTOCOL_RET_CODE ret_code;
  PRIVTOCOL_INDEX index_number;  
  PRIVTOCOL_STATUS  bOn;         
  uint32_t  payload;      
}PRIVTOCOL_BTN_STATUS;

#pragma pop

/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/


/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
void Privtocol_Init(void);
void Privtocol_UartRxCpltHandler(UART_HandleTypeDef *huart);
PRIVTOCOL_STATUS_LIST Privtocol_Get_Communication_Protocol_Version(char *buffer);
PRIVTOCOL_STATUS_LIST Privtocol_Get_Device_ID(char *buffer);
PRIVTOCOL_STATUS_LIST Privtocol_Set_Device_ID(char *buffer);
PRIVTOCOL_STATUS_LIST Privtocol_Control_Lcd(char *buffer);
PRIVTOCOL_STATUS_LIST Privtocol_Control_Wireless(char *buffer);
PRIVTOCOL_STATUS_LIST Privtocol_Control_Beeper(char *buffer);
PRIVTOCOL_STATUS_LIST Privtocol_Control_Time(char *buffer);
PRIVTOCOL_STATUS_LIST Privtocol_Control_Led(char *buffer);
PRIVTOCOL_STATUS_LIST Privtocol_Get_Lcd_Status(char * buffer);
PRIVTOCOL_STATUS_LIST Privtocol_Get_Wireless_Status(char * buffer);
PRIVTOCOL_STATUS_LIST Privtocol_Get_Beeper_Status(char *buffer);
PRIVTOCOL_STATUS_LIST Privtocol_Get_Time_Status(char *buffer);
PRIVTOCOL_STATUS_LIST Privtocol_Get_Btn_Status(char *buffer);
PRIVTOCOL_STATUS_LIST Privtocol_Get_Led_Status(char *buffer);
PRIVTOCOL_STATUS_LIST Privtocol_Sys_Softrst(char *buffer);
#endif

/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_drv_button.h
 [Date]:       
 		2017-12-20
 [Author]:   	
 		carson.qian 
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_DRV_BUTTON_H_
#define _FLEX_DRV_BUTTON_H_

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
#define BUTTON_NAME_SHIFT    8
#define BUTTON_ACT_SHIFT	   0



/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
typedef enum
{
	LIST_BTN_NONE,            // 0
//----------------
	LIST_BTN_0,               // 1
	LIST_BTN_1,               // 2
	LIST_BTN_2,               // 3
	LIST_BTN_3,               // 4
	LIST_BTN_4,               // 5
	LIST_BTN_5,               // 6
	LIST_BTN_6,               // 7
	LIST_BTN_7,               // 8
	LIST_BTN_8,               // 9
	LIST_BTN_9,               // 10
	LIST_BTN_OK,              // 11
	LIST_BTN_DEL,             // 12
//----------------
	LIST_BTN_UP,              // 13
	LIST_BTN_DOWN,            // 14
	LIST_BTN_LEFT,            // 15
	LIST_BTN_RIGHT,           // 16
//----------------
	LIST_BTN_GEAR_1,          // 17
	LIST_BTN_GEAR_2,          // 18
	LIST_BTN_GEAR_3,          // 19
	LIST_BTN_GEAR_4,          // 20
	LIST_BTN_GEAR_5,          // 21
	LIST_BTN_GEAR_6,          // 22
	LIST_BTN_GEAR_7,          // 23
	LIST_BTN_GEAR_8,          // 24
//----------------
	LIST_BTN_WIFI,            // 25
	LIST_BTN_BT,              // 26
//----------------
	LIST_BTN_PLAY,            // 27
	LIST_BTN_PAUSE,           // 28
//---------------- vkey start
	LIST_BTN_PAGE_UP,         // 29
	LIST_BTN_PAGE_DOWN,       // 30
	LIST_BTN_INC,             // 31
	LIST_BTN_DEC,             // 32
//---------------- vkey end
	LIST_BTN_MAX			
}DRV_BTN_ENUM;

typedef enum
{
	LIST_BTN_ACT_RELEASE,
	LIST_BTN_ACT_PRESS,
	LIST_BTN_ACT_MAX,
}DRV_BTN_ACT_ENUM;


typedef struct
{
	DRV_BTN_ENUM nBtn;
	DRV_BTN_ACT_ENUM nAct;  // short for action
}DRV_BTN_INFO_ST;


typedef void (*fpDrvBtnEvent)(DRV_BTN_INFO_ST);



/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/







/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
void Drv_Button_Init(void);






/*========================================================================================================
										End
========================================================================================================*/
#ifdef __cplusplus 
}
#endif

#endif//_FLEX_DRV_BUTTON_H_



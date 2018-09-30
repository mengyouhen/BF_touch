/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_framework_input.h
 [Date]:       
 		2018-06-13
 [Author]:   	
 		carson.qian 
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_FRAMEWORK_INPUT_H_
#define _FLEX_FRAMEWORK_INPUT_H_

#ifdef __cplusplus 
extern "C" {
#endif

/*========================================================================================================
										I N C L U D E
========================================================================================================*/
#include "flex_common.h"
#include "flex_drv_button.h"

/*========================================================================================================
										D E F I N E
========================================================================================================*/
typedef enum
{
	LIST_KEY_INVALID,   
//----------------
	LIST_KEY_0,         
	LIST_KEY_1,         
	LIST_KEY_2,         
	LIST_KEY_3,         
	LIST_KEY_4,         
	LIST_KEY_5,         
	LIST_KEY_6,         
	LIST_KEY_7,         
	LIST_KEY_8,         
	LIST_KEY_9,         
	LIST_KEY_OK,        
	LIST_KEY_DEL,       
//----------------
	LIST_KEY_UP,        
	LIST_KEY_DOWN,      
	LIST_KEY_LEFT,      
	LIST_KEY_RIGHT,     
//----------------
	LIST_KEY_GEAR_1,    
	LIST_KEY_GEAR_2,    
	LIST_KEY_GEAR_3,    
	LIST_KEY_GEAR_4,    
	LIST_KEY_GEAR_5,    
	LIST_KEY_GEAR_6,   
	LIST_KEY_GEAR_7,    
	LIST_KEY_GEAR_8,    
//----------------
	LIST_KEY_WIFI,      
	LIST_KEY_BT,        
//----------------
	LIST_KEY_PLAY,      
	LIST_KEY_PAUSE,     
//----------------
	LIST_KEY_PAGE_UP,   
	LIST_KEY_PAGE_DOWN, 
//----------------
	LIST_KEY_INC,       
	LIST_KEY_DEC, 
//----------------
	LIST_KEY_MAX       
}FRM_KEY_ENUM;


typedef enum
{
	LIST_KEY_ACT_RELEASE,
	LIST_KEY_ACT_PRESS,
	LIST_KEY_ACT_MAX
}FRM_KEY_ACTION_ENUM;


/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
typedef struct
{
	FRM_KEY_ENUM nKey;
	FRM_KEY_ACTION_ENUM nAction;
}FRM_INPUT_KEY_ST;

typedef struct
{
	uint8_t nStrength;
	uint16_t nXpos;
	uint16_t nYpos;
}FRM_INPUT_TOUCH_ST;


typedef void (*fpFrmInputKeyCallback)(FRM_INPUT_KEY_ST);
typedef void (*fpFrmInputTouchCallback)(FRM_INPUT_TOUCH_ST);

/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/







/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
void Frm_Input_Task(void const * argument);
void Frm_Input_Button_Callback(DRV_BTN_INFO_ST btn_info); // for driver use
void Frm_Input_Key_Register_Callback(fpFrmInputKeyCallback func_ptr); // for app use
void Frm_Input_Touch_Register_Callback(fpFrmInputTouchCallback func_ptr); // for app use





/*========================================================================================================
										End
========================================================================================================*/
#ifdef __cplusplus 
}
#endif

#endif//_FLEX_FRAMEWORK_INPUT_H_



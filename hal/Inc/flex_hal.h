/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_hal.h
 [Date]:       
 		2017-12-20
 [Author]:   	
 		carson.qian 
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_HAL_H_
#define _FLEX_HAL_H_

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





/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
//========================================
// GPIO
//========================================
typedef struct
{
	GPIO_TypeDef* nPort;
	uint16_t nPin;
}flex_gpio_st;

typedef enum
{
	LIST_GPIO_ACTION_LOW,
	LIST_GPIO_ACTION_HIGH,
	LIST_GPIO_ACTION_TOGGLE,
//----------------		
	LIST_GPIO_ACTION_MAX
}flex_gpio_action_enum;

typedef enum
{
	LIST_GPIO_STATUS_LOW,
	LIST_GPIO_STATUS_HIGH,
//----------------		
	LIST_GPIO_STATUS_MAX
}flex_gpio_status_enum;

//========================================
// TIMER
//========================================
typedef enum
{
	TIMER_ID_NONE = 0,
//--------------------------
// custom start
//--------------------------
	TIMER_ID_CUSTOM_1,
	TIMER_ID_CUSTOM_2,
	TIMER_ID_CUSTOM_3,
	TIMER_ID_CUSTOM_4,
	
	TIMER_ID_LOGO,
	TIMER_ID_ANIMATION,
//--------------------------
// custom end
//--------------------------
	TIMER_ID_MAX  
}FLEX_TIMER_ID_ENUM;



typedef void (*Timer_pFunc) (void const * argument);





/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/







/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
//========================================
// GPIO
//========================================
//void Flex_Hal_Gpio_Init(flex_gpio_st* pGpio, bool level);
void Flex_Hal_Gpio_Ctrl(const flex_gpio_st* pGpio, flex_gpio_action_enum action);
bool Flex_Hal_Gpio_Get_Status(const flex_gpio_st* pGpio);

//========================================
// TIMER
//========================================
bool IsMyTimerExist(FLEX_TIMER_ID_ENUM nTimerId);
void StartTimer(FLEX_TIMER_ID_ENUM timerid, uint32_t delay, Timer_pFunc funcPtr);
void StopTimer(FLEX_TIMER_ID_ENUM timerid);


//========================================
// INIT
//========================================
void Flex_Hal_Init(void);






/*========================================================================================================
										End
========================================================================================================*/
#ifdef __cplusplus 
}
#endif

#endif//_FLEX_DUMMY_H_



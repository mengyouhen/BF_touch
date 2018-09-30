/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_hal.c
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

// internal
#include "flex_hal.h"

/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME								"[HAL]--"
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
//========================================
// GPIO
//========================================
/*
void Flex_Hal_Gpio_Init(flex_gpio_st* pGpio, bool level)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};
	
	if(!pGpio)
	{
		TRACE_E("[%s]--para is invalid--return.\r\n", __func__);		
		return;
	}
	
	//----------------------------------------			
		if(GPIOA==pGpio->nPort)			{__HAL_RCC_GPIOA_CLK_ENABLE();		TRACE_I("[%s]--Info--GPIO_port is GPIOA.\r\n", __func__);	}
		else if(GPIOB==pGpio->nPort)	{__HAL_RCC_GPIOB_CLK_ENABLE();		TRACE_I("[%s]--Info--GPIO_port is GPIOB.\r\n", __func__);	}
		else if(GPIOC==pGpio->nPort)	{__HAL_RCC_GPIOC_CLK_ENABLE();		TRACE_I("[%s]--Info--GPIO_port is GPIOC.\r\n", __func__);	}
#if defined(GPIOD)
		else if(GPIOD==pGpio->nPort)	{__HAL_RCC_GPIOD_CLK_ENABLE();		TRACE_I("[%s]--Info--GPIO_port is GPIOD.\r\n", __func__);	}
#endif
#if defined(GPIOE)
		else if(GPIOE==pGpio->nPort)	{__HAL_RCC_GPIOE_CLK_ENABLE();		TRACE_I("[%s]--Info--GPIO_port is GPIOE.\r\n", __func__);	}
#endif
#if defined(GPIOF)
		else if(GPIOF==pGpio->nPort)	{__HAL_RCC_GPIOF_CLK_ENABLE();		TRACE_I("[%s]--Info--GPIO_port is GPIOF.\r\n", __func__);	}
#endif
#if defined(GPIOG)
		else if(GPIOG==pGpio->nPort)	{__HAL_RCC_GPIOG_CLK_ENABLE();		TRACE_I("[%s]--Info--GPIO_port is GPIOG.\r\n", __func__);	}
#endif
		else if(GPIOH==pGpio->nPort)	{__HAL_RCC_GPIOH_CLK_ENABLE();		TRACE_I("[%s]--Info--GPIO_port is GPIOH.\r\n", __func__);	}
#if defined(GPIOI)
		else if(GPIOI==pGpio->nPort)	{__HAL_RCC_GPIOI_CLK_ENABLE();		TRACE_I("[%s]--Info--GPIO_port is GPIOI.\r\n", __func__);	}
#endif
		else
		{
			//TRACE_W("[%s]--GPIO_port is invalid.\r\n", __func__);		
		}
	//----------------------------------------			
		if((pGpio->nPort!=NULL)&&(pGpio->nPin!=0))
		{
			//Configure GPIO pin Output Level 
			HAL_GPIO_WritePin(pGpio->nPort, pGpio->nPin, (level? GPIO_PIN_SET: GPIO_PIN_RESET));
			//Configure GPIO pin : PtPin
			GPIO_InitStruct.Pin = pGpio->nPin;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			HAL_GPIO_Init(pGpio->nPort, &GPIO_InitStruct);
		}
}
*/

void Flex_Hal_Gpio_Ctrl(const flex_gpio_st* pGpio, flex_gpio_action_enum action)
{
	if((pGpio->nPort!=NULL)&&(pGpio->nPin!=0))
	{
		switch(action)
		{
			case LIST_GPIO_ACTION_HIGH:				
				HAL_GPIO_WritePin(pGpio->nPort, pGpio->nPin, GPIO_PIN_SET);			
				break;

			case LIST_GPIO_ACTION_LOW:			
				HAL_GPIO_WritePin(pGpio->nPort, pGpio->nPin, GPIO_PIN_RESET);			
				break;				
				
			case LIST_GPIO_ACTION_TOGGLE:		
				HAL_GPIO_TogglePin(pGpio->nPort, pGpio->nPin);										
				break;
				
			default:
				break;
		}
	}
}

bool Flex_Hal_Gpio_Get_Status(const flex_gpio_st* pGpio)
{
	GPIO_PinState status;

	status=HAL_GPIO_ReadPin(pGpio->nPort, pGpio->nPin);
	switch(status)
	{
  	case GPIO_PIN_RESET:		return false;
  	case GPIO_PIN_SET: 		return true;
		default:
			return false;
	}
}



//========================================
// TIMER
//========================================
typedef struct
{
	FLEX_TIMER_ID_ENUM nTimerId;
	osTimerId nHandler;
	bool nExist;
}FLEX_TIMER_LIST_ST;








/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
//========================================
// GPIO
//========================================



//========================================
// TIMER
//========================================
static FLEX_TIMER_LIST_ST g_timer_array[TIMER_ID_MAX];   







/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
//========================================
// GPIO
//========================================



//========================================
// TIMER
//========================================
bool IsMyTimerExist(FLEX_TIMER_ID_ENUM timerid)
{
	return 	g_timer_array[timerid].nExist;
}

void StartTimer(FLEX_TIMER_ID_ENUM timerid, uint32_t delay, Timer_pFunc funcPtr)
{
	if(IsMyTimerExist(timerid))
	{
		StopTimer(timerid);  
	}
	osTimerDef(timerid, funcPtr);
	g_timer_array[timerid].nHandler= osTimerCreate(osTimer(timerid), osTimerOnce, NULL); //osTimerOnce osTimerPeriodic
	g_timer_array[timerid].nExist = true;
	osTimerStart(g_timer_array[timerid].nHandler, delay);
}

void StopTimer(FLEX_TIMER_ID_ENUM timerid)
{
	osTimerStop(g_timer_array[timerid].nHandler);
	g_timer_array[timerid].nExist = false;
}

static void Flex_Hal_Timer_Init(void)
{
	FLEX_TIMER_ID_ENUM index;
/*
	for(index=TIMER_ID_NONE; index<TIMER_ID_MAX; index++)
	{
		TRACE_I("index=%d, nTimerId=%d, nExist=%d, nHandler=%d.\r\n", \
			                                         index,\
			                                         g_timer_array[index].nTimerId,\
			                                         g_timer_array[index].nExist, \
			                                         g_timer_array[index].nHandler);
	}
*/	
	for(index=TIMER_ID_NONE; index<TIMER_ID_MAX; index++)
	{
		g_timer_array[index].nTimerId=TIMER_ID_NONE;
		g_timer_array[index].nExist=false;
		g_timer_array[index].nHandler=NULL;
	}
}




//========================================
// INIT
//========================================
void Flex_Hal_Init(void)
{
	Flex_Hal_Timer_Init();
}


/*========================================================================================================
										End
========================================================================================================*/




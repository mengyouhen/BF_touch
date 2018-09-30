/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_drv_eint.c
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
#include "flex_drv_button.h" 
#include "flex_app_segdisp.h"
#include "flex_framework_esp32.h"
#include "flex_drv_captouch_mpr121.h"
// internal
#include "flex_drv_eint.h"
/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME								"[EINT]--"
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
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	//int32_t send_value=0;
	//int32_t ret=0;
	
	switch(GPIO_Pin)
	{
	#if (BOARD_SELECT==BOARD_STM32L476RGT6_NUCLEO)
		case BUTTON_BLUE_Pin:
			break;
	#elif (BOARD_SELECT==BOARD_STM32L476ZET6_HMI_BUTTERFLY)
			// TODO:
	#endif
		//WIFI/BT
		case MS_BTN2_Pin:
		case MS_BTN1_Pin:
			ESP32_InputHandler(GPIO_Pin);
			break;		
		case MECH_BTN1_Pin:		//Start
		case MECH_BTN2_Pin:		//Stop
		case MCU_RotaryEncoder_PB_Pin:	//RotaryEncoder_PB
		case MCU_RotaryEncoder_A_Pin:		//RotaryEncoder_A			
		case MCU_RotaryEncoder_B_Pin:		//RotaryEncoder_B
		  mech_btn_eint(GPIO_Pin);
			break;		
		case MPR_INT_Pin:		
		  mpr_press_detect();
			break;
		default:
			//TRACE_E("goto default.\r\n");
			break;
	}
}






/*========================================================================================================
										End
========================================================================================================*/




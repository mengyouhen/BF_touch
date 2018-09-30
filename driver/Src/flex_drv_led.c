/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_drv_led.c
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
#include "flex_drv_captouch.h"
#include "flex_hal.h"


// internal
#include "flex_drv_led.h"



/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME								"[LED]--"
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
//------------------------------------------
// LED for Gear
//------------------------------------------
// PG8
#define LED_GEAR_1_ON             (GPIOG->BRR = GPIO_BRR_BR_8)
#define LED_GEAR_1_OFF            (GPIOG->BSRR = GPIO_BSRR_BS_8)
#define LED_GEAR_1_TOGGLE         (GPIOG->ODR ^= GPIO_ODR_OD8)
#define LED_GEAR_1_STATUS         (((GPIOG->ODR&GPIO_ODR_OD8) >> GPIO_ODR_OD8_Pos)? false : true )   
// PG4
#define LED_GEAR_2_ON             (GPIOG->BRR = GPIO_BRR_BR_4)
#define LED_GEAR_2_OFF            (GPIOG->BSRR = GPIO_BSRR_BS_4)
#define LED_GEAR_2_TOGGLE         (GPIOG->ODR ^= GPIO_ODR_OD4)
#define LED_GEAR_2_STATUS         (((GPIOG->ODR&GPIO_ODR_OD4) >> GPIO_ODR_OD4_Pos)? false : true )   
// PG6
#define LED_GEAR_3_ON             (GPIOG->BRR = GPIO_BRR_BR_6)
#define LED_GEAR_3_OFF            (GPIOG->BSRR = GPIO_BSRR_BS_6)
#define LED_GEAR_3_TOGGLE         (GPIOG->ODR ^= GPIO_ODR_OD6)
#define LED_GEAR_3_STATUS         (((GPIOG->ODR&GPIO_ODR_OD6) >> GPIO_ODR_OD6_Pos)? false : true )   
// PG15
#define LED_GEAR_4_ON             (GPIOG->BRR = GPIO_BRR_BR_15)
#define LED_GEAR_4_OFF            (GPIOG->BSRR = GPIO_BSRR_BS_15)
#define LED_GEAR_4_TOGGLE         (GPIOG->ODR ^= GPIO_ODR_OD15)
#define LED_GEAR_4_STATUS         (((GPIOG->ODR&GPIO_ODR_OD15) >> GPIO_ODR_OD15_Pos)? false : true )   
// PG7
#define LED_GEAR_5_ON             (GPIOG->BRR = GPIO_BRR_BR_7)
#define LED_GEAR_5_OFF            (GPIOG->BSRR = GPIO_BSRR_BS_7)
#define LED_GEAR_5_TOGGLE         (GPIOG->ODR ^= GPIO_ODR_OD7)
#define LED_GEAR_5_STATUS         (((GPIOG->ODR&GPIO_ODR_OD7) >> GPIO_ODR_OD7_Pos)? false : true )  
// PC4
#define LED_GEAR_6_ON             (GPIOC->BRR = GPIO_BRR_BR_4)
#define LED_GEAR_6_OFF            (GPIOC->BSRR = GPIO_BSRR_BS_4)
#define LED_GEAR_6_TOGGLE         (GPIOC->ODR ^= GPIO_ODR_OD4)
#define LED_GEAR_6_STATUS         (((GPIOC->ODR&GPIO_ODR_OD4) >> GPIO_ODR_OD4_Pos)? false : true )   
// PC5
#define LED_GEAR_7_ON             (GPIOC->BRR = GPIO_BRR_BR_5)
#define LED_GEAR_7_OFF            (GPIOC->BSRR = GPIO_BSRR_BS_5)
#define LED_GEAR_7_TOGGLE         (GPIOC->ODR ^= GPIO_ODR_OD5)
#define LED_GEAR_7_STATUS         (((GPIOC->ODR&GPIO_ODR_OD5) >> GPIO_ODR_OD5_Pos)? false : true )   
// PD8
#define LED_GEAR_8_ON             (GPIOD->BRR = GPIO_BRR_BR_8)
#define LED_GEAR_8_OFF            (GPIOD->BSRR = GPIO_BSRR_BS_8)
#define LED_GEAR_8_TOGGLE         (GPIOD->ODR ^= GPIO_ODR_OD8)
#define LED_GEAR_8_STATUS         (((GPIOD->ODR&GPIO_ODR_OD8) >> GPIO_ODR_OD8_Pos)? false : true )   

//------------------------------------------
// LED for play and stop
//------------------------------------------
// PG0  
#define LED_PLAY_ON     					(GPIOG->BRR = GPIO_BRR_BR_0)
#define LED_PLAY_OFF       				(GPIOG->BSRR = GPIO_BSRR_BS_0)
#define LED_PLAY_TOGGLE    				(GPIOG->ODR ^= GPIO_ODR_OD0)
#define LED_PLAY_STATUS     			(((GPIOG->ODR&GPIO_ODR_OD0) >> GPIO_ODR_OD0_Pos)? false : true )   
// PB6  
#define LED_STOP_ON     					(GPIOB->BRR = GPIO_BRR_BR_6)
#define LED_STOP_OFF    					(GPIOB->BSRR = GPIO_BSRR_BS_6)
#define LED_STOP_TOGGLE   				(GPIOB->ODR ^= GPIO_ODR_OD6)
#define LED_STOP_STATUS  					(((GPIOB->ODR&GPIO_ODR_OD6) >> GPIO_ODR_OD6_Pos)? false : true )   

//------------------------------------------
// LED for - and +
//------------------------------------------
// PG3
#define LED_MINUS_ON              (GPIOG->BRR = GPIO_BRR_BR_3)
#define LED_MINUS_OFF             (GPIOG->BSRR = GPIO_BSRR_BS_3)
#define LED_MINUS_TOGGLE          (GPIOG->ODR ^= GPIO_ODR_OD3)
#define LED_MINUS_STATUS          (((GPIOG->ODR&GPIO_ODR_OD3) >> GPIO_ODR_OD3_Pos)? false : true )   
// PD4
#define LED_ADD_ON                (GPIOD->BRR = GPIO_BRR_BR_4)
#define LED_ADD_OFF               (GPIOD->BSRR = GPIO_BSRR_BS_4)
#define LED_ADD_TOGGLE            (GPIOD->ODR ^= GPIO_ODR_OD4)
#define LED_ADD_STATUS            (((GPIOD->ODR&GPIO_ODR_OD4) >> GPIO_ODR_OD4_Pos)? false : true )   

//------------------------------------------
// LED for WiFi and BT
//------------------------------------------
// PA5
#define LED_WIFI_ON  							(GPIOA->BRR = GPIO_BRR_BR_5)
#define LED_WIFI_OFF        			(GPIOA->BSRR = GPIO_BSRR_BS_5)
#define LED_WIFI_TOGGLE        		(GPIOA->ODR ^= GPIO_ODR_OD5)
#define LED_WIFI_STATUS          	(((GPIOA->ODR&GPIO_ODR_OD5) >> GPIO_ODR_OD5_Pos)? false : true ) 
// PA4
#define LED_BT_ON         				(GPIOA->BRR = GPIO_BRR_BR_4)
#define LED_BT_OFF        				(GPIOA->BSRR = GPIO_BSRR_BS_4)
#define LED_BT_TOGGLE  						(GPIOA->ODR ^= GPIO_ODR_OD4)
#define LED_BT_STATUS          		(((GPIOA->ODR&GPIO_ODR_OD4) >> GPIO_ODR_OD4_Pos)? false : true ) 

//------------------------------------------
// LED for Wheel
//------------------------------------------
// PC13
#define LED_WHEEL_1_ON            (GPIOC->BRR = GPIO_BRR_BR_13)
#define LED_WHEEL_1_OFF           (GPIOC->BSRR = GPIO_BSRR_BS_13)
#define LED_WHEEL_1_TOGGLE        (GPIOC->ODR ^= GPIO_ODR_OD13)
#define LED_WHEEL_1_STATUS        (((GPIOC->ODR&GPIO_ODR_OD13) >> GPIO_ODR_OD13_Pos)? false : true )  
// PB11
#define LED_WHEEL_2_ON            (GPIOB->BRR = GPIO_BRR_BR_11)
#define LED_WHEEL_2_OFF           (GPIOB->BSRR = GPIO_BSRR_BS_11)
#define LED_WHEEL_2_TOGGLE        (GPIOB->ODR ^= GPIO_ODR_OD11)
#define LED_WHEEL_2_STATUS        (((GPIOB->ODR&GPIO_ODR_OD11) >> GPIO_ODR_OD11_Pos)? false : true ) 
// PB8
#define LED_WHEEL_3_ON            (GPIOB->BRR = GPIO_BRR_BR_8)
#define LED_WHEEL_3_OFF           (GPIOB->BSRR = GPIO_BSRR_BS_8)
#define LED_WHEEL_3_TOGGLE        (GPIOB->ODR ^= GPIO_ODR_OD8)
#define LED_WHEEL_3_STATUS        (((GPIOB->ODR&GPIO_ODR_OD8) >> GPIO_ODR_OD8_Pos)? false : true )   
// PB2
#define LED_WHEEL_4_ON            (GPIOB->BRR = GPIO_BRR_BR_2)
#define LED_WHEEL_4_OFF           (GPIOB->BSRR = GPIO_BSRR_BS_2)
#define LED_WHEEL_4_TOGGLE        (GPIOB->ODR ^= GPIO_ODR_OD2)
#define LED_WHEEL_4_STATUS        (((GPIOB->ODR&GPIO_ODR_OD2) >> GPIO_ODR_OD2_Pos)? false : true )   
// PA15
#define LED_WHEEL_5_ON            (GPIOA->BRR = GPIO_BRR_BR_15)
#define LED_WHEEL_5_OFF           (GPIOA->BSRR = GPIO_BSRR_BS_15)
#define LED_WHEEL_5_TOGGLE        (GPIOA->ODR ^= GPIO_ODR_OD15)
#define LED_WHEEL_5_STATUS        (((GPIOA->ODR&GPIO_ODR_OD15) >> GPIO_ODR_OD15_Pos)? false : true )  
// PA14
#define LED_WHEEL_6_ON            (GPIOA->BRR = GPIO_BRR_BR_14)
#define LED_WHEEL_6_OFF           (GPIOA->BSRR = GPIO_BSRR_BS_14)
#define LED_WHEEL_6_TOGGLE        (GPIOA->ODR ^= GPIO_ODR_OD14)
#define LED_WHEEL_6_STATUS        (((GPIOA->ODR&GPIO_ODR_OD14) >> GPIO_ODR_OD14_Pos)? false : true )  
// PA13
#define LED_WHEEL_7_ON            (GPIOA->BRR = GPIO_BRR_BR_13)
#define LED_WHEEL_7_OFF           (GPIOA->BSRR = GPIO_BSRR_BS_13)
#define LED_WHEEL_7_TOGGLE        (GPIOA->ODR ^= GPIO_ODR_OD13)
#define LED_WHEEL_7_STATUS        (((GPIOA->ODR&GPIO_ODR_OD13) >> GPIO_ODR_OD13_Pos)? false : true )  

//------------------------------------------
// LED for Slide
//------------------------------------------
// PD13
#define LED_SLIDE_1_ON            (GPIOD->BRR = GPIO_BRR_BR_13)
#define LED_SLIDE_1_OFF           (GPIOD->BSRR = GPIO_BSRR_BS_13)
#define LED_SLIDE_1_TOGGLE        (GPIOD->ODR ^= GPIO_ODR_OD13)
#define LED_SLIDE_1_STATUS        (((GPIOD->ODR&GPIO_ODR_OD13) >> GPIO_ODR_OD13_Pos)? false : true )   
// PG1
#define LED_SLIDE_2_ON            (GPIOG->BRR = GPIO_BRR_BR_1)
#define LED_SLIDE_2_OFF           (GPIOG->BSRR = GPIO_BSRR_BS_1)
#define LED_SLIDE_2_TOGGLE        (GPIOG->ODR ^= GPIO_ODR_OD1)
#define LED_SLIDE_2_STATUS        (((GPIOG->ODR&GPIO_ODR_OD1) >> GPIO_ODR_OD1_Pos)? false : true )   
// PD1
#define LED_SLIDE_3_ON            (GPIOD->BRR = GPIO_BRR_BR_1)
#define LED_SLIDE_3_OFF           (GPIOD->BSRR = GPIO_BSRR_BS_1)
#define LED_SLIDE_3_TOGGLE        (GPIOD->ODR ^= GPIO_ODR_OD1)
#define LED_SLIDE_3_STATUS        (((GPIOD->ODR&GPIO_ODR_OD1) >> GPIO_ODR_OD1_Pos)? false : true )  
// PD0
#define LED_SLIDE_4_ON            (GPIOD->BRR = GPIO_BRR_BR_0)
#define LED_SLIDE_4_OFF           (GPIOD->BSRR = GPIO_BSRR_BS_0)
#define LED_SLIDE_4_TOGGLE        (GPIOD->ODR ^= GPIO_ODR_OD0)
#define LED_SLIDE_4_STATUS        (((GPIOD->ODR&GPIO_ODR_OD0) >> GPIO_ODR_OD0_Pos)? false : true )  

//------------------------------------------
// LED for Direction Key
//------------------------------------------
// PF7
#define LED_KEY_UP_ON           	(GPIOF->BRR = GPIO_BRR_BR_7)
#define LED_KEY_UP_OFF          	(GPIOF->BSRR = GPIO_BSRR_BS_7)
#define LED_KEY_UP_TOGGLE       	(GPIOF->ODR ^= GPIO_ODR_OD7)
#define LED_KEY_UP_STATUS       	(((GPIOF->ODR&GPIO_ODR_OD7) >> GPIO_ODR_OD7_Pos)? false : true )  
// PF8
#define LED_KEY_LEFT_ON         	(GPIOF->BRR = GPIO_BRR_BR_8)
#define LED_KEY_LEFT_OFF        	(GPIOF->BSRR = GPIO_BSRR_BS_8)
#define LED_KEY_LEFT_TOGGLE    		(GPIOF->ODR ^= GPIO_ODR_OD8)
#define LED_KEY_LEFT_STATUS     	(((GPIOF->ODR&GPIO_ODR_OD8) >> GPIO_ODR_OD8_Pos)? false : true )  
// PF9
#define LED_KEY_RIGHT_ON        	(GPIOF->BRR = GPIO_BRR_BR_9)
#define LED_KEY_RIGHT_OFF       	(GPIOF->BSRR = GPIO_BSRR_BS_9)
#define LED_KEY_RIGHT_TOGGLE    	(GPIOF->ODR ^= GPIO_ODR_OD9)
#define LED_KEY_RIGHT_STATUS    	(((GPIOF->ODR&GPIO_ODR_OD9) >> GPIO_ODR_OD9_Pos)? false : true ) 
// PF10(v1.0)---> PC2(v2.0)
#define LED_KEY_DOWN_ON         	(GPIOC->BRR = GPIO_BRR_BR_2)
#define LED_KEY_DOWN_OFF        	(GPIOC->BSRR = GPIO_BSRR_BS_2)
#define LED_KEY_DOWN_TOGGLE     	(GPIOC->ODR ^= GPIO_ODR_OD2)
#define LED_KEY_DOWN_STATUS    		(((GPIOC->ODR&GPIO_ODR_OD2) >> GPIO_ODR_OD2_Pos)? false : true )  

//------------------------------------------
// LED for KEY_OK of Matrix Keypad
//------------------------------------------
// PD7
#define LED_KEY_OK_ON             (GPIOD->BRR = GPIO_BRR_BR_7)
#define LED_KEY_OK_OFF            (GPIOD->BSRR = GPIO_BSRR_BS_7)
#define LED_KEY_OK_TOGGLE         (GPIOD->ODR ^= GPIO_ODR_OD7)
#define LED_KEY_OK_STATUS         (((GPIOD->ODR&GPIO_ODR_OD7) >> GPIO_ODR_OD7_Pos)? false : true )  
 
/*========================================================================================================
										T Y P E D E F
========================================================================================================*/







/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/






/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
void Drv_Led_Ctrl(flex_led_name_enum led_name, bool on)
{
	switch(led_name)
	{
	#if (PROJECT_SELECT==PROJECT_HMI_BUTTERFLY)
		case LIST_LED_GEAR_1:		
			(on ? LED_GEAR_1_ON : LED_GEAR_1_OFF);
			break;
			
		case LIST_LED_GEAR_2:		
			(on ? LED_GEAR_2_ON : LED_GEAR_2_OFF);
			break;
			
		case LIST_LED_GEAR_3:		
			(on ? LED_GEAR_3_ON : LED_GEAR_3_OFF);
			break;
			
		case LIST_LED_GEAR_4:		
			(on ? LED_GEAR_4_ON : LED_GEAR_4_OFF);
			break;
			
		case LIST_LED_GEAR_5:		
			(on ? LED_GEAR_5_ON : LED_GEAR_5_OFF);
			break;
			
		case LIST_LED_GEAR_6:		
			(on ? LED_GEAR_6_ON : LED_GEAR_6_OFF);
			break;
			
		case LIST_LED_GEAR_7:		
			(on ? LED_GEAR_7_ON : LED_GEAR_7_OFF);
			break;
			
		case LIST_LED_GEAR_8:		
			(on ? LED_GEAR_8_ON : LED_GEAR_8_OFF);
			break;

	//----------------
		case LIST_LED_PLAY:			
			(on ? LED_PLAY_ON : LED_PLAY_OFF);
			break;
			
		case LIST_LED_STOP:			
			(on ? LED_STOP_ON : LED_STOP_OFF);
			break;
	//----------------
		case LIST_LED_MINUS:			
			(on ? LED_MINUS_ON : LED_MINUS_OFF);
			break;
			
		case LIST_LED_ADD:				
			(on ? LED_ADD_ON : LED_ADD_OFF);
			break;
	//----------------
		case LIST_LED_WIFI:	
			(on ? LED_WIFI_ON : LED_WIFI_OFF);
			break;
			
		case LIST_LED_BT:				
			(on ? LED_BT_ON : LED_BT_OFF);
			break;
			
	//----------------
		case LIST_LED_WHEEL_1:		
			(on ? LED_WHEEL_1_ON : LED_WHEEL_1_OFF);
			break;
			
		case LIST_LED_WHEEL_2:		
			(on ? LED_WHEEL_2_ON : LED_WHEEL_2_OFF);
			break;
			
		case LIST_LED_WHEEL_3:		
			(on ? LED_WHEEL_3_ON : LED_WHEEL_3_OFF);
			break;
			
		case LIST_LED_WHEEL_4:		
			(on ? LED_WHEEL_4_ON : LED_WHEEL_4_OFF);
			break;
			
		case LIST_LED_WHEEL_5:		
			(on ? LED_WHEEL_5_ON : LED_WHEEL_5_OFF);
			break;
			
		case LIST_LED_WHEEL_6:		
			(on ? LED_WHEEL_6_ON : LED_WHEEL_6_OFF);
			break;
			
		case LIST_LED_WHEEL_7:		
			(on ? LED_WHEEL_7_ON : LED_WHEEL_7_OFF);
			break;
			
	//----------------
		case LIST_LED_UP:				
			(on ? LED_KEY_UP_ON : LED_KEY_UP_OFF);
			break;
			
		case LIST_LED_LEFT:			
			(on ? LED_KEY_LEFT_ON: LED_KEY_LEFT_OFF);
			break;	
			
		case LIST_LED_RIGHT:			
			(on ? LED_KEY_RIGHT_ON: LED_KEY_RIGHT_OFF);
			break;
			
		case LIST_LED_DOWN:			
			(on ? LED_KEY_DOWN_ON: LED_KEY_DOWN_OFF);
			break;
			
	//----------------
		case LIST_LED_SLIDE_1:		
			(on ? LED_SLIDE_1_ON: LED_SLIDE_1_OFF);
			break;
			
		case LIST_LED_SLIDE_2:		
			(on ? LED_SLIDE_2_ON: LED_SLIDE_2_OFF);
			break;
			
		case LIST_LED_SLIDE_3:		
			(on ? LED_SLIDE_3_ON: LED_SLIDE_3_OFF);
			break;
			
		case LIST_LED_SLIDE_4:		
			(on ? LED_SLIDE_4_ON: LED_SLIDE_4_OFF);
			break;
			
	//----------------
		case LIST_LED_OK:				
			(on ? LED_KEY_OK_ON: LED_KEY_OK_OFF);
			break;
			
	//----------------
		case LIST_LED_KEY0:	
		case LIST_LED_KEY1:		
		case LIST_LED_KEY2:		
		case LIST_LED_KEY3:		
		case LIST_LED_KEY4:	
		case LIST_LED_KEY5:		
		case LIST_LED_KEY6:		
		case LIST_LED_KEY7:		
		case LIST_LED_KEY8:	
		case LIST_LED_KEY9:
		case LIST_LED_DEL:
			Drv_CapTouch_Set_Led(led_name, on);
			break;
	#endif		
		default:
			break;
	}
}

bool Drv_Led_Get_Status(flex_led_name_enum led_name)
{
	bool bRet=false;
	
	switch(led_name)
	{
	#if (PROJECT_SELECT==PROJECT_HMI_BUTTERFLY)
		case LIST_LED_GEAR_1:		
			bRet=LED_GEAR_1_STATUS;		
			break;
			
		case LIST_LED_GEAR_2:	
			bRet=LED_GEAR_2_STATUS;		
			break;
			
		case LIST_LED_GEAR_3:		
			bRet=LED_GEAR_3_STATUS;		
			break;
			
		case LIST_LED_GEAR_4:		
			bRet=LED_GEAR_4_STATUS;		
			break;
			
		case LIST_LED_GEAR_5:		
			bRet=LED_GEAR_5_STATUS;		
			break;
			
		case LIST_LED_GEAR_6:		
			bRet=LED_GEAR_6_STATUS;		
			break;
			
		case LIST_LED_GEAR_7:		
			bRet=LED_GEAR_7_STATUS;		
			break;
			
		case LIST_LED_GEAR_8:		
			bRet=LED_GEAR_8_STATUS;		
			break;

	//----------------
		case LIST_LED_PLAY:			
			bRet=LED_PLAY_STATUS;		
			break;
			
		case LIST_LED_STOP:			
			bRet=LED_STOP_STATUS;		
			break;
			
	//----------------
		case LIST_LED_MINUS:			
			bRet=LED_MINUS_STATUS;		
			break;
			
		case LIST_LED_ADD:				
			bRet=LED_ADD_STATUS;			
			break;
			
	//----------------
		case LIST_LED_WIFI:			
			bRet=LED_WIFI_STATUS;			
			break;
			
		case LIST_LED_BT:				
			bRet=LED_BT_STATUS;			
			break;
			
	//----------------
		case LIST_LED_WHEEL_1:		
			bRet=LED_WHEEL_1_STATUS;		
			break;
			
		case LIST_LED_WHEEL_2:		
			bRet=LED_WHEEL_2_STATUS;		
			break;
			
		case LIST_LED_WHEEL_3:		
			bRet=LED_WHEEL_3_STATUS;		
			break;
			
		case LIST_LED_WHEEL_4:		
			bRet=LED_WHEEL_4_STATUS;		
			break;
			
		case LIST_LED_WHEEL_5:		
			bRet=LED_WHEEL_5_STATUS;		
			break;
			
		case LIST_LED_WHEEL_6:		
			bRet=LED_WHEEL_6_STATUS;		
			break;
			
		case LIST_LED_WHEEL_7:		
			bRet=LED_WHEEL_7_STATUS;		
			break;
			
	//----------------
		case LIST_LED_UP:				
			bRet=LED_KEY_UP_STATUS;			
			break;
			
		case LIST_LED_LEFT:			
			bRet=LED_KEY_LEFT_STATUS;	
			break;	
			
		case LIST_LED_RIGHT:			
			bRet=LED_KEY_RIGHT_STATUS;	
			break;
			
		case LIST_LED_DOWN:			
			bRet=LED_KEY_DOWN_STATUS;		
			break;
			
	//----------------
		case LIST_LED_SLIDE_1:		
			bRet=LED_SLIDE_1_STATUS;		
			break;
			
		case LIST_LED_SLIDE_2:		
			bRet=LED_SLIDE_2_STATUS;		
			break;
			
		case LIST_LED_SLIDE_3:		
			bRet=LED_SLIDE_3_STATUS;		
			break;
			
		case LIST_LED_SLIDE_4:		
			bRet=LED_SLIDE_4_STATUS;		
			break;
			
	//----------------
		case LIST_LED_OK:				
			bRet=LED_KEY_OK_STATUS;		
			break;
			
	//----------------
		case LIST_LED_KEY0:			
		case LIST_LED_KEY1:			
		case LIST_LED_KEY2:			
		case LIST_LED_KEY3:			
		case LIST_LED_KEY4:			
		case LIST_LED_KEY5:			
		case LIST_LED_KEY6:			
		case LIST_LED_KEY7:			
		case LIST_LED_KEY8:			
		case LIST_LED_KEY9:			
		case LIST_LED_DEL:				
			bRet=Drv_CapTouch_Get_Led(led_name);		
			break;
	#endif		
	
		default: 
			break;
	}
	return bRet;
}

#if 0
static void Drv_Led_Demo(void)
{
	flex_led_name_enum index;
#if 1
	flex_led_name_enum start=LIST_LED_WHEEL_1;
	flex_led_name_enum end=LIST_LED_DEL;
#else
	flex_led_name_enum start=(flex_led_name_enum)(LIST_LED_NONE+1);
	flex_led_name_enum end=(flex_led_name_enum)(LIST_LED_MAX-1);
#endif
	
	TRACE_I("LED DEMO-----------------------------------LED ON.\r\n");
	for(index=start; index<=end; index++)
	{
		Drv_Led_Ctrl(index, true);		
		HAL_Delay(500);
	}

	for(index=start; index<=end; index++)
	{
		TRACE_I("get led status: index=%d, %s.\r\n", index, Drv_Led_Get_Status(index)? "ON":"OFF");
	}

	TRACE_I("LED DEMO-----------------------------------LED OFF.\r\n");
	for(index=start; index<=end; index++)
	{
		Drv_Led_Ctrl(index, false); 	
		HAL_Delay(500);
	}
	for(index=start; index<=end; index++)
	{
		TRACE_I("get led status: index=%d, %s.\r\n", index, Drv_Led_Get_Status(index)? "ON":"OFF");
	}
}
#endif

void Drv_Led_Init(void)
{
#if 0
	flex_led_name_enum index;

	TRACE_F();
	
	for(index=LIST_LED_NONE; index<LIST_LED_MAX; index++)
	{
		Drv_Led_Ctrl(index, false);		
	}
#endif

	//Drv_Led_Demo();
}


/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_drv_captouch.c
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
#include "flex_framework_input.h"
#include "flex_drv_captouch_cap1188.h"
#include "flex_drv_captouch_cap1214.h"
#include "flex_drv_captouch_mpr121.h"

// internal
#include "flex_drv_captouch.h"




/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME								"[CAPTOUCH]--"
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








/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
#if (TSC_TYPE_SELECT&TSC_TYPE_MICROCHIP_CA1188_I2C)
static const DRV_CAPTOUCH_FUNCS_ST *g_Cap1118_Handler = &cap1188_funcs;
#else
static const DRV_CAPTOUCH_FUNCS_ST *g_Cap1118_Handler = NULL;
#endif

#if (TSC_TYPE_SELECT&TSC_TYPE_MICROCHIP_CA1214_I2C)
static const DRV_CAPTOUCH_FUNCS_ST *g_Cap1214_Handler = &cap1214_funcs;
#else
static const DRV_CAPTOUCH_FUNCS_ST *g_Cap1214_Handler = &mpr121_funcs;//NULL;
#endif


/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
bool Drv_CapTouch_Init(void)
{
	bool ret=false;
	
	if(g_Cap1118_Handler!=NULL)
	{
		if(g_Cap1118_Handler->nInit != NULL)
		{
			ret=g_Cap1118_Handler->nInit();
			Drv_CapTouch_Set_Callback(Frm_Input_Button_Callback);
		}
	}	

	if(g_Cap1214_Handler!=NULL)
	{
		if(g_Cap1214_Handler->nInit != NULL)
		{
			ret|=g_Cap1214_Handler->nInit();
			Drv_CapTouch_Set_Callback(Frm_Input_Button_Callback);
		}
	} 

	return ret;
}

bool Drv_CapTouch_Support_Polling(void)
{
	bool ret=false;
	
	if(g_Cap1118_Handler!=NULL)
	{
		if(g_Cap1118_Handler->nSupportPolling!= NULL)
		{
			ret=g_Cap1118_Handler->nSupportPolling();
		}
	}	

	if(g_Cap1214_Handler!=NULL)
	{
		if(g_Cap1214_Handler->nSupportPolling != NULL)
		{
			ret|=g_Cap1214_Handler->nSupportPolling();
		}
	} 

	TRACE_I("Cap Touch Polling %s.\r\n", ret? "Support":"Not Support");
	return ret;
}

void Drv_CapTouch_Polling(void)
{
	if(g_Cap1118_Handler!=NULL)
	{
		if(g_Cap1118_Handler->nPolling!= NULL)
		{
			g_Cap1118_Handler->nPolling();
		}
	}	

	if(g_Cap1214_Handler!=NULL)
	{
		if(g_Cap1214_Handler->nPolling != NULL)
		{
			g_Cap1214_Handler->nPolling();
		}
	} 
}


bool Drv_CapTouch_Get_Led(flex_led_name_enum led_name)
{
  bool bRet=false;
  
  switch(led_name)
  {
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
			if(g_Cap1214_Handler!=NULL)
			{
				if(g_Cap1214_Handler->nGetLedOnOff!= NULL)
				{
					bRet=g_Cap1214_Handler->nGetLedOnOff(led_name);
				}
			} 
      break;
			
		default:
			break;
  }
	
  return bRet;
}

void Drv_CapTouch_Set_Led(flex_led_name_enum led_name, bool on)
{
  switch(led_name)
  {
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
			if(g_Cap1214_Handler!=NULL)
			{
				if(g_Cap1214_Handler->nSetLedOnOff!= NULL)
				{
					g_Cap1214_Handler->nSetLedOnOff(led_name, on);
				}
			} 
      break;
			
		default:
      break;
  }
}

void Drv_CapTouch_Set_Callback(fpDrvBtnEvent func_ptr)
{
	if(g_Cap1118_Handler!=NULL)
	{
		if(g_Cap1118_Handler->nSetCallback!= NULL)
		{
			g_Cap1118_Handler->nSetCallback(func_ptr);
		}
	}	

	if(g_Cap1214_Handler!=NULL)
	{
		if(g_Cap1214_Handler->nSetCallback != NULL)
		{
			g_Cap1214_Handler->nSetCallback(func_ptr);
		}
	} 
}

/*========================================================================================================
										End
========================================================================================================*/




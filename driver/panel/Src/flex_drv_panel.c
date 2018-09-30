/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_drv_panel.c
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
#include "flex_drv_font.h"
#include "flex_drv_panel_def.h"
#include "flex_drv_panel_oled_sh1106_i2c.h"
#include "flex_drv_panel_stnlcd_jlx128x128_spi.h"
#include "flex_hal.h"
#include "flex_app.h"

// internal
#include "flex_drv_panel.h"


/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME								"[PANEL]--"
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
static DRV_PANEL_FUNCS_IF_ST *g_Panel_Main_Handler = NULL;
static DRV_PANEL_FUNCS_IF_ST *g_Panel_Sub_Handler = NULL;







/*========================================================================================================
										F U N C T I O N----private
========================================================================================================*/
static void Flex_Drv_Panel_Config_Func(void)
{
#if (PROJECT_SELECT==PROJECT_HMI_BUTTERFLY)  
	#if (BOARD_SELECT==BOARD_STM32L476ZET6_HMI_BUTTERFLY)
	g_Panel_Main_Handler = (DRV_PANEL_FUNCS_IF_ST *)&panel_stnlcd_jlx128x128_spi_if;
	g_Panel_Sub_Handler  = (DRV_PANEL_FUNCS_IF_ST *)&panel_oled_sh1106_i2c_if;
	#else
	g_Panel_Main_Handler = NULL;
	g_Panel_Sub_Handler  = NULL;
	#endif
#else
	g_Panel_Main_Handler = NULL;
	g_Panel_Sub_Handler  = NULL;
#endif
}

static DRV_PANEL_FUNCS_IF_ST* Flex_Drv_Panel_Get_Func_Handler(DRV_PANEL_NAME_ENUM panel_index)
{
	DRV_PANEL_FUNCS_IF_ST* pFunc=NULL;
	
	switch(panel_index)
	{
		case LIST_PANEL_INDEX_MAIN:		
			pFunc = g_Panel_Main_Handler;	
			break;
			
		case LIST_PANEL_INDEX_SUB:				
			pFunc = g_Panel_Sub_Handler;		
			break;
			
		default:
			break;
	}
	
	return pFunc;
}
 
/*========================================================================================================
										F U N C T I O N----public
========================================================================================================*/
void Drv_Panel_Init(DRV_PANEL_NAME_ENUM panel)
{
	DRV_PANEL_FUNCS_IF_ST *pPanelHandler=NULL;

	TRACE_F();

	Flex_Drv_Panel_Config_Func();
	pPanelHandler = Flex_Drv_Panel_Get_Func_Handler(panel);
	
	if(pPanelHandler!=NULL)
	{
		pPanelHandler->nInit();
		pPanelHandler->nClearAll();
	}	
}


void Drv_Panel_Set_Color(DRV_PANEL_NAME_ENUM panel, DRV_PANEL_COLOR_ENUM color)
{
	DRV_PANEL_FUNCS_IF_ST *pPanelHandler=Flex_Drv_Panel_Get_Func_Handler(panel);
		
	if(pPanelHandler!=NULL)
	{
		pPanelHandler->nSetColor(color);   
	}
}

void Drv_Panel_Set_BkColor(DRV_PANEL_NAME_ENUM panel, DRV_PANEL_COLOR_ENUM color)
{
	DRV_PANEL_FUNCS_IF_ST *pPanelHandler=Flex_Drv_Panel_Get_Func_Handler(panel);
		
	if(pPanelHandler!=NULL)
	{
		pPanelHandler->nSetBkColor(color);   
	}
}

void Drv_Panel_Clear_All(DRV_PANEL_NAME_ENUM panel)
{
	DRV_PANEL_FUNCS_IF_ST *pPanelHandler=Flex_Drv_Panel_Get_Func_Handler(panel);
	
	if(pPanelHandler!=NULL)
	{
	  pPanelHandler->nClearAll();
	}	
}

void Drv_Panel_Clear_Block(DRV_PANEL_NAME_ENUM panel, uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	DRV_PANEL_FUNCS_IF_ST *pPanelHandler= Flex_Drv_Panel_Get_Func_Handler(panel);
	
	if(pPanelHandler!=NULL)
	{
	  pPanelHandler->nClearBlock(x,y,width,height);
	}	
}

void Drv_Panel_Draw_Char(DRV_PANEL_NAME_ENUM panel, DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t ch)
{
	DRV_PANEL_FUNCS_IF_ST *pPanelHandler= Flex_Drv_Panel_Get_Func_Handler(panel);
		
	if(pPanelHandler!=NULL)
	{
		pPanelHandler->nDrawChar(font_size, x, y, ch);   
	}
}

void Drv_Panel_Draw_Str(DRV_PANEL_NAME_ENUM panel, DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t string[])
{
	DRV_PANEL_FUNCS_IF_ST *pPanelHandler= Flex_Drv_Panel_Get_Func_Handler(panel);
		
	if(pPanelHandler!=NULL)
	{
		pPanelHandler->nDrawStr(font_size, x, y, string);   
	}
}

void Drv_Panel_Draw_Image(DRV_PANEL_NAME_ENUM panel, uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t image[])
{
	DRV_PANEL_FUNCS_IF_ST *pPanelHandler=Flex_Drv_Panel_Get_Func_Handler(panel);
		
	if(pPanelHandler!=NULL)
	{
		DRV_PANEL_INFO_ST pInfo={0};
		
		pPanelHandler->nIOCTRL(LIST_PANEL_IOCTL_ID_GET_HSIZE, &pInfo.nHsize);
		pPanelHandler->nIOCTRL(LIST_PANEL_IOCTL_ID_GET_VSIZE, &pInfo.nVsize);
		
		if((width<=pInfo.nHsize) && (height<=pInfo.nVsize))
		{
			pPanelHandler->nDrawImg(x, y, width, height, image);   
		}
	}
}

void Drv_Panel_Refresh_Gram(DRV_PANEL_NAME_ENUM panel,DRV_PANEL_REFRESH_ENUM type)
{
	DRV_PANEL_FUNCS_IF_ST *pPanelHandler=Flex_Drv_Panel_Get_Func_Handler(panel);
	
	if(pPanelHandler!=NULL)
	{
	  pPanelHandler->nRefreshGram(type);
	}	
}


void Drv_Panel_Clear_Gram_All(DRV_PANEL_NAME_ENUM panel)
{
	DRV_PANEL_FUNCS_IF_ST *pPanelHandler=Flex_Drv_Panel_Get_Func_Handler(panel);
	
	if(pPanelHandler!=NULL)
	{
	  pPanelHandler->nClearGramAll();
	}	
}

void Drv_Panel_Clear_Gram_Block(DRV_PANEL_NAME_ENUM panel, uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	DRV_PANEL_FUNCS_IF_ST *pPanelHandler= Flex_Drv_Panel_Get_Func_Handler(panel);
	
	if(pPanelHandler!=NULL)
	{
	  pPanelHandler->nClearGramBlock(x,y,width,height);
	}	
}

void Drv_Panel_Draw_Gram_Char(DRV_PANEL_NAME_ENUM panel, DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t ch)
{
	DRV_PANEL_FUNCS_IF_ST *pPanelHandler= Flex_Drv_Panel_Get_Func_Handler(panel);
		
	if(pPanelHandler!=NULL)
	{
		pPanelHandler->nDrawGramChar(font_size, x, y, ch);   
	}
}

void Drv_Panel_Draw_Gram_Str(DRV_PANEL_NAME_ENUM panel, DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t string[])
{
	DRV_PANEL_FUNCS_IF_ST *pPanelHandler= Flex_Drv_Panel_Get_Func_Handler(panel);
		
	if(pPanelHandler!=NULL)
	{
		pPanelHandler->nDrawGramStr(font_size, x, y, string);   
	}
}

void Drv_Panel_Draw_Gram_Image(DRV_PANEL_NAME_ENUM panel, uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t image[])
{
	DRV_PANEL_FUNCS_IF_ST *pPanelHandler=Flex_Drv_Panel_Get_Func_Handler(panel);
		
	if(pPanelHandler!=NULL)
	{
		DRV_PANEL_INFO_ST pInfo={0};
		
		pPanelHandler->nIOCTRL(LIST_PANEL_IOCTL_ID_GET_HSIZE, &pInfo.nHsize);
		pPanelHandler->nIOCTRL(LIST_PANEL_IOCTL_ID_GET_VSIZE, &pInfo.nVsize);
		
		if((width<=pInfo.nHsize) && (height<=pInfo.nVsize))
		{
			pPanelHandler->nDrawGramImg(x, y, width, height, image);   
		}
	}
}

void Drv_Panel_Power(DRV_PANEL_NAME_ENUM panel, DRV_PANEL_POWER_ENUM power)
{
	DRV_PANEL_FUNCS_IF_ST *pPanelHandler= Flex_Drv_Panel_Get_Func_Handler(panel);
		
	if(pPanelHandler!=NULL)
	{
		pPanelHandler->nSetPower(power);
	}
}

uint8_t Drv_Panel_Get_Brightness(DRV_PANEL_NAME_ENUM index)
{
	DRV_PANEL_FUNCS_IF_ST *pPanelHandler= Flex_Drv_Panel_Get_Func_Handler(index);
	uint8_t value=0;
	
	if(pPanelHandler!=NULL)
	{
		pPanelHandler->nIOCTRL(LIST_PANEL_IOCTL_ID_GET_BRIGHTNESS, &value);
	}

	return value;
}

void Drv_Panel_Set_Brightness(DRV_PANEL_NAME_ENUM index, uint8_t value)
{
	DRV_PANEL_FUNCS_IF_ST *pPanelHandler= Flex_Drv_Panel_Get_Func_Handler(index);
	
	if(pPanelHandler!=NULL)
	{
		pPanelHandler->nIOCTRL(LIST_PANEL_IOCTL_ID_SET_BRIGHTNESS, &value);
	}
}
//Joanna.wang add for query 20180907 start
bool Drv_Panel_Get_Healthy(DRV_PANEL_NAME_ENUM index)
{
	DRV_PANEL_FUNCS_IF_ST *pPanelHandler= Flex_Drv_Panel_Get_Func_Handler(index);
	bool value=0;
	
	if(pPanelHandler!=NULL)
	{
		pPanelHandler->nIOCTRL(LIST_PANEL_IOCTL_ID_GET_HEALTHY, &value);
	}

	return value;	
}

bool Drv_Panel_Get_PwrState(DRV_PANEL_NAME_ENUM index)
{
	DRV_PANEL_FUNCS_IF_ST *pPanelHandler= Flex_Drv_Panel_Get_Func_Handler(index);
	bool value=0;
	
	if(pPanelHandler!=NULL)
	{
		pPanelHandler->nIOCTRL(LIST_PANEL_IOCTL_ID_GET_PWR, &value);
	}

	return value;	
}
//Joanna.wang add for query 20180907 end
/*========================================================================================================
										End
========================================================================================================*/




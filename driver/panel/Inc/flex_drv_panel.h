/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_drv_panel.h
 [Date]:       
 		2017-12-20
 [Author]:   	
 		carson.qian 
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_DRV_PANEL_H_
#define _FLEX_DRV_PANEL_H_

#ifdef __cplusplus 
extern "C" {
#endif

/*========================================================================================================
										I N C L U D E
========================================================================================================*/
#include "flex_common.h"
#include "flex_drv_panel_def.h"


/*========================================================================================================
										D E F I N E
========================================================================================================*/




/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
typedef enum 
{
	ALIGN_IGNORE,
	ALIGN_LEFT,
	ALIGN_CENTER,
	ALIGN_RIGHT,
	ALIGN_MAX
}ALIGN_ENUM;





/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/







/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
void Drv_Panel_Init(DRV_PANEL_NAME_ENUM panel);
void Drv_Panel_Clear_All(DRV_PANEL_NAME_ENUM panel);
void Drv_Panel_Clear_Block(DRV_PANEL_NAME_ENUM panel, uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void Drv_Panel_Draw_Char(DRV_PANEL_NAME_ENUM panel, DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t ch);
void Drv_Panel_Draw_Str(DRV_PANEL_NAME_ENUM panel, DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t string[]);
void Drv_Panel_Draw_Image(DRV_PANEL_NAME_ENUM panel, uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t image[]);
void Drv_Panel_Refresh_Gram(DRV_PANEL_NAME_ENUM panel,DRV_PANEL_REFRESH_ENUM type);
void Drv_Panel_Clear_Gram_All(DRV_PANEL_NAME_ENUM panel);
void Drv_Panel_Clear_Gram_Block(DRV_PANEL_NAME_ENUM panel, uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void Drv_Panel_Draw_Gram_Char(DRV_PANEL_NAME_ENUM panel, DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t ch);
void Drv_Panel_Draw_Gram_Str(DRV_PANEL_NAME_ENUM panel, DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t string[]);
void Drv_Panel_Draw_Gram_Image(DRV_PANEL_NAME_ENUM panel, uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t image[]);

void Drv_Panel_Power(DRV_PANEL_NAME_ENUM panel, DRV_PANEL_POWER_ENUM power);
void Drv_Panel_Set_Color(DRV_PANEL_NAME_ENUM panel, DRV_PANEL_COLOR_ENUM color);
void Drv_Panel_Set_BkColor(DRV_PANEL_NAME_ENUM panel, DRV_PANEL_COLOR_ENUM color);

uint8_t Drv_Panel_Get_Brightness(DRV_PANEL_NAME_ENUM index);
void Drv_Panel_Set_Brightness(DRV_PANEL_NAME_ENUM index, uint8_t value);

//Joanna.wang add for query 20180907
bool Drv_Panel_Get_Healthy(DRV_PANEL_NAME_ENUM index);
bool Drv_Panel_Get_PwrState(DRV_PANEL_NAME_ENUM index);
/*========================================================================================================
										End
========================================================================================================*/
#ifdef __cplusplus 
}
#endif

#endif//_FLEX_DRV_PANEL_H_



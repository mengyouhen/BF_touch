/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_app_osd_draw.h
 [Date]:       
 		2018-05-23
 [Author]:   	
 		carson.qian 
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_APP_OSD_DRAW_H_
#define _FLEX_APP_OSD_DRAW_H_

#ifdef __cplusplus 
extern "C" {
#endif


/*========================================================================================================
										I N C L U D E
========================================================================================================*/
#include "flex_common.h"
#include "flex_app_osd_def.h"
#include "flex_app_osd_global.h"
#include "flex_drv_panel.h"



/*========================================================================================================
										D E F I N E
========================================================================================================*/




/*========================================================================================================
										T Y P E D E F
========================================================================================================*/






/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/







/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
//===============================================================
// COLOR
//===============================================================
void Osd_Set_Color(uint8_t color);
//===============================================================
// CLEAR
//===============================================================
void Osd_Clear_All(void);
void Osd_Clear_Block(uint8_t x, uint8_t y, uint8_t hsize, uint8_t vsize);
//===============================================================
// DRAW STRING
//===============================================================
void Osd_Draw_Char(uint8_t x, uint8_t y, const uint8_t ch);
void Osd_Draw_String(ALIGN_ENUM align, uint8_t x, uint8_t y, const uint8_t string[], uint8_t width);
//===============================================================
// DRAW ICON
//===============================================================
void Osd_Draw_Icon(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t data[]);
//===============================================================
// DRAW FRAME
//===============================================================
void Osd_Draw_Frame(uint8_t x, uint8_t y, uint8_t width, uint8_t height);

/*========================================================================================================
										End
========================================================================================================*/
#ifdef __cplusplus 
}
#endif

#endif//_FLEX_APP_OSD_DRAW_H_



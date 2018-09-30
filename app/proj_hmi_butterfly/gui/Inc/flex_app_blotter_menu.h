/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_app_blotter_menu.h
 [Date]:       
 		2018-05-23
 [Author]:   	
 		carson.qian 
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_APP_BLOTTER_MENU_H_
#define _FLEX_APP_BLOTTER_MENU_H_

#ifdef __cplusplus 
extern "C" {
#endif


/*========================================================================================================
										I N C L U D E
========================================================================================================*/
#include "flex_common.h"
#include "flex_drv_font.h"
#include "flex_drv_button.h"
#include "flex_framework_input.h"
#include "flex_app_blotter_def.h"


/*========================================================================================================
										D E F I N E
========================================================================================================*/
//=============================
// font size
//=============================
#define BLOTTER_FONT_SIZE	        FONT_6X8		 // FONT_6X8  FONT_8X16

//=============================
// menu size
//=============================
#define BLOTTER_PANAL_WIDTH       132	
#define BLOTTER_PANAL_HEIGHT      64	

#if (BLOTTER_FONT_SIZE == FONT_6X8)
#define BLOTTER_FONT_X_RES        6
#define BLOTTER_FONT_Y_RES        8
#define BLOTTER_MENU_H_FONTS      21	// 128/6
#define BLOTTER_MENU_V_FONTS      8		// 64/8
#elif (BLOTTER_FONT_SIZE == FONT_8X16)
#define BLOTTER_FONT_X_RES        8
#define BLOTTER_FONT_Y_RES        16
#define BLOTTER_MENU_H_FONTS      16  // 128/8
#define BLOTTER_MENU_V_FONTS      4		// 64/16
#else
#define BLOTTER_FONT_X_RES        6
#define BLOTTER_FONT_Y_RES        8
#define BLOTTER_MENU_H_FONTS      21	// 128/6
#define BLOTTER_MENU_V_FONTS      8		// 64/8
#endif
//Menu title line number
#define BLOTTER_MENU_TITLE_LINE     1
#define BLOTTER_MENU_FONTS_MAX    ((BLOTTER_MENU_H_FONTS)*((BLOTTER_MENU_V_FONTS)-(BLOTTER_MENU_TITLE_LINE)))


#define XPOS(i)                   (BLOTTER_FONT_X_RES*(i))
#define YPOS(i)                   (BLOTTER_FONT_Y_RES*(i))




/*========================================================================================================
										T Y P E D E F
========================================================================================================*/






/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/







/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
void Blotter_Menu_Clear_All(void);
void Blotter_Menu_Clear_Block(uint8_t x, uint8_t y, uint8_t hsize, uint8_t vsize);
void Blotter_Menu_Draw_Char(uint8_t x, uint8_t y, uint8_t ch);
void Blotter_Menu_Draw_Str(uint8_t x, uint8_t y, const uint8_t string[]);
//==============================================
void Blotter_Menu_Init(void);
void Blotter_Menu_Draw_Title(void);
void Blotter_Menu_Update(BLOTTER_CHAIN_NODE_ST * pNode);
void Blotter_Menu_Page_Scroll(bool page_down);




/*========================================================================================================
										End
========================================================================================================*/
#ifdef __cplusplus 
}
#endif

#endif//_FLEX_APP_BLOTTER_MENU_H_



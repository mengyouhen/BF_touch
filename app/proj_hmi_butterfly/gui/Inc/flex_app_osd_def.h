/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_app_osd_def.h
 [Date]:       
 		2018-05-23
 [Author]:   	
 		carson.qian 
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_APP_OSD_DEF_H_
#define _FLEX_APP_OSD_DEF_H_

#ifdef __cplusplus 
extern "C" {
#endif


/*========================================================================================================
										I N C L U D E
========================================================================================================*/
#include "flex_common.h"
#include "flex_framework_input.h"
#include "flex_app_osd_string.h"



/*========================================================================================================
										S T A R T
========================================================================================================*/
//=============================
// font size
//=============================
#define OSD_FONT_SIZE	        FONT_6X8		 // FONT_6X8  FONT_8X16

//=============================
// menu size
//=============================
/*
  ------------------------------------
  | INDEX  |  TYPE      |    UNIT   |
  ------------------------------------
  | 1      | xpos       |    pixel  |
  ------------------------------------
  | 2      | ypos       |    pixel  |
  ------------------------------------
  | 3      | size       |    pixel  |
  ------------------------------------
  | 4      | width      |    font   |
  ------------------------------------
  | 5      | height     |    font   |
  ------------------------------------
*/
#define OSD_PANAL_HSIZE       128	
#define OSD_PANAL_VSIZE       128	

#if (OSD_FONT_SIZE == FONT_6X8)
#define OSD_FONT_XRES         6
#define OSD_FONT_YRES         8
#define OSD_PANEL_WIDTH       21		// 128/6
#define OSD_PANEL_HEIGHT      16		// 128/8
#define OSD_PANEL_FONTS_MAX   336 	// OSD_PANEL_WIDTH*OSD_PANEL_HEIGHT
#elif (OSD_FONT_SIZE == FONT_8X16)
#define OSD_FONT_XRES         8
#define OSD_FONT_YRES         16
#define OSD_PANEL_WIDTH       16  	// 128/8
#define OSD_PANEL_HEIGHT      8		 	// 128/16
#define OSD_PANEL_FONTS_MAX   128  	// OSD_PANEL_WIDTH*OSD_PANEL_HEIGHT
#else
#define OSD_FONT_XRES         6
#define OSD_FONT_YRES         8
#define OSD_PANEL_WIDTH       21		// 128/6
#define OSD_PANEL_HEIGHT      16		// 128/8
#define OSD_PANEL_FONTS_MAX   336 	// OSD_PANEL_WIDTH*OSD_PANEL_HEIGHT
#endif

#define _X_(i)                   (OSD_FONT_XRES*(i))
#define _Y_(i)                   (OSD_FONT_YRES*(i))

//=============================================================
// MENU PAGE ATTR
//=============================================================
#define MP_STAY         0x01 // Menu Page stay or not
#define MP_AUTO         0x02 // Menu Page auto disappeard or not
#define MP_INVISIBLE    0x04 // Menu Page visible or not
#define MP_MOVING       0x08 // Menu Page moving or not
#define MP_DRAWFRAME    0x10 // Menu Page has frame or not

//=============================================================
// RUN-TIME MASK
//=============================================================
#define RT_NULL         0x00     /* run-time mask for no runtime properties (value must NOT be changed) */
#define RT_YPOS         0x01     /* run-time mask for vertical location                 */
#define RT_XPOS         0x02     /* run-time mask for horizontal location               */
#define RT_COLOR        0x04     /* run-time mask for colour                            */
#define RT_WIDTH        0x08     /* run-time mask for item width                        */
#define RT_HEIGHT       0x10     /* run-time mask for item height                       */
#define RT_DISPLAY      0x20     /* run-time mask for represented display               */
#define RT_VALUE        0x40     /* run-time mask for represented item value            */
#define RT_END          0x80     /* run-time mask for content group end flag		        */



//=============================================================
// MENU PAGE INDEX
//=============================================================
typedef enum   
{
	MENU_ROOT,		
	MENU_ANIMATION,
	MENU_LOGO,								 
	MENU_STATUS_BAR,								 
	//------------ user definition start here
	MENU_MAIN,	
	MENU_SYS_CFG,
	MENU_WIFI_CFG,					
	MENU_BT_CFG,								
	MENU_SYS_INFO,						
	MENU_SYS_STATUS,					
	MENU_CREDITS,	
	MENU_POPUP_BRIGHTNESS,
	MENU_HELP,			
	//------------ user definition end here
	MENU_MAX
} OSD_PAGE_ENUM;

//=============================================================
// ITEM SELECT  
//=============================================================
#define ITEM_STATUS_DISABLE        0
#define ITEM_STATUS_ENABLE         1
#define ITEM_STATUS_SEL_DISABLE    2

typedef enum
{
	MI_DISABLE,
	MI_ENABLE,
	MI_SEL_DISABLE,
	MI_WIFI_CONN_ONLY,
	MI_HELP_NAVI,
	MI_HELP_SLIDE,
	MI_HELP_ARROW_L,
	MI_HELP_ARROW_R,
	MI_HELP_ARROW_COLON,
	MI_HELP_RET,
	MI_MAX
} OSD_ITEM_SELECL_FLAG_ENUM;

//=============================================================
// DRAW STRING
//=============================================================
typedef enum
{    
	LIST_DWP_NOTHING,
	LIST_DWP_TEXT_L,
	LIST_DWP_TEXT_R,
	LIST_DWP_TEXT_C,
	LIST_DWP_LOGO,
	LIST_DWP_ICON,
}OSD_DWP_TYPE_ENUM;

typedef enum
{    
	LIST_DWC_NOTHING,
	LIST_DWC_TEXT_L,
	LIST_DWC_TEXT_R,
	LIST_DWC_TEXT_C,
	LIST_DWC_ICON,
	LIST_DWC_DATE,
	LIST_DWC_TIME,
	LIST_DWC_INFO,
	LIST_DWC_NUMBER_L,	
	LIST_DWC_NUMBER_R,	
	LIST_DWC_NUMBER_C,	
	LIST_DWC_SIGNED_NUM_L,	
	LIST_DWC_SIGNED_NUM_R,	
	LIST_DWC_SIGNED_NUM_C,
	LIST_DWC_CREDIT_TITLE,
	LIST_DWC_CREDIT_LIST,
}OSD_DWC_TYPE_ENUM;

#define DWC_END      0x80

//=============================================================
// COLOR
//=============================================================
typedef enum
{
	LIST_CP_BLACK,		
	LIST_CP_WHITE,		  
	LIST_CP_LIGHTGRAY,	  	
	LIST_CP_DARKGRAY,
/*
	LIST_CP_RED,			
	LIST_CP_GREEN,		
	LIST_CP_BLUE,		 
	LIST_CP_YELLOW,	  
	LIST_CP_CYAN,		  
	LIST_CP_PINK,		  
	LIST_CP_ORANGE,		 
	LIST_CP_BLEND,			    
	LIST_CP_TRANSPARENT	
*/
	LIST_CP_MAX	
} OSD_COLOR_PALETTE_ENUM;

///////////////////////////////////
// COLOT TABLE
// High 4bit: Foreground Color
// Low  4bit: Background Color
///////////////////////////////////
#define CT_MASK_SHIFT            4
#define CT_MASK                  0x0F

#define CT_BLACK_WHITE           ((LIST_CP_BLACK<<4) + LIST_CP_WHITE)         // Foreground Color + Background Color
#define CT_BLACK_LIGHTGRAY       ((LIST_CP_BLACK<<4) + LIST_CP_LIGHTGRAY)     // Foreground Color + Background Color
#define CT_BLACK_DARKGRAY        ((LIST_CP_BLACK<<4) + LIST_CP_DARKGRAY)      // Foreground Color + Background Color
#define CT_WHITE_BLACK           ((LIST_CP_WHITE<<4) + LIST_CP_BLACK)         // Foreground Color + Background Color
#define CT_WHITE_LIGHTGRAY       ((LIST_CP_WHITE<<4) + LIST_CP_LIGHTGRAY)     // Foreground Color + Background Color
#define CT_WHITE_DARKGRAY        ((LIST_CP_WHITE<<4) + LIST_CP_DARKGRAY)      // Foreground Color + Background Color
#define CT_LIGHTGRAY_WHITE       ((LIST_CP_LIGHTGRAY<<4) + LIST_CP_WHITE)     // Foreground Color + Background Color
#define CT_LIGHTGRAY_BLACK       ((LIST_CP_LIGHTGRAY<<4) + LIST_CP_BLACK)     // Foreground Color + Background Color
#define CT_LIGHTGRAY_DARKGRAY    ((LIST_CP_LIGHTGRAY<<4) + LIST_CP_DARKGRAY)  // Foreground Color + Background Color
#define CT_DARKGRAY_WHITE        ((LIST_CP_DARKGRAY<<4) + LIST_CP_WHITE)      // Foreground Color + Background Color
#define CT_DARKGRAY_BLACK        ((LIST_CP_DARKGRAY<<4) + LIST_CP_BLACK)      // Foreground Color + Background Color
#define CT_DARKGRAY_LIGHTGRAY    ((LIST_CP_DARKGRAY<<4) + LIST_CP_LIGHTGRAY)  // Foreground Color + Background Color

///////////////////////////////////
// STATIC COLOR
///////////////////////////////////
#define CT_MENU_DEFALUT          CT_BLACK_WHITE
#define	CT_MENU_ITEM_UNSEL       CT_BLACK_WHITE
#define CT_MENU_ITEM_SEL         CT_WHITE_BLACK
#define CT_MENU_ITEM_SEL_DISABLE CT_BLACK_WHITE//CT_LIGHTGRAY_WHITE
#define	CT_MENU_STATUSBAR        CT_BLACK_WHITE//CT_LIGHTGRAY_WHITE
#define	CT_MENU_TITLE            CT_BLACK_WHITE//CT_LIGHTGRAY_WHITE
#define	CT_MENU_HELP             CT_BLACK_WHITE//CT_LIGHTGRAY_WHITE

///////////////////////////////////
// DYNAMIC COLOR
///////////////////////////////////
#define COLOR_UNSELECT         0
#define COLOR_SELECT           1
#define COLOR_SEL_DISABLE      2

typedef enum
{
	LIST_COLOR_NULL,
	LIST_COLOR_MENU_PROMPT,
	LIST_COLOR_MENU_CONTENT,
	LIST_COLOR_MENU_BAR,
	LIST_COLOR_MAX
 }OSD_COLOR_TYPE_ENUM;



//=============================================================
// KEY   
//=============================================================
typedef enum
{
	LIST_MIA_NOTHING,
	LIST_MIA_MENU,	
	LIST_MIA_GOTONEXT, 
	LIST_MIA_GOTOPREV, 
	LIST_MIA_NEXTITEM, 
	LIST_MIA_PREVITEM, 
	LIST_MIA_REDRAWMENU,
	LIST_MIA_RELOADMENUPAGE,
	LIST_MIA_INCVALUE, 
	LIST_MIA_DECVALUE, 
	LIST_MIA_POP_INC, 
	LIST_MIA_POP_DEC, 
	LIST_MIA_EXECUTE,  
	LIST_MIA_GOTOROOT,
	LIST_MIA_KEY_0,
	LIST_MIA_KEY_1,
	LIST_MIA_KEY_2,
	LIST_MIA_KEY_3,
	LIST_MIA_KEY_4,
	LIST_MIA_KEY_5,
	LIST_MIA_KEY_6,
	LIST_MIA_KEY_7,
	LIST_MIA_KEY_8,
	LIST_MIA_KEY_9,
	LIST_MIA_KEY_DEL,
	LIST_MIA_KEY_OK,
#if defined(ENABLE_OSD_GAME)
	LIST_MIA_UP,
	LIST_MIA_DOWN,
	LIST_MIA_LEFT,
	LIST_MIA_RIGHT,
	LIST_MIA_START,
	LIST_MIA_STOP,
	LIST_MIA_PAUSE,
#endif
} OSD_MIA_ENUM;

typedef enum
{
	LIST_KEYMAP_LOCAL,
	LIST_KEYMAP_GAME
}OSD_KEYMAP_ENUM;


typedef struct
{
	FRM_KEY_ENUM nKey;
	OSD_MIA_ENUM nAction;
}OSD_KEY_ACTION_MAP_ST;

typedef struct
{
	OSD_KEYMAP_ENUM nInputSource;
	OSD_KEY_ACTION_MAP_ST const * nKeyActionMap;
}OSD_ITEM_INPUT_EVENTS_ST;

typedef enum
{
	Func_Null,

//------------------------- 
// STATUS BAR
//------------------------- 
	Func_Get_StatusBar_WiFi,		
	Func_Get_StatusBar_Bt,		
//------------------------- 
// MAIN MENU
//------------------------- 


//------------------------- 
// SUB MENH: WIFI CFG
//------------------------- 
	Func_Get_TimeZone,		
	Func_Set_TimeZone,	
	
	Func_Get_DateFormat,		
	Func_Set_DateFormat,		

//------------------------- 
// SUB MENH: WIFI CFG
//------------------------- 
	Func_Get_Wifi_Status,		
	Func_Get_Wifi_Ssid,		
	Func_Get_Wifi_Rssi, 
	Func_Get_Wifi_Mac, 
	Func_Get_Wifi_Ip, 
//------------------------- 
// SUB MENH: BT CFG
//------------------------- 
	Func_Get_Bt_Status,		

//------------------------- 
// SUB MENH: SYS INFO
//------------------------- 
	Func_Get_SdkVersion,		
	Func_Get_AtVersion, 	
	Func_Get_SerialNo,	
	Func_Get_BuildInfo, 
//------------------------- 
// SUB MENH: SYS STATUS
//------------------------- 
	Func_Get_Brightness,		
	Func_Set_Brightness,		
	
	Func_Get_FanSpeed,		
	Func_Set_FanSpeed,		

	Func_Get_Temp,		
	Func_Set_Temp,	

	Func_Get_Humidity,		

//------------------------- 
// HELP MENH: enter/adjust
//------------------------- 
	Func_Get_Help_LR_label,
//------------------------- 
	Func_Max
} OSD_KEY_FUNCTION_ENUM;

//=============================================================
// pFunc
//=============================================================
typedef void     (*fpOsdExecFunc)(void);
typedef uint8_t  (*fpOsdItemIndex)(void);
typedef void     (*fpOsdAdjustValue)(OSD_MIA_ENUM action);
typedef uint16_t (*fpOsdGetValue)(void);
typedef uint16_t (*fpOsdGetMaxValue)(void);
typedef uint16_t (*fpOsdSetMaxValue)(void);

/*========================================================================================================
										T Y P E D E F----main struct
========================================================================================================*/
//-----------------------------------------
// CONTENT
//-----------------------------------------
typedef struct
{
	uint8_t nRunTime;
	uint8_t nXPos;
	uint8_t nYPos;
	uint8_t nColor;
	uint8_t nDwcType; //OSD_DWC_TYPE_ENUM nDwcType;
	OSD_DISP_ENUM nDispIdx;
	uint8_t nWidth;
	OSD_KEY_FUNCTION_ENUM nKeyFuncIdx;
} OSD_MENU_CONTENT_ST;

//-----------------------------------------
// MENU ITEM 
//-----------------------------------------
typedef struct
{
	OSD_ITEM_SELECL_FLAG_ENUM nSelectFlags;
	uint8_t nRunTime;
	uint8_t nXPos;
	uint8_t nYPos;
	uint8_t nColor;
	uint8_t nWidth;
	OSD_DWP_TYPE_ENUM nDwpType;
	OSD_DISP_ENUM nDispIdx;
	OSD_ITEM_INPUT_EVENTS_ST const * nInputEvents;
	OSD_KEY_FUNCTION_ENUM nKeyFuncIdx;
	OSD_MENU_CONTENT_ST const * nDrawContent;
	OSD_PAGE_ENUM nNextPage;
} OSD_MENU_ITEM_ST;

//-----------------------------------------
// MENU PAGE 
//-----------------------------------------
typedef struct
{
	uint8_t nXpos, nYpos;
	uint8_t nWidth, nHeight;
	OSD_PAGE_ENUM nPrevPage;
	OSD_MENU_ITEM_ST const * nMenuItems;
	uint8_t nMenuItemCount;
	uint8_t nPromptSelSize, nContentSelSize;
	fpOsdExecFunc nExecFunction;
	uint8_t nFlags;
} OSD_MENU_PAGE_ST;

/*========================================================================================================
										End
========================================================================================================*/
#ifdef __cplusplus 
}
#endif

#endif//_FLEX_APP_OSD_DEF_H_



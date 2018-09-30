/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_app_osd_menutbl.h
 [Date]:       
 		2018-05-23
 [Author]:   	
 		carson.qian 
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_APP_OSD_MENUTBL_H_
#define _FLEX_APP_OSD_MENUTBL_H_

#ifdef __cplusplus 
extern "C" {
#endif


/*========================================================================================================
										I N C L U D E
========================================================================================================*/
#include "flex_common.h"
#include "flex_app_osd_def.h"



/*========================================================================================================
										D E F I N E
========================================================================================================*/
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
  
  --------------------------------------
  | 03/07/18       12:03       wifi bt |    -----> status bar
  |              MAIN MENU             |    -----> title
  | WIFI CONFIG        >               |    -|
  | BT CONFIG          >               |     |
  | SYS INFO           >               |     | 
  | SYS STATUS         >               |     |----> items
  | CREDITS            >               |     | 
  | BRIGHTNESS         <   50  >       |     | 
  |                                    |    -|
  |                                    |
  | ------------        ------------   |    -----> help info
  | | <>: ADJ  |        | ^V: NAVI |   |
  | ------------        ------------   |
  --------------------------------------
*/
#if (OSD_FONT_SIZE == FONT_6X8)
// STATUS BAR
#define UI_STATUSBAR_XPOS            _X_(0)
#define UI_STATUSBAR_YPOS            _Y_(0)
#define UI_STATUSBAR_WIDTH           (OSD_PANEL_WIDTH)
#define UI_STATUSBAR_HEIGHT          (1)
#define UI_STATUSBAR_HSIZE           _X_(UI_STATUSBAR_WIDTH)  
#define UI_STATUSBAR_VSIZE           _Y_(UI_STATUSBAR_HEIGHT)  

// MENU---logo
#define UI_LOGO_XPOS                 _X_(4)  
#define UI_LOGO_YPOS                 _Y_(4)  
#define UI_LOGO_HSIZE                _X_(8)  
#define UI_LOGO_VSIZE                _Y_(4)  
// MENU---animation
#define UI_ANIMATION_XPOS            _X_(4)  
#define UI_ANIMATION_YPOS            _Y_(4)  
#define UI_ANIMATION_HSIZE           _X_(8)  
#define UI_ANIMATION_VSIZE           _Y_(4)  

// MENU---size and pos and item
#define UI_MENU_XPOS                 _X_(0)
#define UI_MENU_YPOS                 _Y_(1)
#define UI_MENU_WIDTH                (OSD_PANEL_WIDTH)
#define UI_MENU_HEIGHT               (OSD_PANEL_HEIGHT-UI_STATUSBAR_HEIGHT-UI_HELP_HEIGHT)
#define UI_MENU_HSIZE                (_X_(UI_MENU_WIDTH)+2)   // 2: 128-21x6=2
#define UI_MENU_VSIZE                _Y_(UI_MENU_HEIGHT)  
// MENU---title
#define UI_TITLE_LINE_START          (1)   
#define UI_TITLE_XPOS                _X_(0)
#define UI_TITLE_YPOS                _Y_(1)
#define UI_TITLE_WIDTH               (OSD_PANEL_WIDTH)  
#define UI_TITLE_HEIGHT              (1)  
// MENU---item
#define UI_ITEM_LINE_START           (2)  
#define UI_ITEM_X                    (0)
#define UI_ITEM_WIDTH                (OSD_PANEL_WIDTH)  // 11(prompt)+1(space)+9(content)=21
// MENU---prompt
#define UI_ITEM_PROMPT_MARGIN_L      (0)
#define UI_ITEM_PROMPT_X             (0)
#define UI_ITEM_PROMPT_WIDTH         (11)	
#define UI_ITEM_PROMPT_HEIGHT        (1)	
#define UI_ITEM_PROMPT_MARGIN_R      (1)
#define UI_ITEM_PROMPT_HSIZE         _X_(12)
#define UI_ITEM_PROMPT_VSIZE         _Y_(1)
// MENU---content
#define UI_ITEM_CONTENT_MARGIN_L     (0)			
#define UI_ITEM_CONTENT_X            (12)			
#define UI_ITEM_CONTENT_WIDTH        (9)	
#define UI_ITEM_CONTENT_HEIGHT       (1)	
#define UI_ITEM_CONTENT_MARGIN_R     (0)			
#define UI_ITEM_CONTENT_HSIZE        (_X_(9)+2) // 2: 128-21x6=2
#define UI_ITEM_CONTENT_VSIZE        _Y_(1)
// MENU---contentcontent---- <  XXXXX  >
#define UI_ITEM_BRACKET_L_WIDTH      (1)		
#define UI_ITEM_BRACKET_M_WIDTH      (7)		
#define UI_ITEM_BRACKET_R_WIDTH      (1)		
#define UI_ITEM_BRACKET_L_X          (12)
#define UI_ITEM_BRACKET_M_X          (13)
#define UI_ITEM_BRACKET_R_X          (20)
//                              ______
// MENU---contentcontent---- < |______| >
//
#define UI_ITEM_BAR_L_WIDTH          (1)		
#define UI_ITEM_BAR_M_WIDTH          (7)		
#define UI_ITEM_BAR_R_WIDTH          (1)		
#define UI_ITEM_BAR_L_X              (12)
#define UI_ITEM_BAR_M_X              (13)
#define UI_ITEM_BAR_R_X              (20)

// HELP 
#define UI_HELP_XPOS                 _X_(0)
#define UI_HELP_YPOS                 _Y_(15)
#define UI_HELP_WIDTH                (OSD_PANEL_WIDTH)
#define UI_HELP_HEIGHT               (1)
#define UI_HELP_HSIZE                _X_(UI_HELP_WIDTH)  
#define UI_HELP_VSIZE                _Y_(UI_HELP_HEIGHT)  

// POPUP MENU 
#define UI_POPUP_MENU_XPOS           _X_(3)
#define UI_POPUP_MENU_YPOS           _Y_(6)
#define UI_POPUP_MENU_WIDTH          (16)
#define UI_POPUP_MENU_HEIGHT         (3)
#define UI_POPUP_MENU_HSIZE          _X_(UI_POPUP_MENU_WIDTH) 
#define UI_POPUP_MENU_VSIZE          _Y_(UI_POPUP_MENU_HEIGHT)  

#define UI_POPUP_PROMPT_XPOS         (UI_POPUP_MENU_XPOS+_X_(1))
#define UI_POPUP_PROMPT_YPOS         (UI_POPUP_MENU_YPOS+_Y_(1))
#define UI_POPUP_PROMPT_WIDTH        (11)
#define UI_POPUP_PROMPT_HEIGHT       (1)	
#define UI_POPUP_PROMPT_HSIZE        _X_(UI_POPUP_PROMPT_WIDTH)
#define UI_POPUP_PROMPT_VSIZE        _Y_(UI_POPUP_PROMPT_HEIGHT)

#define UI_POPUP_CONTENT_XPOS        (UI_POPUP_MENU_XPOS+_X_(1+UI_POPUP_PROMPT_WIDTH))			
#define UI_POPUP_CONTENT_YPOS        (UI_POPUP_MENU_YPOS+_Y_(1))			
#define UI_POPUP_CONTENT_WIDTH       (3)	
#define UI_POPUP_CONTENT_HEIGHT      (1)	
#define UI_POPUP_CONTENT_HSIZE       _X_(UI_POPUP_CONTENT_WIDTH)
#define UI_POPUP_CONTENT_VSIZE       _Y_(UI_POPUP_CONTENT_HEIGHT)



#elif (OSD_FONT_SIZE == FONT_8X16)
// TODO:
#endif




/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
typedef enum
{
//--------- MAIN MENU---------//
	ITEM_Y_TITLE = UI_TITLE_LINE_START,
	ITEM_Y_SYS_CFG = UI_ITEM_LINE_START,
	ITEM_Y_WIFI_CFG,
	ITEM_Y_BT_CFG,
	ITEM_Y_SYS_INFO,
	ITEM_Y_SYS_STATUS,
	ITEM_Y_CREDITS,
//--------- SUB MENU: SYS CFG ---------//
	ITEM_Y_SYS_TIMEZONE = UI_ITEM_LINE_START,
	ITEM_Y_SYS_DATEFORMAT,
//--------- SUB MENU: WIFI CFG ---------//
	ITEM_Y_WIFI_STATUS = UI_ITEM_LINE_START,
	ITEM_Y_WIFI_STATUS_VALUE,
	ITEM_Y_WIFI_MAC,
	ITEM_Y_WIFI_MAC_VALUE,
	ITEM_Y_WIFI_SSID,
	ITEM_Y_WIFI_SSID_VALUE,
	ITEM_Y_WIFI_RSSI,
	ITEM_Y_WIFI_RSSI_VALUE,
	ITEM_Y_WIFI_IP,
	ITEM_Y_WIFI_IP_VALUE,
//--------- SUB MENU: BT CFG   ---------//
	ITEM_Y_BT_STATUS = UI_ITEM_LINE_START,
	ITEM_Y_BT_STATUS_VALUE,
	ITEM_Y_BT_MAC,
	ITEM_Y_BT_MAC_VALUE,
//--------- SUB MENU: SYS INFO ---------//
	ITEM_Y_SW_VERSION = UI_ITEM_LINE_START,
	ITEM_Y_SW_VERSION_VALUE,
	ITEM_Y_BUILD_INFO,
	ITEM_Y_BUILD_INFO_VALUE,
	/*
	ITEM_Y_SDK_VERSION,
	ITEM_Y_SDK_VERSION_VALUE,
	*/
	ITEM_Y_AT_VERSION,
	ITEM_Y_AT_VERSION_VALUE,
	ITEM_Y_SERIAL_NUMBER,
	ITEM_Y_SERIAL_NUMBER_VALUE,
//--------- SUB MENU: SYS STATUS ---------//
	ITEM_Y_BRIGHTNESS = UI_ITEM_LINE_START,
	ITEM_Y_FANSPEED,
	ITEM_Y_TEMP,
	ITEM_Y_HUMIDITY,
//--------- SUB MENU: CREDITS ---------//
	ITEM_Y_CREDITS_TITLE = UI_ITEM_LINE_START,
	ITEM_Y_CREDITS_CONTENT,
}Osd_ItemIndex_t;


/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
extern OSD_MIA_ENUM const GlobalMenuItemAction[LIST_KEY_MAX];

#if defined(ENABLE_OSD_GAME)
extern OSD_KEY_ACTION_MAP_ST const KeyActMap_Game[];
#endif

extern OSD_MENU_PAGE_ST const tblMenus[];



/*========================================================================================================
										F U N C T I O N
========================================================================================================*/






/*========================================================================================================
										End
========================================================================================================*/
#ifdef __cplusplus 
}
#endif

#endif//_FLEX_APP_OSD_MENUTBL_H_



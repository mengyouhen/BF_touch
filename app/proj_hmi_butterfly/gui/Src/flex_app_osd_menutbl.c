/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_app_osd_menutbl.c
 [Date]:       
 		2018-05-23
 [Author]:   	
 		carson.qian 
 [Reversion History]:
 		v1.0
*******************************************************************************/

/*========================================================================================================
										I N C L U D E
========================================================================================================*/
// external
#include "flex_app_osd_def.h"

// internal
#include "flex_app_osd_menutbl.h"







/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME								"[OSD MENUTBL]--"
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
//============================================================================
// KEY
//============================================================================
OSD_MIA_ENUM const GlobalMenuItemAction[LIST_KEY_MAX] =
{
	LIST_MIA_NOTHING,       // LIST_KEY_INVALID,   
//----------------
	LIST_MIA_KEY_0,         // LIST_KEY_0,         
	LIST_MIA_KEY_1,         // LIST_KEY_1,         
	LIST_MIA_KEY_2,         // LIST_KEY_2,         
	LIST_MIA_KEY_3,         // LIST_KEY_3,         
	LIST_MIA_KEY_4,         // LIST_KEY_4,         
	LIST_MIA_KEY_5,         // LIST_KEY_5,         
	LIST_MIA_KEY_6,         // LIST_KEY_6,         
	LIST_MIA_KEY_7,         // LIST_KEY_7,         
	LIST_MIA_KEY_8,         // LIST_KEY_8,         
	LIST_MIA_KEY_9,         // LIST_KEY_9,         
	LIST_MIA_KEY_OK,        // LIST_KEY_OK,        
	LIST_MIA_GOTOPREV,      // LIST_KEY_DEL,       
//----------------
	LIST_MIA_PREVITEM,      // LIST_KEY_UP,        
	LIST_MIA_NEXTITEM,      // LIST_KEY_DOWN,      
	LIST_MIA_NOTHING,       // LIST_KEY_LEFT,      
	LIST_MIA_NOTHING,       // LIST_KEY_RIGHT,     
//----------------
	LIST_MIA_NOTHING,       // LIST_KEY_GEAR_1,    
	LIST_MIA_NOTHING,       // LIST_KEY_GEAR_2,    
	LIST_MIA_NOTHING,       // LIST_KEY_GEAR_3,    
	LIST_MIA_NOTHING,       // LIST_KEY_GEAR_4,    
	LIST_MIA_NOTHING,       // LIST_KEY_GEAR_5,    
	LIST_MIA_NOTHING,       // LIST_KEY_GEAR_6,   
	LIST_MIA_NOTHING,       // LIST_KEY_GEAR_7,    
	LIST_MIA_NOTHING,       // LIST_KEY_GEAR_8,    
//----------------
	LIST_MIA_NOTHING,       // LIST_KEY_WIFI,      
	LIST_MIA_NOTHING,       // LIST_KEY_BT,        
//----------------
	LIST_MIA_NOTHING,       // LIST_KEY_PLAY,      
	LIST_MIA_NOTHING,       // LIST_KEY_PAUSE,     
//----------------
	LIST_MIA_NOTHING,       // LIST_KEY_PAGE_UP,   
	LIST_MIA_NOTHING,       // LIST_KEY_PAGE_DOWN, 
//----------------
	LIST_MIA_POP_INC,       // LIST_KEY_INC,       
	LIST_MIA_POP_DEC,       // LIST_KEY_DEC, 
//----------------
};

//---------------------------------------
#if defined(ENABLE_OSD_GAME)
OSD_KEY_ACTION_MAP_ST const KeyActMap_Game[] =
{
	{LIST_KEY_UP,       LIST_MIA_UP},
	{LIST_KEY_DOWN,     LIST_MIA_DOWN}, 
	{LIST_KEY_LEFT,     LIST_MIA_LEFT}, 
	{LIST_KEY_RIGHT,    LIST_MIA_RIGHT},
	{LIST_KEY_PLAY,     LIST_MIA_START}, 
	{LIST_KEY_PAUSE,    LIST_MIA_PAUSE},
	{LIST_KEY_DEL,      LIST_MIA_STOP},
	{LIST_KEY_INVALID,  LIST_MIA_NOTHING},
};
#endif

//---------------------------------------
OSD_KEY_ACTION_MAP_ST const KeyActMap_Root[] =
{
	{LIST_KEY_INVALID,  LIST_MIA_NOTHING},
};

OSD_ITEM_INPUT_EVENTS_ST const ItemInputEvent_Root[] =
{
	LIST_KEYMAP_LOCAL,
	KeyActMap_Root,
};

//---------------------------------------
OSD_KEY_ACTION_MAP_ST const KeyActMap_Navi[] =
{
	{LIST_KEY_RIGHT,    LIST_MIA_GOTONEXT},
	{LIST_KEY_INVALID,  LIST_MIA_NOTHING},
};

OSD_ITEM_INPUT_EVENTS_ST const ItemInputEvent_Navi[] =
{
	LIST_KEYMAP_LOCAL,
	KeyActMap_Navi,
};

//---------------------------------------
OSD_KEY_ACTION_MAP_ST const KeyActMap_NaviExec[] =		// goto next + do action
{
	{LIST_KEY_RIGHT,    LIST_MIA_EXECUTE}, 
	{LIST_KEY_INVALID,  LIST_MIA_NOTHING},
};

OSD_ITEM_INPUT_EVENTS_ST const ItemInputEvent_NaviExec[] =
{
	LIST_KEYMAP_LOCAL,
	KeyActMap_NaviExec,
};

//---------------------------------------
OSD_KEY_ACTION_MAP_ST const KeyActMap_NaviAdjust[] =
{
	{LIST_KEY_LEFT,     LIST_MIA_DECVALUE}, 
	{LIST_KEY_RIGHT,    LIST_MIA_INCVALUE},
	{LIST_KEY_INVALID,  LIST_MIA_NOTHING},
};

OSD_ITEM_INPUT_EVENTS_ST const ItemInputEvent_NaviAdjust[] =
{
	LIST_KEYMAP_LOCAL,
	KeyActMap_NaviAdjust,
};

//---------------------------------------
OSD_KEY_ACTION_MAP_ST const KeyActMap_Popup[] =
{
	{LIST_KEY_DEC,      LIST_MIA_POP_DEC}, 
	{LIST_KEY_INC,      LIST_MIA_POP_INC},
	{LIST_KEY_INVALID,  LIST_MIA_NOTHING},
};

OSD_ITEM_INPUT_EVENTS_ST const ItemInputEvent_Popup[] =
{
	LIST_KEYMAP_LOCAL,
	KeyActMap_Popup,
};


//---------------------------------------
OSD_KEY_ACTION_MAP_ST const KeyActMap_NaviInfo[] =
{
	{LIST_KEY_INVALID,  LIST_MIA_NOTHING},
};

OSD_ITEM_INPUT_EVENTS_ST const ItemInputEvent_NaviInfo[] =
{
	LIST_KEYMAP_LOCAL,
	KeyActMap_NaviInfo,
};

//---------------------------------------
OSD_KEY_ACTION_MAP_ST const KeyActMap_ReadOnly[] =
{
	{LIST_KEY_UP,       LIST_MIA_NOTHING},
	{LIST_KEY_DOWN,     LIST_MIA_NOTHING}, 
	{LIST_KEY_LEFT,     LIST_MIA_NOTHING}, 
	{LIST_KEY_RIGHT,    LIST_MIA_NOTHING},
	{LIST_KEY_INVALID,  LIST_MIA_NOTHING},
};

OSD_ITEM_INPUT_EVENTS_ST const ItemInputEvent_ReadOnly[] =
{
	LIST_KEYMAP_LOCAL,
	KeyActMap_NaviAdjust,
};

//============================================================================
// ITEMS
//============================================================================
OSD_MENU_ITEM_ST const RootMenuItems[] =
{
	{
		MI_ENABLE,                     RT_NULL,                // nSelectFlags   ---   nRunTime
		_X_(0),                        _Y_(0),                 // nXPos          ---   nYPos
		CT_MENU_DEFALUT,               UI_ITEM_PROMPT_WIDTH,   // nColor         ---   nWidth
		LIST_DWP_NOTHING,              str_null,               // nDwpType       ---   nDispIdx
		ItemInputEvent_Root,           Func_Null,              // nInputEvents   ---   nKeyFuncIdx
		NULL,                          MENU_MAIN,              // nDrawContent   ---   nNextPage
	}
};
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - -//
OSD_MENU_ITEM_ST const LogoMenuItems[] =
{
	{
		MI_ENABLE,                     RT_NULL,                // nSelectFlags   ---   nRunTime
		_X_(0),                        _Y_(0),                 // nXPos          ---   nYPos
		CT_MENU_DEFALUT,               UI_ITEM_PROMPT_WIDTH,   // nColor         ---   nWidth
		LIST_DWP_LOGO,                 str_null,               // nDwpType       ---   nDispIdx
		ItemInputEvent_Root,           Func_Null,              // nInputEvents   ---   nKeyFuncIdx
		NULL,                          MENU_MAIN,              // nDrawContent   ---   nNextPage
	}
};
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - -//
OSD_MENU_ITEM_ST const AnimationMenuItems[] =
{
	{
		MI_ENABLE,                     RT_NULL,                // nSelectFlags   ---   nRunTime
		_X_(0),                        _Y_(0),                 // nXPos          ---   nYPos
		CT_MENU_DEFALUT,               UI_ITEM_PROMPT_WIDTH,   // nColor         ---   nWidth
		LIST_DWP_LOGO,                 str_null,               // nDwpType       ---   nDispIdx
		ItemInputEvent_Root,           Func_Null,              // nInputEvents   ---   nKeyFuncIdx
		NULL,                          MENU_MAIN,              // nDrawContent   ---   nNextPage
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - -//
OSD_MENU_CONTENT_ST const StatusBarContentGroup[] =
{
	/* DATE */
	{ 
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_NULL,                                 _X_(0),                        UI_STATUSBAR_YPOS,              CT_MENU_STATUSBAR,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_DATE,                           fstr_null,                     (8),                            Func_Null
	},
	/* TIME */
	{ 
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_NULL,                                 _X_(10),                       UI_STATUSBAR_YPOS,              CT_MENU_STATUSBAR,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TIME,                           fstr_null,                     (5),                            Func_Null
	},
	/* WIFI ICON */
	{ 
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_VALUE|RT_DISPLAY,                     _X_(18),                       UI_STATUSBAR_YPOS,              CT_MENU_STATUSBAR,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_ICON,                           ficon_wifi_select,             (2),                            Func_Get_StatusBar_WiFi
	},
	/* BT ICON */
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_VALUE|RT_DISPLAY,                     _X_(20),                       UI_STATUSBAR_YPOS,              CT_MENU_STATUSBAR,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_ICON|DWC_END,                   ficon_bt_select,               (1),                            Func_Get_StatusBar_Bt
	}
};

OSD_MENU_ITEM_ST const StatusBarMenuItems[] =
{
	{// Status Bar Info
		MI_SEL_DISABLE,                RT_NULL,                    // nSelectFlags   ---   nRunTime
		UI_STATUSBAR_XPOS,             UI_STATUSBAR_YPOS,          // nXPos          ---   nYPos
		CT_MENU_STATUSBAR,             UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_NOTHING,              str_null,                   // nDwpType       ---   nDispIdx
		NULL,                          Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		StatusBarContentGroup,         MENU_ROOT,                  // nDrawContent   ---   nNextPage
	}
};
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - -//
OSD_MENU_CONTENT_ST const SysTimeZoneContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_COLOR,                                _X_(UI_ITEM_BRACKET_L_X-1),    _Y_(2*ITEM_Y_SYS_TIMEZONE-1),   LIST_COLOR_MENU_CONTENT,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_L,                         str_bracket_LR8,               (10),                           Func_Null
	},
	{
		// nRunTime 														 nXPos													nYPos 													nColor
		RT_COLOR|RT_VALUE|RT_DISPLAY,            _X_(UI_ITEM_BRACKET_M_X-1),    _Y_(2*ITEM_Y_SYS_TIMEZONE-1),   LIST_COLOR_MENU_CONTENT,
		// nDwcType 														 nDispIdx 											nWidth													nKeyFuncIdx
		LIST_DWC_SIGNED_NUM_C|DWC_END,           str_null,                      (8),    Func_Get_TimeZone
	}
};

OSD_MENU_CONTENT_ST const SysDateFormatContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_COLOR,                                _X_(UI_ITEM_BRACKET_L_X-1),    _Y_(2*ITEM_Y_SYS_DATEFORMAT-1), LIST_COLOR_MENU_CONTENT,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_L,                         str_bracket_LR8,               (10),                           Func_Null
	},
	{
		// nRunTime 														 nXPos													nYPos 													nColor
		RT_COLOR|RT_VALUE|RT_DISPLAY,            _X_(UI_ITEM_BRACKET_M_X-1),    _Y_(2*ITEM_Y_SYS_DATEFORMAT-1), LIST_COLOR_MENU_CONTENT,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_C|DWC_END,                 fstr_date_format_select,       (8),                            Func_Get_DateFormat
	}
};

OSD_MENU_ITEM_ST const SysConfigMenuItems[] =
{
	{// Title
		MI_SEL_DISABLE,                RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_TITLE),          // nXPos          ---   nYPos
		CT_MENU_TITLE,                 UI_TITLE_WIDTH,             // nColor         ---   nWidth
		LIST_DWP_TEXT_C,               str_sys_cfg,                // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		NULL,                          MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{// TIME ZONE
		MI_ENABLE,                     RT_COLOR,                   // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(2*ITEM_Y_SYS_TIMEZONE-1),// nXPos          ---   nYPos
		LIST_COLOR_MENU_PROMPT,        UI_ITEM_PROMPT_WIDTH,       // nColor		 ---   nWidth
		LIST_DWP_TEXT_L,               str_timezone,               // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviAdjust,     Func_Set_TimeZone,          // nInputEvents   ---   nKeyFuncIdx
		SysTimeZoneContentGroup,       MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{// DATE FORMAT
		MI_ENABLE,                     RT_COLOR,                   // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(2*ITEM_Y_SYS_DATEFORMAT-1),// nXPos          ---   nYPos
		LIST_COLOR_MENU_PROMPT,        UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_dateformat,             // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviAdjust,     Func_Set_DateFormat,        // nInputEvents   ---   nKeyFuncIdx
		SysDateFormatContentGroup,     MENU_ROOT,                  // nDrawContent   ---   nNextPage
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - -//
OSD_MENU_CONTENT_ST const WiFiStatusContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_VALUE|RT_DISPLAY,                     _X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_WIFI_STATUS_VALUE),  CT_MENU_ITEM_SEL_DISABLE,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_R|DWC_END,                 fstr_wifi_status_select,       UI_ITEM_WIDTH,                  Func_Get_Wifi_Status
	}
};

OSD_MENU_CONTENT_ST const WiFiMacContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_NULL,                                 _X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_WIFI_MAC_VALUE),     CT_MENU_ITEM_SEL_DISABLE,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_R|DWC_END,                 fstr_wifi_mac_content,         UI_ITEM_WIDTH,                  Func_Null
	},
};

OSD_MENU_CONTENT_ST const WiFiSsidContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_NULL,                                 _X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_WIFI_SSID_VALUE),    CT_MENU_ITEM_SEL_DISABLE,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_R|DWC_END,                 fstr_wifi_ssid_content,        UI_ITEM_WIDTH,                  Func_Null
	}
};

OSD_MENU_CONTENT_ST const WiFiRssiContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_VALUE|RT_DISPLAY,                     _X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_WIFI_RSSI_VALUE),    CT_MENU_ITEM_SEL_DISABLE,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_R|DWC_END,                 fstr_wifi_rssi_select,         UI_ITEM_WIDTH,                  Func_Get_Wifi_Rssi
	},
};

OSD_MENU_CONTENT_ST const WiFiIpContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_NULL,                                 _X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_WIFI_IP_VALUE),      CT_MENU_ITEM_SEL_DISABLE,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_R|DWC_END,                 fstr_wifi_ip_content,          UI_ITEM_WIDTH,                  Func_Null
	},
};

OSD_MENU_ITEM_ST const WifiConfigMenuItems[] =
{
	{// Title
		MI_SEL_DISABLE,                RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_TITLE),          // nXPos          ---   nYPos
		CT_MENU_TITLE,                 UI_TITLE_WIDTH,             // nColor         ---   nWidth
		LIST_DWP_TEXT_C,               str_wifi_cfg,               // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		NULL,                          MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{// WIFI STATUS
		MI_ENABLE,                     RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_WIFI_STATUS),    // nXPos          ---   nYPos
		CT_MENU_ITEM_SEL_DISABLE,      (UI_ITEM_PROMPT_WIDTH+1),   // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_wifi_status,            // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		WiFiStatusContentGroup,        MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{// MAC(ESP)
		MI_SEL_DISABLE,                RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_WIFI_MAC),       // nXPos          ---   nYPos
		CT_MENU_ITEM_SEL_DISABLE,      UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_wifi_mac,               // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		WiFiMacContentGroup,           MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{// SSID
		MI_WIFI_CONN_ONLY,             RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_WIFI_SSID),      // nXPos          ---   nYPos
		CT_MENU_ITEM_SEL_DISABLE,      UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_wifi_ssid,              // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		WiFiSsidContentGroup,          MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{// RSSI
		MI_WIFI_CONN_ONLY,             RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_WIFI_RSSI),      // nXPos          ---   nYPos
		CT_MENU_ITEM_SEL_DISABLE,      UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_wifi_rssi,              // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		WiFiRssiContentGroup,          MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{// IP
		MI_WIFI_CONN_ONLY,            RT_NULL,                     // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),        _Y_(ITEM_Y_WIFI_IP),         // nXPos          ---   nYPos
		CT_MENU_ITEM_SEL_DISABLE,     UI_ITEM_PROMPT_WIDTH,        // nColor         ---   nWidth
		LIST_DWP_TEXT_L,              str_wifi_ip,                 // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,      Func_Null,                   // nInputEvents   ---   nKeyFuncIdx
		WiFiIpContentGroup,           MENU_ROOT,                   // nDrawContent   ---   nNextPage
	},
};
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - -//
OSD_MENU_CONTENT_ST const BtStatusContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_VALUE|RT_DISPLAY,                     _X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_BT_STATUS_VALUE),    CT_MENU_ITEM_SEL_DISABLE,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_R|DWC_END,                 fstr_bt_status_select,         UI_ITEM_WIDTH,                  Func_Get_Bt_Status
	}
};

OSD_MENU_CONTENT_ST const BtMacContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_NULL,                                 _X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_BT_MAC_VALUE),       CT_MENU_ITEM_SEL_DISABLE,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_R|DWC_END,                 fstr_bt_mac_content,           UI_ITEM_WIDTH,                  Func_Null
	}
};

OSD_MENU_ITEM_ST const BtConfigMenuItems[] =
{
	{// Title
		MI_SEL_DISABLE,                RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_TITLE),          // nXPos          ---   nYPos
		CT_MENU_TITLE,                 UI_TITLE_WIDTH,             // nColor         ---   nWidth
		LIST_DWP_TEXT_C,               str_bt_cfg,                 // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		NULL,                          MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{// BT STATUS
		MI_ENABLE,                     RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_BT_STATUS),      // nXPos          ---   nYPos
		CT_MENU_ITEM_SEL_DISABLE,      UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_bt_status,              // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		BtStatusContentGroup,          MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{// BT MAC
		MI_SEL_DISABLE,                RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_BT_MAC),         // nXPos          ---   nYPos
		CT_MENU_ITEM_SEL_DISABLE,      UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_bt_mac,                 // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		BtMacContentGroup,             MENU_ROOT,                  // nDrawContent   ---   nNextPage
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - -//
OSD_MENU_CONTENT_ST const InfoSwVersionContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_VALUE|RT_DISPLAY,                     _X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_SW_VERSION_VALUE),   CT_MENU_ITEM_SEL_DISABLE,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_R|DWC_END,                 str_sw_version_value,          UI_ITEM_WIDTH,                  Func_Null
	},
};

OSD_MENU_CONTENT_ST const InfoBuildInfoContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_NULL,                                 _X_(UI_ITEM_PROMPT_X+1),       _Y_(ITEM_Y_BUILD_INFO_VALUE),   CT_MENU_ITEM_SEL_DISABLE,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_R,                         str_build_info_date,           11,                             Func_Null
	},
	{
		// nRunTime 				     nXPos					nYPos 				  nColor
		RT_NULL,                                 _X_(UI_ITEM_PROMPT_X+13),      _Y_(ITEM_Y_BUILD_INFO_VALUE),   CT_MENU_ITEM_SEL_DISABLE,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_R|DWC_END,                 str_build_info_time,           8,                              Func_Null
	}
};
/*
OSD_MENU_CONTENT_ST InfoSdkVersionContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_VALUE|RT_DISPLAY,                     _X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_SDK_VERSION_VALUE),  CT_MENU_ITEM_SEL_DISABLE,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_R|DWC_END,                 fstr_sdk_ver_content,          UI_ITEM_WIDTH,                  Func_Null
	},
};
*/
OSD_MENU_CONTENT_ST const InfoAtVersionContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_NULL,                                 _X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_AT_VERSION_VALUE),   CT_MENU_ITEM_SEL_DISABLE,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_R|DWC_END,                 fstr_at_ver_content,           UI_ITEM_WIDTH,                  Func_Null
	}
};

OSD_MENU_CONTENT_ST const InfoSerialNumerContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_NULL,                                 _X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_SERIAL_NUMBER_VALUE),CT_MENU_ITEM_SEL_DISABLE,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_R|DWC_END,                 str_serial_number_value,       UI_ITEM_WIDTH,                  Func_Null
	}
};

OSD_MENU_ITEM_ST const SysInfoMenuItems[] =
{
	{// Title
		MI_SEL_DISABLE,                RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_TITLE),          // nXPos          ---   nYPos
		CT_MENU_TITLE,                 UI_TITLE_WIDTH,             // nColor         ---   nWidth
		LIST_DWP_TEXT_C,               str_sys_info,               // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		NULL,                          MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{// SW VERSION
		MI_ENABLE,                     RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_SW_VERSION),     // nXPos          ---   nYPos
		CT_MENU_ITEM_SEL_DISABLE,      UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_sw_version,             // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		InfoSwVersionContentGroup,     MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{// BUILD INFO
		MI_SEL_DISABLE,                RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_BUILD_INFO),     // nXPos          ---   nYPos
		CT_MENU_ITEM_SEL_DISABLE,      UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_build_info,             // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		InfoBuildInfoContentGroup,     MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	/*
	{// SDK VERSION
		MI_SEL_DISABLE,                RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_SDK_VERSION),    // nXPos          ---   nYPos
		CT_MENU_ITEM_SEL_DISABLE,      UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_sdk_version,            // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		InfoSdkVersionContentGroup,    MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	*/
	{// AT VERSION
		MI_SEL_DISABLE,                RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_AT_VERSION),     // nXPos          ---   nYPos
		CT_MENU_ITEM_SEL_DISABLE,      UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_at_version,             // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		InfoAtVersionContentGroup,     MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{// SN 
		MI_SEL_DISABLE,                RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_SERIAL_NUMBER),  // nXPos          ---   nYPos
		CT_MENU_ITEM_SEL_DISABLE,      UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_serial_number,          // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		InfoSerialNumerContentGroup,   MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - -//
OSD_MENU_CONTENT_ST const StatusBrightnessContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_VALUE,                                _X_(UI_ITEM_CONTENT_X),        _Y_(ITEM_Y_BRIGHTNESS),         CT_MENU_ITEM_SEL_DISABLE,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_NUMBER_R|DWC_END,               str_null,                      UI_ITEM_CONTENT_WIDTH,          Func_Get_Brightness        	
	},
};

OSD_MENU_CONTENT_ST const StatusFanSpeedContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_VALUE|RT_DISPLAY,                     _X_(UI_ITEM_CONTENT_X),        _Y_(ITEM_Y_FANSPEED),           CT_MENU_ITEM_SEL_DISABLE,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_R|DWC_END,                 fstr_fan_speed_select,         UI_ITEM_CONTENT_WIDTH,          Func_Get_FanSpeed
	},
};

OSD_MENU_ITEM_ST const SysStatusMenuItems[] =
{
	{// Title
		MI_SEL_DISABLE,                RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_TITLE),          // nXPos          ---   nYPos
		CT_MENU_TITLE,                 UI_TITLE_WIDTH,             // nColor         ---   nWidth
		LIST_DWP_TEXT_C,               str_sys_status,             // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		NULL,                          MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{// BRIGHTNESS
		MI_ENABLE,                     RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_BRIGHTNESS),     // nXPos          ---   nYPos
		CT_MENU_ITEM_SEL_DISABLE,      UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_brightness,             // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		StatusBrightnessContentGroup,  MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{// FANSPEED
		MI_DISABLE,                    RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_FANSPEED),       // nXPos          ---   nYPos
		CT_MENU_ITEM_SEL_DISABLE,      UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_fanspeed,               // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		StatusFanSpeedContentGroup,    MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - -//
OSD_MENU_CONTENT_ST const CreditsListContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_NULL,                                 _X_(0),                        _Y_(ITEM_Y_CREDITS_TITLE),      CT_MENU_ITEM_SEL_DISABLE,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_CREDIT_TITLE,                   str_null,                      0,                              Func_Null
	},
	{
		// nRunTime 														 nXPos													nYPos 													nColor
		RT_NULL,                                 _X_(0),                        _Y_(ITEM_Y_CREDITS_CONTENT),    CT_MENU_ITEM_SEL_DISABLE,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_CREDIT_LIST|DWC_END,            str_null,                      0,                              Func_Null
	}
};

OSD_MENU_ITEM_ST const CreditsMenuItems[] =
{
	{// Title
		MI_SEL_DISABLE,                RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_TITLE),          // nXPos          ---   nYPos
		CT_MENU_TITLE,                 UI_TITLE_WIDTH,             // nColor         ---   nWidth
		LIST_DWP_TEXT_C,               str_credits,                // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		NULL,                          MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{// CREDITS
		MI_ENABLE,                     RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_CREDITS_TITLE),  // nXPos          ---   nYPos
		CT_MENU_ITEM_SEL_DISABLE,      UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_NOTHING,              str_null,                   // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		CreditsListContentGroup,       MENU_ROOT,                  // nDrawContent   ---   nNextPage
	}
};
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - -//
#if 1
OSD_MENU_CONTENT_ST const PopupBriContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_VALUE|RT_DISPLAY,                     UI_POPUP_CONTENT_XPOS,         UI_POPUP_CONTENT_YPOS,          CT_MENU_ITEM_UNSEL,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_NUMBER_L|DWC_END,               str_null,                      UI_POPUP_CONTENT_WIDTH,         Func_Get_Brightness        	
	},
};

OSD_MENU_ITEM_ST const PopupBriMenuItems[] =
{
	{// BRIGHTNESS
		MI_ENABLE,                     RT_NULL,                    // nSelectFlags   ---   nRunTime
		UI_POPUP_PROMPT_XPOS,          UI_POPUP_PROMPT_YPOS,       // nXPos          ---   nYPos
		CT_MENU_ITEM_UNSEL,            UI_POPUP_PROMPT_WIDTH,      // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_brightness,             // nDwpType       ---   nDispIdx
		ItemInputEvent_Popup,          Func_Set_Brightness,        // nInputEvents   ---   nKeyFuncIdx
		PopupBriContentGroup,          MENU_ROOT,                  // nDrawContent   ---   nNextPage
	}
};
#else
OSD_MENU_CONTENT_ST const PopupBriContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_COLOR|RT_VALUE|RT_DISPLAY,            UI_POPUP_CONTENT_XPOS,         UI_POPUP_CONTENT_YPOS,          LIST_COLOR_MENU_CONTENT,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_NUMBER_C|DWC_END,               str_null,                      UI_POPUP_CONTENT_WIDTH,         Func_Get_Brightness        	
	},
};

OSD_MENU_ITEM_ST const PopupBriMenuItems[] =
{
	{// BRIGHTNESS
		MI_ENABLE,                     RT_COLOR,                   // nSelectFlags   ---   nRunTime
		UI_POPUP_PROMPT_XPOS,          UI_POPUP_PROMPT_YPOS,       // nXPos          ---   nYPos
		LIST_COLOR_MENU_PROMPT,        UI_POPUP_PROMPT_WIDTH,      // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_brightness,             // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviAdjust,     Func_Set_Brightness,        // nInputEvents   ---   nKeyFuncIdx
		PopupBriContentGroup,          MENU_ROOT,                  // nDrawContent   ---   nNextPage
	}
};
#endif
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - -//
OSD_MENU_CONTENT_ST const HelpNaviContentGroup[] =
{
	/* BT ICON */
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_NULL,                                 _X_(3),                        UI_HELP_YPOS,                   CT_MENU_HELP,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_L|DWC_END,                 str_help_select,               (3),                            Func_Null
	}
};

OSD_MENU_CONTENT_ST const HelpControlContentGroup[] =
{
	/* BT ICON */
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_XPOS|RT_VALUE|RT_DISPLAY,             _X_(10),                       UI_HELP_YPOS,                   CT_MENU_HELP,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_L|DWC_END,                 fstr_help_lr_select,           (5),                            Func_Get_Help_LR_label
	}
};

OSD_MENU_CONTENT_ST const HelpReturnContentGroup[] =
{
	/* BT ICON */
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_NULL,                                 _X_(18),                       UI_HELP_YPOS,                   CT_MENU_HELP,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_L|DWC_END,                 str_help_return,               (3),                            Func_Null
	}
};


OSD_MENU_ITEM_ST const HelpMenuItems[] =
{
	{//  ^
		MI_HELP_NAVI,                  RT_NULL,                    // nSelectFlags   ---   nRunTime
		UI_HELP_XPOS+_X_(0),           UI_HELP_YPOS,               // nXPos          ---   nYPos
		CT_MENU_HELP,                  1,                          // nColor         ---   nWidth
		LIST_DWP_ICON,                 icon_arrow_up,              // nDwpType       ---   nDispIdx
		NULL,                          Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		NULL,                          MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{//  v
		MI_HELP_NAVI,                  RT_NULL,                    // nSelectFlags   ---   nRunTime
		UI_HELP_XPOS+_X_(1),           UI_HELP_YPOS,               // nXPos          ---   nYPos
		CT_MENU_HELP,                  1,                          // nColor         ---   nWidth
		LIST_DWP_ICON,                 icon_arrow_down,            // nDwpType       ---   nDispIdx
		NULL,                          Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		NULL,                          MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{//  :
		MI_HELP_NAVI,                  RT_NULL,                    // nSelectFlags   ---   nRunTime
		UI_HELP_XPOS+_X_(2),           UI_HELP_YPOS,               // nXPos          ---   nYPos
		CT_MENU_HELP,                  1,                          // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_help_colon,             // nDwpType       ---   nDispIdx
		NULL,                          Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		HelpNaviContentGroup,          MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
//------------------------------------------------
	{// >>>>
		MI_HELP_SLIDE,                 RT_NULL,                    // nSelectFlags	 ---	 nRunTime
		UI_HELP_XPOS+_X_(5),           UI_HELP_YPOS,               // nXPos 				 ---	 nYPos
		CT_MENU_HELP,                  4,                          // nColor				 ---	 nWidth
		LIST_DWP_ICON,                 icon_slide,                 // nDwpType			 ---	 nDispIdx
		NULL,                          Func_Null,                  // nInputEvents	 ---	 nKeyFuncIdx
		NULL,                          MENU_ROOT,                  // nDrawContent	 ---	 nNextPage
	},
	{// <
		MI_HELP_ARROW_L,               RT_NULL,                    // nSelectFlags   ---   nRunTime
		UI_HELP_XPOS+_X_(7),           UI_HELP_YPOS,               // nXPos          ---   nYPos
		CT_MENU_HELP,                  1,                          // nColor         ---   nWidth
		LIST_DWP_ICON,                 icon_arrow_left,            // nDwpType       ---   nDispIdx
		NULL,                          Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		NULL,                          MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{// >
		MI_HELP_ARROW_R,               RT_XPOS,                    // nSelectFlags   ---   nRunTime
		UI_HELP_XPOS+_X_(8),           UI_HELP_YPOS,               // nXPos          ---   nYPos
		CT_MENU_HELP,                  1,                          // nColor         ---   nWidth
		LIST_DWP_ICON,                 icon_arrow_right,           // nDwpType       ---   nDispIdx
		NULL,                          Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		NULL,                          MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{// :
		MI_HELP_ARROW_COLON,           RT_XPOS,                    // nSelectFlags   ---   nRunTime
		UI_HELP_XPOS+_X_(9),           UI_HELP_YPOS,               // nXPos          ---   nYPos
		CT_MENU_HELP,                  1,                          // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_help_colon,             // nDwpType       ---   nDispIdx
		NULL,                          Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		HelpControlContentGroup,       MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
//------------------------------------------------
	{// RET
		MI_HELP_RET,                   RT_NULL,                    // nSelectFlags   ---   nRunTime
		UI_HELP_XPOS+_X_(15),          UI_HELP_YPOS,               // nXPos          ---   nYPos
		CT_MENU_HELP,                  2,                          // nColor         ---   nWidth
		LIST_DWP_ICON,                 icon_return,                // nDwpType       ---   nDispIdx
		NULL,                          Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		NULL,                          MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{// :
		MI_HELP_RET,                   RT_NULL,                    // nSelectFlags   ---   nRunTime
		UI_HELP_XPOS+_X_(17),          UI_HELP_YPOS,               // nXPos          ---   nYPos
		CT_MENU_HELP,                  1,                          // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_help_colon,             // nDwpType       ---   nDispIdx
		NULL,                          Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		HelpReturnContentGroup,        MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
};
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - -//
OSD_MENU_CONTENT_ST const  MainSysConfigContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_COLOR,                                _X_(UI_ITEM_CONTENT_X),        _Y_(2*ITEM_Y_SYS_CFG-1),        LIST_COLOR_MENU_CONTENT,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_L|DWC_END,                 str_bracket_R,                 UI_ITEM_CONTENT_WIDTH,          Func_Null        	
	}
};

OSD_MENU_CONTENT_ST const  MainWiFiConfigContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_COLOR,                                _X_(UI_ITEM_CONTENT_X),        _Y_(2*ITEM_Y_WIFI_CFG-1),       LIST_COLOR_MENU_CONTENT,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_L|DWC_END,                 str_bracket_R,                 UI_ITEM_CONTENT_WIDTH,          Func_Null        	
	}
};

OSD_MENU_CONTENT_ST const MainBtConfigContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_COLOR,                                _X_(UI_ITEM_CONTENT_X),        _Y_(2*ITEM_Y_BT_CFG-1),         LIST_COLOR_MENU_CONTENT,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_L|DWC_END,                 str_bracket_R,                 UI_ITEM_CONTENT_WIDTH,          Func_Null        	
	}
};

OSD_MENU_CONTENT_ST const MainSysInfoContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_COLOR,                                _X_(UI_ITEM_CONTENT_X),        _Y_(2*ITEM_Y_SYS_INFO-1),       LIST_COLOR_MENU_CONTENT,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_L|DWC_END,                 str_bracket_R,                 UI_ITEM_CONTENT_WIDTH,          Func_Null        	
	}
};

OSD_MENU_CONTENT_ST const MainSysStatusContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_COLOR,                                _X_(UI_ITEM_CONTENT_X),        _Y_(2*ITEM_Y_SYS_STATUS-1),     LIST_COLOR_MENU_CONTENT,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_L|DWC_END,                 str_bracket_R,                 UI_ITEM_CONTENT_WIDTH,          Func_Null        	
	}
};

OSD_MENU_CONTENT_ST const MainCreditsContentGroup[] =
{
	{
		// nRunTime                              nXPos                          nYPos                           nColor
		RT_COLOR,                                _X_(UI_ITEM_CONTENT_X),        _Y_(2*ITEM_Y_CREDITS-1),        LIST_COLOR_MENU_CONTENT,
		// nDwcType                              nDispIdx                       nWidth                          nKeyFuncIdx
		LIST_DWC_TEXT_L|DWC_END,                 str_bracket_R,                 UI_ITEM_CONTENT_WIDTH,          Func_Null        	
	}
};

OSD_MENU_ITEM_ST const MainMenuItems[] =
{
	{// Title
		MI_SEL_DISABLE,                RT_NULL,                    // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(ITEM_Y_TITLE),          // nXPos          ---   nYPos
		CT_MENU_TITLE,                 UI_TITLE_WIDTH,             // nColor         ---   nWidth
		LIST_DWP_TEXT_C,               str_Title,                  // nDwpType       ---   nDispIdx
		ItemInputEvent_NaviInfo,       Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		NULL,                          MENU_ROOT,                  // nDrawContent   ---   nNextPage
	},
	{// SYS CONFIG
		MI_ENABLE,                     RT_COLOR,                   // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(2*ITEM_Y_SYS_CFG-1),    // nXPos          ---   nYPos
		LIST_COLOR_MENU_PROMPT,        UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_sys_cfg,                // nDwpType       ---   nDispIdx
		ItemInputEvent_Navi,           Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		MainSysConfigContentGroup,     MENU_SYS_CFG,               // nDrawContent   ---   nNextPage
	},
	{// WIFI CONFIG
		MI_ENABLE,                     RT_COLOR,                   // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(2*ITEM_Y_WIFI_CFG-1),   // nXPos          ---   nYPos
		LIST_COLOR_MENU_PROMPT,        UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_wifi_cfg,               // nDwpType       ---   nDispIdx
		ItemInputEvent_Navi,           Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		MainWiFiConfigContentGroup,    MENU_WIFI_CFG,              // nDrawContent   ---   nNextPage
	},
	{// BT CONFIG
		MI_ENABLE,                     RT_COLOR,                   // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(2*ITEM_Y_BT_CFG-1),     // nXPos          ---   nYPos
		LIST_COLOR_MENU_PROMPT,        UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_bt_cfg,                 // nDwpType       ---   nDispIdx
		ItemInputEvent_Navi,           Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		MainBtConfigContentGroup,      MENU_BT_CFG,                // nDrawContent   ---   nNextPage
	},
	{// SYSTEM INFO
		MI_ENABLE,                     RT_COLOR,                   // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(2*ITEM_Y_SYS_INFO-1),   // nXPos          ---   nYPos
		LIST_COLOR_MENU_PROMPT,        UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_sys_info,               // nDwpType       ---   nDispIdx
		ItemInputEvent_Navi,           Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		MainSysInfoContentGroup,       MENU_SYS_INFO,              // nDrawContent   ---   nNextPage
	},
	{// SYSTEM STATUS
		MI_ENABLE,                     RT_COLOR,                   // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(2*ITEM_Y_SYS_STATUS-1), // nXPos          ---   nYPos
		LIST_COLOR_MENU_PROMPT,        UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_sys_status,             // nDwpType       ---   nDispIdx
		ItemInputEvent_Navi,           Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		MainSysStatusContentGroup,     MENU_SYS_STATUS,            // nDrawContent   ---   nNextPage
	},
	{// CREDITS
		MI_ENABLE,                     RT_COLOR,                   // nSelectFlags   ---   nRunTime
		_X_(UI_ITEM_PROMPT_X),         _Y_(2*ITEM_Y_CREDITS-1),    // nXPos          ---   nYPos
		LIST_COLOR_MENU_PROMPT,        UI_ITEM_PROMPT_WIDTH,       // nColor         ---   nWidth
		LIST_DWP_TEXT_L,               str_credits,                // nDwpType       ---   nDispIdx
		ItemInputEvent_Navi,           Func_Null,                  // nInputEvents   ---   nKeyFuncIdx
		MainCreditsContentGroup,       MENU_CREDITS,               // nDrawContent   ---   nNextPage
	},
};

//============================================================================
// menu table
//============================================================================
OSD_MENU_PAGE_ST const tblMenus[]=
{
// ------------------------- 
// 0: MENU_ROOT
// ------------------------- 
	{ 
		UI_MENU_XPOS, UI_MENU_YPOS,                             // nXpos, nYpos;
		UI_MENU_WIDTH, UI_MENU_HEIGHT,                          // nWidth, nHeight;
		MENU_ROOT,                                              // nPrevPage;
		RootMenuItems,                                          // nMenuItems;
		sizeof(RootMenuItems)/sizeof(OSD_MENU_ITEM_ST),         // nMenuItemCount;
		_X_(0), _Y_(0),                                         // nPromptSelSize, nContentSelSize;
		NULL,                                                   // nExecFunction;
		MP_INVISIBLE                                            // nFlags;
	},
// ------------------------- 
// 1: MENU_LOGO
// ------------------------- 
	{ 
		UI_LOGO_XPOS, UI_LOGO_YPOS,                             // nXpos, nYpos;
		UI_LOGO_HSIZE, UI_LOGO_VSIZE,                           // nWidth, nHeight;
		MENU_ROOT,                                              // nPrevPage;
	#if defined(ENABLE_OSD_LOGO)
		LogoMenuItems,                                          // nMenuItems;
		sizeof(LogoMenuItems)/sizeof(OSD_MENU_ITEM_ST),         // nMenuItemCount;
	#else		
		RootMenuItems,                                          // nMenuItems;
		sizeof(RootMenuItems)/sizeof(OSD_MENU_ITEM_ST),         // nMenuItemCount;
	#endif		
		_X_(0), _Y_(0),                                         // nPromptSelSize, nContentSelSize;
		NULL,                                                   // nExecFunction;
		MP_STAY                                                 // nFlags;
	},
// ------------------------- 
// 2: MENU_ANIMATION
// ------------------------- 
	{ 
		UI_ANIMATION_XPOS, UI_ANIMATION_YPOS,                   // nXpos, nYpos;
		UI_ANIMATION_HSIZE, UI_ANIMATION_VSIZE,                 // nWidth, nHeight;
		MENU_ROOT,                                              // nPrevPage;
#if defined(ENABLE_OSD_ANIMATION)
		AnimationMenuItems,                                     // nMenuItems;
		sizeof(AnimationMenuItems)/sizeof(OSD_MENU_ITEM_ST),    // nMenuItemCount;
#else		
		RootMenuItems,                                          // nMenuItems;
		sizeof(RootMenuItems)/sizeof(OSD_MENU_ITEM_ST),         // nMenuItemCount;
#endif		
		_X_(0), _Y_(0),                                         // nPromptSelSize, nContentSelSize;
		NULL,                                                   // nExecFunction;
		MP_MOVING                                               // nFlags;
	},
// ------------------------- 
// 3: MENU_STATUS_BAR
// ------------------------- 
	{ 
		UI_STATUSBAR_XPOS, UI_STATUSBAR_YPOS,                   // nXpos, nYpos;
		UI_STATUSBAR_WIDTH, UI_STATUSBAR_HEIGHT,                // nWidth, nHeight;
		MENU_ROOT,                                              // nPrevPage;
		StatusBarMenuItems,                                     // nMenuItems;
		sizeof(StatusBarMenuItems)/sizeof(OSD_MENU_ITEM_ST),    // nMenuItemCount;
		_X_(0), _Y_(0),                                         // nPromptSelSize, nContentSelSize;
		NULL,                                                   // nExecFunction;
		MP_STAY                                                 // nFlags;
	},
// ------------------------- 
// 4: MENU_MAIN
// ------------------------- 
	{ 
		UI_MENU_XPOS, UI_MENU_YPOS,                             // nXpos, nYpos;
		UI_MENU_WIDTH, UI_MENU_HEIGHT,                          // nWidth, nHeight;
		MENU_ROOT,                                              // nPrevPage;
		MainMenuItems,                                          // nMenuItems;
		sizeof(MainMenuItems)/sizeof(OSD_MENU_ITEM_ST),         // nMenuItemCount;
		UI_ITEM_PROMPT_HSIZE, UI_ITEM_CONTENT_HSIZE,            // nPromptSelSize, nContentSelSize;
		NULL,                                                   // nExecFunction;
		MP_STAY                                                 // nFlags;
	},
// ------------------------- 
// 5: MENU_SYS_CFG
// ------------------------- 
	{ 
		UI_MENU_XPOS, UI_MENU_YPOS,                             // nXpos, nYpos;
		UI_MENU_WIDTH, UI_MENU_HEIGHT,                          // nWidth, nHeight;
		MENU_MAIN,                                              // nPrevPage;
		SysConfigMenuItems,                                     // nMenuItems;
		sizeof(SysConfigMenuItems)/sizeof(OSD_MENU_ITEM_ST),    // nMenuItemCount;
		UI_ITEM_PROMPT_HSIZE, UI_ITEM_CONTENT_HSIZE,            // nPromptSelSize, nContentSelSize;
		NULL,                                                   // nExecFunction;
		MP_STAY                                                 // nFlags;
	},
// ------------------------- 
// 6: MENU_WIFI_CFG
// ------------------------- 
	{ 
		UI_MENU_XPOS, UI_MENU_YPOS,                             // nXpos, nYpos;
		UI_MENU_WIDTH, UI_MENU_HEIGHT,                          // nWidth, nHeight;
		MENU_MAIN,                                              // nPrevPage;
		WifiConfigMenuItems,                                    // nMenuItems;
		sizeof(WifiConfigMenuItems)/sizeof(OSD_MENU_ITEM_ST),   // nMenuItemCount;
		UI_ITEM_PROMPT_HSIZE, UI_ITEM_CONTENT_HSIZE,            // nPromptSelSize, nContentSelSize;
		NULL,                                                   // nExecFunction;
		MP_STAY                                                 // nFlags;
	},
// ------------------------- 
// 7: MENU_BT_CFG
// ------------------------- 
	{ 
		UI_MENU_XPOS, UI_MENU_YPOS,                             // nXpos, nYpos;
		UI_MENU_WIDTH, UI_MENU_HEIGHT,                          // nWidth, nHeight;
		MENU_MAIN,                                              // nPrevPage;
		BtConfigMenuItems,                                      // nMenuItems;
		sizeof(BtConfigMenuItems)/sizeof(OSD_MENU_ITEM_ST),     // nMenuItemCount;
		UI_ITEM_PROMPT_HSIZE, UI_ITEM_CONTENT_HSIZE,            // nPromptSelSize, nContentSelSize;
		NULL,                                                   // nExecFunction;
		MP_STAY                                                 // nFlags;
	},
// ------------------------- 
// 8: MENU_SYS_INFO
// ------------------------- 
	{ 
		UI_MENU_XPOS, UI_MENU_YPOS,                             // nXpos, nYpos;
		UI_MENU_WIDTH, UI_MENU_HEIGHT,                          // nWidth, nHeight;
		MENU_MAIN,                                              // nPrevPage;
		SysInfoMenuItems,                                       // nMenuItems;
		sizeof(SysInfoMenuItems)/sizeof(OSD_MENU_ITEM_ST),      // nMenuItemCount;
		UI_ITEM_PROMPT_HSIZE, UI_ITEM_CONTENT_HSIZE,            // nPromptSelSize, nContentSelSize;
		NULL,                                                   // nExecFunction;
		MP_STAY                                                 // nFlags;
	},
// ------------------------- 
// 9: MENU_SYS_STATUS
// ------------------------- 
	{ 
		UI_MENU_XPOS, UI_MENU_YPOS,                             // nXpos, nYpos;
		UI_MENU_WIDTH, UI_MENU_HEIGHT,                          // nWidth, nHeight;
		MENU_MAIN,                                              // nPrevPage;
		SysStatusMenuItems,                                     // nMenuItems;
		sizeof(SysStatusMenuItems)/sizeof(OSD_MENU_ITEM_ST),    // nMenuItemCount;
		UI_ITEM_PROMPT_HSIZE, UI_ITEM_CONTENT_HSIZE,            // nPromptSelSize, nContentSelSize;
		NULL,                                                   // nExecFunction;
		MP_STAY                                                 // nFlags;
	},
// ------------------------- 
// 10: MENU_CREDITS
// ------------------------- 
	{ 
		UI_MENU_XPOS, UI_MENU_YPOS,                             // nXpos, nYpos;
		UI_MENU_WIDTH, UI_MENU_HEIGHT,                          // nWidth, nHeight;
		MENU_MAIN,                                              // nPrevPage;
		CreditsMenuItems,                                       // nMenuItems;
		sizeof(CreditsMenuItems)/sizeof(OSD_MENU_ITEM_ST),      // nMenuItemCount;
		UI_ITEM_PROMPT_HSIZE, UI_ITEM_CONTENT_HSIZE,            // nPromptSelSize, nContentSelSize;
		NULL,                                                   // nExecFunction;
		MP_STAY                                                 // nFlags;
	},
// ------------------------- 
// 10: MENU_POPUP_BRIGHTNESS
// ------------------------- 
	{ 
		UI_POPUP_MENU_XPOS, UI_POPUP_MENU_YPOS,                 // nXpos, nYpos;
		UI_POPUP_MENU_WIDTH, UI_POPUP_MENU_HEIGHT,							// nWidth, nHeight;
		MENU_ROOT,																							// nPrevPage;
		PopupBriMenuItems,																			// nMenuItems;
		sizeof(PopupBriMenuItems)/sizeof(OSD_MENU_ITEM_ST), 		// nMenuItemCount;
		UI_POPUP_PROMPT_HSIZE, UI_POPUP_CONTENT_HSIZE,					// nPromptSelSize, nContentSelSize;
		NULL, 																									// nExecFunction;
		MP_DRAWFRAME|MP_AUTO																		// nFlags;
	},
// ------------------------- 
// 12: MENU_HELP
// ------------------------- 
	{ 
		UI_HELP_XPOS, UI_HELP_YPOS,                             // nXpos, nYpos;
		UI_HELP_WIDTH, UI_HELP_HEIGHT,                          // nWidth, nHeight;
		MENU_ROOT,                                              // nPrevPage;
		HelpMenuItems,                                          // nMenuItems;
		sizeof(HelpMenuItems)/sizeof(OSD_MENU_ITEM_ST),         // nMenuItemCount;
		_X_(0), _Y_(0),                                         // nPromptSelSize, nContentSelSize;
		NULL,                                                   // nExecFunction;
		MP_STAY                                                 // nFlags;
	},
};

/*========================================================================================================
										F U N C T I O N
========================================================================================================*/







/*========================================================================================================
										End
========================================================================================================*/




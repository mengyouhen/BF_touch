/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_app_osd_string.c
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
#include "flex_drv_panel.h"
#include "flex_app_osd_def.h"

// internal
#include "flex_app_osd_string.h"







/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME								"[OSD STR]--"
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
typedef struct
{
	OSD_DISP_ENUM nDispId;
	const uint8_t* nData;
}OSD_DISPID2DATA_ST;

typedef struct
{
	OSD_DISP_ENUM nDispId;
	const OSD_DISP_ENUM* nListBox;
	uint8_t nCount;
}OSD_DISPID2LISTBOX_ST;

/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
//--------------------------------------
// Icon Data
//--------------------------------------
static uint8_t const Icon_Data_dummy[]=	  // 6x8
{
	0x00,0x00,0x00,0x00,0x00,0x00
};

static uint8_t const Icon_Data_wifi_off[]=	  // 6x8
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  // 2 fonts
};

static uint8_t const Icon_Data_wifi_discconnect[]=	  // 6x8
{
	0x00,0x04,0x12,0x09,0x25,0x95,0x25,0x09,0x12,0x04,0x00,0xB8  // 2 fonts
};

static uint8_t const Icon_Data_wifi_low[]=	   // 6x8
{
  0x00,0x00,0x00,0x00,0x20,0x90,0x20,0x00,0x00,0x00,0x00,0x00  // 2 fonts
};

static uint8_t const Icon_Data_wifi_middle[]=	 // 6x8 
{
	0x00,0x00,0x10,0x08,0x24,0x94,0x24,0x08,0x10,0x00,0x00,0x00  // 2 fonts
};

static uint8_t const Icon_Data_wifi_high[]=	  // 6x8
{
  0x00,0x04,0x12,0x09,0x25,0x95,0x25,0x09,0x12,0x04,0x00,0x00  // 2 fonts
};

static uint8_t const Icon_Data_bt_off[]=	  
{
  0x00,0x00,0x00,0x00,0x00,0x00  // 1 fonts
};

static uint8_t const Icon_Data_bt_discconnect[]=	  
{
	0x00,0x42,0x24,0xFF,0x5A,0x24,
};

static uint8_t const Icon_Data_bt_connect[]=	  
{
	0x00,0x42,0x24,0xFF,0x5A,0x24,
};

static uint8_t const Icon_Data_Arrow_Up[]=
{
	0x00,0x60,0x78,0x7E,0x78,0x60,  
};

static uint8_t const Icon_Data_Arrow_Down[]=
{
	0x00,0x06,0x1E,0x7E,0x1E,0x06,
};

static uint8_t const Icon_Data_Arrow_Left[]=
{
	0x00,0x10,0x38,0x7C,0xFE,0x00,
};

static uint8_t const Icon_Data_Arrow_Right[]=
{
	0x00,0xFE,0x7C,0x38,0x10,0x00,
};

static uint8_t const Icon_Data_Slide[]=
{
	0x7E,0x81,0x81,0xC5,0xA9,0x91,
	0x81,0x81,0xC5,0xA9,0x91,0x81,
	0x81,0xC5,0xA9,0x91,0x81,0x81,
	0xC5,0xA9,0x91,0x81,0x81,0x7E,
};

static uint8_t const Icon_Data_Return[]=
{
	0x18,0x24,0x42,0x81,0xC3,0xA5,0x99,0x99,0xA5,0xC3,0x81,0xFF, // symbol: backspace icon
	//0x42,0x24,0x18,0x18,0x24,0x42,// symbol: x
	//0x42,0x47,0x42,0x42,0x42,0x3C, // symbol: return
};


//--------------------------------------
// index--->data
//--------------------------------------
static OSD_DISPID2DATA_ST const Data_Map[]=
{
//--------------------------------------
// Fixed Text Index
//--------------------------------------
	{str_null,                STRING_DUMMY},
	{str_bracket_L,           "<"},
	{str_bracket_R,           ">"},
	{str_bracket_LR7,         "<       >"},
	{str_bracket_LR8,         "<        >"},
	{str_Title,               "MAIN MENU"},
	{str_sys_cfg,             "SYS CONFIG"},
	{str_wifi_cfg,            "WIFI CONFIG"},
	{str_bt_cfg,              "BT CONFIG"},
	{str_sys_info,            "SYS INFO"},
	{str_sys_status,          "SYS STATUS"},
	{str_credits,             "CREDITS"},
	{str_timezone,            "TIME ZONE"},
	{str_dateformat,          "DATE FORMAT"},
	{str_wifi_status,         "STATUS:"},
	{str_wifi_ssid,           "SSID:"},
	{str_wifi_rssi,           "RSSI:"},
	{str_wifi_mac,            "MAC:"},
	{str_wifi_ip,             "IP:"},
	{str_bt_status,           "STATUS:"},
	{str_bt_mac,              "MAC:"},
	{str_sw_version,          "SW VERSION:"},
	{str_sw_version_value, 	  SW_VERSION},	
	{str_sdk_version,         "SDK VERSION:"},
	{str_at_version,          "AT VERSION:"},
	{str_serial_number,       "SN:"},
	{str_serial_number_value, "HBH20S180704QC"},
	{str_build_info,          "BUILD INFO:"},
#if defined(__DATE__)
	{str_build_info_date,     __DATE__},
#else
	{str_build_info_date,     STRING_DUMMY},
#endif
#if defined(__TIME__)
	{str_build_info_time,     __TIME__},
#else
	{str_build_info_time, 	  STRING_DUMMY},
#endif
	{str_brightness,          "BRIGHTNESS"},
	{str_fanspeed,            "FAN SPEED"},
	{str_help_colon,          ":"},
	{str_help_select,         "SEL"},
	{str_help_enter,          "ENTER"},
	{str_help_adjust,         "ADJ  "},
	{str_help_return,         "RET"},
	{str_off,                 "OFF"},
	{str_on,                  "ON"},
	{str_low,                 "LOW"},
	{str_medium,              "MEDIUM"},
	{str_high,                "HIGH"},
	{str_weak,                "WEAK"},
	{str_normal,              "NORMAL"},
	{str_strong,              "STRONG"},
	{str_disconnect,          "DISCONNECT"},
	{str_connect,             "CONNECT"},
	{str_yymmdd,              "YY/MM/DD"},
	{str_mmddyy,              "MM/DD/YY"},
	{str_ddmmyy,              "DD/MM/YY"},
	{str_max,                 STRING_DUMMY},
//--------------------------------------
// ListBox Text index
//--------------------------------------
	{fstr_null,               ""},
	{fstr_onoff_select,       ""},
	{fstr_fan_speed_select,   ""},
	{fstr_date_format_select, ""},
	{fstr_wifi_status_select, ""},
	{fstr_wifi_mac_content,   ""},
	{fstr_wifi_ssid_content,  ""},
	{fstr_wifi_rssi_select,   ""},
	{fstr_wifi_ip_content,    ""},
	{fstr_bt_status_select,   ""},
	{fstr_bt_mac_content,     ""},
	{fstr_sdk_ver_content,    ""},
	{fstr_at_ver_content,     ""},
	{fstr_help_lr_select,     ""},
	{fstr_max,                ""},
//--------------------------------------
// Fixed Icon Index
//--------------------------------------
	{icon_null,               Icon_Data_dummy},
	{icon_wifi_off,           Icon_Data_wifi_off},
	{icon_wifi_disconnect,    Icon_Data_wifi_discconnect},
	{icon_wifi_low,           Icon_Data_wifi_low},
	{icon_wifi_middle,        Icon_Data_wifi_middle},
	{icon_wifi_high,          Icon_Data_wifi_high},
	{icon_bt_off,             Icon_Data_bt_off},
	{icon_bt_disconnect,      Icon_Data_bt_discconnect},
	{icon_bt_connect,         Icon_Data_bt_connect},
	{icon_arrow_up,           Icon_Data_Arrow_Up},
	{icon_arrow_down,         Icon_Data_Arrow_Down},
	{icon_arrow_left,         Icon_Data_Arrow_Left},
	{icon_arrow_right,        Icon_Data_Arrow_Right},
    {icon_slide,              Icon_Data_Slide}, 
	{icon_return,             Icon_Data_Return},
	{icon_max,                Icon_Data_dummy},
//--------------------------------------
// ListBox Icon index
//--------------------------------------
	{ficon_null,              ""},
	{ficon_wifi_select,       ""},
	{ficon_bt_select,         ""},
	{ficon_max,               ""},
};



//--------------------------------------
// Display ListBox
//--------------------------------------
static OSD_DISP_ENUM  const Str_ListboxId_Dummy[] =
{
	str_null,
};

static OSD_DISP_ENUM  const Str_ListboxId_OffOn[] =
{
	str_off, str_on,
};

static OSD_DISP_ENUM  const Str_ListboxId_DateFormat[] =
{
	str_mmddyy, str_ddmmyy, str_yymmdd,
};

static OSD_DISP_ENUM  const Str_ListboxId_LowMidHigh[] =
{
	str_low, str_medium, str_high,
};

static OSD_DISP_ENUM  const Str_ListboxId_WifiStatus[] =
{
	str_off, str_disconnect, str_connect,
};

static OSD_DISP_ENUM  const Str_ListboxId_BtStatus[] =
{
	str_off, str_disconnect, str_connect,
};

static OSD_DISP_ENUM  const Str_ListboxId_HelpLR[] =
{
	str_help_enter, str_help_adjust,
};

//---------------------------------------
static OSD_DISP_ENUM  const Icon_ListboxId_Dummy[] =
{
	icon_null,
};

static OSD_DISP_ENUM  const Icon_ListboxId_WiFiStatus[] =
{
	icon_wifi_off, icon_wifi_disconnect, icon_wifi_low, icon_wifi_middle, icon_wifi_high
};

static OSD_DISP_ENUM  const Icon_ListboxId_BtStatus[] =
{
	icon_bt_off, icon_bt_disconnect, icon_bt_connect,
};

static OSD_DISPID2LISTBOX_ST  const ListBox_Map[]=
{
	{fstr_null,                  Str_ListboxId_Dummy,          1 }, 
	{fstr_onoff_select,          Str_ListboxId_OffOn,          2 }, 
	{fstr_fan_speed_select,      Str_ListboxId_LowMidHigh,     3 }, 
	{fstr_date_format_select,    Str_ListboxId_DateFormat,     3 },
	{fstr_wifi_status_select,    Str_ListboxId_WifiStatus,     3 }, 
	{fstr_wifi_mac_content,      Str_ListboxId_Dummy,          1 }, 
	{fstr_wifi_ssid_content,     Str_ListboxId_Dummy,          1 },
	{fstr_wifi_rssi_select,      Str_ListboxId_LowMidHigh,     3 },
	{fstr_wifi_ip_content,       Str_ListboxId_Dummy,          1 }, 
	{fstr_bt_status_select,      Str_ListboxId_BtStatus,       3 }, 
	{fstr_bt_mac_content,        Str_ListboxId_Dummy,          1 }, 
	{fstr_sdk_ver_content,       Str_ListboxId_Dummy,          1 }, 
	{fstr_at_ver_content,        Str_ListboxId_Dummy,          1 }, 
	{fstr_help_lr_select,        Str_ListboxId_HelpLR,         2 },
	{fstr_max,                   Str_ListboxId_Dummy,          1 }, 
//---------------------------------
	{ficon_null,                 Icon_ListboxId_Dummy,         1 }, 
	{ficon_wifi_select,          Icon_ListboxId_WiFiStatus,    5 }, 
	{ficon_bt_select,            Icon_ListboxId_BtStatus,      3 }, 
	{ficon_max,                  Icon_ListboxId_Dummy,         1 }, 
};

/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
//--------------------------------------
// Fixed Text Index
//--------------------------------------
const uint8_t* Osd_Resource_Get_Disp_Data(OSD_DISP_ENUM disp_id)
{	
	const uint8_t* p;

	if(disp_id<str_max)
	{
		//TRACE_I("[STRING]: to find disp_id=%d.\r\n", disp_id);
		p = Data_Map[disp_id].nData;
	}
	else if((disp_id>=icon_null) && (disp_id<icon_max))
	{
		//TRACE_I("[ICON]: to find disp_id=%d.\r\n", disp_id);
		/*
		TRACE_I("[ICON]: ref icon_wifi_off        =%d.\r\n", icon_wifi_off);
		TRACE_I("[ICON]: ref icon_wifi_disconnect =%d.\r\n", icon_wifi_disconnect);
		TRACE_I("[ICON]: ref icon_wifi_low        =%d.\r\n", icon_wifi_low);
		TRACE_I("[ICON]: ref icon_wifi_middle     =%d.\r\n", icon_wifi_middle);
		TRACE_I("[ICON]: ref icon_wifi_high       =%d.\r\n", icon_wifi_high);
		TRACE_I("[ICON]: ref icon_bt_off          =%d.\r\n", icon_bt_off);
		TRACE_I("[ICON]: ref icon_bt_disconnect   =%d.\r\n", icon_bt_disconnect);
		TRACE_I("[ICON]: ref icon_bt_connect      =%d.\r\n", icon_bt_connect);
		*/
		p = Data_Map[disp_id].nData;
	}
	else
	{
		p=STRING_DUMMY;
	}

	return p;
}

//--------------------------------------
// ListBox Text index
//--------------------------------------
OSD_DISP_ENUM Osd_Resource_Get_ListBox_Index(OSD_DISP_ENUM disp_id, uint8_t value)
{
	OSD_DISP_ENUM ret;
	
	const OSD_DISPID2LISTBOX_ST* p=ListBox_Map;

// init, for search nothing
	if((disp_id>=fstr_null) && (disp_id<fstr_max))
	{
		ret = fstr_null;

		do
		{
			if(disp_id==p->nDispId)
			{
				if(value>p->nCount)
				{
					break;
				}
				else
				{
					ret = p->nListBox[value];
					break;
				}
			}
			p++;
		}while(p->nDispId<fstr_max);
	}
	else if((disp_id>=ficon_null) && (disp_id<ficon_max))
	{
		ret = ficon_null;
		do
		{
			if(disp_id==p->nDispId)
			{
				if(value>p->nCount)
				{
					break;
				}
				else
				{
					ret = p->nListBox[value];
					break;
				}
			}
			p++;
		}while(p->nDispId<ficon_max);
	}

	return ret;
}



/*========================================================================================================
										End
========================================================================================================*/




/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_app_osd_menu.c
 [Date]:       
 		2018-06-17
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

#include "flex_framework_input.h"
#include "flex_framework_esp32.h"
#include "flex_app_osd_menutbl.h"
#include "flex_app_osd_user.h"
#include "flex_app_osd_func.h"
#include "flex_app_osd_draw.h"
#include "flex_app_osd_application.h"
// internal
#include "flex_app_osd_menu.h"

/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME								"[OSD MENU]--"
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
#define TEST_DRAW_TICKS		false




/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
typedef struct
{
	uint8_t nYoffset;
	struct 
	{
		uint8_t nXoffset;
		uint8_t* nText;
	}nSort[3];
}OSD_CREDITS_CNTX_ST;

/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
static const OSD_CREDITS_CNTX_ST CreditsArray[]=
{
		{_Y_(0), {{_X_(0), "NAME"        },  {_X_(12), "SEX"},  {_X_(16), "TITLE"}} },
		{_Y_(0), {{_X_(0), "YingY.Feng"  },  {_X_(13), "F"  },  {_X_(18), "PM"   }} },
		{_Y_(1), {{_X_(0), "Tim.Lin"     },  {_X_(13), "M"  },  {_X_(18), "SW"   }} },
		{_Y_(2), {{_X_(0), "Carson.Qian" },  {_X_(13), "M"  },  {_X_(18), "SW"   }} },
		{_Y_(3), {{_X_(0), "Joanna.Wang" },  {_X_(13), "F"  },  {_X_(18), "SW"   }} },
		{_Y_(4), {{_X_(0), "Roger.Ji"    },  {_X_(13), "M"  },  {_X_(18), "HW"   }} },
		{_Y_(5), {{_X_(0), "Peter.Wang"  },  {_X_(13), "M"  },  {_X_(18), "HW"   }} },
		{_Y_(6), {{_X_(0), "Sky.Chen"    },  {_X_(13), "M"	},	{_X_(18), "MD"	 }} },
};

/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
uint8_t Osd_Get_Menu_Deep(OSD_PAGE_ENUM MenuIdx)
{
	uint8_t deep=0;
	
	switch(MenuIdx)
	{
		case MENU_ROOT:
			deep = 0;
			break;
	
		case MENU_LOGO:
		case MENU_MAIN:
			deep = 1;
			break;
			
		case MENU_WIFI_CFG:				
		case MENU_BT_CFG:						
		case MENU_SYS_INFO:				
		case MENU_SYS_STATUS:			
		case MENU_CREDITS:
			deep = 2;
			break;

		default:
			deep = 3;
			break;
	}
	return deep;
}

//=======================================================================
uint8_t Osd_Status_Director(OSD_ITEM_SELECL_FLAG_ENUM flag)
{
	uint8_t ret = ITEM_STATUS_DISABLE;

	switch(flag)
	{
		case MI_DISABLE:		
			ret = ITEM_STATUS_DISABLE;			
			break;

		case MI_ENABLE:		
			ret = ITEM_STATUS_ENABLE;			
			break;

		case MI_SEL_DISABLE:		
			ret = ITEM_STATUS_SEL_DISABLE;			
			break;
			
	//---------------------------------
		case MI_WIFI_CONN_ONLY:		
			if(ESP32_WIFI_CONN==Osd_Value_Director(true, Func_Get_Wifi_Status, LIST_MIA_NOTHING)\
				||(ESP32_WIFI_GOTIP==Osd_Value_Director(true, Func_Get_Wifi_Status, LIST_MIA_NOTHING)))
			{
				ret = ITEM_STATUS_ENABLE; 
			}
			break;	

		//---------------------------------
		case MI_HELP_NAVI:
			switch(g_osd_menu_data.nPage)
			{
				case MENU_MAIN:
				case MENU_SYS_CFG:
					ret = ITEM_STATUS_SEL_DISABLE;		
					break;
				
				default:
					ret = ITEM_STATUS_DISABLE;		
					break;
			}
			break;
			
		case MI_HELP_SLIDE:
			if(MENU_POPUP_BRIGHTNESS==g_osd_menu_data.nPage)
			{
				ret = ITEM_STATUS_SEL_DISABLE;	
			}
			else
			{
				ret = ITEM_STATUS_DISABLE;		
			}
			break;
			
		case MI_HELP_ARROW_L:
			switch(g_osd_menu_data.nPage)
			{
				case MENU_SYS_CFG:
					ret = ITEM_STATUS_SEL_DISABLE;		
					break;
					
				default:
					ret = ITEM_STATUS_DISABLE;		
					break;
			}
			break;

		case MI_HELP_ARROW_R:
			switch(g_osd_menu_data.nPage)
			{
				case MENU_MAIN:
				case MENU_SYS_CFG:
					ret = ITEM_STATUS_SEL_DISABLE;		
					break;
					
				default:
					ret = ITEM_STATUS_DISABLE;		
					break;
			}
			break;

		case MI_HELP_ARROW_COLON:
			switch(g_osd_menu_data.nPage)
			{
				case MENU_MAIN:
				case MENU_SYS_CFG:
				case MENU_POPUP_BRIGHTNESS:
					ret = ITEM_STATUS_SEL_DISABLE;		
					break;
					
				default:
					ret = ITEM_STATUS_DISABLE;		
					break;
			}
			break;

		case MI_HELP_RET:
			switch(g_osd_menu_data.nPage)
			{
				case MENU_MAIN:
					ret = ITEM_STATUS_DISABLE;		
					break;
					
				default:
					ret = ITEM_STATUS_SEL_DISABLE;		
					break;
			}
			break;
	//---------------------------------
		default:
			break;
	}
	return ret;
}

bool Osd_RunTime_Director(uint8_t RunTime,uint8_t action_RunTime)
{
	bool ret = (bool)(RunTime&action_RunTime);

	return ret;
}


uint8_t Osd_Color_Director(bool dynamic, uint8_t Color, uint8_t SelectFlag, uint8_t MenuItemIdx)
{
	uint8_t ret;
	
	if (!dynamic)
	{
		ret = Color;
	}
	else
	{
		switch (Color)
		{
			case LIST_COLOR_MENU_PROMPT:
			case LIST_COLOR_MENU_CONTENT:
			case LIST_COLOR_MENU_BAR:
				if (COLOR_SEL_DISABLE == SelectFlag)
				{
					ret = CT_MENU_ITEM_SEL_DISABLE;
				}
				else if (COLOR_SELECT == SelectFlag)
				{
					ret = CT_MENU_ITEM_SEL;
				}
				else
				{
					ret = CT_MENU_ITEM_UNSEL;
				}
				break;
				
 			default:
				break;
		}
	}
	return ret;
}

uint8_t Osd_Pos_Director(bool dynamic, uint8_t position)
{
	uint8_t ret = 0;
	
	if (!dynamic)
	{
		ret = position;
	}
	else
	{
		switch(g_osd_menu_data.nPage)
		{
			case MENU_MAIN:
				ret=position-_X_(1);
				break;

			default:
				ret = position;
				break;
		}
	}
	
	return ret;
}

uint8_t Osd_Width_Director(bool dynamic, uint8_t width)
{
	uint8_t ret = 0;
	
	if (!dynamic)
	{
		ret = width;
	}
	else
	{
		switch (width)
		{
			default:
				break;
		}
	}
	return ret;
}

OSD_DISP_ENUM Osd_Display_Director(bool dynamic, OSD_DISP_ENUM DisplayId, uint32_t value)
{
	OSD_DISP_ENUM ret;
	
	if (!dynamic)
	{
		ret = DisplayId;
	}
	else
	{
		ret = Osd_Resource_Get_ListBox_Index(DisplayId, value);					
	}
	return ret;
}

//=======================================================================
void Osd_Menu_Draw_Prompt(OSD_PAGE_ENUM PageIdx, uint8_t ItemIdx, bool bSelect)
{
	uint8_t x;
	uint8_t y;
	uint8_t width;
	int16_t value;
	uint8_t color;
	OSD_DISP_ENUM DisplayId;
	const OSD_MENU_ITEM_ST* pItem = &tblMenus[PageIdx].nMenuItems[ItemIdx];

	if (ITEM_STATUS_ENABLE==Osd_Status_Director(pItem->nSelectFlags)	|| ITEM_STATUS_SEL_DISABLE==Osd_Status_Director(pItem->nSelectFlags))
	{
		//TRACE_I("[Draw_Prompt]:PageIdx=%d, ItemIdx=%d, %s.\r\n", PageIdx, ItemIdx, bSelect?"SEL":"UNSEL");
	// get prompt info		
		x = Osd_Pos_Director(Osd_RunTime_Director(pItem->nRunTime, RT_XPOS), pItem->nXPos);
		y = Osd_Pos_Director(Osd_RunTime_Director(pItem->nRunTime, RT_YPOS), pItem->nYPos);
		width = Osd_Width_Director(Osd_RunTime_Director(pItem->nRunTime, RT_WIDTH), pItem->nWidth);  
		value = Osd_Value_Director(Osd_RunTime_Director(pItem->nRunTime, RT_VALUE), pItem->nKeyFuncIdx, LIST_MIA_NOTHING);
		if (ITEM_STATUS_SEL_DISABLE==Osd_Status_Director(pItem->nSelectFlags))
		{
			color = Osd_Color_Director(Osd_RunTime_Director(pItem->nRunTime, RT_COLOR), pItem->nColor, COLOR_SEL_DISABLE, ItemIdx);
		}
		else
		{
			if(bSelect)
			{
				color = Osd_Color_Director(Osd_RunTime_Director(pItem->nRunTime, RT_COLOR), pItem->nColor, COLOR_SELECT, ItemIdx);
			}
			else
			{
				color = Osd_Color_Director(Osd_RunTime_Director(pItem->nRunTime, RT_COLOR), pItem->nColor, COLOR_UNSELECT, ItemIdx);
			}
		}
	// clear
		Osd_Set_Color(color);
		Osd_Clear_Block(x, y, UI_ITEM_PROMPT_HSIZE, UI_ITEM_PROMPT_VSIZE);
		//TRACE_I("PROMPT ----page=%d, item=%d, x=%03d, y=%02d, width=%02d.\r\n", PageIdx, ItemIdx, x, y, width);
	// draw
		switch (pItem->nDwpType)
		{
			case LIST_DWP_TEXT_L:
				DisplayId = Osd_Display_Director(Osd_RunTime_Director(pItem->nRunTime, RT_DISPLAY), pItem->nDispIdx, value);
				Osd_Draw_String(ALIGN_LEFT, x, y, Osd_Resource_Get_Disp_Data(DisplayId), width);
				break;

			case LIST_DWP_TEXT_R:
				DisplayId = Osd_Display_Director(Osd_RunTime_Director(pItem->nRunTime, RT_DISPLAY), pItem->nDispIdx, value);
				Osd_Draw_String(ALIGN_RIGHT, x, y, Osd_Resource_Get_Disp_Data(DisplayId), width);
				break;
				
			case LIST_DWP_TEXT_C:
				DisplayId = Osd_Display_Director(Osd_RunTime_Director(pItem->nRunTime, RT_DISPLAY), pItem->nDispIdx, value);
				Osd_Draw_String(ALIGN_CENTER, x, y, Osd_Resource_Get_Disp_Data(DisplayId), width);
				break;

			case LIST_DWP_LOGO:
				break;

			case LIST_DWP_ICON:
				DisplayId = Osd_Display_Director(Osd_RunTime_Director(pItem->nRunTime, RT_DISPLAY), pItem->nDispIdx, value);
				Osd_Draw_Icon(x, y, width, 1, Osd_Resource_Get_Disp_Data(DisplayId));
				break;

			default:
				break;
		}
	}
}

void Osd_Menu_Draw_Content(OSD_PAGE_ENUM PageIdx, uint8_t ItemIdx, bool bSelect)
{
	uint8_t x;
	uint8_t y;
	uint8_t width;
	int16_t value;
	uint8_t color;
	uint8_t* pString;
	uint8_t ucDrawType;
	bool bClead=false;
	OSD_DISP_ENUM DisplayId;
	const OSD_MENU_ITEM_ST* pItem = &tblMenus[PageIdx].nMenuItems[ItemIdx];
	const OSD_MENU_CONTENT_ST* pContent = pItem->nDrawContent;

	if (ITEM_STATUS_ENABLE==Osd_Status_Director(pItem->nSelectFlags)|| ITEM_STATUS_SEL_DISABLE==Osd_Status_Director(pItem->nSelectFlags))
	{
		if( NULL == pContent )
			return;

		//TRACE_I("[Draw_Content]:PageIdx=%d, ItemIdx=%d, %s.\r\n", PageIdx, ItemIdx, bSelect?"SEL":"UNSEL");

		while(true)
		{
		// get prompt info		
			x = Osd_Pos_Director(Osd_RunTime_Director(pContent->nRunTime, RT_XPOS), pContent->nXPos);
			y = Osd_Pos_Director(Osd_RunTime_Director(pContent->nRunTime, RT_YPOS), pContent->nYPos);
			width = Osd_Width_Director(Osd_RunTime_Director(pContent->nRunTime, RT_WIDTH), pContent->nWidth);  
			value = Osd_Value_Director(Osd_RunTime_Director(pContent->nRunTime, RT_VALUE), pContent->nKeyFuncIdx, LIST_MIA_NOTHING);
			if (ITEM_STATUS_SEL_DISABLE==Osd_Status_Director(pItem->nSelectFlags))
			{
				color = Osd_Color_Director(Osd_RunTime_Director(pContent->nRunTime, RT_COLOR), pContent->nColor, COLOR_SEL_DISABLE, ItemIdx);
			}
			else
			{
				if(bSelect)
				{
					color = Osd_Color_Director(Osd_RunTime_Director(pContent->nRunTime, RT_COLOR), pContent->nColor, COLOR_SELECT, ItemIdx);
				}
				else
				{
					color = Osd_Color_Director(Osd_RunTime_Director(pContent->nRunTime, RT_COLOR), pContent->nColor, COLOR_UNSELECT, ItemIdx);
				}
			}
		// clear
			Osd_Set_Color(color);
			if(!bClead)
			{
			#if 1
				Osd_Clear_Block(x, y, _X_(pContent->nWidth), UI_ITEM_CONTENT_VSIZE);
			#else
				Osd_Clear_Block(x, y, UI_ITEM_CONTENT_HSIZE, UI_ITEM_CONTENT_VSIZE);
			#endif
				bClead=true;
			}
		// draw
			ucDrawType=pContent->nDwcType&(~DWC_END);
			switch (ucDrawType)
			{
				case LIST_DWC_TEXT_L:
				case LIST_DWC_TEXT_R:
				case LIST_DWC_TEXT_C:
					DisplayId = Osd_Display_Director(Osd_RunTime_Director(pContent->nRunTime, RT_DISPLAY), pContent->nDispIdx, (uint32_t)value);
					switch(DisplayId)
					{
						case fstr_wifi_mac_content:
 							pString=(uint8_t*)ESP32_GetWifiMac();
							//TRACE_I("wifi mac=%s.\r\n", pString);
							break;

						case fstr_wifi_ssid_content:
 							pString=(uint8_t*)(ESP32_GetAPInfo()->ssid);
							//TRACE_I("wifi ssid=%s.\r\n", pString);
							break;

						case fstr_wifi_ip_content:
 							pString=(uint8_t*)ESP32_GetWifiIP();
							//TRACE_I("wifi ip=%s.\r\n", pString);
							break;
					
						case fstr_bt_mac_content:
 							pString=(uint8_t*)ESP32_GetBleMac();
							//TRACE_I("wifi mac=%s.\r\n", pString);
							break;
  							
						case fstr_sdk_ver_content:
							pString=(uint8_t*)ESP32_GetSDKVersion();
							//TRACE_I("sdk version=%s.\r\n", pString);
							break;
							
						case fstr_at_ver_content:
							pString=(uint8_t*)ESP32_GetATVersion();
							//TRACE_I("at version=%s.\r\n", pString);
							break;
							
						default:
							pString=(uint8_t*)Osd_Resource_Get_Disp_Data(DisplayId);
							break;
					}
					
					if(pString!=NULL)
					{
						if(LIST_DWC_TEXT_R==ucDrawType)
						{
							Osd_Draw_String(ALIGN_RIGHT, x, y, pString, width);
						}
						else if(LIST_DWC_TEXT_C==ucDrawType)
						{
							Osd_Draw_String(ALIGN_CENTER, x, y, pString, width);
						}
						else
						{
							Osd_Draw_String(ALIGN_LEFT, x, y, pString, width);
						}
					}
					break;

				case LIST_DWC_ICON:
					DisplayId = Osd_Display_Director(Osd_RunTime_Director(pContent->nRunTime, RT_DISPLAY), pContent->nDispIdx, value);
					//TRACE_I("LIST_DWC_ICON----value=%d, DisplayId=%d.\r\n", value, DisplayId);
					/*
					TRACE_I("ref  icon_wifi_off       =%d.\r\n", icon_wifi_off);
					TRACE_I("ref  icon_wifi_disconnect=%d.\r\n", icon_wifi_disconnect);
					TRACE_I("ref  icon_wifi_low       =%d.\r\n", icon_wifi_low);
					TRACE_I("ref  icon_wifi_middle    =%d.\r\n", icon_wifi_middle);
					TRACE_I("ref  icon_wifi_high      =%d.\r\n", icon_wifi_high);
					TRACE_I("ref  icon_bt_off         =%d.\r\n", icon_bt_off);
					TRACE_I("ref  icon_bt_disconnect  =%d.\r\n", icon_bt_disconnect);
					TRACE_I("ref  icon_bt_connect     =%d.\r\n", icon_bt_connect);
					*/
					Osd_Draw_Icon(x, y, width, 1, Osd_Resource_Get_Disp_Data(DisplayId));
					break;

				case LIST_DWC_DATE:
					{
						char buff[9]={0};
						RTC_DateTypeDef data=Osd_Func_Get_Date();
						switch(g_osd_user_data.nDateformat)
						{
							case DATE_FORMAT_MMDDYY:
							default:
								sprintf(buff, "%02d/%02d/%02d", data.Month,data.Date,data.Year);
								break;
								
							case DATE_FORMAT_DDMMYY:
								sprintf(buff, "%02d/%02d/%02d", data.Date,data.Month,data.Year);
								break;

							case DATE_FORMAT_YYMMDD:
								sprintf(buff, "%02d/%02d/%02d", data.Year,data.Month,data.Date);
								break;
						}
						Osd_Draw_String(ALIGN_LEFT,x, y, (uint8_t *)buff, width);
					}
					break;

				case LIST_DWC_TIME:
					{
						char buff[6]={0};
						RTC_TimeTypeDef time=Osd_Func_Get_Time();
						sprintf(buff, "%02d:%02d", time.Hours, time.Minutes);
						Osd_Draw_String(ALIGN_LEFT,x, y, (uint8_t *)buff, width);
					}
					break;
				
				case LIST_DWC_INFO:
					break;
					
				case LIST_DWC_NUMBER_L:
				case LIST_DWC_NUMBER_R:
				case LIST_DWC_NUMBER_C:
				case LIST_DWC_SIGNED_NUM_L:	
				case LIST_DWC_SIGNED_NUM_R:	
				case LIST_DWC_SIGNED_NUM_C:				
					{
						uint8_t bits=Common_Calc_Bits(value);
						//TRACE_I("value=%d, bits=%d.\r\n", value, bits);
						
						if(bits<=width)
						{
							char* pStr=(char*)pf_malloc(bits+1);
							if(pStr)
							{
								if((LIST_DWC_SIGNED_NUM_L==ucDrawType)\
									||(LIST_DWC_SIGNED_NUM_R==ucDrawType)\
									||(LIST_DWC_SIGNED_NUM_C==ucDrawType))
								{
									if(value>0)
									{
										sprintf(pStr, "+%d", value);
									}
									else if (0==value)
									{
										sprintf(pStr, " %d", value);
									}
									else
									{
										sprintf(pStr, "%d", value);
									}
								}
								else
								{
									sprintf(pStr, "%d", value);
								}
								
								if((LIST_DWC_NUMBER_R==ucDrawType)||(LIST_DWC_SIGNED_NUM_R==ucDrawType))
								{
									Osd_Draw_String(ALIGN_RIGHT, x, y, (uint8_t *)pStr, width);
								}
								else if((LIST_DWC_NUMBER_C==ucDrawType)||(LIST_DWC_SIGNED_NUM_C==ucDrawType))
								{
									Osd_Draw_String(ALIGN_CENTER, x, y, (uint8_t *)pStr, width);
								}
								else
								{
									Osd_Draw_String(ALIGN_LEFT, x, y, (uint8_t *)pStr, width);
								}
								pf_free(pStr);
							}
						}
					}
					break;

				case LIST_DWC_CREDIT_TITLE:
					{
						Osd_Draw_String(ALIGN_LEFT,x+CreditsArray[0].nSort[0].nXoffset, y+CreditsArray[0].nYoffset, \
							             CreditsArray[0].nSort[0].nText, strlen((char*)(CreditsArray[0].nSort[0].nText)) );
						
						Osd_Draw_String(ALIGN_LEFT,x+CreditsArray[0].nSort[1].nXoffset, y+CreditsArray[0].nYoffset, \
							             CreditsArray[0].nSort[1].nText, strlen((char*)(CreditsArray[0].nSort[1].nText)) );
						
						Osd_Draw_String(ALIGN_LEFT,x+CreditsArray[0].nSort[2].nXoffset, y+CreditsArray[0].nYoffset, \
							             CreditsArray[0].nSort[2].nText, strlen((char*)(CreditsArray[0].nSort[2].nText)) );
					}
					break;
					
				case LIST_DWC_CREDIT_LIST:
					{
						uint8_t idx;
						for(idx=1; idx<=7; idx++)
						{
							Osd_Draw_String(ALIGN_LEFT, x+CreditsArray[idx].nSort[0].nXoffset, y+CreditsArray[idx].nYoffset, \
								              CreditsArray[idx].nSort[0].nText, strlen((char*)(CreditsArray[idx].nSort[0].nText)) );
							
							Osd_Draw_String(ALIGN_LEFT, x+CreditsArray[idx].nSort[1].nXoffset, y+CreditsArray[idx].nYoffset, \
								              CreditsArray[idx].nSort[1].nText, strlen((char*)(CreditsArray[idx].nSort[1].nText)) );
							
							Osd_Draw_String(ALIGN_LEFT, x+CreditsArray[idx].nSort[2].nXoffset, y+CreditsArray[idx].nYoffset, \
								              CreditsArray[idx].nSort[2].nText, strlen((char*)(CreditsArray[idx].nSort[2].nText)) );
						}
					}
					break;
					
				default:
					break;
			}

			if(pContent->nDwcType & DWC_END)
				break;
			else
				pContent++;
		}
	}
}

void Osd_Menu_Draw_Item(OSD_PAGE_ENUM PageIdx, uint8_t ItemIdx, bool bSelect)
{
	//TRACE_I("[Draw_Item]:PageIdx=%d, ItemIdx=%d, %s.\r\n", PageIdx, ItemIdx, bSelect?"SEL":"UNSEL");
	Osd_Menu_Draw_Prompt(PageIdx, ItemIdx, bSelect);
	Osd_Menu_Draw_Content(PageIdx, ItemIdx, bSelect);
}

void Osd_Menu_Draw_Page(OSD_PAGE_ENUM PageIdx, uint8_t SelItemIdx)
{
	uint8_t ItemIdx;
	uint8_t SelItemIdxTemp=SelItemIdx;
#if TEST_DRAW_TICKS
	uint32_t tickstart = HAL_GetTick();
#endif

	//TRACE_I("[Draw_Page]:PageIdx=%d, SelItemIdx=%d.\r\n", PageIdx, SelItemIdx);

	if((PageIdx!=MENU_STATUS_BAR )&&(PageIdx!=MENU_HELP))
	{
		if((CURRENT_MENU.nFlags)&MP_INVISIBLE)
		{
			//TRACE_I("WWW----MP_INVISIBLE.\r\n");
			return;
		}
		if((CURRENT_MENU.nFlags)&MP_DRAWFRAME)
		{
			//TRACE_I("WWW----MP_DRAWFRAME.\r\n");
			Osd_Draw_Frame((CURRENT_MENU.nXpos), (CURRENT_MENU.nYpos), (CURRENT_MENU.nWidth), (CURRENT_MENU.nHeight));
		}
		if((CURRENT_MENU.nFlags)&MP_MOVING)
		{
			//TRACE_I("WWW----MP_MOVING.\r\n");
			// TODO:
		}
		if((CURRENT_MENU.nFlags)&MP_AUTO)
		{
			//TRACE_I("WWW----MP_AUTO.\r\n");
			App_Osd_Set_Duration(OSD_DURATION_TIME);
		}
	}

	if(MENU_HELP==PageIdx)
	{
		//TRACE_I("111111111111111111111111111122222222222222222222222233333333333333333333333\r\n");
		Osd_Set_Color(CT_MENU_DEFALUT);
		Osd_Clear_Block(UI_HELP_XPOS, UI_HELP_YPOS, UI_HELP_HSIZE, UI_HELP_VSIZE);
	}

	switch(PageIdx)
	{
		case MENU_STATUS_BAR:
		case MENU_HELP:
			for(ItemIdx=0; ItemIdx<tblMenus[PageIdx].nMenuItemCount; ItemIdx++)
			{
				Osd_Menu_Draw_Item(PageIdx, ItemIdx, false);
			}
			break;

		default:
			for(ItemIdx=0; ItemIdx<tblMenus[PageIdx].nMenuItemCount; ItemIdx++)
			{
				Osd_Menu_Draw_Item(PageIdx, ItemIdx, (ItemIdx == SelItemIdxTemp)? true : false);
			}
			break;
	}
	
#if TEST_DRAW_TICKS
	DBG_MSG("osd delta=%d.\r\n", HAL_GetTick()-tickstart);   
#endif
}

//=======================================================================
uint8_t Osd_Menu_Get_First_Item_Index(OSD_PAGE_ENUM PageIdx)
{
	uint8_t ItemIndex;
	
	for(ItemIndex=0; ItemIndex<=tblMenus[PageIdx].nMenuItemCount; ItemIndex++)
	{
		if(ITEM_STATUS_ENABLE == Osd_Status_Director(tblMenus[PageIdx].nMenuItems[ItemIndex].nSelectFlags))
		{
			break;
		}
	}

	return ItemIndex;
}

uint8_t Osd_Menu_Get_Next_Item_Index(OSD_MIA_ENUM Action)
{
	uint8_t ItemIndex;

	ItemIndex = g_osd_menu_data.nItem;

	Action = (Action==LIST_MIA_NEXTITEM)? LIST_MIA_INCVALUE : LIST_MIA_DECVALUE;
	
	while (1)
	{
		ItemIndex = DecIncValue_Cycle(Action, ItemIndex, 0, CURRENT_MENU_ITEM_COUNT-1, 1);
		if (ItemIndex==g_osd_menu_data.nItem)
		{
			break;
		}
		else if (ITEM_STATUS_ENABLE == Osd_Status_Director(CURRENT_MENU_ITEMS[ItemIndex].nSelectFlags)) 
		{
			break;
		}
	}
	return ItemIndex;
}

uint8_t Osd_Menu_Get_ParentMenu_Index(uint8_t ParentMenuIdx, uint8_t SonMenuIdx)
{
	uint8_t FatherMenuIdx;

	do
	{
		FatherMenuIdx = tblMenus[SonMenuIdx].nPrevPage;
		
		if(ParentMenuIdx != FatherMenuIdx)
		{
			SonMenuIdx = FatherMenuIdx;
		}
		else
		{
			const OSD_MENU_ITEM_ST* pItem;
			uint8_t MenuItemNum, ItemIdx;

			pItem = tblMenus[ParentMenuIdx].nMenuItems;  
			MenuItemNum = tblMenus[ParentMenuIdx].nMenuItemCount;  
			for(ItemIdx = 0; ItemIdx < MenuItemNum; ++ItemIdx )
			{
				if( SonMenuIdx == pItem->nNextPage)
				{
					return ItemIdx;
				}
				++pItem;
			}
			return 0;
		}

	} while(FatherMenuIdx != MENU_ROOT);

	return 0;
}

/*========================================================================================================
										End
========================================================================================================*/




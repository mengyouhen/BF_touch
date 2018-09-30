/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_app_osd_application.c
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
#include "flex_framework_input.h"
#include "flex_app_logo.h"
#include "flex_app_osd_user.h"
#include "flex_app_osd_func.h"
#include "flex_app_osd_menu.h"
#include "flex_app_osd_menutbl.h"
#include "flex_app_osd_draw.h"

// internal
#include "flex_app_osd_application.h"







/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME								"[OSD APP]--"
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
#if defined(ENABLE_OSD_LOGO)
#define OSD_TASK_LOGO_MASK         _BIT0_
#endif
#define OSD_TASK_KEY_MASK          _BIT1_
#define OSD_TASK_ESP32_MASK        _BIT2_
#define OSD_TASK_RTC_MASK          _BIT3_


#define ESP32_EVENT_DEBUG			false

/*========================================================================================================
										T Y P E D E F
========================================================================================================*/








/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
// Task
osThreadId g_TaskId_Osd = NULL;

// Timer
#if defined(ENABLE_OSD_LOGO)
static osTimerId g_TimerId_Osd_Logo  = NULL;
#endif
static osTimerId g_TimerId_Osd_Duration=NULL;


static ESP32_EVENT g_esp32_event;

/*========================================================================================================
										F U N C T I O N----local
========================================================================================================*/
//=========================================================
// LOGO
//=========================================================
#if defined(ENABLE_OSD_LOGO)
static void App_Osd_Logo_Timer_Expired(void const *argument)
{
	int32_t ret = 0;
	
	//if (g_TaskId_Osd != NULL)
	{
		ret = osSignalSet(g_TaskId_Osd, OSD_TASK_LOGO_MASK);
		
		if (0x80000000 == ret)
		{
			TRACE_E("[TIMER_LOGO]-ret(%d).\r\n", ret);
		}
	}
}
#endif

//=========================================================
// OSD Duration
//=========================================================
static void App_Osd_Duration_Callback(void const *argument)// exit current menu page
{
	osTimerStop(g_TimerId_Osd_Duration);
	/*
	switch(g_osd_menu_data.nPage)
	{
		case MENU_POPUP_BRIGHTNESS:
			Osd_User_Data_Save(LIST_NVRAM_TAG_BRIGHTNESS);
			break;
			
		default:
			break;
	}
	*/
	g_osd_menu_data.nPage = g_osd_menu_data.nPrevPage;
	g_osd_menu_data.nItem = g_osd_menu_data.nPrevItem;
	Osd_Set_Color(CT_MENU_DEFALUT);
	Osd_Clear_Block((CURRENT_MENU.nXpos), (CURRENT_MENU.nYpos), _X_(CURRENT_MENU.nWidth), _Y_(CURRENT_MENU.nHeight));
	Osd_Menu_Draw_Page(g_osd_menu_data.nPage, g_osd_menu_data.nItem);
	Osd_Menu_Draw_Page(MENU_HELP,0);
}

void App_Osd_Set_Duration(uint16_t ms)
{
	osTimerStart(g_TimerId_Osd_Duration, ms);
}

//=========================================================
// KEY
//=========================================================
static FRM_INPUT_KEY_ST App_Osd_key_Get_Info(void)
{
	return g_osd_menu_data.nKeyInfo;
}

static void App_Osd_Key_Set_Info(FRM_INPUT_KEY_ST key_info)
{
	pf_memcpy(&g_osd_menu_data.nKeyInfo, &key_info, sizeof(FRM_INPUT_KEY_ST));

	App_Osd_key_Get_Info();
}

static void App_Osd_Input_Key_Callback(FRM_INPUT_KEY_ST key_info)
{
#if 1
	//TRACE_F();
	//TRACE_I("11111111111111111111111111----OSD.\r\n");
	App_Osd_Key_Set_Info(key_info);
	osSignalSet(g_TaskId_Osd, OSD_TASK_KEY_MASK);
#else
	int32_t ret = 0;

	TRACE_F();

	App_Osd_Key_Set_Info(input_info);
	
	ret = osSignalSet(g_TaskId_Osd, OSD_TASK_KEY_MASK);

	if (0x80000000 == ret)
	{
		TRACE_W("osSignalSet[OSD-KEY]-ret(%d).\r\n", ret);
	}
#endif
}


void App_Osd_Key_Execute(OSD_MIA_ENUM mia)
{
	//TRACE_I("------------------------------------[Key_Execute]: mia=%d.\r\n", mia);
	
	switch(mia)
	{
		case LIST_MIA_NEXTITEM:
		case LIST_MIA_PREVITEM:
			//TRACE_I("navi item.\r\n");
			g_osd_menu_data.nPrevItem = g_osd_menu_data.nItem;
			g_osd_menu_data.nItem = Osd_Menu_Get_Next_Item_Index(mia);
			//TRACE_I("[NAVI_ITEM]:PrevItem=%d, CurrItem=%d.\r\n", g_osd_menu_data.nPrevItem, g_osd_menu_data.nItem);
			if (g_osd_menu_data.nPrevItem != g_osd_menu_data.nItem)
			{
				Osd_Menu_Draw_Item(g_osd_menu_data.nPage, g_osd_menu_data.nPrevItem, false);
				Osd_Menu_Draw_Item(g_osd_menu_data.nPage, g_osd_menu_data.nItem, true);
			}
			break;

		case LIST_MIA_GOTONEXT:
		case LIST_MIA_GOTOPREV:
			//TRACE_I("navi page.\r\n");
			if( LIST_MIA_GOTONEXT == mia )
			{
				//TRACE_I("GOTO NEXT.\r\n");
				if(CURRENT_MENU_ITEM.nNextPage!=MENU_ROOT)
				{
					g_osd_menu_data.nPrevPage = g_osd_menu_data.nPage;
					g_osd_menu_data.nPage = CURRENT_MENU_ITEM.nNextPage;
					g_osd_menu_data.nItem = Osd_Menu_Get_First_Item_Index(g_osd_menu_data.nPage);
					Osd_Set_Color(CT_MENU_DEFALUT);
					Osd_Clear_Block((CURRENT_MENU.nXpos), (CURRENT_MENU.nYpos), _X_(CURRENT_MENU.nWidth), _Y_(CURRENT_MENU.nHeight));
					Osd_Menu_Draw_Page(g_osd_menu_data.nPage, g_osd_menu_data.nItem);
					Osd_Menu_Draw_Page(MENU_HELP,0);
				}
			}
			else
			{
				//TRACE_I("GOTO PREV.\r\n");

				if((CURRENT_MENU.nFlags)&MP_AUTO)
				{
					App_Osd_Duration_Callback(NULL);
				}
				else
				{
					if(CURRENT_MENU.nPrevPage != MENU_ROOT)
					{
					/*
						switch(g_osd_menu_data.nPage)
						{
							case MENU_SYS_CFG:
								Osd_User_Data_Save(LIST_NVRAM_TAG_TIMEZONE);
								Osd_User_Data_Save(LIST_NVRAM_TAG_DATEFORMAT);
								break;
								
							default:
								break;
						}
						*/
						g_osd_menu_data.nPrevPage = g_osd_menu_data.nPage;
						g_osd_menu_data.nPage = CURRENT_MENU.nPrevPage;
						g_osd_menu_data.nItem = Osd_Menu_Get_ParentMenu_Index(g_osd_menu_data.nPage, g_osd_menu_data.nPrevPage);
						Osd_Set_Color(CT_MENU_DEFALUT);
						Osd_Clear_Block((CURRENT_MENU.nXpos), (CURRENT_MENU.nYpos), _X_(CURRENT_MENU.nWidth), _Y_(CURRENT_MENU.nHeight));
						Osd_Menu_Draw_Page(g_osd_menu_data.nPage, g_osd_menu_data.nItem);
						Osd_Menu_Draw_Page(MENU_HELP,0);
					}
				}
			}
			break;

		case LIST_MIA_INCVALUE:
		case LIST_MIA_DECVALUE:
			//TRACE_I("navi value.\r\n");
			if(CURRENT_MENU_ITEM_FUNC)
			{
				if((bool)Osd_Value_Director(true, CURRENT_MENU_ITEM_FUNC, mia)) // adjust success --> update content
				{
				/*
					if((MENU_WIFI_CFG==g_osd_menu_data.nPage)&&(_Y_(ITEM_Y_WIFI_STATUS)==CURRENT_MENU_ITEM.nYPos))
					{
					  // need update current and related items of current page
						Osd_Set_Color(CT_MENU_DEFALUT);
  					Osd_Clear_Block(0, ITEM_Y_WIFI_STATUS*OSD_FONT_YRES, UI_MENU_WIDTH*OSD_FONT_XRES, (UI_MENU_HEIGHT-ITEM_Y_WIFI_MAC)*OSD_FONT_YRES);
						Osd_Menu_Draw_Page(g_osd_menu_data.nPage, g_osd_menu_data.nItem);
						Osd_Menu_Draw_Item(g_osd_menu_data.nPage, g_osd_menu_data.nItem, true);
					}
					else if((MENU_BT_CFG==g_osd_menu_data.nPage)&&(_Y_(ITEM_Y_BT_STATUS)==CURRENT_MENU_ITEM.nYPos))
					{
					  // need update current and related items of current page
						Osd_Set_Color(CT_MENU_DEFALUT);
  					Osd_Clear_Block(0, ITEM_Y_BT_STATUS*OSD_FONT_YRES, UI_MENU_WIDTH*OSD_FONT_XRES, (UI_MENU_HEIGHT-ITEM_Y_BT_STATUS)*OSD_FONT_YRES);
						Osd_Menu_Draw_Page(g_osd_menu_data.nPage, g_osd_menu_data.nItem);
						Osd_Menu_Draw_Item(g_osd_menu_data.nPage, g_osd_menu_data.nItem, true);
					}
					else
				*/
					{
						// only need update current item
						Osd_Menu_Draw_Content(g_osd_menu_data.nPage, g_osd_menu_data.nItem, true);
					}
				}
			}
			break;

		case LIST_MIA_POP_INC:
		case LIST_MIA_POP_DEC:
			{
				OSD_MIA_ENUM eMia;

				if(LIST_MIA_POP_INC==mia)
				{
					eMia=LIST_MIA_INCVALUE;
				}
				else
				{
					eMia=LIST_MIA_DECVALUE;
				}

				if((g_osd_menu_data.nPage>=MENU_POPUP_BRIGHTNESS)&&(g_osd_menu_data.nPage<=g_osd_menu_data.nPage)) 
				{
				// when popup menu is on, do this
					if((CURRENT_MENU.nFlags)&MP_AUTO)
					{
						App_Osd_Set_Duration(OSD_DURATION_TIME); // reset timer count
					}
				
					if(CURRENT_MENU_ITEM_FUNC)
					{
						if((bool)Osd_Value_Director(true, CURRENT_MENU_ITEM_FUNC, eMia)) // adjust success --> update content
						{
							Osd_Menu_Draw_Content(g_osd_menu_data.nPage, g_osd_menu_data.nItem, true);
						}
					}
				}
				else
				{
					//TRACE_I("POPUP MENU.\r\n");
					g_osd_menu_data.nPrevPage = g_osd_menu_data.nPage;
					g_osd_menu_data.nPrevItem = g_osd_menu_data.nItem;
					g_osd_menu_data.nPage = MENU_POPUP_BRIGHTNESS;
					g_osd_menu_data.nItem = 0;
					Osd_Set_Color(CT_MENU_DEFALUT);
					Osd_Clear_Block((CURRENT_MENU.nXpos), (CURRENT_MENU.nYpos), _X_(CURRENT_MENU.nWidth), _Y_(CURRENT_MENU.nHeight));
					Osd_Menu_Draw_Page(g_osd_menu_data.nPage,g_osd_menu_data.nItem);	
					Osd_Menu_Draw_Page(MENU_HELP,0);
					
				#if 1 // patch for do action when pop up menu page
					if(CURRENT_MENU_ITEM_FUNC)
					{
						if((bool)Osd_Value_Director(true, CURRENT_MENU_ITEM_FUNC, eMia)) // adjust success --> update content
						{
							Osd_Menu_Draw_Content(g_osd_menu_data.nPage, g_osd_menu_data.nItem, true);
						}
					}
				#endif
				}
			}
			break;

			
		case LIST_MIA_MENU:
			break;
			
		case LIST_MIA_REDRAWMENU:
			break;
			
		case LIST_MIA_RELOADMENUPAGE:
			break;
			
		
		case LIST_MIA_EXECUTE:
			break;
			
		case LIST_MIA_GOTOROOT:
			break;
		
		case LIST_MIA_KEY_0:	
		case LIST_MIA_KEY_1:	
		case LIST_MIA_KEY_2:	
		case LIST_MIA_KEY_3:	
		case LIST_MIA_KEY_4:	
		case LIST_MIA_KEY_5:	
		case LIST_MIA_KEY_6:	
		case LIST_MIA_KEY_7:
		case LIST_MIA_KEY_8:	
		case LIST_MIA_KEY_9:
			break;
			
		case LIST_MIA_KEY_DEL:
			break;
			
		case LIST_MIA_KEY_OK:
			break;
			
	#if defined(ENABLE_OSD_GAME)
		case LIST_MIA_UP:
			break;
			
		case LIST_MIA_DOWN:
			break;
			
		case LIST_MIA_LEFT:
			break;
			
		case LIST_MIA_RIGHT:
			break;
			
		case LIST_MIA_START:
			break;
			
		case LIST_MIA_STOP:
			break;
			
		case LIST_MIA_PAUSE:
			break;
	#endif

		default:
			break;
	}
}


static void App_Osd_Input_Handler(void)
{
#if defined(ENABLE_OSD_LOGO)
	if (g_TimerId_Osd_Logo )
	{
		return;
	}
#endif

	if(LIST_KEY_ACT_RELEASE == g_osd_menu_data.nKeyInfo.nAction)
	{
		FRM_KEY_ENUM key = g_osd_menu_data.nKeyInfo.nKey;
		const OSD_ITEM_INPUT_EVENTS_ST* pItemInputEvents = CURRENT_MENU_ITEM.nInputEvents;
		const OSD_KEY_ACTION_MAP_ST* pKeyActionMap;
		OSD_MIA_ENUM mia;
		
    //sTRACE_I("Page=%d, Item=%d, Key=%d ", g_osd_menu_data.nPage, g_osd_menu_data.nItem, key);
			
	//-----------------------------------
    if ((pItemInputEvents != NULL) && (key>LIST_KEY_INVALID)&&(key<LIST_KEY_MAX))
  	{
      if (LIST_KEYMAP_LOCAL == pItemInputEvents->nInputSource)
      {
          mia = GlobalMenuItemAction[key];
      }
		#if defined(ENABLE_OSD_GAME)
      if (pItemInputEvents->nInputSource & LIST_KEYMAP_GAME)
      {
        if (1) // in game page
				{
					pKeyActionMap = KeyActMap_Game;
				}
				else
				{
					pKeyActionMap = pItemInputEvents->nKeyActionMap;
				}	
      }
			else
			{
				pKeyActionMap = pItemInputEvents->nKeyActionMap;
			}
		#else
      pKeyActionMap = pItemInputEvents->nKeyActionMap;
		#endif
		
      while(pKeyActionMap->nKey != LIST_KEY_INVALID)
      {
        if (key == pKeyActionMap->nKey)
        {
          mia = pKeyActionMap->nAction;
          break;
        }
        pKeyActionMap++;
      }
    }
	//-----------------------------------
    if (mia != LIST_MIA_NOTHING)
    {
       App_Osd_Key_Execute(mia);
    }
	//-----------------------------------
	}
}

//=========================================================
// ESP event
//=========================================================
void App_Osd_Add_EspEvent(ESP32_EVENT event)
{
	g_esp32_event = event;
	if(g_TaskId_Osd)
	{
		osSignalSet(g_TaskId_Osd, OSD_TASK_ESP32_MASK);
	}
}

void App_Osd_Add_RtcEvent(void)
{
	if(g_TaskId_Osd)
	{
		osSignalSet(g_TaskId_Osd, OSD_TASK_RTC_MASK);
	}
}


static void App_Osd_Esp32_Event_Handler(void)
{
#if defined(ENABLE_OSD_LOGO)
	if (g_TimerId_Osd_Logo )
	{
		return;
	}
#endif

#if ESP32_EVENT_DEBUG
	TRACE_I("=========================================ESP32 EVENT COMMING.\r\n");

	switch(g_esp32_event)
	{
		case ESP32_EVENT_WIFI_OFF:						TRACE_I("[ESP32 EVENT]:ESP32_EVENT_WIFI_OFF.\r\n");						break;									
		case ESP32_EVENT_WIFI_CONN:						TRACE_I("[ESP32 EVENT]:ESP32_EVENT_WIFI_CONN.\r\n");					break;
		case ESP32_EVENT_WIFI_GOTIP:					TRACE_I("[ESP32 EVENT]:ESP32_EVENT_WIFI_GOTIP.\r\n");					break;
		case ESP32_EVENT_WIFI_DISCONN:				TRACE_I("[ESP32 EVENT]:ESP32_EVENT_WIFI_DISCONN.\r\n");				break;
		case ESP32_EVENT_WIFI_SMARTCONFIG:		TRACE_I("[ESP32 EVENT]:ESP32_EVENT_WIFI_SMARTCONFIG.\r\n");		break;
	#ifdef ESP32_AP_LIST_SUPPORT
		case ESP32_EVENT_WIFI_AP_LIST:				TRACE_I("[ESP32 EVENT]:ESP32_EVENT_WIFI_AP_LIST.\r\n");				break;
	#endif
		case ESP32_EVENT_SNTP_TIME:						TRACE_I("[ESP32 EVENT]:ESP32_EVENT_SNTP_TIME.\r\n");					break;
		case ESP32_EVENT_SMARTCFG_INFO:				TRACE_I("[ESP32 EVENT]:ESP32_EVENT_SMARTCFG_INFO.\r\n");			break;
		case ESP32_EVENT_BT_OFF:							TRACE_I("[ESP32 EVENT]:ESP32_EVENT_BT_OFF.\r\n");							break;
		case ESP32_EVENT_BT_CONN:							TRACE_I("[ESP32 EVENT]:ESP32_EVENT_BT_CONN.\r\n");						break;
		case ESP32_EVENT_BT_DISCONN:					TRACE_I("[ESP32 EVENT]:ESP32_EVENT_BT_DISCONN.\r\n");					break;
		default:
			TRACE_W("[ESP32 EVENT]:goto default.\r\n");
			break;
	}
#endif

	switch(g_esp32_event)
	{
		case ESP32_EVENT_WIFI_OFF:
		case ESP32_EVENT_WIFI_CONN:
		case ESP32_EVENT_WIFI_GOTIP:
		case ESP32_EVENT_WIFI_DISCONN:
		case ESP32_EVENT_WIFI_SMARTCONFIG:
			#if ESP32_EVENT_DEBUG
			switch(ESP32_GetWifiState())
			{
				case ESP32_WIFI_OFF:						TRACE_I("[WIFI STATUS]:ESP32_WIFI_OFF.\r\n");						break;	
				case ESP32_WIFI_DISCONN:				TRACE_I("[WIFI STATUS]:ESP32_WIFI_DISCONN.\r\n");				break;	
				case ESP32_WIFI_CONN:						TRACE_I("[WIFI STATUS]:ESP32_WIFI_CONN.\r\n");					break;	
				case ESP32_WIFI_GOTIP:					TRACE_I("[WIFI STATUS]:ESP32_WIFI_GOTIP.\r\n");					break;	
				case ESP32_WIFI_SMARTCONFIG:		TRACE_I("[WIFI STATUS]:ESP32_WIFI_SMARTCONFIG.\r\n");		break;	
				default:
					TRACE_W("[WIFI STATUS]:wifi status goto default.\r\n");
					break;
			}
			#endif
			
			TRACE_I("wifi status changed: refresh status bar.\r\n");
			Osd_Menu_Draw_Page(MENU_STATUS_BAR, 0);
			if(MENU_WIFI_CFG==g_osd_menu_data.nPage)
			{
				TRACE_I("wifi status changed: refresh cfg menu.\r\n");
				if(1)//((ESP32_WIFI_OFF==ESP32_GetWifiState())||(ESP32_WIFI_CONN==ESP32_GetWifiState()))// 
				{
					Osd_Set_Color(CT_MENU_DEFALUT);
					Osd_Clear_Block(0, ITEM_Y_WIFI_STATUS*OSD_FONT_YRES, UI_MENU_WIDTH*OSD_FONT_XRES, (UI_MENU_HEIGHT-ITEM_Y_WIFI_MAC)*OSD_FONT_YRES);
				}
				Osd_Menu_Draw_Page(g_osd_menu_data.nPage, g_osd_menu_data.nItem);
				//Osd_Menu_Draw_Item(g_osd_menu_data.nPage, g_osd_menu_data.nItem, true);
			}
			break;

			
	#ifdef ESP32_AP_LIST_SUPPORT
		case ESP32_EVENT_WIFI_AP_LIST:
			break;
	#endif
	
		case ESP32_EVENT_SNTP_TIME:
			break;
			
		case ESP32_EVENT_SMARTCFG_INFO:
			break;
			
		case ESP32_EVENT_BT_OFF:
		case ESP32_EVENT_BT_CONN:
		case ESP32_EVENT_BT_DISCONN:
			TRACE_I("bt status changed: refresh status bar.\r\n");
			Osd_Menu_Draw_Page(MENU_STATUS_BAR, 0);
			if(MENU_BT_CFG==g_osd_menu_data.nPage)
			{
				TRACE_I("bt status changed: refresh cfg menu.\r\n");
				Osd_Menu_Draw_Page(g_osd_menu_data.nPage, g_osd_menu_data.nItem);
				//Osd_Menu_Draw_Item(g_osd_menu_data.nPage, g_osd_menu_data.nItem, true);
			}
			break;

		default:
			break;
	}
	
}



//=========================================================
// Task
//=========================================================
static void App_Osd_Task_Init(void)
{
// Init
	Osd_Init_Variable();
// Timer---Logo
#if defined(ENABLE_OSD_LOGO)
	osTimerDef(OsdLogoDispTimer, App_Osd_Logo_Timer_Expired);
	g_TimerId_Osd_Logo  = osTimerCreate(osTimer(OsdLogoDispTimer), osTimerOnce, NULL);
	assert_param(g_TimerId_Osd_Logo  != NULL);
	
	Flex_App_Draw_Osd_Logo();
	Drv_Panel_Power(LIST_PANEL_INDEX_MAIN, LIST_PANEL_ON);  // Turn On Panel
	//load brightness from nvram
	Osd_User_Data_Load(LIST_NVRAM_TAG_BRIGHTNESS);
	//set brightness
	Drv_Panel_Set_Brightness(LIST_PANEL_INDEX_MAIN,g_osd_user_data.nBrightness);
	osTimerStart(g_TimerId_Osd_Logo , LOGO_ON_PERIOD);
#else
	Osd_Menu_Draw_Page(MENU_STATUS_BAR, 0);
	g_osd_menu_data.nPage = MENU_MAIN;
	g_osd_menu_data.nItem = Osd_Menu_Get_First_Item_Index(g_osd_menu_data.nPage);
	Osd_Menu_Draw_Page(g_osd_menu_data.nPage, g_osd_menu_data.nItem);
	Osd_Menu_Draw_Page(MENU_HELP, 0);
	Drv_Panel_Power(LIST_PANEL_INDEX_MAIN, LIST_PANEL_ON);  // Turn On Panel
#endif
	// Timer---Osd Duration
	osTimerDef(OsdDurationTimer, App_Osd_Duration_Callback);
	g_TimerId_Osd_Duration= osTimerCreate(osTimer(OsdDurationTimer), osTimerOnce, NULL);
	assert_param(g_TimerId_Osd_Duration != NULL);

// set key callback
	Frm_Input_Key_Register_Callback(App_Osd_Input_Key_Callback);
}

/*========================================================================================================
										F U N C T I O N----extern
========================================================================================================*/
//=========================================================
// Task
//=========================================================
void App_Osd_Task(void const * argument)
{
	osEvent event;

	TRACE_F();
	
	g_TaskId_Osd = osThreadGetId();
	
	App_Osd_Task_Init();
	

  /* Infinite loop */
	for(;;)
	{
		event=osSignalWait(0x7FFFFFFF,osWaitForever);  

		if(event.status==osEventSignal)  
		{
		//---------------------------------------------------
		#if defined(ENABLE_OSD_LOGO)
			if (event.value.signals & OSD_TASK_LOGO_MASK)
			{
				Osd_Set_Color(CT_MENU_DEFALUT);
				Osd_Clear_All();
				Osd_Menu_Draw_Page(MENU_STATUS_BAR, 0);
				g_osd_menu_data.nPage = MENU_MAIN;
				g_osd_menu_data.nItem = Osd_Menu_Get_First_Item_Index(g_osd_menu_data.nPage);
				Osd_Menu_Draw_Page(g_osd_menu_data.nPage, g_osd_menu_data.nItem);
				Osd_Menu_Draw_Page(MENU_HELP, 0);
			// timer delete
				osTimerDelete(g_TimerId_Osd_Logo );
				g_TimerId_Osd_Logo  = NULL;
			}
		#endif
		//---------------------------------------------------
			if (event.value.signals & OSD_TASK_KEY_MASK)
			{
				App_Osd_Input_Handler();
			}
		//---------------------------------------------------
			if (event.value.signals & OSD_TASK_ESP32_MASK)
			{
				App_Osd_Esp32_Event_Handler();
			}
		//---------------------------------------------------
			if (event.value.signals & OSD_TASK_RTC_MASK)
			{
				Osd_Menu_Draw_Page(MENU_STATUS_BAR, 0);
			}
		
		}
	}
}


/*========================================================================================================
										End
========================================================================================================*/




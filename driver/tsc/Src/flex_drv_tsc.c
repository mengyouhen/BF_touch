/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_drv_tsc.c
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
#include "flex_drv_led.h"
#include "tsl_user.h"
#include "flex_framework_input.h"
// internal
#include "flex_drv_tsc.h"


/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME								"[TSC]--"
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
extern TSL_ChannelData_T MyChannels_Data[TSLPRM_TOTAL_CHANNELS];


#define WHEEL_0_DELTA    (MyChannels_Data[0].Delta)
#define WHEEL_1_DELTA    (MyChannels_Data[1].Delta)
#define WHEEL_2_DELTA    (MyChannels_Data[2].Delta)
#define WHEEL_3_DELTA    (MyChannels_Data[3].Delta)
#define WHEEL_4_DELTA    (MyChannels_Data[4].Delta)
#define WHEEL_5_DELTA    (MyChannels_Data[5].Delta)

#define BTN_UP_DELTA     (MyChannels_Data[6].Delta)
#define BTN_RIGHT_DELTA  (MyChannels_Data[7].Delta)

#define SLIDE_0_DELTA    (MyChannels_Data[8].Delta)
#define SLIDE_1_DELTA    (MyChannels_Data[9].Delta)
#define SLIDE_2_DELTA    (MyChannels_Data[10].Delta)

/*========================================================================================================
										D E F I N E
========================================================================================================*/
#define ENABLE_TSC_WHEEL      true  // define this to avoid mixrure of g_tsc_wheel_cntx and g_tsc_slide_cntx when copy code
#define ENABLE_TSC_SLIDE      true	// define this to avoid mixrure of g_tsc_wheel_cntx and g_tsc_slide_cntx when copy code
#define ENABLE_TSC_KEY        true	

//////////////////////////////////////////////////////
/////////////////////  WHEEL   ///////////////////////
//////////////////////////////////////////////////////
#if ENABLE_TSC_WHEEL
#define WHEEL_DETECT          (MyLinRots[0].p_Data->StateId == TSL_STATEID_DETECT) 
#define WHEEL_POSITION        (MyLinRots[0].p_Data->Position)
#define WHEEL_STATE_ID        (MyLinRots[0].p_Data->StateId)
#endif

//////////////////////////////////////////////////////
/////////////////////  SLIDE   ///////////////////////
//////////////////////////////////////////////////////
#if ENABLE_TSC_SLIDE
#define SLIDE_DETECT          (MyLinRots[1].p_Data->StateId == TSL_STATEID_DETECT) 
#define SLIDE_POSITION        (MyLinRots[1].p_Data->Position)
#define SLIDE_STATE_ID        (MyLinRots[1].p_Data->StateId)
#endif

//////////////////////////////////////////////////////
/////////////////////   KEY    ///////////////////////
//////////////////////////////////////////////////////
#if ENABLE_TSC_KEY
#define KEY_UP_DETECT         (MyTKeys[0].p_Data->StateId == TSL_STATEID_DETECT)
#define KEY_RIGHT_DETECT      (MyTKeys[1].p_Data->StateId == TSL_STATEID_DETECT)
#endif


//////////////////////////////////////////////////////
/////////////////////  GESTURE  //////////////////////
//////////////////////////////////////////////////////
#define IGNORE_COUNT              3
#define GETSTURE_JUDGE_CONDITION  8
#define PATCH_FOR_HALF_END_CH			true // patch fot one channel into two end
#define PATCH_FOR_WHEEL_POS_JUMP  true
/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
typedef struct
{
	flex_led_name_enum nLedPreIdx;	
	flex_led_name_enum nLedCurIdx;	
	int8_t nPos;
#if PATCH_FOR_HALF_END_CH 
	uint8_t nPrevPos;
#endif
	uint8_t nPosStart;
	uint8_t nPosEnd;
	uint8_t nDelta;
	bool nFlag;
	DRV_BTN_INFO_ST nBtnInfo;
}TSC_TOUCH_INFO_ST;


/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
#if ENABLE_TSC_WHEEL
static TSC_TOUCH_INFO_ST g_tsc_wheel_cntx;
#endif

#if ENABLE_TSC_SLIDE
static TSC_TOUCH_INFO_ST g_tsc_slide_cntx;
#endif

#if ENABLE_TSC_KEY
static DRV_BTN_INFO_ST g_tsc_input={.nBtn=LIST_BTN_NONE, .nAct=LIST_BTN_ACT_RELEASE,};
static fpDrvBtnEvent g_tsc_button_cb=NULL;
static bool g_keyup_flag=false;
static bool g_keyright_flag=false;
#endif

#if PATCH_FOR_WHEEL_POS_JUMP
static uint16_t g_wheel_deltas[6]={0};
#endif
/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
#if ENABLE_TSC_WHEEL
static void Drv_Tsc_Process_Wheel(tsl_user_status_t status)
{
	static uint8_t ignore_count=0;
	static bool flag_released=true;
	
	if (WHEEL_DETECT)
	{
	#if PATCH_FOR_WHEEL_POS_JUMP
		g_wheel_deltas[0]=FLEX_ABS(WHEEL_0_DELTA);
		g_wheel_deltas[1]=FLEX_ABS(WHEEL_1_DELTA);
		g_wheel_deltas[2]=FLEX_ABS(WHEEL_2_DELTA);
		g_wheel_deltas[3]=FLEX_ABS(WHEEL_3_DELTA);
		g_wheel_deltas[4]=FLEX_ABS(WHEEL_4_DELTA);
		g_wheel_deltas[5]=FLEX_ABS(WHEEL_5_DELTA);
	#endif
	
		flag_released=false;
	//------------ FILTER DATA ------------
		if(ignore_count<IGNORE_COUNT)
		{
			if(0==ignore_count)
			{
				TRACE_I("=======================================================\r\n");
			}
			// filter
			TRACE_I("g_tsc_wheel_cntx.nPos=%d.\r\n", g_tsc_wheel_cntx.nPos);
			TRACE_I("wheel(XXXXX)----status=%d, pos=%d, StateId=%d.\r\n", status, WHEEL_POSITION, WHEEL_STATE_ID);
			ignore_count++;
			return;
		}
		else
		{
		#if PATCH_FOR_WHEEL_POS_JUMP
			switch(Find_Max_u16Value_Index(g_wheel_deltas,6))
			{
				case 0:
					if(WHEEL_POSITION!=15)
					{
						WHEEL_POSITION=0;
					}
					break;
					
				case 1: 	
					WHEEL_POSITION=2; 
					break;
					
				case 2: 		
					WHEEL_POSITION=5; 
					break;
					
				case 3: 			
					WHEEL_POSITION=8; 
					break;
					
				case 4: 	
					WHEEL_POSITION=11; 
					break;
					
				case 5: 		
					WHEEL_POSITION=14; 
					break;
					
				default:
					break;
			}


			if(2==WHEEL_POSITION)
			{
				if((WHEEL_0_DELTA>35)&&(WHEEL_1_DELTA<180))
				{
					WHEEL_POSITION=0;
				}
			}
			else if(14==WHEEL_POSITION)
			{
				if((WHEEL_0_DELTA>25)&&(WHEEL_5_DELTA<110))
				{
					WHEEL_POSITION=15;
				}
			}
		#endif
		
			if(-1 == g_tsc_wheel_cntx.nPos)
			{
				TRACE_I("wheel(valid)----status=%d, pos=%d, StateId=%d.\r\n", status, WHEEL_POSITION, WHEEL_STATE_ID);
				g_tsc_wheel_cntx.nPos = WHEEL_POSITION;
			}
			else
			{
				if(FLEX_DELTA(g_tsc_wheel_cntx.nPos,WHEEL_POSITION)>=8)
				{
					// ignore
					TRACE_I("wheel(OOOOO)----(>8)----status=%d, pos=%d, WHEEL_POSITION=%d, StateId=%d.\r\n", status, g_tsc_wheel_cntx.nPos, WHEEL_POSITION, WHEEL_STATE_ID);
					return;
				}
				else
				{
					TRACE_I("wheel(valid)----status=%d, pos=%d, StateId=%d.\r\n", status, WHEEL_POSITION, WHEEL_STATE_ID);
					g_tsc_wheel_cntx.nPos = WHEEL_POSITION;
				}
			}
		}
		
	// ------------ LED ------------
    /*
      -------------------------------------------
      |  o     o     o     o     o     o     o  |
      -------------------------------------------
         0     2     4     8     11    14    15
    */
		if (g_tsc_wheel_cntx.nPos == 0)
		{
			g_tsc_wheel_cntx.nLedCurIdx = LIST_LED_WHEEL_1;
		}
		else if (g_tsc_wheel_cntx.nPos < 4)
		{
			g_tsc_wheel_cntx.nLedCurIdx = LIST_LED_WHEEL_2;
		}
		else if (g_tsc_wheel_cntx.nPos < 8)
		{
			g_tsc_wheel_cntx.nLedCurIdx = LIST_LED_WHEEL_3;
		}
		else if (g_tsc_wheel_cntx.nPos < 11)
		{
			g_tsc_wheel_cntx.nLedCurIdx = LIST_LED_WHEEL_4;
		}
		else if (g_tsc_wheel_cntx.nPos < 14)
		{
			g_tsc_wheel_cntx.nLedCurIdx = LIST_LED_WHEEL_5;
		}
		else if (g_tsc_wheel_cntx.nPos < 15)
		{
			g_tsc_wheel_cntx.nLedCurIdx = LIST_LED_WHEEL_6;
		}
		else if (g_tsc_wheel_cntx.nPos == 15)
		{
			g_tsc_wheel_cntx.nLedCurIdx = LIST_LED_WHEEL_7;
		}

		if((g_tsc_wheel_cntx.nLedCurIdx != LIST_LED_NONE) && (g_tsc_wheel_cntx.nLedCurIdx != g_tsc_wheel_cntx.nLedPreIdx))
		{
			Drv_Led_Ctrl(g_tsc_wheel_cntx.nLedPreIdx, false);	
			Drv_Led_Ctrl(g_tsc_wheel_cntx.nLedCurIdx, true);	
			g_tsc_wheel_cntx.nLedPreIdx = g_tsc_wheel_cntx.nLedCurIdx;
		}

	// ------------ DELTA ------------
		if(! g_tsc_wheel_cntx.nFlag)
		{
			// START POSITION
			g_tsc_wheel_cntx.nFlag = true;
			g_tsc_wheel_cntx.nPosStart=g_tsc_wheel_cntx.nPos;
		}
		else
		{
	#if PATCH_FOR_HALF_END_CH 
			if( (g_tsc_wheel_cntx.nPrevPos<=3) && (g_tsc_wheel_cntx.nPos>=12))
			{
				// CONFIRM START POSITION
				//TRACE_I("TOUCH RIGHT HALF CHANNEL----recalc start postion(pos=%d, pre=%d).\r\n", g_tsc_wheel_cntx.nPos, g_tsc_wheel_cntx.nPrevPos);
				g_tsc_wheel_cntx.nPos =15;
				g_tsc_wheel_cntx.nPosStart=g_tsc_wheel_cntx.nPos;  
			}
			else
	#endif
			{
				// CALC DELATA
				uint8_t delta;
				
				delta=FLEX_DELTA(g_tsc_wheel_cntx.nPosStart, g_tsc_wheel_cntx.nPos);
				
				if(delta>g_tsc_wheel_cntx.nDelta)
				{
					g_tsc_wheel_cntx.nPosEnd=g_tsc_wheel_cntx.nPos; // update end pos
					g_tsc_wheel_cntx.nDelta=delta;
				}
			}
		}
#if PATCH_FOR_HALF_END_CH 
		g_tsc_wheel_cntx.nPrevPos=g_tsc_wheel_cntx.nPos;
#endif
	}
	else	
	{
		if(g_tsc_wheel_cntx.nPrevPos>=14)
		{
			if((WHEEL_0_DELTA>25)&&(WHEEL_5_DELTA<110))
			{
				TRACE_I("RELEASE-->DETECT_LAST(pos=%d)\r\n", WHEEL_POSITION);
				MyLinRots[0].p_Data->StateId = TSL_STATEID_DETECT;
				WHEEL_POSITION=15;
				Drv_Led_Ctrl(LIST_LED_WHEEL_7, true); 
				return;
			}
		}
		/*
		else if(g_tsc_wheel_cntx.nPrevPos<=2)
		{
			if((WHEEL_0_DELTA>35)&&(WHEEL_1_DELTA<180))
			{
				TRACE_I("RELEASE-->DETECT_LAST(pos=%d)\r\n", WHEEL_POSITION);
				MyLinRots[0].p_Data->StateId = TSL_STATEID_DETECT;
				WHEEL_POSITION=0;
				Drv_Led_Ctrl(LIST_LED_WHEEL_1, true); 
				return;
			}
		}
		*/
	
		if(!flag_released)
		{

		
			TRACE_I("--------------------relese-------------------\r\n");
			flag_released=true;
		// ------------ LED ------------
			Drv_Led_Ctrl(LIST_LED_WHEEL_1, false);	
			Drv_Led_Ctrl(LIST_LED_WHEEL_2, false);	
			Drv_Led_Ctrl(LIST_LED_WHEEL_3, false);	
			Drv_Led_Ctrl(LIST_LED_WHEEL_4, false);	
			Drv_Led_Ctrl(LIST_LED_WHEEL_5, false);	
			Drv_Led_Ctrl(LIST_LED_WHEEL_6, false);	
			Drv_Led_Ctrl(LIST_LED_WHEEL_7, false);	

		// ------------ GESTURE ------------
			if(g_tsc_button_cb!=NULL)
			{
			/*
				if(g_tsc_wheel_cntx.nDelta>0)
				{
					TRACE_I("delta=%d, ref=%d.\r\n", g_tsc_wheel_cntx.nDelta, GETSTURE_JUDGE_CONDITION);
				}
			*/
				if(g_tsc_wheel_cntx.nDelta >= GETSTURE_JUDGE_CONDITION) // gesture generate
				{
					if(g_tsc_wheel_cntx.nPosStart > g_tsc_wheel_cntx.nPosEnd)
					{
						g_tsc_wheel_cntx.nBtnInfo.nBtn = LIST_BTN_PAGE_UP;
						//TRACE_I("slide----report BTN_PAGE_UP.\r\n");
					}
					else 
					{
						g_tsc_wheel_cntx.nBtnInfo.nBtn = LIST_BTN_PAGE_DOWN;
						//TRACE_I("slide----report BTN_PAGE_DOWN.\r\n");
					}
					g_tsc_wheel_cntx.nBtnInfo.nAct = LIST_BTN_ACT_RELEASE;
					g_tsc_button_cb(g_tsc_wheel_cntx.nBtnInfo);  
				}
			}
		// ------------ RESET ------------
			pf_memset(&g_tsc_wheel_cntx, 0, sizeof(g_tsc_wheel_cntx));
			g_tsc_wheel_cntx.nPos=-1;
			ignore_count=0;

			//WHEEL_POSITION=0;
		}
	}
}
#endif

#if ENABLE_TSC_SLIDE
static void Drv_Tsc_Process_Slide(tsl_user_status_t status)
{
	static uint8_t ignore_count=0;
	static bool flag_released=true;
	
	if (SLIDE_DETECT)
	{
	#if 1 // patch
		if(0==SLIDE_POSITION)
		{
			if(SLIDE_0_DELTA<100)
			{
				return;
			}
			/*
			else
			{
				if((SLIDE_1_DELTA-SLIDE_2_DELTA)<=8)
				{
					MyLinRots[1].p_Data->Position=15;
				}
			}*/
		}

		if(15==SLIDE_POSITION)
		{
			if(SLIDE_0_DELTA<100)
			{
				return;
			}
		}
	#endif
	
		flag_released=false;
	//------------ FILTER DATA ------------
		if(ignore_count<IGNORE_COUNT)
		{
			if(0==ignore_count)
			{
				TRACE_I("=======================================================\r\n");
			}
			// filter
			TRACE_I("g_tsc_slide_cntx.nPos=%d.\r\n", g_tsc_slide_cntx.nPos);
			TRACE_I("slide(XXXXX)----status=%d, pos=%d, StateId=%d.\r\n", status, SLIDE_POSITION, SLIDE_STATE_ID);
			ignore_count++;
			return;
		}
		else
		{
			if(-1 == g_tsc_slide_cntx.nPos)
			{
				TRACE_I("slide(valid)----status=%d, pos=%d, StateId=%d.\r\n", status, SLIDE_POSITION, SLIDE_STATE_ID);
				g_tsc_slide_cntx.nPos = SLIDE_POSITION;
			}
			else
			{
				if(FLEX_DELTA(g_tsc_slide_cntx.nPos,SLIDE_POSITION)>=8)
				{
					// ignore
					TRACE_I("slide(OOOOO)----(>8)----status=%d, pos=%d, StateId=%d.\r\n", status, SLIDE_POSITION, SLIDE_STATE_ID);
					return;
				}
				else
				{
					TRACE_I("slide(valid)----status=%d, pos=%d, StateId=%d.\r\n", status, SLIDE_POSITION, SLIDE_STATE_ID);
					g_tsc_slide_cntx.nPos = SLIDE_POSITION;
				}
			}
		}
		
	// ------------ LED ------------
    /*
    --------------------------
    |  >     >     >     >  |
    --------------------------
       0     5     10    15
    */
		if (g_tsc_slide_cntx.nPos <= 1)
		{
			g_tsc_slide_cntx.nLedCurIdx = LIST_LED_SLIDE_1;
		}
		else if (g_tsc_slide_cntx.nPos <= 7)
		{
			g_tsc_slide_cntx.nLedCurIdx = LIST_LED_SLIDE_2;
		}
		else if (g_tsc_slide_cntx.nPos <= 13)
		{
			g_tsc_slide_cntx.nLedCurIdx = LIST_LED_SLIDE_3;
		}
		else if (g_tsc_slide_cntx.nPos <= 15)
		{
			g_tsc_slide_cntx.nLedCurIdx = LIST_LED_SLIDE_4;
		}

		if((g_tsc_slide_cntx.nLedCurIdx != LIST_LED_NONE) && (g_tsc_slide_cntx.nLedCurIdx != g_tsc_slide_cntx.nLedPreIdx))
		{
			Drv_Led_Ctrl(g_tsc_slide_cntx.nLedPreIdx, false);	
			Drv_Led_Ctrl(g_tsc_slide_cntx.nLedCurIdx, true);	
			g_tsc_slide_cntx.nLedPreIdx = g_tsc_slide_cntx.nLedCurIdx;
		}
		
	// ------------ DELTA ------------
		if(! g_tsc_slide_cntx.nFlag)
		{
			// START POSITION
			g_tsc_slide_cntx.nFlag = true;
			g_tsc_slide_cntx.nPosStart=g_tsc_slide_cntx.nPos;
		}
		else
		{
		#if PATCH_FOR_HALF_END_CH 
			if( (g_tsc_slide_cntx.nPrevPos<=3) && (g_tsc_slide_cntx.nPos>=12))
			{
				// CONFIRM START POSITION
				//TRACE_I("TOUCH RIGHT HALF CHANNEL----recalc start postion(pos=%d, pre=%d).\r\n", g_tsc_slide_cntx.nPos, g_tsc_slide_cntx.nPrevPos);
				g_tsc_slide_cntx.nPos =15;
				g_tsc_slide_cntx.nPosStart=g_tsc_slide_cntx.nPos;  
			}
			else
		#endif
			{
				// CALC DELATA
				uint8_t delta;
				
				delta=FLEX_DELTA(g_tsc_slide_cntx.nPosStart, g_tsc_slide_cntx.nPos);
				
				if(delta>g_tsc_slide_cntx.nDelta)
				{
					g_tsc_slide_cntx.nPosEnd=g_tsc_slide_cntx.nPos; // update end pos
					g_tsc_slide_cntx.nDelta=delta;
				}
			}
		}
	#if PATCH_FOR_HALF_END_CH 
		g_tsc_slide_cntx.nPrevPos=g_tsc_slide_cntx.nPos;
	#endif
	}
	else	
	{
		if(!flag_released)
		{
			TRACE_I("--------------------relese-------------------\r\n");
			flag_released=true;
		// ------------ LED ------------
			Drv_Led_Ctrl(LIST_LED_SLIDE_1, false);	
			Drv_Led_Ctrl(LIST_LED_SLIDE_2, false);	
			Drv_Led_Ctrl(LIST_LED_SLIDE_3, false);	
			Drv_Led_Ctrl(LIST_LED_SLIDE_4, false);	

		// ------------ GESTURE ------------
			if(g_tsc_button_cb!=NULL)
			{	
			/*
				if(g_tsc_slide_cntx.nDelta>0)
				{
					TRACE_I("delta=%d, ref=%d.\r\n", g_tsc_slide_cntx.nDelta, GETSTURE_JUDGE_CONDITION);
				}
			*/
				
				if(g_tsc_slide_cntx.nDelta >= GETSTURE_JUDGE_CONDITION) // gesture generate
				{
					if(g_tsc_slide_cntx.nPosStart > g_tsc_slide_cntx.nPosEnd)
					{
						g_tsc_slide_cntx.nBtnInfo.nBtn = LIST_BTN_DEC;
						//TRACE_I("slide----report BTN_DEC.\r\n");
					}
					else 
					{
						g_tsc_slide_cntx.nBtnInfo.nBtn = LIST_BTN_INC;
						//TRACE_I("slide----report BTN_INC.\r\n");
					}
					g_tsc_slide_cntx.nBtnInfo.nAct = LIST_BTN_ACT_RELEASE;
					g_tsc_button_cb(g_tsc_slide_cntx.nBtnInfo);  
				}
			}
			
		// ------------ RESET ------------
			pf_memset(&g_tsc_slide_cntx, 0, sizeof(g_tsc_slide_cntx));
			g_tsc_slide_cntx.nPos=-1;
			ignore_count=0;
		}
	}
}
#endif

#if ENABLE_TSC_KEY
/**
  * @brief  set callback func when tsc key is detected
  * @param  callback function
  * @retval None
  */
void Drv_Tsc_Set_Callback(fpDrvBtnEvent func_ptr)
{   
	if(func_ptr!=NULL)
	{
		g_tsc_button_cb = func_ptr;
		
		// if there is key evnet occurs when registe this callback funtion
		if(g_tsc_input.nBtn!= LIST_BTN_NONE)
		{
			g_tsc_button_cb(g_tsc_input); 
		}
	}
}

static void Drv_Tsc_Key_Report(DRV_BTN_ENUM btn, DRV_BTN_ACT_ENUM act)
{
	if((btn != LIST_BTN_NONE) &&(g_tsc_button_cb!=NULL))
	{
		g_tsc_input.nBtn = btn;  
		g_tsc_input.nAct = act;
		g_tsc_button_cb(g_tsc_input);  	
	}
}

static void Drv_Tsc_Process_Key(tsl_user_status_t status)
{
/* 
	sometimes, when enter this funtion, old key released and new key pressed are both occurred.
	for ig. finger move KEY_UP to KEY_RIGHT, or otherwise.
	so, we need report old key released event first.
*/
//--------------------------------------------------
	if (!KEY_UP_DETECT)
	{
		if(g_keyup_flag)
		{
			TRACE_I("============================report: BUTTON_UP--- release.\r\n");
			g_keyup_flag=false;
			Drv_Tsc_Key_Report(LIST_BTN_UP, LIST_BTN_ACT_RELEASE);  
		}
	}
	
	if (!KEY_RIGHT_DETECT)
	{
		if(g_keyright_flag)
		{
			TRACE_I("============================report: BUTTON_RIGHT--- release.\r\n");
			g_keyright_flag=false;
			Drv_Tsc_Key_Report(LIST_BTN_RIGHT, LIST_BTN_ACT_RELEASE);  
		}
	}
//--------------------------------------------------
	if (KEY_UP_DETECT)
	{
		if(!g_keyup_flag)
		{
			TRACE_I("============================report: BUTTON_UP--- press.\r\n");
			g_keyup_flag=true;
			Drv_Tsc_Key_Report(LIST_BTN_UP, LIST_BTN_ACT_PRESS);	 
		}
	}
	else if (KEY_RIGHT_DETECT)
	{
		if(!g_keyright_flag)
		{
			TRACE_I("============================report: BUTTON_RIGHT--- press.\r\n");
			g_keyright_flag=true;
			Drv_Tsc_Key_Report(LIST_BTN_RIGHT, LIST_BTN_ACT_PRESS);  
		}
	}
//--------------------------------------------------
}
#endif

/**
  * @brief  Display sensors information on LEDs
  * @param  status TSL user status
  * @retval None
  */
void Drv_Tsc_Process_Sensors(tsl_user_status_t status) 
{
#if ENABLE_TSC_WHEEL
	Drv_Tsc_Process_Wheel(status);
#endif
#if ENABLE_TSC_SLIDE
	Drv_Tsc_Process_Slide(status);
#endif
#if ENABLE_TSC_KEY
	Drv_Tsc_Process_Key(status);
#endif
//----------------------------------	
#ifdef ECS_INFO  
  /* ECS information */
	switch (status)
	{
		case TSL_USER_STATUS_OK_ECS_OFF:
			break;
			
		case TSL_USER_STATUS_OK_ECS_ON:
			break;
			
		default:
			break;
	}
#endif  
}

/**
  * @brief  This function handles a tick.
  * @param  None
  * @retval None
  */

void Drv_Tsc_Init(void)
{
	TRACE_F();
	
#if ENABLE_TSC_WHEEL
	pf_memset(&g_tsc_wheel_cntx, 0, sizeof(g_tsc_wheel_cntx));
	g_tsc_wheel_cntx.nPos =-1;
#endif

#if ENABLE_TSC_SLIDE
	pf_memset(&g_tsc_slide_cntx, 0, sizeof(g_tsc_slide_cntx));
	g_tsc_slide_cntx.nPos =-1;
#endif

#if ENABLE_TSC_KEY
	
#endif
	/* Initialize the STMTouch driver */
	tsl_user_Init();

// key generated source 
	Drv_Tsc_Set_Callback(Frm_Input_Button_Callback);

}






/*========================================================================================================
										End
========================================================================================================*/




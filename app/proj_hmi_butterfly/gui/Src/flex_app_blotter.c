/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		flex_app_blotter.c
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
#include "flex_app_blotter_menu.h"

// internal
#include "flex_app_blotter.h"


/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME					"[BLOTTER]--"
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
//#define SIMULATE_ESP_CMD     // only for simulate

#if defined(ENABLE_BLOTTER_LOGO)
#define BLOTTER_TASK_LOGO_MASK         _BIT0_
#endif
#define BLOTTER_TASK_REFREASH_MASK     _BIT1_
#define BLOTTER_TASK_KEY_MASK          _BIT2_
#if defined(SIMULATE_ESP_CMD)
#define BLOTTER_TASK_SAVE_MSG_MASK     _BIT3_
#endif



//----------------------------
// chain
//----------------------------
#define BLOTTER_CHAIN_NODE_MAX    30
#define BLOTTER_MSG_INDEX_MAX     999 // 1~999
#define BLOTTER_MSG_INDEX_LEN     5	// [999] acorrding to the len of BLOTTER_MSG_INDEX_MAX
#define BLOTTER_RECV_LEN_MAX			(BLOTTER_MENU_FONTS_MAX-BLOTTER_MSG_INDEX_LEN) // max len of recv buffer from driver


/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
#if defined(SIMULATE_ESP_CMD)
typedef enum
{
	LIST_BLOTTER_EVENT_WIFI_STATUS,
	LIST_BLOTTER_EVENT_BT_STATUS,
	LIST_BLOTTER_EVENT_BT_DATA,
	LIST_BLOTTER_EVENT_CMD_DATA,
	LIST_BLOTTER_EVENT_MAX
} DRV_BLOTTER_EVENT_ENUM;
#endif


/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
// Task
static osThreadId g_TaskId_Blotter = NULL;
// Timer
#if defined(ENABLE_BLOTTER_LOGO)
static osTimerId g_TimerId_BlotterLogo = NULL;
#endif


// simulate esp activity
#if defined(SIMULATE_ESP_CMD)
static osTimerId g_TimerId_EspEvent = NULL;
static fpBlotterGetEvent g_drv_esp_cb = NULL;
char *const gBlotterEventMsg[LIST_BLOTTER_EVENT_MAX] =
{
	"wifi disconnect",
	"bt connect",
	"wifi connnect to Flex_Skynet(00:12:78:fe:39:01)",
	"Flex provides a comprehensive set of services that enable companies, from startups to multinationals successfully innovate, create, and gain access to new markets.",
};
#endif

static BLOTTER_CHAIN_NODE_ST *g_blotter_chain_pHead = NULL;
static BLOTTER_CHAIN_NODE_ST *g_blotter_chain_pTail = NULL;
static uint8_t g_blotter_chain_node_count = 0; // MAX 10
static uint16_t g_blotter_msg_index = 0;


static FRM_KEY_ENUM g_blotter_key = LIST_KEY_INVALID;
/*========================================================================================================
										F U N C T I O N----local
========================================================================================================*/
//=========================================================
// LOGO
//=========================================================
#if defined(ENABLE_BLOTTER_LOGO)
static void App_Blotter_Logo_Timer_Expired(void const *argument)
{
	int32_t ret = 0;

	//if (g_TaskId_Blotter != NULL)
	{
		ret = osSignalSet(g_TaskId_Blotter, BLOTTER_TASK_LOGO_MASK);

		if (0x80000000 == ret)
		{
			TRACE_W("[TIMER_LOGO]-ret(%d).\r\n", ret);
		}
	}
}

static void App_Blotter_Logo_Show_Off(void)
{
	if (g_TimerId_BlotterLogo)
	{
		//logo is still on display
		osTimerStop(g_TimerId_BlotterLogo);
		osTimerDelete(g_TimerId_BlotterLogo);
		g_TimerId_BlotterLogo = NULL;
		//clear logo scree and draw title
		Blotter_Menu_Clear_All();
		Blotter_Menu_Draw_Title();
	}
}
#endif

//=========================================================
// simulate
//=========================================================
#if defined(SIMULATE_ESP_CMD)
static void Drv_Esp_Set_Callback(fpBlotterGetEvent func_ptr)
{
	if (func_ptr)
	{
		g_drv_esp_cb = func_ptr;

		if (g_drv_esp_cb)
		{
			// do nothing
		}
	}
}

static void Drv_Esp_Recv_Data_Process(const uint8_t *pBuff, uint16_t len)
{
	if (pBuff && (len > 0) && g_drv_esp_cb)
	{
		blotter_messgae_st msg;
		msg.len = len;
		msg.buffer = (uint8_t * )pBuff;
		g_drv_esp_cb(&msg);
	}
}

static void Drv_Esp_Event_Occur(void const *argument)   // timer expired func
{
	static uint8_t count=1;

	TRACE_I("Event count=%d.\r\n",count);
	
	if(30==count)
	{
		osTimerStart(g_TimerId_EspEvent, 60000);
	}

	if (g_TaskId_Blotter)
	{
		osSignalSet(g_TaskId_Blotter, BLOTTER_TASK_SAVE_MSG_MASK);
	}

	if(count<0xFF)
		count++;
	else
		count=1;
	
	
	//DBG_MSG("free heap:%d\n", xPortGetFreeHeapSize());
}
#endif

//=========================================================
// chain
//=========================================================
static void Blotter_Chain_Print(void)
{
#if	0
	BLOTTER_CHAIN_NODE_ST *pLoop = g_blotter_chain_pHead;
	TRACE_I("\r\n---Chain from Head---\r\n");

	do
	{
		TRACE_I("[%03d]:%s,%d\r\n", pLoop->index, pLoop->msg.buffer, pLoop->msg.len);
		pLoop = pLoop->next;
	}
	while (pLoop != NULL);

#endif

#if	0
	BLOTTER_CHAIN_NODE_ST *pLoop = g_blotter_chain_pTail;
	TRACE_I("\r\n---Chain from Tail---\r\n");

	do
	{
		TRACE_I("[%03d]:%s,%d\r\n", pLoop->index, pLoop->msg.buffer, pLoop->msg.len);
		pLoop = pLoop->prev;
	}
	while (pLoop != NULL);

#endif

}

/*
static void Blotter_Chain_Create(void)
{
	if(NULL == g_blotter_chain_pHead)
	{
		g_blotter_chain_pHead = (BLOTTER_CHAIN_NODE_ST*)pf_malloc(sizeof(BLOTTER_CHAIN_NODE_ST));
		if(g_blotter_chain_pHead)
		{
			g_blotter_chain_pTail = g_blotter_chain_pHead;
			g_blotter_chain_pHead->next = NULL;
		}
	}
}
*/

static void Blotter_Chain_Destroy(void)
{
	//pf_free(g_blotter_chain_pHead);
	g_blotter_chain_pHead = NULL;
	g_blotter_chain_pTail = NULL;
}

/*
static void Blotter_Chain_Add(BLOTTER_CHAIN_NODE_ST* pNode)
{
	if(g_blotter_chain_pHead && pNode)
	{

	}
}
*/

static void Blotter_Chain_Del(BLOTTER_CHAIN_NODE_ST *pNode)
{
	if (g_blotter_chain_pHead)
	{
		if (pNode)
		{
			if (pNode->msg.buffer)
			{
				pf_free(pNode->msg.buffer);
			}

			pf_free(pNode);
			g_blotter_chain_node_count--;

			if (0 == g_blotter_chain_node_count)
			{
				Blotter_Chain_Destroy();
			}
		}
	}
}

static bool Blotter_Message_Save_To_Chain(blotter_messgae_st *pData)
{
	uint16_t bufferlen;
	BLOTTER_CHAIN_NODE_ST *pNode;

	// check msg valid or not
	if (pData && pData->buffer && pData->len > 0)
	{
		if (g_blotter_msg_index < BLOTTER_MSG_INDEX_MAX)
		{
			g_blotter_msg_index++;
		}
		else
		{
			g_blotter_msg_index = 1;
		}
	}
	else
	{
		goto error_para;
	}

	// create a new node
	pNode = (BLOTTER_CHAIN_NODE_ST *)pf_malloc(sizeof(BLOTTER_CHAIN_NODE_ST));

	if (pNode)
	{
		bufferlen = (pData->len > BLOTTER_RECV_LEN_MAX) ? BLOTTER_RECV_LEN_MAX : (pData->len);
		pNode->msg.buffer = (uint8_t *)pf_malloc(bufferlen);

		if (pNode->msg.buffer)
		{
			pNode->index = g_blotter_msg_index;
			pf_memcpy(pNode->msg.buffer, pData->buffer, bufferlen);

			if (pData->len > BLOTTER_RECV_LEN_MAX)
			{
				pNode->msg.buffer[BLOTTER_RECV_LEN_MAX] = '\0';
			}

			pNode->msg.len = bufferlen;
			pNode->next = NULL;
			pNode->prev = NULL;

			//TRACE_I("[Chain Add]--[%d],%s,%d.\r\n", pNode->index, pNode->msg.buffer, pNode->msg.len);
		}
		else
		{
			goto error_malloc_msg_buffer;
		}
	}
	else
	{
		goto error_malloc_node;
	}

	// add into chain
	if (NULL == g_blotter_chain_pHead)
	{
		//TRACE_I("[Chain Add]--creat chain.\r\n");
		g_blotter_chain_pHead = pNode;
		g_blotter_chain_pTail = pNode;
	}
	else
	{
		if (BLOTTER_CHAIN_NODE_MAX == g_blotter_chain_node_count) // if chain if full , delete first node
		{
		#if NEW_NODE_INSERT_HEADER
			BLOTTER_CHAIN_NODE_ST *pDelNode = g_blotter_chain_pTail;
			TRACE_I("[Chain Add]--chain is full, delete oldest node.\r\n");
			pDelNode->prev->next = NULL;
			g_blotter_chain_pTail = pDelNode->prev;
			//pDelNode->prev=NULL;  // not neccessary
			Blotter_Chain_Del(pDelNode);
		#else
			BLOTTER_CHAIN_NODE_ST *pDelNode = g_blotter_chain_pHead;
			TRACE_I("[Chain Add]--chain is full, delete oldest node.\r\n");
			pDelNode->next->prev = NULL;
			g_blotter_chain_pHead = pDelNode->next;
			//pDelNode->next=NULL;	// not neccessary
			Blotter_Chain_Del(pDelNode);
		#endif
		}

	#if NEW_NODE_INSERT_HEADER
		g_blotter_chain_pHead->prev = pNode;
		pNode->next = g_blotter_chain_pHead;
		g_blotter_chain_pHead = pNode;
	#else
		pNode->prev = g_blotter_chain_pTail;
		g_blotter_chain_pTail->next = pNode;
		g_blotter_chain_pTail = pNode;
	#endif
	}

	g_blotter_chain_node_count++;

	//TRACE_I("[Chain Add]--result:success.\r\n");
	Blotter_Chain_Print(); // test

	return true;

error_malloc_msg_buffer:
	pf_free(pNode);
error_malloc_node:
error_para:
	TRACE_I("[Chain Add]--result:failure.\r\n");
	return false;
}

void App_Blotter_Add_Event(const char *message)
{
#if defined(SIMULATE_ESP_CMD)
	// DO NOTHING
#else
	if (message)
	{
		blotter_messgae_st msg;
		msg.len = strlen(message) + 1;
		msg.buffer = (uint8_t * )message;
		App_Blotter_Event_Callback(&msg);
	}
#endif
}

/*
	save message content into chain
*/
static void App_Blotter_Event_Callback(blotter_messgae_st *pMsg)
{
	bool bSuccess = false;

	if (pMsg )
	{
		if (pMsg->buffer && pMsg->len > 0)
		{
			TRACE_I("----------------------------------------------------------------------------------.\r\n");
			TRACE_I("RECV: buff=%s, len=%d.\r\n", pMsg->buffer, pMsg->len);

			bSuccess = Blotter_Message_Save_To_Chain(pMsg);

			if (bSuccess)
			{
				int32_t ret = osSignalSet(g_TaskId_Blotter, BLOTTER_TASK_REFREASH_MASK);

				if (0x80000000 == ret)
				{
					TRACE_W("osSignalSet[g_TaskId_Blotter] failure-ret(%d).\r\n", ret);
				}
			}
		}
	}
}

static void App_Blotter_Input_Key_Callback(FRM_INPUT_KEY_ST key_info)
{
	//TRACE_F();
	//TRACE_I("11111111111111111111111111----BLOTTER.\r\n");

	if ( LIST_KEY_ACT_RELEASE == key_info.nAction)
	{
	#if 0
		g_blotter_key = key_info.nKey;
		osSignalSet(g_TaskId_Blotter, BLOTTER_TASK_KEY_MASK);
	#else
		int32_t ret = 0;

		g_blotter_key = key_info.nKey;
		ret = osSignalSet(g_TaskId_Blotter, BLOTTER_TASK_KEY_MASK);

		if (0x80000000 == ret)
		{
			TRACE_W("[TIMER_CAP1214]-ret(%d).\r\n", ret);
		}
	#endif
	}
}

static void App_Blotter_Task_Init(void)
{
	// Timer---Logo
#if defined(ENABLE_BLOTTER_LOGO)
	osTimerDef(BlotterLogoDispTimer, App_Blotter_Logo_Timer_Expired);
	g_TimerId_BlotterLogo = osTimerCreate(osTimer(BlotterLogoDispTimer), osTimerOnce, NULL);
	assert_param(g_TimerId_BlotterLogo != NULL);
	Flex_App_Draw_Blotter_Logo();
	Drv_Panel_Power(LIST_PANEL_INDEX_SUB, LIST_PANEL_ON);		// Turn On Panel
	osTimerStart(g_TimerId_BlotterLogo, 3000);
#else
	Blotter_Menu_Draw_Title();
	Drv_Panel_Power(LIST_PANEL_INDEX_SUB, LIST_PANEL_ON);		// Turn On Panel
#endif

	// Timer---ESP
#if defined(SIMULATE_ESP_CMD)
	// set event callback
	Drv_Esp_Set_Callback(App_Blotter_Event_Callback);
	// timer for simulate esp
	osTimerDef(EspTimer, Drv_Esp_Event_Occur);
	g_TimerId_EspEvent = osTimerCreate(osTimer(EspTimer), osTimerPeriodic, NULL);
	assert_param(g_TimerId_EspEvent != NULL);
	osTimerStart(g_TimerId_EspEvent, 3000);
#endif
	// Input Callback
	Frm_Input_Key_Register_Callback(App_Blotter_Input_Key_Callback);

	// Menu Init
	Blotter_Menu_Init();
}

/*========================================================================================================
										F U N C T I O N----for extern
========================================================================================================*/
void App_Blotter_Task(void const *argument)
{
	osEvent event;

	TRACE_F();

	g_TaskId_Blotter = osThreadGetId();

	App_Blotter_Task_Init();

	/* Infinite loop */
	for (;;)
	{
		event = osSignalWait(0x7FFFFFFF, osWaitForever);

		if (event.status == osEventSignal)
		{
		//---------------------------------------------------
		#if defined(ENABLE_BLOTTER_LOGO)
			if (event.value.signals & BLOTTER_TASK_LOGO_MASK)
			{
				App_Blotter_Logo_Show_Off();
			}
		#endif
		//---------------------------------------------------
			if ((event.value.signals) & BLOTTER_TASK_REFREASH_MASK)
			{
			#if defined(ENABLE_BLOTTER_LOGO)
				App_Blotter_Logo_Show_Off();
			#endif

			#if NEW_NODE_INSERT_HEADER
				Blotter_Menu_Update(g_blotter_chain_pHead);
			#else
				Blotter_Menu_Update(g_blotter_chain_pTail);
			#endif
			}
		//---------------------------------------------------
		#if defined(SIMULATE_ESP_CMD)
			if ((event.value.signals) & BLOTTER_TASK_SAVE_MSG_MASK)
			{
				static DRV_BLOTTER_EVENT_ENUM event = (DRV_BLOTTER_EVENT_ENUM)0;

				if (event >= LIST_BLOTTER_EVENT_MAX)
				{
					event = (DRV_BLOTTER_EVENT_ENUM)0;
				}

				const char *data = gBlotterEventMsg[event];
				Drv_Esp_Recv_Data_Process((const uint8_t *)data, strlen(data) + 1);
				event++;
			}
		#endif
		//---------------------------------------------------
			if ((event.value.signals) & BLOTTER_TASK_KEY_MASK)
			{
				switch (g_blotter_key)
				{
					case LIST_KEY_PAGE_UP:
						//TRACE_I("LIST_KEY_PAGE_UP.\r\n");
						Blotter_Menu_Page_Scroll(false);
						break;

					case LIST_KEY_PAGE_DOWN:
						//TRACE_I("LIST_KEY_PAGE_DOWN.\r\n");
						Blotter_Menu_Page_Scroll(true);
						break;

					default:
						break;
				}
			}
		//---------------------------------------------------
		}
	}
}


/*========================================================================================================
										End
========================================================================================================*/




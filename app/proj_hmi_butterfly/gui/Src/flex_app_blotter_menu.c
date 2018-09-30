/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_app_blotter_menu.c
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
#include "flex_app_blotter.h"
// internal
#include "flex_app_blotter_menu.h"

/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME								"[BLOTTER-MENU]--"
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
#define TEST_DRAW_TICKS		0


#define ENABLE_PAGE_SCROLL	true

/*========================================================================================================
										T Y P E D E F
========================================================================================================*/




/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
#if ENABLE_PAGE_SCROLL
static BLOTTER_CHAIN_NODE_ST* g_pPageTopNode=NULL;
static BLOTTER_CHAIN_NODE_ST* g_pPageBottomNode=NULL;
#endif




/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
void Blotter_Menu_Clear_All(void)
{
	Drv_Panel_Clear_All(LIST_PANEL_INDEX_SUB);
}

void Blotter_Menu_Clear_Block(uint8_t x, uint8_t y, uint8_t hsize, uint8_t vsize)
{
	Drv_Panel_Clear_Block(LIST_PANEL_INDEX_SUB, x, y, hsize, vsize);
}

void Blotter_Menu_Draw_Char(uint8_t x, uint8_t y, uint8_t ch)
{
	DRV_FONT_SIZE_ENUM font_size;

#if (BLOTTER_FONT_SIZE == FONT_6X8)
	font_size = LIST_FONT_6X8;
#elif (BLOTTER_FONT_SIZE == FONT_8X16)
	font_size = LIST_FONT_8X16;
#else
	font_size = LIST_FONT_6X8;
#endif
	Drv_Panel_Draw_Char(LIST_PANEL_INDEX_SUB, font_size, x, y, ch);
}

void Blotter_Menu_Draw_Str(uint8_t x, uint8_t y, const uint8_t string[])
{
	DRV_FONT_SIZE_ENUM font_size;

#if (BLOTTER_FONT_SIZE == FONT_6X8)
	font_size = LIST_FONT_6X8;
#elif (BLOTTER_FONT_SIZE == FONT_8X16)
	font_size = LIST_FONT_8X16;
#else
	font_size = LIST_FONT_6X8;
#endif
	Drv_Panel_Draw_Str(LIST_PANEL_INDEX_SUB, font_size, x, y, string);
}

void Blotter_Menu_Draw_Str_Align(ALIGN_ENUM align, uint8_t x, uint8_t y, const uint8_t string[], uint8_t width)
{
	DRV_FONT_SIZE_ENUM font_size;
	uint8_t offset=0;
	uint8_t len=strlen((char*)string);

#if (BLOTTER_FONT_SIZE == FONT_6X8)
	font_size = LIST_FONT_6X8;
#elif (BLOTTER_FONT_SIZE == FONT_8X16)
	font_size = LIST_FONT_8X16;
#else
	font_size = LIST_FONT_6X8;
#endif

	switch(align)
	{
		case ALIGN_LEFT:
			offset=0;
			break;

		case ALIGN_RIGHT:
			if(len>width)
				offset=0;
			else
				offset=width-len;
			break;

		case ALIGN_CENTER:
			if(len>width)
				offset=0;
			else
				offset=(width-len)/2;
			break;

		default:
			break;
	}

	offset=offset*BLOTTER_FONT_X_RES;

	Drv_Panel_Draw_Str(LIST_PANEL_INDEX_SUB, font_size, x+offset, y, string);
}

//================================================================
void Blotter_Menu_Init(void)
{
	// define some variables
	
}

void Blotter_Menu_Draw_Title(void)
{
	Blotter_Menu_Draw_Str_Align(ALIGN_CENTER, XPOS(0), YPOS(0), "EVENT LOG", BLOTTER_MENU_H_FONTS);
}

static void Blotter_Menu_Draw_Older_Nodes(BLOTTER_CHAIN_NODE_ST * pNode)  // draw sequence: top ---> bottom
{
	BLOTTER_CHAIN_NODE_ST* pLoop=pNode;
  char str_msg_index[12];    
	uint8_t y_line_start=0+BLOTTER_MENU_TITLE_LINE; // 0+1=1 
	uint16_t msg_len=0;  
	uint8_t msg_lines=0;  
	uint8_t dummy_lines=BLOTTER_MENU_V_FONTS-BLOTTER_MENU_TITLE_LINE;  // 8-1=7
#if TEST_DRAW_TICKS
  uint32_t tickstart;
  uint32_t tickend;
#endif
	uint8_t column;
	
	if(!pNode)
		return;

#if TEST_DRAW_TICKS
  tickstart = HAL_GetTick();
#endif

	g_pPageTopNode=pNode;
	//TRACE_I("Bottom Node: index=%d, buff:%s.\r\n", g_pPageBottomNode->index, g_pPageBottomNode->msg.buffer);

	while((pLoop!=NULL))
	{
		if((pLoop->msg.buffer)&&(pLoop->msg.len>0)) 
		{
			//TRACE_I("==================================[Draw_Older_Node]=====================================.\r\n");
			//TRACE_I("NODE: index=%d, buff=%s, len(sizeof buff)=%d.\r\n",pLoop->index, pLoop->msg.buffer, pLoop->msg.len);
		// index	
			sprintf(str_msg_index, "[%d]", pLoop->index);

		// calculate msg len		
			msg_len = strlen(str_msg_index)+ (pLoop->msg.len) -1;
			//TRACE_I("msg total len=%d(%d+%d).\r\n", msg_len, strlen(str_msg_index), ((pLoop->msg.len) -1)); // -1 means not display '\0'

		// calculate start column of blank area
			column = msg_len%BLOTTER_MENU_H_FONTS;
			//TRACE_I("start column of blank area=%d.\r\n", column);
			
		// calculate lines occupy
			msg_lines = (msg_len/BLOTTER_MENU_H_FONTS) + ((msg_len%BLOTTER_MENU_H_FONTS)? 1 : 0);
			//TRACE_I("dummy_lines=%d, msg_lines=%d.\r\n", dummy_lines, msg_lines);
			
			if(msg_lines<=dummy_lines)
			{
				//TRACE_I("Current Draw:y_line_start=%d.\r\n", y_line_start);
			// draw msg index
				Blotter_Menu_Draw_Str(XPOS(0), YPOS(y_line_start), (uint8_t*)str_msg_index);
			// draw msg buff
				Blotter_Menu_Draw_Str(XPOS(0+strlen(str_msg_index)), YPOS(y_line_start), pLoop->msg.buffer);
				y_line_start+=msg_lines;
				dummy_lines-=msg_lines;
				//clear remain columns in last string line
				if(column)
				{
					//TRACE_I("######clear column  y=%d.\r\n", y_line_start+1);
					Blotter_Menu_Clear_Block(XPOS(column),YPOS(y_line_start-1), 
							(BLOTTER_MENU_H_FONTS) * (BLOTTER_FONT_X_RES) - XPOS(column), YPOS(1));
				}
			}
			else
			{
				//TRACE_I("XXXXXXXXXXXXXXXXXXX---(msg_lines<=dummy_lines).\r\n");
				break;
			}
		}
		else
		{
			//TRACE_I("XXXXXXXXXXXXXXXXXXX---((pLoop->msg.buffer)&&(pLoop->msg.len>0)).\r\n");
		}

	#if ENABLE_PAGE_SCROLL
		g_pPageBottomNode=pLoop;
		//TRACE_I("Top Node: index=%d, buff:%s.\r\n", g_pPageTopNode->index, g_pPageTopNode->msg.buffer);
	#endif

	#if NEW_NODE_INSERT_HEADER
		pLoop=pLoop->next;
	#else
		pLoop=pLoop->prev;
	#endif

		if(NULL==pLoop)
		{
			//TRACE_I("XXXXXXXXXXXXXXXXXXX---(NULL==pLoop).\r\n");
		}
	}

	//clear remaining lines
	if(dummy_lines)
	{
		Blotter_Menu_Clear_Block(XPOS(0),YPOS(y_line_start), BLOTTER_PANAL_WIDTH, (BLOTTER_PANAL_HEIGHT-YPOS(y_line_start)));
	}
	
#if TEST_DRAW_TICKS
  tickend = HAL_GetTick();
	DBG_MSG("blotter delta=%d.\r\n", tickend-tickstart);   
#endif
}

static void Blotter_Menu_Draw_Newer_Nodes(BLOTTER_CHAIN_NODE_ST * pNode)  // draw sequence: bottom ---> top 
{
	BLOTTER_CHAIN_NODE_ST* pLoop=pNode;
	char str_msg_index[12]; 	 
	uint8_t y_line_start=BLOTTER_MENU_V_FONTS;  // 8
	uint16_t msg_len=0;  
	uint8_t msg_lines=0;	
	uint8_t dummy_lines=BLOTTER_MENU_V_FONTS-BLOTTER_MENU_TITLE_LINE;  // 8-1=7
#if TEST_DRAW_TICKS
	uint32_t tickstart;
	uint32_t tickend;
#endif
	uint8_t column;

	if(!pNode)
		return;

#if TEST_DRAW_TICKS
	tickstart = HAL_GetTick();
#endif

	g_pPageBottomNode=pNode; // para node is bottom node in menu
	//TRACE_I("Bottom Node: index=%d, buff:%s.\r\n", g_pPageBottomNode->index, g_pPageBottomNode->msg.buffer);

	while((pLoop!=NULL))
	{
		if((pLoop->msg.buffer)&&(pLoop->msg.len>0)) 
		{
			//TRACE_I("==================================[Draw_Older_Node]=====================================.\r\n");
			//TRACE_I("NODE: index=%d, buff=%s, len(sizeof buff)=%d.\r\n",pLoop->index, pLoop->msg.buffer, pLoop->msg.len);
		// index	
			sprintf(str_msg_index, "[%d]", pLoop->index);

		// calculate msg len		
			msg_len = strlen(str_msg_index)+ (pLoop->msg.len) -1;
			//TRACE_I("msg total len=%d(%d+%d).\r\n", msg_len, strlen(str_msg_index), ((pLoop->msg.len) -1)); // -1 means not display '\0'

		// calculate start column of blank area
			column = msg_len%BLOTTER_MENU_H_FONTS;
			//TRACE_I("start column of blank area=%d.\r\n", column);
			
		// calculate lines occupy
			msg_lines = (msg_len/BLOTTER_MENU_H_FONTS) + ((msg_len%BLOTTER_MENU_H_FONTS)? 1 : 0);
			//TRACE_I("dummy_lines=%d, msg_lines=%d.\r\n", dummy_lines, msg_lines);
			
			if(msg_lines<=dummy_lines)
			{
			// draw msg index
				y_line_start-=msg_lines; // eg: lines=2, ypos=8-2=6, lines=1, ypos=8-1=7
				//TRACE_I("Current Draw:y_line_start=%d.\r\n", y_line_start);
				Blotter_Menu_Draw_Str(XPOS(0), YPOS(y_line_start), (uint8_t*)str_msg_index);
			// draw msg buff
				Blotter_Menu_Draw_Str(XPOS(0+strlen(str_msg_index)), YPOS(y_line_start), pLoop->msg.buffer);
				dummy_lines-=msg_lines;
				//clear remain columns in current string line
				if(column)
				{
					//TRACE_I("######clear column  y=%d.\r\n", y_line_start+msg_lines-1);
					Blotter_Menu_Clear_Block(XPOS(column),YPOS(y_line_start+msg_lines-1), 
							(BLOTTER_MENU_H_FONTS) * (BLOTTER_FONT_X_RES) - XPOS(column), YPOS(1));
				}
			}
			else
			{
				//TRACE_I("XXXXXXXXXXXXXXXXXXX---(msg_lines<=dummy_lines).\r\n");
				break;
			}
		}
		else
		{
			//TRACE_I("XXXXXXXXXXXXXXXXXXX---((pLoop->msg.buffer)&&(pLoop->msg.len>0)).\r\n");
		}

#if ENABLE_PAGE_SCROLL
		g_pPageTopNode=pLoop;
		//TRACE_I("Top Node: index=%d, buff:%s.\r\n", g_pPageTopNode->index, g_pPageTopNode->msg.buffer);
#endif

#if NEW_NODE_INSERT_HEADER
		pLoop=pLoop->prev;
#else
		pLoop=pLoop->next;
#endif

		if(NULL==pLoop)
		{
			//TRACE_I("XXXXXXXXXXXXXXXXXXX---(NULL==pLoop).\r\n");
		}
	}

	//clear remaining lines
	if(dummy_lines)
	{
		Blotter_Menu_Clear_Block(XPOS(0),YPOS(BLOTTER_MENU_TITLE_LINE), BLOTTER_PANAL_WIDTH, (BLOTTER_PANAL_HEIGHT-YPOS(dummy_lines)));
	}
	
#if TEST_DRAW_TICKS
	tickend = HAL_GetTick();
	DBG_MSG("blotter delta=%d.\r\n", tickend-tickstart);	 
#endif
}


void Blotter_Menu_Update(BLOTTER_CHAIN_NODE_ST * pNode) // draw menu: pNode is the newest one
{
	Blotter_Menu_Draw_Older_Nodes(pNode); 
}

void Blotter_Menu_Page_Scroll(bool page_down)
{
#if ENABLE_PAGE_SCROLL
	if(page_down)
	{
		// find newer than Top Node
		#if NEW_NODE_INSERT_HEADER
			if(g_pPageTopNode->prev !=NULL)
			{
				Blotter_Menu_Draw_Newer_Nodes(g_pPageTopNode->prev);
			}
		#else
			if(g_pPageTopNode->next!=NULL)
			{
				Blotter_Menu_Draw_Newer_Nodes(g_pPageTopNode->next);
			}
		#endif
	}
	else
	{
		// find older than Bottom Node
	#if NEW_NODE_INSERT_HEADER
		if(g_pPageBottomNode->next !=NULL)
		{
			Blotter_Menu_Draw_Older_Nodes(g_pPageBottomNode->next);
		}
	#else
		if(g_pPageBottomNode->prev!=NULL)
		{
			Blotter_Menu_Draw_Older_Nodes(g_pPageBottomNode->prev);
		}
	#endif
	}
#endif
}

/*========================================================================================================
										End
========================================================================================================*/




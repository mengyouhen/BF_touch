/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_app_osd_draw.c
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


// internal
#include "flex_app_osd_draw.h"







/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME								"[OSD DRAW]--"
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
static uint8_t const frame_left_up[]= 	// 6x8
{
	0x00,0x00,0xF0,0xF8,0x18,0x18
};

static uint8_t const frame_right_up[]= 	// 6x8
{
	0x18,0x18,0xF8,0xF0,0x00,0x00
};

static uint8_t const frame_left_down[]= 	// 6x8
{
	0x00,0x00,0x0F,0x1F,0x18,0x18
};

static uint8_t const frame_right_down[]= 	// 6x8
{
	0x18,0x18,0x1F,0x0F,0x00,0x00
};

static uint8_t const frame_left[]= 	// 6x8
{
	0x00,0x00,0xFF,0xFF,0x00,0x00
};

static uint8_t const frame_right[]= 	// 6x8
{
	0x00,0x00,0xFF,0xFF,0x00,0x00
};

static uint8_t const frame_top[]= 	// 6x8
{
	0x18,0x18,0x18,0x18,0x18,0x18
};

static uint8_t const frame_bottom[]= 	// 6x8
{
	0x18,0x18,0x18,0x18,0x18,0x18
};

/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
//===============================================================
// COLOR
//===============================================================
void Osd_Set_Color(uint8_t color)
{
	DRV_PANEL_COLOR_ENUM fore_color; 
	DRV_PANEL_COLOR_ENUM back_color; 
	
	switch((color>>CT_MASK_SHIFT)&CT_MASK)
	{
		case LIST_CP_BLACK:
			fore_color = LIST_PANEL_COLOR_BLACK;
			//TRACE_I("fore_color = BLACK \r\n");
			break;
			
		case LIST_CP_WHITE:
			fore_color = LIST_PANEL_COLOR_WHITE;
			//TRACE_I("fore_color = WHITE \r\n");
			break;
			
		case LIST_CP_LIGHTGRAY: 	
			fore_color = LIST_PANEL_COLOR_LIGHTGRAY;
			//TRACE_I("fore_color = LIGHTGRAY \r\n");
			break;
			
		case LIST_CP_DARKGRAY:
			fore_color = LIST_PANEL_COLOR_DARKGRAY;
			//TRACE_I("fore_color = DARKGRAY \r\n");
			break;
			
		default:
			fore_color = LIST_PANEL_COLOR_BLACK;
			//TRACE_I("fore_color = BLACK \r\n");
			break;
	}

	switch(color&CT_MASK)
	{
		case LIST_CP_BLACK:
			back_color = LIST_PANEL_COLOR_BLACK;
			//TRACE_I("back_color = BLACK \r\n");
			break;
			
		case LIST_CP_WHITE:
			back_color = LIST_PANEL_COLOR_WHITE;
			//TRACE_I("back_color = WHITE \r\n");
			break;
			
		case LIST_CP_LIGHTGRAY: 	
			back_color = LIST_PANEL_COLOR_LIGHTGRAY;
			//TRACE_I("back_color = LIGHTGRAY \r\n");
			break;
			
		case LIST_CP_DARKGRAY:
			back_color = LIST_PANEL_COLOR_DARKGRAY;
			//TRACE_I("back_color = DARKGRAY \r\n");
			break;
			
		default:
			back_color = LIST_PANEL_COLOR_WHITE;
			//TRACE_I("back_color = WHITE \r\n");
			break;
	}

	Drv_Panel_Set_Color(LIST_PANEL_INDEX_MAIN, fore_color);
	Drv_Panel_Set_BkColor(LIST_PANEL_INDEX_MAIN, back_color);
}

//===============================================================
// CLEAR
//===============================================================
void Osd_Clear_All(void)
{
	Drv_Panel_Clear_All(LIST_PANEL_INDEX_MAIN);
}

void Osd_Clear_Block(uint8_t x, uint8_t y, uint8_t hsize, uint8_t vsize)
{
	//TRACE_I("x=%d, y=%d, hsize=%d, vsize=%d.\r\n", x, y, hsize, vsize);
	Drv_Panel_Clear_Block(LIST_PANEL_INDEX_MAIN, x, y, hsize, vsize);
}

//===============================================================
// DRAW STRING
//===============================================================
void Osd_Draw_Char(uint8_t x, uint8_t y, const uint8_t ch)
{
	DRV_FONT_SIZE_ENUM font_size;

#if (OSD_FONT_SIZE == FONT_6X8)
	font_size = LIST_FONT_6X8;
#elif (OSD_FONT_SIZE == FONT_8X16)
	font_size = LIST_FONT_8X16;
#else
	font_size = LIST_FONT_6X8;
#endif
	Drv_Panel_Draw_Char(LIST_PANEL_INDEX_MAIN, font_size, x, y, ch);
}

void Osd_Draw_String(ALIGN_ENUM align, uint8_t x, uint8_t y, const uint8_t string[], uint8_t width)
{
	DRV_FONT_SIZE_ENUM font_size;
	uint8_t len=strlen((char*)string);
	uint8_t offset=0;

#if (OSD_FONT_SIZE == FONT_6X8)
	font_size = LIST_FONT_6X8;
#elif (OSD_FONT_SIZE == FONT_8X16)
	font_size = LIST_FONT_8X16;
#else
	font_size = LIST_FONT_6X8;
#endif

	if(len>width)
	{
		uint8_t* p=pf_malloc(width);
		pf_memcpy(p, string, width);
		p[width]='\0';
		Drv_Panel_Draw_Str(LIST_PANEL_INDEX_MAIN, font_size, x, y, p);
		free(p);
	}
	else
	{
		switch(align)
		{
			case ALIGN_IGNORE:
			case ALIGN_LEFT:
				offset=0;
				break;

			case ALIGN_CENTER:
				offset=(width-len)/2;
				break;


			case ALIGN_RIGHT:
				offset=width-len;
				break;

			default:
				break;
				
		}
		Drv_Panel_Draw_Str(LIST_PANEL_INDEX_MAIN, font_size, x+_X_(offset), y, string);
	}
}

void Osd_Draw_Icon(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t data[])
{
	Drv_Panel_Draw_Image(LIST_PANEL_INDEX_MAIN, x, y, _X_(width), _Y_(height), data);
}


void Osd_Draw_Frame(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	if(width<2 || height<2)
	{
		return;
	}
	else
	{
	#if 1 // font by font
		uint8_t index;
	
	// draw up line
		Drv_Panel_Draw_Image(LIST_PANEL_INDEX_MAIN, x, y, _X_(1), _Y_(1), frame_left_up);
		for(index=0; index<width-2; index++)
		{
		Drv_Panel_Draw_Image(LIST_PANEL_INDEX_MAIN, x+_X_(1+index), y, _X_(1), _Y_(1), frame_top);
		}
		Drv_Panel_Draw_Image(LIST_PANEL_INDEX_MAIN, x+_X_(width-1), y, _X_(1), _Y_(1), frame_right_up);
		
	// draw left
		for(index=0; index<height-2; index++)
		{
		Drv_Panel_Draw_Image(LIST_PANEL_INDEX_MAIN, x, y+_Y_(1+index), _X_(1), _Y_(1), frame_left);
		}

	// draw right
		for(index=0; index<height-2; index++)
		{
		Drv_Panel_Draw_Image(LIST_PANEL_INDEX_MAIN, x+_X_(width-1), y+_Y_(1+index), _X_(1), _Y_(1), frame_right);
		}
	// draw bottom line
		Drv_Panel_Draw_Image(LIST_PANEL_INDEX_MAIN, x, y+_Y_(height-1), _X_(1), _Y_(1), frame_left_down);
		for(index=0; index<width-2; index++)
		{
		Drv_Panel_Draw_Image(LIST_PANEL_INDEX_MAIN, x+_X_(1+index), y+_Y_(height-1), _X_(1), _Y_(1), frame_bottom);
		}
		Drv_Panel_Draw_Image(LIST_PANEL_INDEX_MAIN, x+_X_(width-1), y+_Y_(height-1), _X_(1), _Y_(1), frame_right_down);

	#else // block
		uint8_t* pData_up=pf_malloc(_X_(width-2));
		uint8_t* pData_down=pf_malloc(_X_(width-2));
		uint8_t* pData_left=pf_malloc(_X_(height-2));
		uint8_t* pData_right=pf_malloc(_X_(height-2));
	// draw left up	
		Drv_Panel_Draw_Image(LIST_PANEL_INDEX_MAIN, x, y, _X_(1), _Y_(1), frame_left_up);
	// draw up
		
	
	// draw right up	
		Drv_Panel_Draw_Image(LIST_PANEL_INDEX_MAIN, x+_X_(width-1), y, _X_(1), _Y_(1), frame_left_up);
	// draw left
	
	// draw left dlown	
		Drv_Panel_Draw_Image(LIST_PANEL_INDEX_MAIN, x, y+_Y_(height-1), _X_(1), _Y_(1), frame_left_down);
	// draw right down	
		Drv_Panel_Draw_Image(LIST_PANEL_INDEX_MAIN, x+_X_(width-1), y+_Y_(height-1), _X_(1), _Y_(1), frame_left_up);
	#endif
	}
}

/*========================================================================================================
										End
========================================================================================================*/




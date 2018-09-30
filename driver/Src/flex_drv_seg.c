/******************************************************************************
 Copyright (c) 2018 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		flex_drv_seg.c
 [Date]:
 		2018-05-02
 [Author]:
 		joanna.wang
 [Reversion History]:
 		v1.0
*******************************************************************************/

/*========================================================================================================
										I N C L U D E
========================================================================================================*/
// external
#include "lcd.h"

// internal
#include "flex_drv_seg.h"


/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME								"[SEG]--"
#endif

#if defined(__FLEX_TRACE_ENABLE__)
#define TRACE_F(fmt, arg...)				DBG_MSG(MOD_NAME "[F]-%s.\r\n", __func__)
#define TRACE_I(fmt, arg...)				DBG_MSG(MOD_NAME "[I]-" fmt, ##arg)
#define TRACE_W(fmt, arg...)				DBG_MSG(MOD_NAME "[W]-" fmt, ##arg)
#else
#define TRACE_F()
#define TRACE_I(fmt, arg...)
#define TRACE_W(fmt, arg...)
#endif
#define TRACE_E(fmt, arg...)				DBG_MSG(MOD_NAME "[E]-" fmt, ##arg)

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
	char	character;
	struct
	{
		uint8_t Mask;//bitmap:1-clear,0-kept
		uint8_t Data;
	} com[4];
} GDC04212_SEG_DATA;

typedef struct
{
	GPIO_TypeDef *nPort;
	uint16_t nPin;
	GPIO_PinState sState;
} SEG_GPIO_DEF;


typedef struct
{
	char	character;
	//A,B,C,D,E,F,G,Dx segment
	SEG_GPIO_DEF element[8];
} LED_SEG_DATA;


/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
/*
	GDC04212 LCD segment mapping:
	--------------------
	  -----A-----
	  | 		|
	  F 		B
	  | 		|
	  -----G-----
	  | 		|
	  E 		C
	  | 		|
	  ------D------P |_|

	GDC04212(used):
	COM1: 1F  1A   2F	2A	 3F  3A   4F  4A
	COM2: 1G  1B   2G	2B	 3G  3B   4G  4B
	COM3: 1E  1C   2E	2C	 3E  3C   4E  4C
	COM4: P1  1D   P2	2D	 P3  3D   P4  4D
*/

const GDC04212_SEG_DATA GDC04212SegTable[] =
{
	{ '0', {0x3, 0x3, 0x3, 0x2, 0x3, 0x3, 0x2, 0x2}},
	{ '1', {0x3, 0x0, 0x3, 0x2, 0x3, 0x2, 0x2, 0x0}},
	{ '2', {0x3, 0x2, 0x3, 0x3, 0x3, 0x1, 0x2, 0x2}},
	{ '3', {0x3, 0x2, 0x3, 0x3, 0x3, 0x2, 0x2, 0x2}},
	{ '4', {0x3, 0x1, 0x3, 0x3, 0x3, 0x2, 0x2, 0x0}},
	{ '5', {0x3, 0x3, 0x3, 0x1, 0x3, 0x2, 0x2, 0x2}},
	{ '6', {0x3, 0x3, 0x3, 0x1, 0x3, 0x3, 0x2, 0x2}},
	{ '7', {0x3, 0x2, 0x3, 0x2, 0x3, 0x2, 0x2, 0x0}},
	{ '8', {0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x2, 0x2}},
	{ '9', {0x3, 0x3, 0x3, 0x3, 0x3, 0x2, 0x2, 0x2}},
	{ 'F', {0x3, 0x3, 0x3, 0x1, 0x3, 0x1, 0x2, 0x0}},
	{ '.', {0x3, 0x0, 0x3, 0x0, 0x3, 0x0, 0x1, 0x1}},
	//{ ':', {0x3, 0x0, 0x3, 0x0, 0x3, 0x0, 0x1, 0x1}},
	//NULL as last one - turn off current 8-segments
	{ NULL, {0x3, 0x0, 0x3, 0x0, 0x3, 0x0, 0x3, 0x0}},
};

//AOP C404LB B/W
const LED_SEG_DATA AopC404lbTable[] =
{
	{
		'0',
		{
			MCU_LED_SEGA_GPIO_Port, MCU_LED_SEGA_Pin, GPIO_PIN_SET,
			MCU_LED_SEGB_GPIO_Port, MCU_LED_SEGB_Pin, GPIO_PIN_SET,
			MCU_LED_SEGC_GPIO_Port, MCU_LED_SEGC_Pin, GPIO_PIN_SET,
			MCU_LED_SEGD_GPIO_Port, MCU_LED_SEGD_Pin, GPIO_PIN_SET,
			MCU_LED_SEGE_GPIO_Port, MCU_LED_SEGE_Pin, GPIO_PIN_SET,
			MCU_LED_SEGF_GPIO_Port, MCU_LED_SEGF_Pin, GPIO_PIN_SET,
			MCU_LED_SEGG_GPIO_Port, MCU_LED_SEGG_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGDX_GPIO_Port, MCU_LED_SEGDX_Pin, GPIO_PIN_RESET
		},
	},
	{
		'1',
		{
			MCU_LED_SEGA_GPIO_Port, MCU_LED_SEGA_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGB_GPIO_Port, MCU_LED_SEGB_Pin, GPIO_PIN_SET,
			MCU_LED_SEGC_GPIO_Port, MCU_LED_SEGC_Pin, GPIO_PIN_SET,
			MCU_LED_SEGD_GPIO_Port, MCU_LED_SEGD_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGE_GPIO_Port, MCU_LED_SEGE_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGF_GPIO_Port, MCU_LED_SEGF_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGG_GPIO_Port, MCU_LED_SEGG_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGDX_GPIO_Port, MCU_LED_SEGDX_Pin, GPIO_PIN_RESET
		},
	},
	{
		'2',
		{
			MCU_LED_SEGA_GPIO_Port, MCU_LED_SEGA_Pin, GPIO_PIN_SET,
			MCU_LED_SEGB_GPIO_Port, MCU_LED_SEGB_Pin, GPIO_PIN_SET,
			MCU_LED_SEGC_GPIO_Port, MCU_LED_SEGC_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGD_GPIO_Port, MCU_LED_SEGD_Pin, GPIO_PIN_SET,
			MCU_LED_SEGE_GPIO_Port, MCU_LED_SEGE_Pin, GPIO_PIN_SET,
			MCU_LED_SEGF_GPIO_Port, MCU_LED_SEGF_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGG_GPIO_Port, MCU_LED_SEGG_Pin, GPIO_PIN_SET,
			MCU_LED_SEGDX_GPIO_Port, MCU_LED_SEGDX_Pin, GPIO_PIN_RESET
		},
	},
	{
		'3',
		{
			MCU_LED_SEGA_GPIO_Port, MCU_LED_SEGA_Pin, GPIO_PIN_SET,
			MCU_LED_SEGB_GPIO_Port, MCU_LED_SEGB_Pin, GPIO_PIN_SET,
			MCU_LED_SEGC_GPIO_Port, MCU_LED_SEGC_Pin, GPIO_PIN_SET,
			MCU_LED_SEGD_GPIO_Port, MCU_LED_SEGD_Pin, GPIO_PIN_SET,
			MCU_LED_SEGE_GPIO_Port, MCU_LED_SEGE_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGF_GPIO_Port, MCU_LED_SEGF_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGG_GPIO_Port, MCU_LED_SEGG_Pin, GPIO_PIN_SET,
			MCU_LED_SEGDX_GPIO_Port, MCU_LED_SEGDX_Pin, GPIO_PIN_RESET
		},
	},
	{
		'4',
		{
			MCU_LED_SEGA_GPIO_Port, MCU_LED_SEGA_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGB_GPIO_Port, MCU_LED_SEGB_Pin, GPIO_PIN_SET,
			MCU_LED_SEGC_GPIO_Port, MCU_LED_SEGC_Pin, GPIO_PIN_SET,
			MCU_LED_SEGD_GPIO_Port, MCU_LED_SEGD_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGE_GPIO_Port, MCU_LED_SEGE_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGF_GPIO_Port, MCU_LED_SEGF_Pin, GPIO_PIN_SET,
			MCU_LED_SEGG_GPIO_Port, MCU_LED_SEGG_Pin, GPIO_PIN_SET,
			MCU_LED_SEGDX_GPIO_Port, MCU_LED_SEGDX_Pin, GPIO_PIN_RESET
		},
	},
	{
		'5',
		{
			MCU_LED_SEGA_GPIO_Port, MCU_LED_SEGA_Pin, GPIO_PIN_SET,
			MCU_LED_SEGB_GPIO_Port, MCU_LED_SEGB_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGC_GPIO_Port, MCU_LED_SEGC_Pin, GPIO_PIN_SET,
			MCU_LED_SEGD_GPIO_Port, MCU_LED_SEGD_Pin, GPIO_PIN_SET,
			MCU_LED_SEGE_GPIO_Port, MCU_LED_SEGE_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGF_GPIO_Port, MCU_LED_SEGF_Pin, GPIO_PIN_SET,
			MCU_LED_SEGG_GPIO_Port, MCU_LED_SEGG_Pin, GPIO_PIN_SET,
			MCU_LED_SEGDX_GPIO_Port, MCU_LED_SEGDX_Pin, GPIO_PIN_RESET
		},
	},
	{
		'6',
		{
			MCU_LED_SEGA_GPIO_Port, MCU_LED_SEGA_Pin, GPIO_PIN_SET,
			MCU_LED_SEGB_GPIO_Port, MCU_LED_SEGB_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGC_GPIO_Port, MCU_LED_SEGC_Pin, GPIO_PIN_SET,
			MCU_LED_SEGD_GPIO_Port, MCU_LED_SEGD_Pin, GPIO_PIN_SET,
			MCU_LED_SEGE_GPIO_Port, MCU_LED_SEGE_Pin, GPIO_PIN_SET,
			MCU_LED_SEGF_GPIO_Port, MCU_LED_SEGF_Pin, GPIO_PIN_SET,
			MCU_LED_SEGG_GPIO_Port, MCU_LED_SEGG_Pin, GPIO_PIN_SET,
			MCU_LED_SEGDX_GPIO_Port, MCU_LED_SEGDX_Pin, GPIO_PIN_RESET
		},
	},
	{
		'7',
		{
			MCU_LED_SEGA_GPIO_Port, MCU_LED_SEGA_Pin, GPIO_PIN_SET,
			MCU_LED_SEGB_GPIO_Port, MCU_LED_SEGB_Pin, GPIO_PIN_SET,
			MCU_LED_SEGC_GPIO_Port, MCU_LED_SEGC_Pin, GPIO_PIN_SET,
			MCU_LED_SEGD_GPIO_Port, MCU_LED_SEGD_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGE_GPIO_Port, MCU_LED_SEGE_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGF_GPIO_Port, MCU_LED_SEGF_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGG_GPIO_Port, MCU_LED_SEGG_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGDX_GPIO_Port, MCU_LED_SEGDX_Pin, GPIO_PIN_RESET
		},
	},
	{
		'8',
		{
			MCU_LED_SEGA_GPIO_Port, MCU_LED_SEGA_Pin, GPIO_PIN_SET,
			MCU_LED_SEGB_GPIO_Port, MCU_LED_SEGB_Pin, GPIO_PIN_SET,
			MCU_LED_SEGC_GPIO_Port, MCU_LED_SEGC_Pin, GPIO_PIN_SET,
			MCU_LED_SEGD_GPIO_Port, MCU_LED_SEGD_Pin, GPIO_PIN_SET,
			MCU_LED_SEGE_GPIO_Port, MCU_LED_SEGE_Pin, GPIO_PIN_SET,
			MCU_LED_SEGF_GPIO_Port, MCU_LED_SEGF_Pin, GPIO_PIN_SET,
			MCU_LED_SEGG_GPIO_Port, MCU_LED_SEGG_Pin, GPIO_PIN_SET,
			MCU_LED_SEGDX_GPIO_Port, MCU_LED_SEGDX_Pin, GPIO_PIN_RESET
		},
	},
	{
		'9',
		{
			MCU_LED_SEGA_GPIO_Port, MCU_LED_SEGA_Pin, GPIO_PIN_SET,
			MCU_LED_SEGB_GPIO_Port, MCU_LED_SEGB_Pin, GPIO_PIN_SET,
			MCU_LED_SEGC_GPIO_Port, MCU_LED_SEGC_Pin, GPIO_PIN_SET,
			MCU_LED_SEGD_GPIO_Port, MCU_LED_SEGD_Pin, GPIO_PIN_SET,
			MCU_LED_SEGE_GPIO_Port, MCU_LED_SEGE_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGF_GPIO_Port, MCU_LED_SEGF_Pin, GPIO_PIN_SET,
			MCU_LED_SEGG_GPIO_Port, MCU_LED_SEGG_Pin, GPIO_PIN_SET,
			MCU_LED_SEGDX_GPIO_Port, MCU_LED_SEGDX_Pin, GPIO_PIN_RESET
		},
	},
	{
		'.',
		{
			MCU_LED_SEGA_GPIO_Port, MCU_LED_SEGA_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGB_GPIO_Port, MCU_LED_SEGB_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGC_GPIO_Port, MCU_LED_SEGC_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGD_GPIO_Port, MCU_LED_SEGD_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGE_GPIO_Port, MCU_LED_SEGE_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGF_GPIO_Port, MCU_LED_SEGF_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGG_GPIO_Port, MCU_LED_SEGG_Pin, GPIO_PIN_RESET,
			MCU_LED_SEGDX_GPIO_Port, MCU_LED_SEGDX_Pin, GPIO_PIN_SET
		},
	},
};

const uint16_t ledCOM[] = {MCU_LED_COM0_Pin, MCU_LED_COM1_Pin, MCU_LED_COM2_Pin, MCU_LED_COM3_Pin};


/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
static void LCD_UpdateDisplayRequest(bool block);


/*!
*\brief clear (turn off) all Led segments display.
*\retval none
*/
void ClearLedSegmentsDisplay(void)
{
	HAL_GPIO_WritePin(MCU_LED_COM0_GPIO_Port, MCU_LED_COM0_Pin | MCU_LED_COM1_Pin | MCU_LED_COM2_Pin | MCU_LED_COM3_Pin, GPIO_PIN_SET);
}

/*!
*\brief display one character in specific position if character is supported,otherwise off
*\param [in] position - segment position from left to right.
*\param [in] character - character to be displayed.
*
*\retval none
*/
void LedSegmentDisplay(SEGMENT_POS position, char character)
{
	if (position < SEG_POS_MAX)
	{
		ClearLedSegmentsDisplay();

		for (uint8_t i = 0; i < sizeof(AopC404lbTable) / sizeof(LED_SEG_DATA); i++)
		{
			if (character == AopC404lbTable[i].character)
			{
				for (uint8_t j = 0; j < 8; j++)
				{
					//set segment line voltage level
					HAL_GPIO_WritePin(AopC404lbTable[i].element[j].nPort, AopC404lbTable[i].element[j].nPin, AopC404lbTable[i].element[j].sState);
				}

				//turn on specfic COM (Segment)
				HAL_GPIO_WritePin(MCU_LED_COM0_GPIO_Port, ledCOM[position], GPIO_PIN_RESET);
				break;
			}
		}
	}
}


/*!
*\brief Turn on or off backlight of LCD segment display.
*\param [in] enable - true to turn on backlight, otherwise turn off.
*
*\retval none
*/
void EnableLcdBacklight(bool enable)
{
	if (enable)
	{
		HAL_GPIO_WritePin(MCU_LCD_BKLT_GPIO_Port, MCU_LCD_BKLT_Pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(MCU_LCD_BKLT_GPIO_Port, MCU_LCD_BKLT_Pin, GPIO_PIN_RESET);
	}
}

/*!
*\brief Request LCD to flush its content from LCD_RAM.
*\param [in] wud -true to wait till LCD request is done, false to set request without waiting it is done.
*
*\retval none
*/
static void LCD_UpdateDisplayRequest(bool wud)
{
	if (wud)
	{
		//wait till update request done
		HAL_LCD_UpdateDisplayRequest(&hlcd);
	}
	else
	{
		//HAL_LCD_UpdateDisplayRequest() is implemented as blocking way to wait UDR clear (10+ms) after UDR is set.
		//This function is implemented as unblocking way without waiting UDR clear after set .
		//Causion here is application code need to call LcdSegmentReadyToUpdate() to determine
		//if Lcd display is ready to update or not before lcd segment display writes as LCD RAM is write protected
		//while UDR is set.

		/* Clear the Update Display Done flag before starting the update display request */
		__HAL_LCD_CLEAR_FLAG(&hlcd, LCD_FLAG_UDD);

		/* Enable the display request */
		(&hlcd)->Instance->SR |= LCD_SR_UDR;
		(&hlcd)->State = HAL_LCD_STATE_READY;

		/* Process Unlocked */
		__HAL_UNLOCK(&hlcd);
	}

}


/*!
*\brief clear (turn off) all segments RAM and flush to display if flush is set true.
*\param [in] flush - true to flush RAM to display device immedidate, false to clear LCD RAM only.
*\retval none
*/
void ClearLcdSegmentsDisplay(bool flush)
{
	//clear all contents from COM0~COM4
	for (uint8_t i = 0; i < 4; i++)
	{
		HAL_LCD_Write(&hlcd, 2 * i, 0xFFFFFF00, 0x0);
	}

	//HAL_LCD_UpdateDisplayRequest() is not used here as it is implemented as blocking way to
	//wait UDR clear (10+ms) after UDR is set. Instead use LCD_UpdateDisplayRequest(false) without waiting
	//UDR clear. Causion here is application code need to call LcdSegmentReadyToUpdate() to determine
	//if Lcd display is ready to update or not before lcd segment display writes as LCD RAM is write protected
	//while UDR is set.
	if (flush)
	{
		LCD_UpdateDisplayRequest(false);
	}
}

/*!
*\brief display one character in specific position if character is supported,otherwise no change in the position.
*\param [in] position - segment position from left to right.
*\param [in] character - character to be displayed.
*\param [in] flush - true to flush RAM to display device immedidate, false to clear LCD RAM only.
*
*\retval none
*/
void LcdSegmentDisplay(SEGMENT_POS position, char character, bool flush)
{
	uint8_t i;

	if (position < SEG_POS_MAX)
	{
		for (i = 0; i < sizeof(GDC04212SegTable) / sizeof(GDC04212_SEG_DATA); i++)
		{
			if (GDC04212SegTable[i].character == character)
			{
				for (uint8_t j = 0; j < 4; j++)
				{
					HAL_LCD_Write(&hlcd, 2 * j, ~(GDC04212SegTable[i].com[j].Mask << (2 * position)),
								  GDC04212SegTable[i].com[j].Data << (2 * position));
				}

				//HAL_LCD_UpdateDisplayRequest() is not used here as it is implemented as blocking way to
				//wait UDR clear (10+ms) after UDR is set. Instead use LCD_UpdateDisplayRequest(false) without waiting
				//UDR clear. Causion here is application code need to call LcdSegmentReadyToUpdate() to determine
				//if Lcd display is ready to update or not before lcd segment display writes as LCD RAM is write protected
				//while UDR is set.
				if (flush)
				{
					LCD_UpdateDisplayRequest(false);
				}

				break;
			}
		}
	}
}

/*!
*\brief check if lcd segment is ready to update content.
*\retval true - yes, it's ready to update.
*\retval false - no, busy.
*/
bool LcdSegmentReadyToUpdate(void)
{
	return (__HAL_LCD_GET_FLAG(&hlcd, LCD_FLAG_UDR) == RESET);
}

/*========================================================================================================
										End
========================================================================================================*/

/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		flex_drv_panel_oled_sh1106_i2c.c
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
#include <string.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_conf.h"
#if defined(HAL_I2C_MODULE_ENABLED)
#include "i2c.h"
#endif
#include "freertos.h"
#include "flex_drv_font.h"
#include "flex_drv_panel_def.h"


// internal
#include "flex_drv_panel_oled_sh1106_i2c.h"


/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME								"[SH1106]--"
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
#define SH1106_I2C_INDEX		hi2c2
#define SH1106_I2C_ADDR			0x78  // 7bit
#define SH1106_I2C_TIMEOUT 		50
//---------------------------
#define SH1106_HTOTAL			132
#define SH1106_VTOTAL			64
#define SH1106_HSIZE				128
#define SH1106_VSIZE				64

#define SH1106_PAGES			8 // SH1106_VTOTAL/8


/*========================================================================================================
										T Y P E D E F
========================================================================================================*/








/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
static const char sh1106Name[] = "OLED_SH1106_128X64";

static const uint8_t sh1106_init_data[] =
{
	0xAE,   /*display off*/
	0x02,    /*set lower column address*/
	0x10,    /*set higher column address*/
	0x40,    /*set display start line*/
	0xB0,    /*set page address*/
	0x81,    /*contract control*/
	0x80,    /*128*/
	0xA1,    /*set segment remap*/
	0xA6,    /*normal / reverse*/
	0xA8,    /*multiplex ratio*/
	0x3F,    /*duty = 1/32*/
	0xad,    /*set charge pump enable*/
	0x8b,     /*    0x8a    ??VCC   */
	0x30,    /*0X30---0X33  set VPP   9V liangdu!!!!*/
	0xC8,    /*Com scan direction*/
	0xD3,    /*set display offset*/
	0x00,   /*   0x20  */
	0xD5,    /*set osc division*/
	0x80,
	0xD9,    /*set pre-charge period*/
	0x1f,    /*0x22*/
	0xDA,    /*set COM pins*/
#if 1 //carson 2018-01-02
	0x12,	//0x12:逐行显示
#else
	0x02,	//0x02:隔行显示
#endif
	0xdb,    /*set vcomh*/
	0x40,
	//	0xAF,    /*display ON*/
};

static DRV_PANEL_INFO_ST g_st_panel_sh1106_i2c_info = {0};


/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
static void SH1106_I2C_OSD_Clear_All(void);
static void SH1106_I2C_OSD_Clear_Block(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
static void SH1106_I2C_OSD_Draw_Str(DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t string[]);


//========================================
// I2C
//========================================
static HAL_StatusTypeDef SH1106_I2C_Burst_Write(uint8_t *pBuff, uint16_t len)
{
	return HAL_I2C_Master_Transmit(&SH1106_I2C_INDEX, SH1106_I2C_ADDR, pBuff, len, SH1106_I2C_TIMEOUT);
}

static HAL_StatusTypeDef SH1106_I2C_Burst_Write_Command(const uint8_t *data, uint16_t len)
{
	HAL_StatusTypeDef bRet = HAL_ERROR;
	uint8_t *pBuff = pf_malloc(len + 1);

	/*
		bit7
			0: this is the last control byte, following is data byte(command and ram_data bytes)
				 eg:control byte + data byte + ....+ data byte + data byte
			1: next two bytes are a data byte and another control byte
			   eg:control byte + data byte + ....+ control byte + data byte

		bit6:
		    this bit indicate data byte is a command operation or for ram operation.
	*/
	if (pBuff)
	{
		pBuff[0] = 0x00;
		pf_memcpy(pBuff + 1, data, len);
		bRet = SH1106_I2C_Burst_Write(pBuff, (len + 1));
		pf_free(pBuff);
	}

	return bRet;
}

static HAL_StatusTypeDef SH1106_I2C_Burst_Write_RamData(const uint8_t *data, uint16_t len)
{
	HAL_StatusTypeDef bRet = HAL_ERROR;
	uint8_t *pBuff = pf_malloc(len + 1);

	/*
		bit7
			0: this is the last control byte, following is data byte(command and ram_data bytes)
				 eg:control byte + data byte + ....+ data byte + data byte
			1: next two bytes are a data byte and another control byte
			   eg:control byte + data byte + ....+ control byte + data byte

		bit6:
		    this bit indicate data byte is a command operation or for ram operation.
	*/
	if (pBuff)
	{
		pBuff[0] = 0x40;
		pf_memcpy(pBuff + 1, data, len);
		bRet = SH1106_I2C_Burst_Write(pBuff, (len + 1));
		pf_free(pBuff);
	}

	return bRet;
}

//----------------------------
// driver
//----------------------------
static bool SH1106_I2C_Init(void)
{
	HAL_StatusTypeDef bResult;
	uint8_t array_num;

	// add info
	g_st_panel_sh1106_i2c_info.nPanelType = LIST_PANEL_TYPE_DOTS;

	// write init code
	array_num = sizeof(sh1106_init_data) / sizeof(uint8_t);
	bResult = SH1106_I2C_Burst_Write_Command(sh1106_init_data, array_num);

	if (HAL_OK == bResult)
	{
		g_st_panel_sh1106_i2c_info.nHealthy = true;
	}
	else
	{
		g_st_panel_sh1106_i2c_info.nHealthy = false;
	}

	TRACE_I("%s, Healthy=%d.\r\n", __func__, g_st_panel_sh1106_i2c_info.nHealthy);

	return g_st_panel_sh1106_i2c_info.nHealthy;
}

static void SH1106_I2C_Exit_Sleep(void)
{
	uint8_t buff[3] = {0x8D, 0x14, 0xAF};

	if (!g_st_panel_sh1106_i2c_info.nHealthy)
	{
		return;
	}

	SH1106_I2C_Burst_Write_Command(buff, 3);
	/*
		SH1106_I2C_Write_Cmd(0x8D);   //SET DCDC
		SH1106_I2C_Write_Cmd(0x14);   //DCDC ON
		SH1106_I2C_Write_Cmd(0xAF);   //DISPLAY ON
	*/
}

static void SH1106_I2C_Enter_Sleep(void)
{
	uint8_t buff[3] = {0x8D, 0x10, 0xAE};

	if (!g_st_panel_sh1106_i2c_info.nHealthy)
	{
		return;
	}

	SH1106_I2C_Burst_Write_Command(buff, 3);
	/*
		SH1106_I2C_Write_Cmd(0x8D);   //SET DCDC
		SH1106_I2C_Write_Cmd(0x10);   //DCDC OFF
		SH1106_I2C_Write_Cmd(0xAE);   //DISPLAY OFF
	*/
}

static void SH1106_I2C_Set_Power(DRV_PANEL_POWER_ENUM power)
{
	g_st_panel_sh1106_i2c_info.nPwr = power;//Joanna.wang add for query 20180907
	switch (power)
	{
		case LIST_PANEL_OFF:
		case LIST_PANEL_ENTER_SLEEP:
			SH1106_I2C_Enter_Sleep();
			break;

		case LIST_PANEL_ON:
		case LIST_PANEL_EXIT_SLEEP:
			SH1106_I2C_Exit_Sleep();
			break;

		default:
			break;
	}
}


//========================================
// OSD
//========================================
static void SH1106_I2C_OSD_Set_Pos(uint8_t column, uint8_t page)
{
	uint8_t buff[3];

	if (!g_st_panel_sh1106_i2c_info.nHealthy)
	{
		return;
	}

	buff[0] = 0xb0 + page;
	buff[1] = ((column >> 4) & 0x0f) | 0x10;
	buff[2] = (column & 0x0f) | 0x01;

	SH1106_I2C_Burst_Write_Command(buff, 3);
}

static void setColor(DRV_PANEL_COLOR_ENUM color)
{
}

static void setBkColor(DRV_PANEL_COLOR_ENUM color)
{
}


static void SH1106_I2C_OSD_Clear_All(void)
{
	SH1106_I2C_OSD_Clear_Block(0, 0, SH1106_HTOTAL, SH1106_VTOTAL);
}

static void SH1106_I2C_OSD_Clear_Block(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	uint8_t page;
	uint8_t page_start;
	uint8_t page_end;
	uint8_t column_start;
	uint8_t column_end;
	uint8_t *p;

	if (!g_st_panel_sh1106_i2c_info.nHealthy)
	{
		return;
	}

	page_start = y / 8;
	page_end = (height + y) / 8 + (((height + y) % 8) ? 1 : 0);
	column_start = x;
	column_end = x + width;
	//TRACE_I("page_start=%d, page_end=%d.\r\n", page_start, page_end);
	//TRACE_I("column_start=%d, column_end=%d.\r\n", column_start, column_end);
	page_end = FLEX_MIN(page_end, SH1106_PAGES);
	column_end = FLEX_MIN(column_end, SH1106_HTOTAL);

	p = (uint8_t *)pf_malloc(column_end - column_start );

	if (!p)
	{
		return;
	}

	pf_memset((void *)p, 0, column_end - column_start);

	for (page = page_start; page < page_end; page++)
	{
		SH1106_I2C_OSD_Set_Pos(column_start, page);
		SH1106_I2C_Burst_Write_RamData(p, column_end - column_start);
	}

	pf_free(p);
}

static void SH1106_I2C_OSD_Draw_Char(DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t ch)
{
	uint8_t buffer[2] = {0};
	buffer[0] = ch;
	SH1106_I2C_OSD_Draw_Str(font_size, x, y, buffer);
}

static void SH1106_I2C_OSD_Draw_Str(DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t string[])
{
	uint8_t c = 0;
	uint8_t j = 0;
	uint8_t column;

	if (!g_st_panel_sh1106_i2c_info.nHealthy)
	{
		return;
	}

	if (x >= SH1106_HTOTAL || y >= SH1106_VTOTAL)
	{
		return;
	}

	//allocate memory buffer
	uint8_t *buffer = (uint8_t *)pf_malloc(SH1106_HSIZE);

	if (!buffer)
	{
		return;
	}

	switch (font_size)
	{
		case LIST_FONT_6X8:
			while (string[j] != '\0')
			{
				column = 0;
				SH1106_I2C_OSD_Set_Pos(x, (y / 8));

				//construct page content
				while ((x <= (SH1106_HSIZE - 6)) && (string[j] != '\0'))
				{
					c = string[j] - 32;
					pf_memcpy(&buffer[column], &Flex_Font_6x8[c][0], 6);
					x += 6;
					column += 6;
					j++;
				}

				SH1106_I2C_Burst_Write_RamData(buffer, column);
				x = 0;
				y += 8;

				//check if page reaches end
				if (y >= SH1106_VTOTAL)
				{
					break;
				}
			}

			break;

		case LIST_FONT_8X16:
			while (string[j] != '\0')
			{
				c = string[j] - 32;

				if (x > SH1106_HSIZE - 8) // turn to next line
				{
					x = 0;
					y += 16;

					if (y >= SH1106_VTOTAL)
					{
						break;
					}
				}

				SH1106_I2C_OSD_Set_Pos(x, (y / 8));
				SH1106_I2C_Burst_Write_RamData((uint8_t *)&Flex_Font_8X16[c][0], 8);
				SH1106_I2C_OSD_Set_Pos(x, (y / 8) + 1);
				SH1106_I2C_Burst_Write_RamData((uint8_t *)&Flex_Font_8X16[c][8], 8);
				x += 8;
				j++;
			}

			break;

		default:
			break;
	}

	//release memory buffer
	pf_free(buffer);
}

static void SH1106_I2C_OSD_Draw_Img(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t image[])
{
	uint8_t page;
	uint8_t page_start;
	uint8_t page_disp;
	uint32_t data_index = 0;

	if (!g_st_panel_sh1106_i2c_info.nHealthy)
	{
		return;
	}

	if (x >= SH1106_HTOTAL || y >= SH1106_VTOTAL)
	{
		return;
	}

	page_start = (y % 8 == 0) ? (y / 8) : (y / 8 + 1);
	page_disp = (height % 8 == 0) ? (height / 8) : (height / 8 + 1);

	for (page = page_start; page < (page_start + page_disp); page++)
	{
		SH1106_I2C_OSD_Set_Pos(x, page);
		SH1106_I2C_Burst_Write_RamData(&image[data_index], width);
		data_index += width;
	}
}

static void RefreshGram(DRV_PANEL_REFRESH_ENUM type)
{
}


static void ClearGramAll(void)
{
}

static void ClearGramBlock(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
}

static void DrawGramChar(DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t nDraw_Char)
{
}

static void DrawGramStr(DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t string[])
{
}

static void DrawGramImg(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t image[])
{
}


static DRV_PANEL_IOCTRL_RESULT_ENUM SH1106_I2C_IOCTRL(DRV_PANEL_IOCTRL_ID_ENUM id, void *Parameters)
{
	DRV_PANEL_IOCTRL_RESULT_ENUM ret = LIST_PANEL_IOCTRL_RESULT_OK;
	DRV_PANEL_INFO_ST *pPanelInfo = &g_st_panel_sh1106_i2c_info;

	switch (id)
	{
		case LIST_PANEL_IOCTL_ID_GET_NAME:
			Parameters = (void *)sh1106Name;
			break;

		//------------------------------------------
		case LIST_PANEL_IOCTL_ID_GET_HEALTHY:
			*(bool *)(Parameters) = pPanelInfo->nHealthy;
			break;

		//------------------------------------------
		//Joanna.wang add for query 20180907 start
		case LIST_PANEL_IOCTL_ID_GET_PWR:
			*(bool *)(Parameters) = pPanelInfo->nPwr;
			break;
		
		//------------------------------------------
		case LIST_PANEL_IOCTL_ID_GET_PANEL_TYPE:
			*(DRV_PANEL_TYPE_ENUM *)(Parameters) = pPanelInfo->nPanelType;
			break;

		//------------------------------------------
		case LIST_PANEL_IOCTL_ID_GET_IF_MODE:
			*(DRV_PANEL_IF_MODE_ENUM *)(Parameters) = LIST_PANEL_IF_MODE_I2C;
			break;

		//------------------------------------------
		case LIST_PANEL_IOCTL_ID_GET_HSIZE:
			*(uint16_t *)Parameters = SH1106_HSIZE;
			break;

		case LIST_PANEL_IOCTL_ID_GET_VSIZE:
			*(uint16_t *)Parameters = SH1106_VSIZE;
			break;

		//------------------------------------------
		case LIST_PANEL_IOCTL_ID_GET_MIRROR:
			*(bool *)(Parameters) = pPanelInfo->nMirror;
			break;

		case LIST_PANEL_IOCTL_ID_SET_MIRROR:
			pPanelInfo->nMirror = *(bool *)(Parameters) ;
			break;

		//------------------------------------------
		case LIST_PANEL_IOCTL_ID_GET_FLIP:
			*(bool *)(Parameters) = pPanelInfo->nFlip;
			break;

		case LIST_PANEL_IOCTL_ID_SET_FLIP:
			pPanelInfo->nFlip = *(bool *)(Parameters) ;
			break;

		//------------------------------------------
		case LIST_PANEL_IOCTL_ID_GET_ROTATE:
			*(DRV_PANEL_ROTATE_ENUM *)(Parameters) = pPanelInfo->nRotate;
			break;

		case LIST_PANEL_IOCTL_ID_SET_ROTATE:
			pPanelInfo->nRotate = *(DRV_PANEL_ROTATE_ENUM *)(Parameters) ;
			break;

		//------------------------------------------
		case LIST_PANEL_IOCTL_ID_GET_BRIGHTNESS:
			*(uint8_t *)(Parameters) = pPanelInfo->nBrightness;
			break;

		case LIST_PANEL_IOCTL_ID_SET_BRIGHTNESS:
			pPanelInfo->nBrightness = *(uint8_t *)(Parameters) ;
			break;

		//------------------------------------------
		case LIST_PANEL_IOCTL_ID_GET_CONTRAST:
			*(uint16_t *)(Parameters) = pPanelInfo->nContrast;
			break;

		case LIST_PANEL_IOCTL_ID_SET_CONTRAST:
			pPanelInfo->nContrast = *(uint16_t *)(Parameters) ;
			break;

		//------------------------------------------
		case LIST_PANEL_IOCTL_ID_GET_FRAME_RATE:
			*(uint8_t *)(Parameters) = pPanelInfo->nFrameRate;
			break;

		case LIST_PANEL_IOCTL_ID_SET_FRAME_RATE:
			pPanelInfo->nFrameRate = *(uint8_t *)(Parameters) ;
			break;

		//------------------------------------------
		case LIST_PANEL_IOCTL_ID_ESD_CHECK:
			ret = LIST_PANEL_IOCTRL_RESULT_NOT_SUPPORT;
			break;

		case LIST_PANEL_IOCTL_ID_ESD_RESET:
			ret = LIST_PANEL_IOCTRL_RESULT_NOT_SUPPORT;
			break;

		//------------------------------------------
		default:
			break;
	}

	return ret;
}

const DRV_PANEL_FUNCS_IF_ST panel_oled_sh1106_i2c_if =
{
	.nInit           = SH1106_I2C_Init,
	.nSetPower       = SH1106_I2C_Set_Power,
	.nSetColor       = setColor,
	.nSetBkColor     = setBkColor,
	.nClearAll       = SH1106_I2C_OSD_Clear_All,
	.nClearBlock     = SH1106_I2C_OSD_Clear_Block,
	.nDrawChar       = SH1106_I2C_OSD_Draw_Char,
	.nDrawStr        = SH1106_I2C_OSD_Draw_Str,
	.nDrawImg        = SH1106_I2C_OSD_Draw_Img,
	.nClearGramAll   = ClearGramAll,
	.nClearGramBlock = ClearGramBlock,
	.nRefreshGram    = RefreshGram,
	.nDrawGramChar   = DrawGramChar,
	.nDrawGramStr    = DrawGramStr,
	.nDrawGramImg    = DrawGramImg,
	.nIOCTRL         = SH1106_I2C_IOCTRL,

};


/*========================================================================================================
										End
========================================================================================================*/




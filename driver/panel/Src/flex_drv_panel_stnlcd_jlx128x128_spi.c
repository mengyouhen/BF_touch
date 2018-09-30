/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		flex_drv_panel_stnlcd_jlx128x128_spi.c
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
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_conf.h"
#if defined(HAL_SPI_MODULE_ENABLED)
#include "spi.h"
#endif
#if defined(HAL_TIM_MODULE_ENABLED)
#include "tim.h"
#endif
#include "flex_drv_font.h"
#include "flex_drv_panel_def.h"
#include "flex_hal.h"


// internal
#include "flex_drv_panel_stnlcd_jlx128x128_spi.h"

/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME								"[STNLCD]--"
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
//macro to enable/disable GRAM feature
#define ENABLE_JLX128X128_GRAM

#define JLX128X128_SPI_LCD_CHANNEL		hspi1
//#define JLX128X128_SPI_ROM_CHANNEL		hspi2
#define JLX128X128_SPI_TIMEOUT		    50
//---------------------------
#define JLX128X128_HTOTAL		128
#define JLX128X128_VTOTAL		128
#define JLX128X128_HSIZE			128
#define JLX128X128_VSIZE			128

#define JLX128X128_PAGES		16 // JLX128X128_VTOTAL/8


/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
typedef enum
{
	LIST_SPI_LCD_RST,
	LIST_SPI_LCD_RS,
	LIST_SPI_LCD_CS,
	//------------------
	LIST_SPI_MAX,
} jlx_lcd_spi_enum;

typedef struct
{
	jlx_lcd_spi_enum nType;
	char nName[10];
	flex_gpio_st nGpio;
} jlx_lcd_spi_gpio_cntx_st;




/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
static const char gJlx128X128Name[] = "STNLCD_JLX_128X128";

static const jlx_lcd_spi_gpio_cntx_st g_st_jlx_spi_gpio_cntx[LIST_SPI_MAX] =
{
	{LIST_SPI_LCD_RST,   "LCD_RST",       {STNLCD_RST_GPIO_Port,    STNLCD_RST_Pin} },
	{LIST_SPI_LCD_RS,    "LCD_RS",        {STNLCD_RS_GPIO_Port,     STNLCD_RS_Pin } },
	{LIST_SPI_LCD_CS,    "LCD_LEDA",      {STNLCD_CS_GPIO_Port,     STNLCD_CS_Pin } },
};

static DRV_PANEL_INFO_ST g_st_jlx_spi_info = {0};
static DRV_PANEL_RECTANGLE_ST gRectangleInGram;

#ifdef ENABLE_JLX128X128_GRAM
static uint8_t gGramBuffer[JLX128X128_PAGES][JLX128X128_HTOTAL * 2];
#endif

/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
static void JLX128X128_SPI_OSD_Clear_Block(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
static void JLX128X128_SPI_OSD_Draw_Str(DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t string[]);
static void ClearRetangleGram(void);
static void TraceRetangleGram(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
static void DrawGramStr(DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t string[]);
static void DrawGramImg(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t image[]);


static void jlx_delay_ms(uint16_t ms)
{
	osDelay(ms);
	//HAL_Delay(ms);
}

static void setColor(DRV_PANEL_COLOR_ENUM color)
{
	if (color < LIST_PANEL_COLOR_MAX)
	{
		g_st_jlx_spi_info.nFgColor = color;
	}
}

static void setBkColor(DRV_PANEL_COLOR_ENUM color)
{
	if (color < LIST_PANEL_COLOR_MAX)
	{
		g_st_jlx_spi_info.nBgColor = color;
	}
}

static void jlx_generate_gray_data(uint8_t data, uint8_t buffer[2])
{
	/*
	DDRAM Data [ 2n : 2n+1 ]
	----------------------------------
	|  2n  |  2n + 1 |  Gray Scale   |
	----------------------------------
	|  0   |  0      |  White        |
	|  0   |  1      |  Light Gray   |
	|  1   |  0      |  Dark Gray    |
	|  1   |  1      |  Black        |
	----------------------------------
	*/

#ifdef JLX_CODE_SIZE_OPTIMIZE
	DRV_PANEL_COLOR_ENUM color;

	//initial as white gray scale
	buffer[0] = 0;
	buffer[1] = 0;

	for (int i = 0; i < 8; i++)
	{
		buffer[0] =  buffer[0] << 1;
		buffer[1] =  buffer[1] << 1;

		if (data & 0x80)
		{
			//pixel on,set front color
			color = g_st_jlx_spi_info.nFgColor;
		}
		else
		{
			//pixel off, set background color
			color = g_st_jlx_spi_info.nBgColor;
		}

		switch (color)
		{
			case LIST_PANEL_COLOR_WHITE:
				break;

			case LIST_PANEL_COLOR_LIGHTGRAY:
				buffer[1]  = buffer[1]	| 0x1;
				break;

			case LIST_PANEL_COLOR_DARKGRAY:
				buffer[0]  = buffer[0]	| 0x1;
				break;

			case LIST_PANEL_COLOR_BLACK:
				buffer[0]  = buffer[0]	| 0x1;
				buffer[1]  = buffer[1]	| 0x1;
				break;

			default:
				break;
		}

		data = (uint8_t)(data << 1);
	}

#else
	/*
	Better performance, bigger code footprint
	*/

	//initial as white gray scale
	buffer[0] = 0;
	buffer[1] = 0;

	switch (g_st_jlx_spi_info.nBgColor)
	{
		case LIST_PANEL_COLOR_WHITE:
			switch ( g_st_jlx_spi_info.nFgColor)
			{
				case LIST_PANEL_COLOR_LIGHTGRAY:
					buffer[1]  = data;
					break;

				case LIST_PANEL_COLOR_DARKGRAY:
					buffer[0]  = data;
					break;

				case LIST_PANEL_COLOR_BLACK:
					buffer[0]  = data;
					buffer[1]  = data;
					break;

				default:
					break;
			}

			break;

		case LIST_PANEL_COLOR_LIGHTGRAY:
			switch ( g_st_jlx_spi_info.nFgColor)
			{
				case LIST_PANEL_COLOR_WHITE:
					buffer[1]  =  data;
					break;

				case LIST_PANEL_COLOR_LIGHTGRAY:
					buffer[1]  = 0xff;
					break;

				case LIST_PANEL_COLOR_DARKGRAY:
					buffer[0]  = data;
					buffer[1]  = (uint8_t) ~buffer[0];
					break;

				case LIST_PANEL_COLOR_BLACK:
					buffer[0]  = data;
					buffer[1]  = 0xff;
					break;

				default:
					break;
			}

			break;

		case LIST_PANEL_COLOR_DARKGRAY:
			switch ( g_st_jlx_spi_info.nFgColor)
			{
				case LIST_PANEL_COLOR_WHITE:
					buffer[0]  =  (uint8_t) ~ data;
					break;

				case LIST_PANEL_COLOR_LIGHTGRAY:
					buffer[0]  = (uint8_t) ~data;
					buffer[1]  = (uint8_t) ~buffer[0];
					break;

				case LIST_PANEL_COLOR_DARKGRAY:

					buffer[0]  = 0xff;
					break;

				case LIST_PANEL_COLOR_BLACK:
					buffer[0]  = 0xff;
					buffer[1]  = data;
					break;

				default:
					break;
			}

			break;

		case LIST_PANEL_COLOR_BLACK:
			switch ( g_st_jlx_spi_info.nFgColor)
			{
				case LIST_PANEL_COLOR_WHITE:
					buffer[0]  = (uint8_t) ~data;
					buffer[1]  = buffer[0];
					break;

				case LIST_PANEL_COLOR_LIGHTGRAY:
					buffer[1]  = 0xff;
					buffer[0]  = (uint8_t) ~data;
					break;

				case LIST_PANEL_COLOR_DARKGRAY:
					buffer[0]  = 0xff;
					buffer[1]  = (uint8_t) ~data;
					break;

				case LIST_PANEL_COLOR_BLACK:
					buffer[0]  = 0xff;
					buffer[1]  = 0xff;
					break;

				default:
					break;
			}

			break;

		default:
			break;
	}

#endif

}

static HAL_StatusTypeDef jlx_burst_write(bool isCmd, uint8_t data[], uint16_t len)
{
	HAL_StatusTypeDef ret = HAL_ERROR;
	GPIO_PinState rsState;

	HAL_GPIO_WritePin(g_st_jlx_spi_gpio_cntx[LIST_SPI_LCD_CS].nGpio.nPort,
					  g_st_jlx_spi_gpio_cntx[LIST_SPI_LCD_CS].nGpio.nPin, GPIO_PIN_RESET);

	rsState = isCmd ? GPIO_PIN_RESET : GPIO_PIN_SET;

	HAL_GPIO_WritePin(g_st_jlx_spi_gpio_cntx[LIST_SPI_LCD_RS].nGpio.nPort,
					  g_st_jlx_spi_gpio_cntx[LIST_SPI_LCD_RS].nGpio.nPin, rsState);

	ret = HAL_SPI_Transmit(&JLX128X128_SPI_LCD_CHANNEL, data, len, JLX128X128_SPI_TIMEOUT);

	if ( ret != HAL_OK)
	{
		TRACE_E("%s\r\n", __func__);
	}

	HAL_GPIO_WritePin(g_st_jlx_spi_gpio_cntx[LIST_SPI_LCD_CS].nGpio.nPort,
					  g_st_jlx_spi_gpio_cntx[LIST_SPI_LCD_CS].nGpio.nPin, GPIO_PIN_SET);

	return ret;
}


static HAL_StatusTypeDef jlx_write_cmd(uint8_t cmd)
{
	return jlx_burst_write(true, &cmd, 1);
}

static HAL_StatusTypeDef jlx_burst_write_data(const uint8_t data[], uint16_t len)
{
	HAL_StatusTypeDef ret = HAL_ERROR;

	uint8_t *buffer = (uint8_t *) pf_malloc(2 * len);

	if (!buffer)
	{
		return ret;
	}

	for (uint16_t i = 0; i < len; i++)
	{
		jlx_generate_gray_data(data[i], &buffer[2 * i]);
	}

	jlx_burst_write(false, buffer, 2 * len);

	pf_free(buffer);
	return ret;
}


//========================================
// BRIGHTNESS
//========================================

static void panel_backlight_set_pwm(uint16_t value)
{
	TIM_OC_InitTypeDef sConfigOC;
	//TRACE_I("%s, value=%d. \r\n", __func__, value);

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = value;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}

static uint8_t JLX128X128_SPI_Get_Brightness(void)
{
	return g_st_jlx_spi_info.nBrightness;
}

static void JLX128X128_SPI_Set_Brightness(uint8_t value)
{
//	uint16_t pwm = 0;

	g_st_jlx_spi_info.nBrightness = value;
/*
	switch (g_st_jlx_spi_info.nBrightness)
	{
		case LIST_BRIGHTNESS_LEVEL_0:
			pwm = 0;
			break;

		case LIST_BRIGHTNESS_LEVEL_1:
			pwm = 50;
			break;

		case LIST_BRIGHTNESS_LEVEL_2:
			pwm = 60;
			break;

		case LIST_BRIGHTNESS_LEVEL_3:
			pwm = 70;
			break;

		case LIST_BRIGHTNESS_LEVEL_4:
			pwm = 80;
			break;

		case LIST_BRIGHTNESS_LEVEL_5:
			pwm = 100;
			break;

		default:
			break;
	}
*/
	panel_backlight_set_pwm(value);
}

//========================================
// FUNC
//========================================
static bool JLX128X128_SPI_Is_Healthy(void)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t timeout = 3;

	do
	{
		status = jlx_write_cmd(0xb0);   // set page0
		timeout--;
	}
	while ((status != HAL_OK) && (timeout != 0));

	if (status == HAL_OK)
	{
		//TRACE_I("jlx is healthy. \r\n");
		return true;
	}
	else
	{
		TRACE_E("jlx is unhealthy. \r\n");
		return false;
	}
}


static void jlx_lcd_init(void)
{
	Flex_Hal_Gpio_Ctrl(&g_st_jlx_spi_gpio_cntx[LIST_SPI_LCD_RST].nGpio,  LIST_GPIO_ACTION_LOW);//reset=0;        /*低电平复位*/
	jlx_delay_ms(10);
	Flex_Hal_Gpio_Ctrl(&g_st_jlx_spi_gpio_cntx[LIST_SPI_LCD_RST].nGpio,  LIST_GPIO_ACTION_HIGH);//reset=1;		    /*复位完毕*/
	jlx_delay_ms(100);

	jlx_write_cmd(0x2c);
	jlx_delay_ms(200);
	jlx_write_cmd(0x2e);
	jlx_delay_ms(200);
	jlx_write_cmd(0x2f);
	jlx_delay_ms(10);

	jlx_write_cmd(0xae);		//显示关
	jlx_write_cmd(0x38);		//模式设置
	jlx_write_cmd(0xb8);		//85HZ
	jlx_write_cmd(0xc8);		//行扫描顺序
	jlx_write_cmd(0xa0);		//列扫描顺序
	jlx_write_cmd(0x44);		//Set initial COM0 register
	jlx_write_cmd(0x00);
	jlx_write_cmd(0x40);		//Set initial display line register
	jlx_write_cmd(0x00);
	jlx_write_cmd(0xab);
	jlx_write_cmd(0x67);
#if 1
	jlx_write_cmd(0x27);		//粗调对比度，可设置范围0x20～0x27
	jlx_write_cmd(0x81);		//微调对比度
	jlx_write_cmd(0x30);		//微调对比度的值，可设置范围0x00～0x3f
	jlx_write_cmd(0x57);		//0x57  1/12 bias
#else
	//datasheet
	jlx_write_cmd(0x26);		//粗调对比度，可设置范围0x20～0x27
	jlx_write_cmd(0x81);		//微调对比度
	jlx_write_cmd(0x29);		//微调对比度的值，可设置范围0x00～0x3f
	jlx_write_cmd(0x54);		//0x57  1/12 bias
#endif

	jlx_write_cmd(0xf3);
	jlx_write_cmd(0x04);
	jlx_write_cmd(0x93);
	//jlx_write_cmd(0xaf);		//显示开
}

static bool JLX128X128_SPI_Init(void)
{
	g_st_jlx_spi_info.nPanelType = LIST_PANEL_TYPE_DOTS;
	g_st_jlx_spi_info.nHealthy = JLX128X128_SPI_Is_Healthy();
	TRACE_I("%s, Healthy=%d.\r\n", __func__, g_st_jlx_spi_info.nHealthy);

	if (!g_st_jlx_spi_info.nHealthy)
	{
		return false;
	}

	//set default front color and background color
	setColor(LIST_PANEL_COLOR_BLACK);
	setBkColor(LIST_PANEL_COLOR_WHITE);

	//turn off backlight
	JLX128X128_SPI_Set_Brightness(0); 

	jlx_lcd_init();
	ClearRetangleGram();

	return true;
}


static void JLX128X128_SPI_Exit_Sleep(void)
{
	if (!g_st_jlx_spi_info.nHealthy)
	{
		return;
	}

	jlx_write_cmd(0xe1);
}

static void JLX128X128_SPI_Enter_Sleep(void)
{
	if (!g_st_jlx_spi_info.nHealthy)
	{
		return;
	}

	jlx_write_cmd(0xa9);
}

static void JLX128X128_SPI_Disply_On(void)
{
	if (!g_st_jlx_spi_info.nHealthy)
	{
		return;
	}

	jlx_write_cmd(0xaf);

}

static void JLX128X128_SPI_Display_Off(void)
{
	if (!g_st_jlx_spi_info.nHealthy)
	{
		return;
	}
	jlx_write_cmd(0xae);
}

static void JLX128X128_SPI_Set_Power(DRV_PANEL_POWER_ENUM power)
{
	g_st_jlx_spi_info.nPwr = power;//Joanna.wang add for query 20180907
	switch (power)
	{
		case LIST_PANEL_OFF:
			JLX128X128_SPI_Display_Off();
			break;

		case LIST_PANEL_ON:
			JLX128X128_SPI_Disply_On();
			break;

		case LIST_PANEL_ENTER_SLEEP:
			JLX128X128_SPI_Enter_Sleep();
			break;

		case LIST_PANEL_EXIT_SLEEP:
			JLX128X128_SPI_Exit_Sleep();
			break;

		default:
			break;
	}
}


//========================================
// OSD
//========================================
static void JLX128X128_SPI_OSD_Set_Pos(uint8_t column, uint8_t page)
{
	uint8_t buff[3];

	if (!g_st_jlx_spi_info.nHealthy)
	{
		return;
	}

	buff[0] = 0xb0 + page;
	buff[1] = ((column >> 4) & 0x0f) + 0x10;
	buff[2] = column & 0x0f;
	jlx_burst_write(true, buff, 3);
}

static void JLX128X128_SPI_OSD_Clear_All(void)
{
	JLX128X128_SPI_OSD_Clear_Block(0, 0, JLX128X128_HTOTAL, JLX128X128_VTOTAL);
}

static void JLX128X128_SPI_OSD_Clear_Block(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	uint8_t *p;
	uint8_t page;
	uint8_t page_start, page_end;
	uint8_t column_start, column_end;

	if (!g_st_jlx_spi_info.nHealthy)
	{
		return;
	}

	page_start = y / 8;
	page_end = (height + y) / 8 + (((height + y) % 8) ? 1 : 0);
	column_start = x;
	column_end = x + width;
	//TRACE_I("page_start=%d, page_end=%d.\r\n", page_start, page_end);
	//TRACE_I("column_start=%d, column_end=%d.\r\n", column_start, column_end);
	page_end = FLEX_MIN(page_end, JLX128X128_PAGES);
	column_end = FLEX_MIN(column_end, JLX128X128_HTOTAL);

	p = (uint8_t *)pf_malloc(column_end - column_start);

	if (!p)
	{
		return;
	}

	pf_memset((void *)p, 0, column_end - column_start);

	for (page = page_start; page < page_end; page++)
	{
		JLX128X128_SPI_OSD_Set_Pos(column_start, page);
		jlx_burst_write_data(p, column_end - column_start);
	}

	pf_free(p);
}

static void JLX128X128_SPI_OSD_Draw_Char(DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t ch)
{
	uint8_t buffer[2] = {0};
	buffer[0] = ch;
	JLX128X128_SPI_OSD_Draw_Str(font_size, x, y, buffer);
}

static void JLX128X128_SPI_OSD_Draw_Str(DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t string[])
{
	uint8_t c = 0;
	uint8_t j = 0;
	uint8_t column;

	if (!g_st_jlx_spi_info.nHealthy)
	{
		return;
	}

	if (x >= JLX128X128_HTOTAL || y >= JLX128X128_VTOTAL)
	{
		return;
	}

	//allocate memory buffer
	uint8_t *buffer = (uint8_t *)pf_malloc(JLX128X128_HSIZE);

	if(!buffer)
	{
		return;
	}

	switch (font_size)
	{
		case LIST_FONT_6X8:
			while (string[j] != '\0')
			{
				column = 0;
				JLX128X128_SPI_OSD_Set_Pos(x, (y / 8));

				//construct page content
				while ((x <= (JLX128X128_HSIZE - 6)) && (string[j] != '\0'))
				{
					c = string[j] - 32;
					pf_memcpy(&buffer[column], &Flex_Font_6x8[c][0], 6);
					x += 6;
					column += 6;
					j++;
				}

				jlx_burst_write_data(buffer, column);
				x = 0;
				y += 8;

				//check if page reaches end
				if (y >= JLX128X128_VSIZE)
				{
					break;
				}
			}

			break;

		case LIST_FONT_8X16:
			while (string[j] != '\0')
			{
				c = string[j] - 32;

				if (x > JLX128X128_HSIZE - 8)
				{
					x = 0;
					y += 2 * 8;

					if (y >= JLX128X128_VSIZE)
					{
						break;
					}
				}

				JLX128X128_SPI_OSD_Set_Pos(x, y / 8);
				jlx_burst_write_data(&Flex_Font_8X16[c][0], 8);

				JLX128X128_SPI_OSD_Set_Pos(x, (y / 8) + 1);
				jlx_burst_write_data(&Flex_Font_8X16[c][8], 8);

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

static void JLX128X128_SPI_OSD_Draw_Img(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t image[])
{
	uint8_t page;
	uint8_t page_start;
	uint8_t page_disp;
	uint32_t data_index = 0;

	if (!g_st_jlx_spi_info.nHealthy)
	{
		return;
	}

	if (x >= JLX128X128_HTOTAL || y >= JLX128X128_VTOTAL)
	{
		return;
	}

	page_start = (y % 8 == 0) ? (y / 8) : (y / 8 + 1);
	page_disp = (height % 8 == 0) ? (height / 8) : (height / 8 + 1);

	for (page = page_start; page < (page_start + page_disp); page++)
	{
		JLX128X128_SPI_OSD_Set_Pos(x, page);
		jlx_burst_write_data(&image[data_index], width);
		data_index += width;
	}
}

static void ClearRetangleGram(void)
{
	gRectangleInGram.minX = JLX128X128_HTOTAL;
	gRectangleInGram.minY = JLX128X128_VTOTAL;
	gRectangleInGram.maxX = 0;
	gRectangleInGram.maxY = 0;
}

static void TraceRetangleGram(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	if (x < gRectangleInGram.minX)
	{
		gRectangleInGram.minX = x;
	}

	if (y < gRectangleInGram.minY)
	{
		gRectangleInGram.minY = y;
	}

	if ((x + width) > gRectangleInGram.maxX)
	{
		gRectangleInGram.maxX = x + width;
	}

	if ((y + height) > gRectangleInGram.maxY)
	{
		gRectangleInGram.maxY = (y + height);
	}
}

static void WriteGramByte(uint8_t x, uint8_t y, uint8_t value)
{
#ifdef ENABLE_JLX128X128_GRAM
	uint8_t i;
	uint8_t page;
	uint8_t page_start;
	uint8_t page_end;
	uint8_t height = 8;
	uint8_t startY;		//start line index in page
	uint8_t hInPage;	//height in page
	uint8_t boundary;
	uint8_t mask;
	uint8_t gray[2];
	uint8_t lowByte;
	uint8_t highByte;
	uint16_t index;

	if ((x >= JLX128X128_HTOTAL) || (y >= JLX128X128_VTOTAL))
	{
		return;
	}

	jlx_generate_gray_data(value, gray);

	page_start = y / 8;
	page_end = page_start + 1;

	if (y % 8)
	{
		page_end++;
	}

	page_end = FLEX_MIN(page_end, JLX128X128_PAGES);

	for (page = page_start; page < page_end; page++)
	{
		//calculate start line index in page
		startY = y % 8;
		//calculate height in page
		boundary = y + 8 - startY;

		if ( (y + height) > boundary)
		{
			hInPage = boundary -  y;
		}
		else
		{
			hInPage = height;
		}

		//generate mask bitfield
		mask = 1;
		i = hInPage;

		while (--i)
		{
			mask = (mask << 1)  +  1;
		}

		lowByte =  (uint8_t)((gray[0] & mask) << startY);
		highByte =  (uint8_t)((gray[1] & mask) << startY);
		mask = mask << startY;
		mask =  (uint8_t)~mask;

		//set value
		index = 2*x;
		gGramBuffer[page][index] &= mask;
		gGramBuffer[page][index] |= lowByte;
		gGramBuffer[page][index + 1] &= mask;
		gGramBuffer[page][index + 1] |= highByte;

		//update value, y and remaining height
		y = boundary;
		height = height - hInPage;
		gray[0] = gray[0] >> hInPage;
		gray[1] = gray[1] >> hInPage;
	}

#endif
}


static void RefreshGram(DRV_PANEL_REFRESH_ENUM type)
{
#ifdef ENABLE_JLX128X128_GRAM
	uint8_t x;
	uint8_t y;
	int16_t width;
	int16_t height;
	uint8_t page;
	uint8_t page_start;
	uint8_t page_end;

	if (!g_st_jlx_spi_info.nHealthy)
	{
		return;
	}

	switch (type)
	{
		case REFRESH_ALL:
			x = 0;
			y = 0;
			width = JLX128X128_HSIZE;
			height = JLX128X128_VSIZE;
			break;

		case REFRESH_PARTIAL:
			x = gRectangleInGram.minX;
			y = gRectangleInGram.minY;
			width = (int16_t)(gRectangleInGram.maxX - x);
			height = (int16_t)(gRectangleInGram.maxY - y);
			break;

		default:
			return;
	}

	if ((x >= JLX128X128_HTOTAL) || (y >= JLX128X128_VTOTAL))
	{
		return;
	}

	if ((width > 0) && (height > 0))
	{
		page_start = y / 8;
		page_end = (height + y) / 8 + (((height + y) % 8) ? 1 : 0);
		//do boundary check
		page_end = FLEX_MIN(page_end, JLX128X128_PAGES);

		if ( (x + width) > JLX128X128_HTOTAL)
		{
			width = JLX128X128_HTOTAL - x;
		}

		for (page = page_start; page < page_end; page++)
		{
			JLX128X128_SPI_OSD_Set_Pos(x, page);
			jlx_burst_write(false, &gGramBuffer[page][2 * x], 2 * width);
		}

		ClearRetangleGram();
	}

#endif
}

static void ClearGramAll(void)
{
#ifdef ENABLE_JLX128X128_GRAM
	uint8_t gray[2];
	jlx_generate_gray_data(0, gray);

	if (g_st_jlx_spi_info.nHealthy)
	{
		for (uint8_t i = 0; i < JLX128X128_PAGES; i++)
		{
			for (uint16_t j = 0; j < JLX128X128_HTOTAL; j++)
			{
				*(uint16_t *)&gGramBuffer[i][j * 2] = (uint16_t)(gray[1] << 8) + gray[0];
			}
		}
	}

	TraceRetangleGram(0, 0, JLX128X128_HTOTAL, JLX128X128_VSIZE);
#endif
}

static void ClearGramBlock(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
#ifdef ENABLE_JLX128X128_GRAM
	/*ClearGramBlock() doesn't leverage WriteGramByte() for better performance.*/
	uint8_t i;
	uint8_t page;
	uint8_t page_start;
	uint8_t page_end;
	uint8_t column_end;
	uint8_t startY; 	//start line index in page
	uint8_t hInPage;	//height in page
	uint8_t boundary;
	uint8_t mask;
	uint8_t gray[2];
	uint8_t lowByte;
	uint8_t highByte;

	if (!g_st_jlx_spi_info.nHealthy)
	{
		return;
	}

	if ((x >= JLX128X128_HTOTAL) || (y >= JLX128X128_VTOTAL))
	{
		return;
	}

	TraceRetangleGram(x, y, width, height);
	jlx_generate_gray_data(0, gray);

	page_start = y / 8;
	page_end = (height + y) / 8 + (((height + y) % 8) ? 1 : 0);
	page_end = FLEX_MIN(page_end, JLX128X128_PAGES);
	column_end = FLEX_MIN(x + width, JLX128X128_HTOTAL);

	for (page = page_start; page < page_end; page++)
	{
		//calculate start line index in page
		startY = y % 8;
		//calculate height in page
		boundary = (page + 1)  * 8;

		if ( (y + height) > boundary)
		{
			hInPage = boundary -  y;
		}
		else
		{
			hInPage = height;
		}

		//generate mask bitfield
		mask = 1;
		i = hInPage;

		while (--i)
		{
			mask = (mask << 1)	+  1;
		}

		lowByte =  (uint8_t)((gray[0] & mask) << startY);
		highByte =  (uint8_t)((gray[1] & mask) << startY);
		mask = mask << startY;
		mask =  (uint8_t)~mask;

		for (uint8_t i = x; i < column_end; i++)
		{
			gGramBuffer[page][2 * i] &= mask;
			gGramBuffer[page][2 * i]  |= lowByte;
			gGramBuffer[page][(2 * i) + 1] &= mask;
			gGramBuffer[page][(2 * i) + 1]  |= highByte;
		}

		//update value, y and remaining height
		y = boundary;
		height = height - hInPage;
	}

#endif
}

static void DrawGramChar(DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t ch)
{
	uint8_t buffer[2] = {0};
	buffer[0] = ch;
	DrawGramStr(font_size, x, y, (const uint8_t *)buffer);
}

static void DrawGramStr(DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t string[])
{
	uint8_t c = 0;
	uint8_t j = 0;

	if (!g_st_jlx_spi_info.nHealthy)
	{
		return;
	}

	if (x >= JLX128X128_HTOTAL || y >= JLX128X128_VTOTAL)
	{
		return;
	}

	switch (font_size)
	{
		case LIST_FONT_6X8:
			while (string[j] != '\0')
			{
				//construct page content
				while ((x <= (JLX128X128_HSIZE - 6)) && (string[j] != '\0'))
				{
					c = string[j++] - 32;
					DrawGramImg(x, y, 6, 8, &Flex_Font_6x8[c][0]);
					x += 6;
				}

				x = 0;
				y += 8;

				//check if page reaches end
				if (y >= JLX128X128_VSIZE)
				{
					break;
				}
			}

			break;

		case LIST_FONT_8X16:
			while (string[j] != '\0')
			{
				//construct page content
				while ((x <= (JLX128X128_HSIZE - 8)) && (string[j] != '\0'))
				{
					c = string[j++] - 32;
					DrawGramImg(x, y, 8, 8, &Flex_Font_8X16[c][0]);
					DrawGramImg(x, y + 8, 8, 8, &Flex_Font_8X16[c][8]);
					x += 8;
				}

				x = 0;
				y += 16;

				//check if page reaches end
				if (y >= JLX128X128_VSIZE)
				{
					break;
				}

			}

			break;

		default:
			break;
	}
}

static void DrawGramImg(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t image[])
{
	uint16_t index = 0;
	uint8_t column_end;
	uint8_t row_end;
	uint8_t page;
	uint8_t *p;
	bool pageAlign = (0 == (y % 8));

	if (!g_st_jlx_spi_info.nHealthy)
	{
		return;
	}

	if ((x >= JLX128X128_HTOTAL) || (y >= JLX128X128_VTOTAL))
	{
		return;
	}

	TraceRetangleGram(x, y, width, height);

	column_end = FLEX_MIN( x + width, JLX128X128_HSIZE);
	row_end = FLEX_MIN( y + height, JLX128X128_VSIZE);

	while (y < row_end)
	{
		p = (uint8_t *)&image[index];
		page = y/8;

		for (uint8_t i = x; i < column_end; i++)
		{
			if (pageAlign)
			{
				//although WriteGramByte() can work in bit alighment, I do it sperately for page alighment
				//to achieve faster speed.
				uint8_t gray[2];
				jlx_generate_gray_data(*p, gray);
				gGramBuffer[page][2 * i] = gray[0];
				gGramBuffer[page][2 * i + 1] =  gray[1];
			}
			else
			{
				//non page alignment
				WriteGramByte(i, y, *p);
			}
			p++;
		}

		y = y + 8;
		//align in width
		index += width;
	}

}


static DRV_PANEL_IOCTRL_RESULT_ENUM JLX128X128_SPI_IOCTRL(DRV_PANEL_IOCTRL_ID_ENUM id, void *Parameters)
{
	DRV_PANEL_IOCTRL_RESULT_ENUM ret = LIST_PANEL_IOCTRL_RESULT_OK;
	DRV_PANEL_INFO_ST *pPanelInfo = &g_st_jlx_spi_info;

	//TRACE_F();

	switch (id)
	{
		case LIST_PANEL_IOCTL_ID_GET_NAME:
			Parameters = (void *)gJlx128X128Name;
			//strcpy((char *)Parameters, JLX128X128_NAME);
			break;

		//------------------------------------------
		case LIST_PANEL_IOCTL_ID_GET_HEALTHY:
			*(bool *)(Parameters) = pPanelInfo->nHealthy;
			break;

		//------------------------------------------
		//Joanna.wang add for query 20180907
		case LIST_PANEL_IOCTL_ID_GET_PWR:
			*(bool *)(Parameters) = pPanelInfo->nPwr;
			break;
		
		//------------------------------------------
		case LIST_PANEL_IOCTL_ID_GET_PANEL_TYPE:
			*(DRV_PANEL_TYPE_ENUM *)(Parameters) = pPanelInfo->nPanelType;
			break;

		//------------------------------------------
		case LIST_PANEL_IOCTL_ID_GET_IF_MODE:
			*(DRV_PANEL_IF_MODE_ENUM *)(Parameters) = LIST_PANEL_IF_MODE_SPI;
			break;

		//------------------------------------------
		case LIST_PANEL_IOCTL_ID_GET_HSIZE:
			*(uint16_t *)Parameters = JLX128X128_HSIZE;
			break;

		case LIST_PANEL_IOCTL_ID_GET_VSIZE:
			*(uint16_t *)Parameters = JLX128X128_VSIZE;
			break;

		//------------------------------------------
		case LIST_PANEL_IOCTL_ID_GET_MIRROR:
			*(bool *)(Parameters) = pPanelInfo->nMirror;
			break;

		case LIST_PANEL_IOCTL_ID_SET_MIRROR:
			pPanelInfo->nMirror = *(bool *)(Parameters) ;
			//TRACE_I("SET MIRROT panel_info.nMirror=%d.\r\n", g_st_jlx_spi_info.nMirror);
			break;

		//------------------------------------------
		case LIST_PANEL_IOCTL_ID_GET_FLIP:
			*(bool *)(Parameters) = pPanelInfo->nFlip;
			break;

		case LIST_PANEL_IOCTL_ID_SET_FLIP:
			pPanelInfo->nFlip = *(bool *)(Parameters) ;
			//TRACE_I("SET FLIP panel_info.nFlip=%d.\r\n", g_st_jlx_spi_info.nFlip);
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
			*(uint8_t *)(Parameters) = JLX128X128_SPI_Get_Brightness();
			break;

		case LIST_PANEL_IOCTL_ID_SET_BRIGHTNESS:
			JLX128X128_SPI_Set_Brightness(*(uint8_t *)Parameters);
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


const DRV_PANEL_FUNCS_IF_ST panel_stnlcd_jlx128x128_spi_if =
{
	.nInit           = JLX128X128_SPI_Init,
	.nSetPower       = JLX128X128_SPI_Set_Power,
	.nSetColor       = setColor,
	.nSetBkColor     = setBkColor,
	.nClearAll       = JLX128X128_SPI_OSD_Clear_All,
	.nClearBlock     = JLX128X128_SPI_OSD_Clear_Block,
	.nDrawChar       = JLX128X128_SPI_OSD_Draw_Char,
	.nDrawStr        = JLX128X128_SPI_OSD_Draw_Str,
	.nDrawImg        = JLX128X128_SPI_OSD_Draw_Img,
	.nClearGramAll   = ClearGramAll,
	.nClearGramBlock = ClearGramBlock,
	.nRefreshGram    = RefreshGram,
	.nDrawGramChar   = DrawGramChar,
	.nDrawGramStr    = DrawGramStr,
	.nDrawGramImg    = DrawGramImg,
	.nIOCTRL         = JLX128X128_SPI_IOCTRL,
};


/*========================================================================================================
										End
========================================================================================================*/




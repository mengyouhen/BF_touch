/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_drv_panel_def.h
 [Date]:       
 		2017-12-20
 [Author]:   	
 		carson.qian 
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_DRV_PANEL_DEF_H_
#define _FLEX_DRV_PANEL_DEF_H_

#ifdef __cplusplus 
extern "C" {
#endif

/*========================================================================================================
										I N C L U D E
========================================================================================================*/
#include "flex_common.h"




/*========================================================================================================
										D E F I N E
========================================================================================================*/
#define PANEL_NAME_LEN   25




/*========================================================================================================
										T Y P E D E F
========================================================================================================*/

//========================================
// PANEL GRAM REFRESH TYPE
//========================================
typedef enum
{
	REFRESH_PARTIAL,
	REFRESH_ALL,
}DRV_PANEL_REFRESH_ENUM;

//========================================
// PANEL INDEX
//========================================
typedef enum
{
	LIST_PANEL_INDEX_MAIN,
	LIST_PANEL_INDEX_SUB,
	LIST_PANEL_INDEX_MAX
}DRV_PANEL_NAME_ENUM;

//========================================
// PANEL TYPE
//========================================
typedef enum
{
	LIST_PANEL_TYPE_DOTS,		
	LIST_PANEL_TYPE_SEGMENT,
	LIST_PANEL_TYPE_MAX
}DRV_PANEL_TYPE_ENUM;

//========================================
// PANEL INTERFACE MODE
//========================================
typedef enum
{
	LIST_PANEL_IF_MODE_I2C,
	LIST_PANEL_IF_MODE_SPI,
	LIST_PANEL_IF_MODE_SPI_GPIO,
	LIST_PANEL_IF_MODE_SEGMENT,
	LIST_PANEL_IF_MODE_MAX
}DRV_PANEL_IF_MODE_ENUM;

//========================================
// PANEL ROTATE
//========================================
typedef enum
{
	LIST_PANEL_ROTATE_90,
	LIST_PANEL_ROTATE_180,
	LIST_PANEL_ROTATE_270,
	LIST_PANEL_ROTATE_MAX
}DRV_PANEL_ROTATE_ENUM;

//========================================
// CLEAR COLOR 
//========================================
/*
typedef enum
{
	LIST_PANEL_CLEAR_COLOR_BLACK,
	LIST_PANEL_CLEAR_COLOR_WHITE,
	LIST_PANEL_CLEAR_COLOR_MAX
}flex_panel_clear_color_enum;
*/
//========================================
// BRIGHTNESS  
//========================================
/*
typedef enum
{
	LIST_BRIGHTNESS_LEVEL_0,
	LIST_BRIGHTNESS_LEVEL_1,
	LIST_BRIGHTNESS_LEVEL_2,
	LIST_BRIGHTNESS_LEVEL_3,
	LIST_BRIGHTNESS_LEVEL_4,
	LIST_BRIGHTNESS_LEVEL_5,
	LIST_BRIGHTNESS_MAX
}DRV_PANEL_BRIGHTNESS_ENUM;
*/
//========================================
// FONT SIZE 
//========================================
typedef enum
{
	LIST_FONT_6X8,
	LIST_FONT_8X16,
	LIST_FONT_MAX
}DRV_FONT_SIZE_ENUM;

//========================================
// PANEL IOCTRL
//========================================
typedef enum
{
	LIST_PANEL_IOCTRL_RESULT_OK,
	LIST_PANEL_IOCTRL_RESULT_FAIL,
	LIST_PANEL_IOCTRL_RESULT_NOT_SUPPORT
}DRV_PANEL_IOCTRL_RESULT_ENUM;

typedef enum
{
	LIST_PANEL_IOCTL_ID_GET_NAME,		
	LIST_PANEL_IOCTL_ID_GET_HEALTHY,		
	LIST_PANEL_IOCTL_ID_GET_PWR,	
	LIST_PANEL_IOCTL_ID_GET_PANEL_TYPE,	
	LIST_PANEL_IOCTL_ID_GET_IF_MODE,		

	LIST_PANEL_IOCTL_ID_GET_HSIZE,		
	LIST_PANEL_IOCTL_ID_GET_VSIZE,	
	
	LIST_PANEL_IOCTL_ID_GET_MIRROR,		
	LIST_PANEL_IOCTL_ID_SET_MIRROR,		
	
	LIST_PANEL_IOCTL_ID_GET_FLIP,		
	LIST_PANEL_IOCTL_ID_SET_FLIP,		

	LIST_PANEL_IOCTL_ID_GET_ROTATE,		
	LIST_PANEL_IOCTL_ID_SET_ROTATE,		

	LIST_PANEL_IOCTL_ID_GET_BRIGHTNESS,		
	LIST_PANEL_IOCTL_ID_SET_BRIGHTNESS,		

	LIST_PANEL_IOCTL_ID_GET_CONTRAST,		
	LIST_PANEL_IOCTL_ID_SET_CONTRAST,		

	LIST_PANEL_IOCTL_ID_GET_FRAME_RATE,		
	LIST_PANEL_IOCTL_ID_SET_FRAME_RATE,	
	
	LIST_PANEL_IOCTL_ID_ESD_CHECK,		
	LIST_PANEL_IOCTL_ID_ESD_RESET,		
//-----------------
  LIST_PANEL_IOCTL_ID_MAX = 0xFF
}DRV_PANEL_IOCTRL_ID_ENUM;


//========================================
// RECTANGLE
//========================================
typedef struct 
{
	uint16_t minX;
	uint16_t minY;
	uint16_t maxX;
	uint16_t maxY;
}DRV_PANEL_RECTANGLE_ST;

//========================================
// PANEL PARAMS
//========================================
typedef struct
{
  DRV_PANEL_NAME_ENUM nPnlName;
  DRV_PANEL_TYPE_ENUM nPnlType;
  DRV_PANEL_IF_MODE_ENUM nPnlIfMode;
  uint32_t nPhysicalWidth;
  uint32_t nPhysicalHeight;
  uint32_t nFrameWidth;
  uint32_t nFrameHeight;
} DRV_PANEL_PARAMS_ST;

typedef enum
{
	LIST_PANEL_OFF,
	LIST_PANEL_ON,
	LIST_PANEL_ENTER_SLEEP,
	LIST_PANEL_EXIT_SLEEP,
}DRV_PANEL_POWER_ENUM;

typedef enum
{
	LIST_PANEL_COLOR_WHITE,
	LIST_PANEL_COLOR_LIGHTGRAY,
	LIST_PANEL_COLOR_DARKGRAY,
	LIST_PANEL_COLOR_BLACK,
	LIST_PANEL_COLOR_MAX,
}DRV_PANEL_COLOR_ENUM;



//========================================
// func
//========================================
typedef struct
{
  //const char* name;
  //void (*nGetParams)(DRV_PANEL_PARAMS_ST *params);
  bool (* nInit)(void);
  void (* nSetPower)(DRV_PANEL_POWER_ENUM power);
  void (* nSetColor)(DRV_PANEL_COLOR_ENUM color);
  void (* nSetBkColor)(DRV_PANEL_COLOR_ENUM color);
  void (* nClearAll)(void);
  void (* nClearBlock)(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
  void (* nDrawChar)(DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t ch);
  void (* nDrawStr)(DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t string[]);
  void (* nDrawImg)(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t image[]);
  void (* nClearGramAll)(void);
  void (* nClearGramBlock)(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
  void (* nRefreshGram)(DRV_PANEL_REFRESH_ENUM type);  
  void (* nDrawGramChar)(DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t ch);
  void (* nDrawGramStr)(DRV_FONT_SIZE_ENUM font_size, uint8_t x, uint8_t y, const uint8_t string[]);
  void (* nDrawGramImg)(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t image[]);  
  DRV_PANEL_IOCTRL_RESULT_ENUM (* nIOCTRL)(DRV_PANEL_IOCTRL_ID_ENUM id, void* Parameters);
}DRV_PANEL_FUNCS_IF_ST;

//========================================
// PANEL INFO
//========================================
typedef struct
{
	uint16_t nHsize;
	uint16_t nVsize;
	
	bool nHealthy;
	bool nMirror;
	bool nFlip;
	DRV_PANEL_POWER_ENUM nPwr;
	DRV_PANEL_TYPE_ENUM nPanelType;
	DRV_PANEL_IF_MODE_ENUM nIfMode;
	DRV_PANEL_ROTATE_ENUM nRotate;
	
	uint8_t nBrightness;
	DRV_PANEL_COLOR_ENUM nFgColor; //foreground color
	DRV_PANEL_COLOR_ENUM nBgColor; //backbroud color
	uint16_t nContrast;
	uint8_t nFrameRate;
}DRV_PANEL_INFO_ST;







/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/







/*========================================================================================================
										F U N C T I O N
========================================================================================================*/






/*========================================================================================================
										End
========================================================================================================*/
#ifdef __cplusplus 
}
#endif

#endif//_FLEX_DRV_PANEL_DEF_H_



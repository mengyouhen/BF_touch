
/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_project_config.h
 [Date]:       
 		2017-12-20
 [Author]:   	
 		carson.qian 
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_PROJECT_CONFIG_H_
#define _FLEX_PROJECT_CONFIG_H_

/*========================================================================================================
										I N C L U D E
========================================================================================================*/




/*========================================================================================================
										D E F I N E
========================================================================================================*/
//=====================================
// BOARD 
//=====================================
#define BOARD_STM32F401RET6_NUCLEO          1 // ST 
#define BOARD_STM32L476RGT6_NUCLEO          2	// ST 
#define BOARD_STM32L476ZET6_HMI_BUTTERFLY   3	// Flex
//------------------------------
#define BOARD_SELECT      BOARD_STM32L476ZET6_HMI_BUTTERFLY


//=====================================
// PROJECT
//=====================================
#define PROJECT_HMI_BUTTERFLY       1
//------------------------------
#define PROJECT_SELECT    PROJECT_HMI_BUTTERFLY

/*========================================================================================================
										C O N F I G----VERSION
========================================================================================================*/
#if (PROJECT_SELECT==PROJECT_HMI_BUTTERFLY)
	#if (BOARD_SELECT==BOARD_STM32L476RGT6_NUCLEO)
	#define SW_VERSION			"V0.01"
	#elif (BOARD_SELECT==BOARD_STM32L476ZET6_HMI_BUTTERFLY)
	#define SW_VERSION			"V0.30"
	#endif
#else
	#define SW_VERSION			"V0.01"
#endif

/*========================================================================================================
										C O N F I G----DEVICE
========================================================================================================*/
//=====================================
// PANEL 
//=====================================
#define PANEL_NULL                          0x0000			
#define PANEL_OLED_SH1106_I2C               0x0001			
#define PANEL_OLED_SSD1306_SPI              0x0002
#define PANEL_STNLCD_JLX12864_SPI           0x0004
#define PANEL_STNLCD_JLX128128_SPI          0x0008
//------------------------------
#if (PROJECT_SELECT==PROJECT_HMI_BUTTERFLY)
  #if (BOARD_SELECT==BOARD_STM32F401RET6_NUCLEO)
  #define PANEL_TYPE_SELECT		(PANEL_NULL)
  #elif (BOARD_SELECT==BOARD_STM32L476RGT6_NUCLEO)
  #define PANEL_TYPE_SELECT		(PANEL_OLED_SH1106_I2C|PANEL_STNLCD_JLX12864_SPI)
  #elif (BOARD_SELECT==BOARD_STM32L476ZET6_HMI_BUTTERFLY)
  #define PANEL_TYPE_SELECT		(PANEL_OLED_SH1106_I2C|PANEL_STNLCD_JLX128128_SPI)
  #else
  #define PANEL_TYPE_SELECT		(PANEL_NULL)
  #endif
#else
  #define PANEL_TYPE_SELECT		(PANEL_NULL)
#endif

//=====================================
// OPTION SWITCH 
//=====================================
#if (PROJECT_SELECT==PROJECT_HMI_BUTTERFLY)
  #define ENABLE_BLOTTER_LOGO
  #define ENABLE_OSD_LOGO
  #define ENABLE_OSD_ANIMATION
  #define ENABLE_OSD_GAME
#endif

#if defined(ENABLE_OSD_LOGO)
	#define LOGO_ON_PERIOD      3000 // unit:ms
#endif
	#define OSD_DURATION_TIME		3000 // unit:ms



//=====================================
// LED
//=====================================



//=====================================
// BUTTON
//=====================================




//=====================================
// TSC
//=====================================
#define TSC_TYPE_NULL                   0x0000
#define TSC_TYPE_MICROCHIP_CA1188_I2C   0x0001
#define TSC_TYPE_MICROCHIP_CA1214_I2C   0x0002
#define TSC_TYPE_MICROCHIP_MPR121_I2C   0x0004
//------------------------------
#if (PROJECT_SELECT==PROJECT_HMI_BUTTERFLY)
  #define TSC_TYPE_SELECT		TSC_TYPE_MICROCHIP_MPR121_I2C//TSC_TYPE_MICROCHIP_CA1214_I2C
#else
  #define TSC_TYPE_SELECT		TSC_TYPE_NULL
#endif

/*========================================================================================================
										T Y P E D E F
========================================================================================================*/




/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/




/*========================================================================================================
										F U N C T I O N
========================================================================================================*/



/*========================================================================================================
										End
========================================================================================================*/
#endif//_FLEX_PROJECT_CONFIG_H_






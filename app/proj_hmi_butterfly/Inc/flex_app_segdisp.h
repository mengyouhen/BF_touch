/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		flex_app_segdisp.h
 [Date]:
 		2018-05-20
 [Author]:
 		joanna.wang
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_APP_SEGDISP_H_
#define _FLEX_APP_SEGDISP_H_
/*========================================================================================================
										I N C L U D E
========================================================================================================*/
// FLEX
#include <stdbool.h>
#include "flex_common.h"

#include "stdint.h"
#include "gpio.h"
#include "flex_drv_eint.h"

/*========================================================================================================
										D E F I N E
========================================================================================================*/
//led segment display level: 0 - application level. 1 - interrupt level
#define LED_SEGMENT_DISP_LEVEL	0

/*========================================================================================================
										T Y P E D E F
========================================================================================================*/

typedef struct
{
	GPIO_TypeDef *nIntPort;
	uint16_t nIntPin;
	GPIO_TypeDef *nLedPort;
	uint16_t nLedPin;
	uint16_t nTemperature;
} int_led_str;



/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/


/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
void mech_btn_eint(uint16_t Pin);
int8_t SearchMechDiagPosition(void);
void StartSegDisplayTask(void const *argument);
#if (LED_SEGMENT_DISP_LEVEL)
void LedSegRefreshHandler(void);
#endif

#endif

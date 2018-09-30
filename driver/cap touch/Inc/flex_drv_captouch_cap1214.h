/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_drv_captouch_cap1214.h
 [Date]:       
 		2017-12-20
 [Author]:   	
 		carson.qian 
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_DRV_CAPTOUCH_CAP1214_H_
#define _FLEX_DRV_CAPTOUCH_CAP1214_H_

#ifdef __cplusplus 
extern "C" {
#endif

/*========================================================================================================
										I N C L U D E
========================================================================================================*/
#include "flex_common.h"
#include "flex_drv_captouch_def.h"
#include "flex_drv_led.h"



/*========================================================================================================
										D E F I N E
========================================================================================================*/




/*========================================================================================================
										T Y P E D E F
========================================================================================================*/






/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/







/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
#if (TSC_TYPE_SELECT&TSC_TYPE_MICROCHIP_CA1214_I2C)
extern const DRV_CAPTOUCH_FUNCS_ST cap1214_funcs;
#endif




/*========================================================================================================
										End
========================================================================================================*/
#ifdef __cplusplus 
}
#endif

#endif//_FLEX_DRV_CAPTOUCH_CAP1214_H_



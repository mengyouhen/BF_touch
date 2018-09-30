/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_drv_captouch_mpr121.h
 [Date]:       
 		2018-09-12
 [Author]:   	
 		Joanna.wang 
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_DRV_CAPTOUCH_MPR121_H_
#define _FLEX_DRV_CAPTOUCH_MPR121_H_

#ifdef __cplusplus 
extern "C" {
#endif

/*========================================================================================================
										I N C L U D E
========================================================================================================*/
#include "flex_common.h"
#include "flex_drv_captouch_def.h"



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
extern const DRV_CAPTOUCH_FUNCS_ST mpr121_funcs;
void mpr_press_detect(void);

/*========================================================================================================
										End
========================================================================================================*/
#ifdef __cplusplus 
}
#endif

#endif//_FLEX_DRV_CAPTOUCH_MPR121_H_


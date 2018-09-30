/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_app_osd_func.h
 [Date]:       
 		2018-05-23
 [Author]:   	
 		carson.qian 
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_APP_OSD_FUNC_H_
#define _FLEX_APP_OSD_FUNC_H_

#ifdef __cplusplus 
extern "C" {
#endif


/*========================================================================================================
										I N C L U D E
========================================================================================================*/
#include "flex_common.h"
#include "flex_app_osd_def.h"
#include "flex_app_osd_global.h"



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
#if 1
int16_t DecIncValue(OSD_MIA_ENUM action, int16_t Value, int16_t MinValue, int16_t MaxValue, uint8_t step);
int16_t DecIncValue_Cycle(OSD_MIA_ENUM action, int16_t Value, int16_t MinValue, int16_t MaxValue, uint8_t step);
#else
uint16_t DecIncValue(OSD_MIA_ENUM action, uint16_t Value, uint16_t MinValue, uint16_t MaxValue, uint8_t step);
uint16_t DecIncValue_Cycle(OSD_MIA_ENUM action, uint16_t Value, uint16_t MinValue, uint16_t MaxValue, uint8_t step);
#endif
//----------------------------------------
int16_t Osd_Value_Director(bool dynamic, OSD_KEY_FUNCTION_ENUM KeyFuncIdx, OSD_MIA_ENUM action);
//----------------------------------------
RTC_DateTypeDef Osd_Func_Get_Date(void);
RTC_TimeTypeDef Osd_Func_Get_Time(void);
/*========================================================================================================
										End
========================================================================================================*/
#ifdef __cplusplus 
}
#endif

#endif//_FLEX_APP_OSD_FUNC_H_



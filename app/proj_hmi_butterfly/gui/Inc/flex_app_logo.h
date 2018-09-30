/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_app_logo.h
 [Date]:       
 		2018-05-23
 [Author]:   	
 		carson.qian 
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_APP_LOGO_H_
#define _FLEX_APP_LOGO_H_

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




/*========================================================================================================
										T Y P E D E F
========================================================================================================*/






/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/







/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
#if defined(ENABLE_BLOTTER_LOGO)
void Flex_App_Draw_Blotter_Logo(void);

#endif

#if defined(ENABLE_OSD_LOGO)
void Flex_App_Draw_Osd_Logo(void);
#endif

/*========================================================================================================
										End
========================================================================================================*/
#ifdef __cplusplus 
}
#endif

#endif//_FLEX_APP_GUI_DEF_H_



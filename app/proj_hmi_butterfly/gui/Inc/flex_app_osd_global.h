/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_app_osd_global.h
 [Date]:       
 		2018-06-17
 [Author]:   	
 		carson.qian 
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_APP_OSD_GLOBAL_H_
#define _FLEX_APP_OSD_GLOBAL_H_

#ifdef __cplusplus 
extern "C" {
#endif


/*========================================================================================================
										I N C L U D E
========================================================================================================*/
#include "flex_common.h"
#include "flex_app_osd_def.h"



/*========================================================================================================
										D E F I N E
========================================================================================================*/
#define CURRENT_MENU            (tblMenus[g_osd_menu_data.nPage])
#define CURRENT_MENU_ITEMS      (CURRENT_MENU.nMenuItems)
#define CURRENT_MENU_ITEM_COUNT (CURRENT_MENU.nMenuItemCount)

#define CURRENT_MENU_ITEM       (CURRENT_MENU_ITEMS[g_osd_menu_data.nItem])
#define CURRENT_MENU_ITEM_FUNC  (CURRENT_MENU_ITEM.nKeyFuncIdx)


/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
typedef struct 
{
	OSD_PAGE_ENUM nPage;
	OSD_PAGE_ENUM nPrevPage;
	uint8_t nItem;
	uint8_t nPrevItem;
	FRM_INPUT_KEY_ST nKeyInfo;
	OSD_MIA_ENUM nMia;
#if defined(ENABLE_OSD_LOGO)
	uint8_t nLogoXpos;
	uint8_t nLogoYpos;
#endif	
}OSD_MENU_DATA_ST;





/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
extern OSD_MENU_DATA_ST g_osd_menu_data;




/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
void Osd_Init_Variable(void);






/*========================================================================================================
										End
========================================================================================================*/
#ifdef __cplusplus 
}
#endif

#endif//_FLEX_APP_OSD_GLOBAL_H_



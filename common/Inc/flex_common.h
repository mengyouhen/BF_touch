/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_common.h
 [Date]:       
 		2017-12-20
 [Author]:   	
 		carson.qian 
 [Reversion History]:
 		v1.0
*******************************************************************************/
#ifndef _FLEX_COMMON_H_
#define _FLEX_COMMON_H_

#ifdef __cplusplus 
extern "C" {
#endif


/*========================================================================================================
										I N C L U D E
========================================================================================================*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "flex_project_config.h"
#include "flex_types.h"
#include "flex_trace.h"

/*========================================================================================================
										D E F I N E
========================================================================================================*/
//=============================================================
// compare
//=============================================================
#define FLEX_MIN(A,B)		(((A)>=(B))?(B):(A))
#define FLEX_MAX(A,B)		(((A)>=(B))?(A):(B))
#define FLEX_DELTA(A,B)	(((A)>=(B))?((A)-(B)):((B)-(A)))
#define FLEX_ABS(x)			(((x)>=0)?(x):(-(x)))
//=============================================================
// bit operate
//=============================================================
#define _BIT0_	0x00000001
#define _BIT1_	0x00000002
#define _BIT2_	0x00000004
#define _BIT3_	0x00000008
#define _BIT4_	0x00000010
#define _BIT5_	0x00000020
#define _BIT6_	0x00000040
#define _BIT7_	0x00000080
#define _BIT8_	0x00000100
#define _BIT9_	0x00000200
#define _BIT10_	0x00000400
#define _BIT11_	0x00000800
#define _BIT12_	0x00001000
#define _BIT13_	0x00002000
#define _BIT14_	0x00004000
#define _BIT15_	0x00008000
#define _BIT16_	0x00010000
#define _BIT17_	0x00020000
#define _BIT18_	0x00040000
#define _BIT19_	0x00080000
#define _BIT20_	0x00100000
#define _BIT21_	0x00200000
#define _BIT22_	0x00400000
#define _BIT23_	0x00800000
#define _BIT24_	0x01000000
#define _BIT25_	0x02000000
#define _BIT26_	0x04000000
#define _BIT27_	0x08000000
#define _BIT28_	0x10000000
#define _BIT29_	0x20000000
#define _BIT30_	0x40000000
#define _BIT31_	0x80000000


#define _get_bit_(val, mask)    ((val) & _BIT##mask##_)
#define _set_bit_(val, mask)    ((val) | _BIT##mask##_)
#define _clr_bit_(val, mask)    ((val) & (~_BIT##mask##_))

// GET BIT
#define _bit0_(val)  ((val) & _BIT0_)
#define _bit1_(val)  ((val) & _BIT1_)
#define _bit2_(val)  ((val) & _BIT2_)
#define _bit3_(val)  ((val) & _BIT3_)
#define _bit4_(val)  ((val) & _BIT4_)
#define _bit5_(val)  ((val) & _BIT5_)
#define _bit6_(val)  ((val) & _BIT6_)
#define _bit7_(val)  ((val) & _BIT7_)
#define _bit8_(val)  ((val) & _BIT8_)
#define _bit9_(val)  ((val) & _BIT9_)
#define _bit10_(val) ((val) & _BIT10_)
#define _bit11_(val) ((val) & _BIT11_)
#define _bit12_(val) ((val) & _BIT12_)
#define _bit13_(val) ((val) & _BIT13_)
#define _bit14_(val) ((val) & _BIT14_)
#define _bit15_(val) ((val) & _BIT15_)
#define _bit16_(val) ((val) & _BIT16_)
#define _bit17_(val) ((val) & _BIT17_)
#define _bit18_(val) ((val) & _BIT18_)
#define _bit19_(val) ((val) & _BIT19_)
#define _bit20_(val) ((val) & _BIT20_)
#define _bit21_(val) ((val) & _BIT21_)
#define _bit22_(val) ((val) & _BIT22_)
#define _bit23_(val) ((val) & _BIT23_)
#define _bit24_(val) ((val) & _BIT24_)
#define _bit25_(val) ((val) & _BIT25_)
#define _bit26_(val) ((val) & _BIT26_)
#define _bit27_(val) ((val) & _BIT27_)
#define _bit28_(val) ((val) & _BIT28_)
#define _bit29_(val) ((val) & _BIT29_)
#define _bit30_(val) ((val) & _BIT30_)
#define _bit31_(val) ((val) & _BIT31_)

// SET BIT
#define _bit0_set_(val)  ((val) | _BIT0_)
#define _bit1_set_(val)  ((val) | _BIT1_)
#define _bit2_set_(val)  ((val) | _BIT2_)
#define _bit3_set_(val)  ((val) | _BIT3_)
#define _bit4_set_(val)  ((val) | _BIT4_)
#define _bit5_set_(val)  ((val) | _BIT5_)
#define _bit6_set_(val)  ((val) | _BIT6_)
#define _bit7_set_(val)  ((val) | _BIT7_)
#define _bit8_set_(val)  ((val) | _BIT8_)
#define _bit9_set_(val)  ((val) | _BIT9_)
#define _bit10_set_(val) ((val) | _BIT10_)
#define _bit11_set_(val) ((val) | _BIT11_)
#define _bit12_set_(val) ((val) | _BIT12_)
#define _bit13_set_(val) ((val) | _BIT13_)
#define _bit14_set_(val) ((val) | _BIT14_)
#define _bit15_set_(val) ((val) | _BIT15_)
#define _bit16_set_(val) ((val) | _BIT16_)
#define _bit17_set_(val) ((val) | _BIT17_)
#define _bit18_set_(val) ((val) | _BIT18_)
#define _bit19_set_(val) ((val) | _BIT19_)
#define _bit20_set_(val) ((val) | _BIT20_)
#define _bit21_set_(val) ((val) | _BIT21_)
#define _bit22_set_(val) ((val) | _BIT22_)
#define _bit23_set_(val) ((val) | _BIT23_)
#define _bit24_set_(val) ((val) | _BIT24_)
#define _bit25_set_(val) ((val) | _BIT25_)
#define _bit26_set_(val) ((val) | _BIT26_)
#define _bit27_set_(val) ((val) | _BIT27_)
#define _bit28_set_(val) ((val) | _BIT28_)
#define _bit29_set_(val) ((val) | _BIT29_)
#define _bit30_set_(val) ((val) | _BIT30_)
#define _bit31_set_(val) ((val) | _BIT31_)

// CLEAR BIT
#define _bit0_clr_(val)  ((val) & (~_BIT0_))
#define _bit1_clr_(val)  ((val) & (~_BIT1_))
#define _bit2_clr_(val)  ((val) & (~_BIT2_))
#define _bit3_clr_(val)  ((val) & (~_BIT3_))
#define _bit4_clr_(val)  ((val) & (~_BIT4_))
#define _bit5_clr_(val)  ((val) & (~_BIT5_))
#define _bit6_clr_(val)  ((val) & (~_BIT6_))
#define _bit7_clr_(val)  ((val) & (~_BIT7_))
#define _bit8_clr_(val)  ((val) & (~_BIT8_))
#define _bit9_clr_(val)  ((val) & (~_BIT9_))
#define _bit10_clr_(val) ((val) & (~_BIT10_))
#define _bit11_clr_(val) ((val) & (~_BIT11_))
#define _bit12_clr_(val) ((val) & (~_BIT12_))
#define _bit13_clr_(val) ((val) & (~_BIT13_))
#define _bit14_clr_(val) ((val) & (~_BIT14_))
#define _bit15_clr_(val) ((val) & (~_BIT15_))
#define _bit16_clr_(val) ((val) & (~_BIT16_))
#define _bit17_clr_(val) ((val) & (~_BIT17_))
#define _bit18_clr_(val) ((val) & (~_BIT18_))
#define _bit19_clr_(val) ((val) & (~_BIT19_))
#define _bit20_clr_(val) ((val) & (~_BIT20_))
#define _bit21_clr_(val) ((val) & (~_BIT21_))
#define _bit22_clr_(val) ((val) & (~_BIT22_))
#define _bit23_clr_(val) ((val) & (~_BIT23_))
#define _bit24_clr_(val) ((val) & (~_BIT24_))
#define _bit25_clr_(val) ((val) & (~_BIT25_))
#define _bit26_clr_(val) ((val) & (~_BIT26_))
#define _bit27_clr_(val) ((val) & (~_BIT27_))
#define _bit28_clr_(val) ((val) & (~_BIT28_))
#define _bit29_clr_(val) ((val) & (~_BIT29_))
#define _bit30_clr_(val) ((val) & (~_BIT30_))
#define _bit31_clr_(val) ((val) & (~_BIT31_))

//=============================================================
//
//=============================================================
#define pf_malloc  	pvPortMalloc
#define pf_free   	vPortFree
#define pf_memset 	memset
#define pf_memcpy 	memcpy


/*========================================================================================================
										T Y P E D E F
========================================================================================================*/






/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/






/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
uint8_t Common_Calc_Bits(int32_t num);
uint8_t Find_MaxValue(uint8_t* pData, uint8_t count);
uint8_t Find_MaxAbsValue_Index(int8_t* pData, uint8_t count);

uint16_t Find_Max_u16Value(uint16_t* pData, uint8_t count);
uint8_t Find_Max_u16Value_Index(uint16_t* pData, uint8_t count);

osStatus Queue_MsgPut (osMessageQId queue_id, void *message, uint32_t millisec);


bool atolEx(char *s, int32_t *value, int base);
bool atoulEx(char *s, uint32_t *value, int base);
char *strnchr(const char *s, char c);
bool CheckTimeElapsed(uint32_t preTick, uint32_t duration);


/*========================================================================================================
										End
========================================================================================================*/
#ifdef __cplusplus 
}
#endif

#endif//_FLEX_COMMON_H_


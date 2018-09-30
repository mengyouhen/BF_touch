/**
  ******************************************************************************
  * @file    TouchSensing/TouchSensing_Linear/Inc/tsl_user.h
  * @author  MCD Application Team
  * @brief   Header for tsl_user.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TSL_USER_H
#define __TSL_USER_H

#include "tsl.h"
#include "flex_project_config.h"
#if (BOARD_SELECT==BOARD_STM32L476ZET6_HMI_BUTTERFLY)||(BOARD_SELECT==BOARD_STM32L476RGT6_NUCLEO)
#include "stm32l4xx_hal.h"
#else
#include "stm32l0xx_hal.h"
#endif

/* Select if you use STMStudio software (0=No, 1=Yes) */
#define USE_STMSTUDIO (0)

#if USE_STMSTUDIO > 0
#include "stmCriticalSection.h"
#define STMSTUDIO_LOCK {enterLock();}
#define STMSTUDIO_UNLOCK {exitLock();}
#else
#define STMSTUDIO_LOCK
#define STMSTUDIO_UNLOCK
#endif

typedef enum
{
  TSL_USER_STATUS_BUSY       = 0, /**< The bank acquisition is on-going */
  TSL_USER_STATUS_OK_NO_ECS  = 1, /**< The bank acquisition is ok, no time for ECS */
  TSL_USER_STATUS_OK_ECS_ON  = 2, /**< The bank acquisition is ok, ECS finished */
  TSL_USER_STATUS_OK_ECS_OFF = 3  /**< The bank acquisition is ok, ECS not executed */
} tsl_user_status_t;

//////////////////////////////////////////////////////
/////////////////////  WHEEL   ///////////////////////
//////////////////////////////////////////////////////
/* Channel IOs definition */
#define WHEEL_CHN_0_IO_MSK    (TSC_GROUP4_IO2)
#define WHEEL_CHN_0_GRP_MSK   (TSC_GROUP4)
#define WHEEL_CHN_0_SRC       (TSC_GROUP4_IDX) 	/* Index in source register (TSC->IOGXCR[]) */
#define WHEEL_CHN_0_DEST      (0) 							/* Index in destination result array */

#define WHEEL_CHN_1_IO_MSK    (TSC_GROUP1_IO3)
#define WHEEL_CHN_1_GRP_MSK   (TSC_GROUP1)
#define WHEEL_CHN_1_SRC       (TSC_GROUP1_IDX)
#define WHEEL_CHN_1_DEST      (1)

#define WHEEL_CHN_2_IO_MSK    (TSC_GROUP5_IO3)
#define WHEEL_CHN_2_GRP_MSK   (TSC_GROUP5)
#define WHEEL_CHN_2_SRC       (TSC_GROUP5_IDX)
#define WHEEL_CHN_2_DEST      (2)

#define WHEEL_CHN_3_IO_MSK    (TSC_GROUP6_IO2)
#define WHEEL_CHN_3_GRP_MSK   (TSC_GROUP6)
#define WHEEL_CHN_3_SRC       (TSC_GROUP6_IDX)
#define WHEEL_CHN_3_DEST      (3)

#define WHEEL_CHN_4_IO_MSK    (TSC_GROUP2_IO2)
#define WHEEL_CHN_4_GRP_MSK   (TSC_GROUP2)
#define WHEEL_CHN_4_SRC       (TSC_GROUP2_IDX)
#define WHEEL_CHN_4_DEST      (4)

#define WHEEL_CHN_5_IO_MSK    (TSC_GROUP1_IO4)
#define WHEEL_CHN_5_GRP_MSK   (TSC_GROUP1)
#define WHEEL_CHN_5_SRC       (TSC_GROUP1_IDX)
#define WHEEL_CHN_5_DEST      (5)

//////////////////////////////////////////////////////
/////////////////////    KEY   ///////////////////////
//////////////////////////////////////////////////////
#define KEY_CHN_0_IO_MSK      (TSC_GROUP6_IO3)   // KEY_UP
#define KEY_CHN_0_GRP_MSK     (TSC_GROUP6)
#define KEY_CHN_0_SRC         (TSC_GROUP6_IDX)
#define KEY_CHN_0_DEST        (6)

#define KEY_CHN_1_IO_MSK      (TSC_GROUP5_IO4)   // KEY_RIGHT
#define KEY_CHN_1_GRP_MSK     (TSC_GROUP5)
#define KEY_CHN_1_SRC         (TSC_GROUP5_IDX)
#define KEY_CHN_1_DEST        (7)

//////////////////////////////////////////////////////
/////////////////////  SLIDE   ///////////////////////
//////////////////////////////////////////////////////
#define SLIDE_CHN_0_IO_MSK    (TSC_GROUP1_IO2)
#define SLIDE_CHN_0_GRP_MSK   (TSC_GROUP1)
#define SLIDE_CHN_0_SRC       (TSC_GROUP1_IDX) 	/* Index in source register (TSC->IOGXCR[]) */
#define SLIDE_CHN_0_DEST      (8) 							/* Index in destination result array */

#define SLIDE_CHN_1_IO_MSK    (TSC_GROUP4_IO3)
#define SLIDE_CHN_1_GRP_MSK   (TSC_GROUP4)
#define SLIDE_CHN_1_SRC       (TSC_GROUP4_IDX)
#define SLIDE_CHN_1_DEST      (9)

#define SLIDE_CHN_2_IO_MSK    (TSC_GROUP5_IO2)
#define SLIDE_CHN_2_GRP_MSK   (TSC_GROUP5)
#define SLIDE_CHN_2_SRC       (TSC_GROUP5_IDX)
#define SLIDE_CHN_2_DEST      (10)

//////////////////////////////////////////////////////
/* Shield IOs definition */
#define SHIELD_IO_MSK       (0)

/* Banks definition */
#define BANK_0_NBCHANNELS    	(4)
#define BANK_0_MSK_CHANNELS  	(WHEEL_CHN_0_IO_MSK  | WHEEL_CHN_1_IO_MSK  | WHEEL_CHN_2_IO_MSK  | WHEEL_CHN_3_IO_MSK  | SHIELD_IO_MSK)
#define BANK_0_MSK_GROUPS    	(WHEEL_CHN_0_GRP_MSK | WHEEL_CHN_1_GRP_MSK | WHEEL_CHN_2_GRP_MSK | WHEEL_CHN_3_GRP_MSK)

#define BANK_1_NBCHANNELS    	(4)
#define BANK_1_MSK_CHANNELS  	(WHEEL_CHN_4_IO_MSK  | WHEEL_CHN_5_IO_MSK  | KEY_CHN_0_IO_MSK  | KEY_CHN_1_IO_MSK | SHIELD_IO_MSK)
#define BANK_1_MSK_GROUPS    	(WHEEL_CHN_4_GRP_MSK | WHEEL_CHN_5_GRP_MSK | KEY_CHN_0_GRP_MSK | KEY_CHN_1_GRP_MSK)

#define BANK_2_NBCHANNELS    	(3)
#define BANK_2_MSK_CHANNELS  	(SLIDE_CHN_0_IO_MSK  | SLIDE_CHN_1_IO_MSK  | SLIDE_CHN_2_IO_MSK  | SHIELD_IO_MSK)
#define BANK_2_MSK_GROUPS    	(SLIDE_CHN_0_GRP_MSK | SLIDE_CHN_1_GRP_MSK | SLIDE_CHN_2_GRP_MSK)


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
/* User Parameters */
extern CONST TSL_Bank_T MyBanks[];
extern CONST TSL_LinRot_T MyLinRots[];
extern CONST TSL_TouchKey_T MyTKeys[];
extern CONST TSL_Object_T MyObjects[];
extern TSL_ObjectGroup_T MyObjGroup;

void tsl_user_Init(void);
tsl_user_status_t tsl_user_Exec(void);
void tsl_user_SetThresholds(void);

#endif /* __TSL_USER_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

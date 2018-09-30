/**
  ******************************************************************************
  * File Name          : main.hpp
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define MECH_DIAL_IN1_Pin GPIO_PIN_2
#define MECH_DIAL_IN1_GPIO_Port GPIOE
#define MCU_LED_SEGD_Pin GPIO_PIN_3
#define MCU_LED_SEGD_GPIO_Port GPIOE
#define MECH_DIAL_IN2_Pin GPIO_PIN_4
#define MECH_DIAL_IN2_GPIO_Port GPIOE
#define MECH_DIAL_IN3_Pin GPIO_PIN_5
#define MECH_DIAL_IN3_GPIO_Port GPIOE
#define MECH_DIAL_IN4_Pin GPIO_PIN_6
#define MECH_DIAL_IN4_GPIO_Port GPIOE
#define LED_WHEEL_1_Pin GPIO_PIN_13
#define LED_WHEEL_1_GPIO_Port GPIOC
#define MCU_LED_COM0_Pin GPIO_PIN_2
#define MCU_LED_COM0_GPIO_Port GPIOF
#define MECH_DIAL_IN8_Pin GPIO_PIN_3
#define MECH_DIAL_IN8_GPIO_Port GPIOF
#define MCU_LED_COM1_Pin GPIO_PIN_4
#define MCU_LED_COM1_GPIO_Port GPIOF
#define MCU_LED_COM2_Pin GPIO_PIN_5
#define MCU_LED_COM2_GPIO_Port GPIOF
#define MCU_LED_COM3_Pin GPIO_PIN_6
#define MCU_LED_COM3_GPIO_Port GPIOF
#define LED_UP_Pin GPIO_PIN_7
#define LED_UP_GPIO_Port GPIOF
#define LED_LEFT_Pin GPIO_PIN_8
#define LED_LEFT_GPIO_Port GPIOF
#define LED_RIGHT_Pin GPIO_PIN_9
#define LED_RIGHT_GPIO_Port GPIOF
#define MS_BTN2_Pin GPIO_PIN_10
#define MS_BTN2_GPIO_Port GPIOF
#define MS_BTN2_EXTI_IRQn EXTI15_10_IRQn
#define TLC_LAT_Pin GPIO_PIN_0
#define TLC_LAT_GPIO_Port GPIOC
#define MECH_DIAL_IN7_Pin GPIO_PIN_1
#define MECH_DIAL_IN7_GPIO_Port GPIOC
#define TLC_BLANK_Pin GPIO_PIN_2
#define TLC_BLANK_GPIO_Port GPIOC
#define STNLCD_LEDA_Pin GPIO_PIN_0
#define STNLCD_LEDA_GPIO_Port GPIOA
#define MS_LED1_Pin GPIO_PIN_4
#define MS_LED1_GPIO_Port GPIOA
#define MS_LED2_Pin GPIO_PIN_5
#define MS_LED2_GPIO_Port GPIOA
#define MECH_DIAL_LED6_Pin GPIO_PIN_4
#define MECH_DIAL_LED6_GPIO_Port GPIOC
#define MECH_DIAL_LED7_Pin GPIO_PIN_5
#define MECH_DIAL_LED7_GPIO_Port GPIOC
#define LED_WHEEL_4_Pin GPIO_PIN_2
#define LED_WHEEL_4_GPIO_Port GPIOB
#define MCU_RotaryEncoder_A_Pin GPIO_PIN_11
#define MCU_RotaryEncoder_A_GPIO_Port GPIOF
#define MCU_RotaryEncoder_A_EXTI_IRQn EXTI15_10_IRQn
#define MCU_RotaryEncoder_B_Pin GPIO_PIN_12
#define MCU_RotaryEncoder_B_GPIO_Port GPIOF
#define MCU_RotaryEncoder_B_EXTI_IRQn EXTI15_10_IRQn
#define MS_BTN1_Pin GPIO_PIN_13
#define MS_BTN1_GPIO_Port GPIOF
#define MS_BTN1_EXTI_IRQn EXTI15_10_IRQn
#define MECH_BTN1_Pin GPIO_PIN_14
#define MECH_BTN1_GPIO_Port GPIOF
#define MECH_BTN1_EXTI_IRQn EXTI15_10_IRQn
#define MECH_BTN2_Pin GPIO_PIN_15
#define MECH_BTN2_GPIO_Port GPIOF
#define MECH_BTN2_EXTI_IRQn EXTI15_10_IRQn
#define MECH_BTN2_LN_Pin GPIO_PIN_0
#define MECH_BTN2_LN_GPIO_Port GPIOG
#define LED_SLIDE_2_Pin GPIO_PIN_1
#define LED_SLIDE_2_GPIO_Port GPIOG
#define MECH_DIAL_IN5_Pin GPIO_PIN_7
#define MECH_DIAL_IN5_GPIO_Port GPIOE
#define MCU_RotaryEncoder_PB_Pin GPIO_PIN_8
#define MCU_RotaryEncoder_PB_GPIO_Port GPIOE
#define MCU_RotaryEncoder_PB_EXTI_IRQn EXTI9_5_IRQn
#define MCU_LCD_BKLT_Pin GPIO_PIN_9
#define MCU_LCD_BKLT_GPIO_Port GPIOE
#define TLC_SCLK_Pin GPIO_PIN_10
#define TLC_SCLK_GPIO_Port GPIOB
#define LED_WHEEL_2_Pin GPIO_PIN_11
#define LED_WHEEL_2_GPIO_Port GPIOB
#define MECH_DIAL_LED8_Pin GPIO_PIN_8
#define MECH_DIAL_LED8_GPIO_Port GPIOD
#define STNLCD_RS_Pin GPIO_PIN_9
#define STNLCD_RS_GPIO_Port GPIOD
#define LED_SLIDE_1_Pin GPIO_PIN_13
#define LED_SLIDE_1_GPIO_Port GPIOD
#define STNLCD_RST_Pin GPIO_PIN_14
#define STNLCD_RST_GPIO_Port GPIOD
#define MCU_LED_SEGA_Pin GPIO_PIN_15
#define MCU_LED_SEGA_GPIO_Port GPIOD
#define MCU_RotaryEncoder_LED_N_Pin GPIO_PIN_3
#define MCU_RotaryEncoder_LED_N_GPIO_Port GPIOG
#define MECH_DIAL_LED2_Pin GPIO_PIN_4
#define MECH_DIAL_LED2_GPIO_Port GPIOG
#define MECH_DIAL_LED3_Pin GPIO_PIN_6
#define MECH_DIAL_LED3_GPIO_Port GPIOG
#define MECH_DIAL_LED5_Pin GPIO_PIN_7
#define MECH_DIAL_LED5_GPIO_Port GPIOG
#define MECH_DIAL_LED1_Pin GPIO_PIN_8
#define MECH_DIAL_LED1_GPIO_Port GPIOG
#define MPR_INT_Pin GPIO_PIN_9
#define MPR_INT_GPIO_Port GPIOC
#define MPR_INT_EXTI_IRQn EXTI9_5_IRQn
#define LED_WHEEL_7_Pin GPIO_PIN_13
#define LED_WHEEL_7_GPIO_Port GPIOA
#define LED_WHEEL_6_Pin GPIO_PIN_14
#define LED_WHEEL_6_GPIO_Port GPIOA
#define LED_WHEEL_5_Pin GPIO_PIN_15
#define LED_WHEEL_5_GPIO_Port GPIOA
#define MCU_LED_SEGE_Pin GPIO_PIN_10
#define MCU_LED_SEGE_GPIO_Port GPIOC
#define MCU_LED_SEGF_Pin GPIO_PIN_11
#define MCU_LED_SEGF_GPIO_Port GPIOC
#define MCU_LED_SEGG_Pin GPIO_PIN_12
#define MCU_LED_SEGG_GPIO_Port GPIOC
#define LED_SLIDE_4_Pin GPIO_PIN_0
#define LED_SLIDE_4_GPIO_Port GPIOD
#define LED_SLIDE_3_Pin GPIO_PIN_1
#define LED_SLIDE_3_GPIO_Port GPIOD
#define MCU_LED_SEGDX_Pin GPIO_PIN_2
#define MCU_LED_SEGDX_GPIO_Port GPIOD
#define STNLCD_CS_Pin GPIO_PIN_3
#define STNLCD_CS_GPIO_Port GPIOD
#define TLC_SIN_Pin GPIO_PIN_4
#define TLC_SIN_GPIO_Port GPIOD
#define LED_OK_Pin GPIO_PIN_7
#define LED_OK_GPIO_Port GPIOD
#define MECH_DIAL_LED4_Pin GPIO_PIN_15
#define MECH_DIAL_LED4_GPIO_Port GPIOG
#define MECH_BTN1_LN_Pin GPIO_PIN_6
#define MECH_BTN1_LN_GPIO_Port GPIOB
#define MCU_BEEP_Pin GPIO_PIN_7
#define MCU_BEEP_GPIO_Port GPIOB
#define LED_WHEEL_3_Pin GPIO_PIN_8
#define LED_WHEEL_3_GPIO_Port GPIOB
#define MCU_LED_SEGB_Pin GPIO_PIN_0
#define MCU_LED_SEGB_GPIO_Port GPIOE
#define MCU_LED_SEGC_Pin GPIO_PIN_1
#define MCU_LED_SEGC_GPIO_Port GPIOE

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

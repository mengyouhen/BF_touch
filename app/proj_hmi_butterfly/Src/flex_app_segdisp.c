/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		flex_app_segdisp.c
 [Date]:
 		2018-05-20
 [Author]:
 		joanna.wang
 [Reversion History]:
 		v1.0
*******************************************************************************/
/*========================================================================================================
										I N C L U D E
========================================================================================================*/
#include <stdio.h>
#include <string.h>
#include "flex_drv_beeper.h"
#include "flex_drv_seg.h"
#include "flex_app_segdisp.h"
#include "stm32l4xx_hal.h"



/*========================================================================================================
										D E F I N E
========================================================================================================*/
#define TASK_SIGNAL_POLL_INPUT_MASK	_BIT0_
#if (LED_SEGMENT_DISP_LEVEL == 0)
#define TASK_SIGNAL_LEDDISP_MASK		_BIT1_
#endif

//mechnical diag input poll interval in ms
#define MECH_DIAG_IN_POLL_INTERVAL		200

#define MAX_COUNTER_VALUE	9999

//led segment display interval in ms,frame rate = 1000/(5*SEG_LEDDISP_INTERVAL)
#define LED_SEG_DISP_INTERVAL  2
//blink interval in ms
#define BLINK_INTERVAL  400

//beep on/off duration in ms
#define BEEP_ON_DURATION	200
#define BEEP_OFF_DURATION	200

//rotary led on duration in ms
#define ROTARY_LED_DURATION	100


/*========================================================================================================
										E X T E R N
========================================================================================================*/


/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
typedef enum
{
	COUNT_DOWN_ON = 0,
	COUNT_DOWN_STOP,
	COUNT_DOWN_CLEAR,
	COUNT_DOWN_OFF,
} COUNT_DOWN_STATE;

typedef enum
{
	BEEP_ON = 0,
	BEEP_OFF,
	BEEP_STOP,
} BEEP_STATE;


/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
static osThreadId segDisplayTaskHandle = NULL;
static osTimerId counterDownTimerHandle;
#if (LED_SEGMENT_DISP_LEVEL == 0)
static osTimerId segLedDispTimerHandle;
#endif
static osTimerId diagInPollTimerHandle;
static osTimerId beepTimerHandle;
static osTimerId rotaryLedTimerHandle;


static uint16_t counter;
static uint8_t rotaryAChgNum = 0;
static uint8_t rotaryBChgNum = 0;
static bool lastRotaryALevel;
static bool lastRotaryBLevel;
//static uint32_t rotaryATick = 0;
//static uint32_t rotaryBTick = 0;
static uint32_t pushBtnTick = 0;
static uint8_t blinkPos = SEG_POS_MAX;
static COUNT_DOWN_STATE countDownState = COUNT_DOWN_OFF;
static uint8_t beepCount;
static BEEP_STATE beepState = BEEP_STOP;

const int_led_str int_led_list[] =
{
	{MECH_DIAL_IN5_GPIO_Port, MECH_DIAL_IN5_Pin, MECH_DIAL_LED1_GPIO_Port, MECH_DIAL_LED1_Pin, 0},
	//{MECH_DIAL_IN6_GPIO_Port, MECH_DIAL_IN6_Pin, MECH_DIAL_LED2_GPIO_Port, MECH_DIAL_LED2_Pin, 50},
	{MECH_DIAL_IN7_GPIO_Port, MECH_DIAL_IN7_Pin, MECH_DIAL_LED3_GPIO_Port, MECH_DIAL_LED3_Pin, 100},
	{MECH_DIAL_IN8_GPIO_Port, MECH_DIAL_IN8_Pin, MECH_DIAL_LED4_GPIO_Port, MECH_DIAL_LED4_Pin, 150},
	{MECH_DIAL_IN1_GPIO_Port, MECH_DIAL_IN1_Pin, MECH_DIAL_LED5_GPIO_Port, MECH_DIAL_LED5_Pin, 200},
	{MECH_DIAL_IN2_GPIO_Port, MECH_DIAL_IN2_Pin, MECH_DIAL_LED6_GPIO_Port, MECH_DIAL_LED6_Pin, 250},
	{MECH_DIAL_IN3_GPIO_Port, MECH_DIAL_IN3_Pin, MECH_DIAL_LED7_GPIO_Port, MECH_DIAL_LED7_Pin, 300},
	{MECH_DIAL_IN4_GPIO_Port, MECH_DIAL_IN4_Pin, MECH_DIAL_LED8_GPIO_Port, MECH_DIAL_LED8_Pin, 350},
};


/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
static void PollMechDiagInput(void);
static void PollDiagInputTimerCallback(void const *argument);
static void CountDownTimerCallback(void const *argument);
#if (LED_SEGMENT_DISP_LEVEL == 0)
static void LedSegDispTimerCallback(void const *argument);
#endif
static void BeepTimerCallback(void const *argument);
static void ShowFahrenheitOnLcdSeg(uint16_t temperature);
static void ShowCounterOnLedSeg(void);
static void UpdateCounter(uint8_t nPOS, bool IsIncrease);
static inline void TurnOnLedSegDisplay(void);
static inline void SetCountDownState(COUNT_DOWN_STATE state);
static void SetBeepState(BEEP_STATE state);
static void StartBeep(uint8_t repeat);
static void RotaryLedTimerCallback(void const *argument);



/*!
*\brief show temperature in fahrenheith on LCD segment display
*\param [in] temperature - temperature in fahrenheith
*
*\retval none
*/
static void ShowFahrenheitOnLcdSeg(uint16_t temperature)
{
	int8_t i;
	char buffer[SEG_POS_MAX + 1];
	SEGMENT_POS pos;
	static uint16_t lastTemp = (uint16_t)0xffff;

	//only update if temperature changes
	if (lastTemp != temperature)
	{
		//check if lcd segment is ready to update
		if (LcdSegmentReadyToUpdate())
		{
			lastTemp = temperature;

			if (0 == temperature)
			{
				//turn off LCD backlight
				EnableLcdBacklight(false);
				//clear Lcd segments
				ClearLcdSegmentsDisplay(true);
			}
			else
			{
				snprintf(buffer, SEG_POS_MAX + 1, "%dF", temperature);

				//display on right alignment
				pos = SEG_POS3;
				i = strlen(buffer) - 1;
				//clear LCD segments
				ClearLcdSegmentsDisplay(false);

				while (i >= 0)
				{
					//flush device only when whole content is updated to lcd ram
					LcdSegmentDisplay(pos, buffer[i], (i == 0));
					i--;
					pos--;
				}

				//turn on LCD backlight
				EnableLcdBacklight(true);
			}
		}
	}
}

/*!
*\brief search mechanical diag position
*\retval <0 - none, e.g. disconnecton
*\retval others - position from 0
*
*/
int8_t SearchMechDiagPosition(void)
{
	for (int8_t j = 0; j < sizeof(int_led_list) / sizeof(int_led_str); j++)
	{
		if (HAL_GPIO_ReadPin(int_led_list[j].nIntPort, int_led_list[j].nIntPin) == GPIO_PIN_RESET)
		{
			return j;
		}
	}

	return -1;
}

/*!
*\brief poll input of mechanical diag input
*
*/
static void PollMechDiagInput(void)
{
	int8_t j;
	static int8_t lastPos = -1;
	int8_t position = SearchMechDiagPosition();

	if (lastPos != position)
	{
		lastPos = position;

		if (position < 0)
		{
			//force to identital to off state
			position = 0;
		}

		if (0 == position)
		{
#if 1 // carson 2018-07-27
			// turn on leftmost led, and turn off other LEDs
			HAL_GPIO_WritePin(int_led_list[0].nLedPort, int_led_list[0].nLedPin, GPIO_PIN_RESET);

			for (j = 1; j < sizeof(int_led_list) / sizeof(int_led_str); j++)
			{
				HAL_GPIO_WritePin(int_led_list[j].nLedPort, int_led_list[j].nLedPin, GPIO_PIN_SET);
			}

#else

			//turn off all LEDs
			for (j = 0; j < sizeof(int_led_list) / sizeof(int_led_str); j++)
			{
				HAL_GPIO_WritePin(int_led_list[j].nLedPort, int_led_list[j].nLedPin, GPIO_PIN_SET);
			}

#endif
		}
		else
		{
			//turn off remaining LEDs
			for (j = position + 1; j <  sizeof(int_led_list) / sizeof(int_led_str); j++)
			{
				HAL_GPIO_WritePin(int_led_list[j].nLedPort, int_led_list[j].nLedPin, GPIO_PIN_SET);
			}

			//turn on all LEDs from 0 to position
			for (j = 0; j <= position; j++)
			{
				HAL_GPIO_WritePin(int_led_list[j].nLedPort, int_led_list[j].nLedPin, GPIO_PIN_RESET);
			}
		}

		ShowFahrenheitOnLcdSeg(int_led_list[position].nTemperature);
	}
}

#if (LED_SEGMENT_DISP_LEVEL)
/*!
*\brief led segment refresh service in interrupt mode
*\note this function is invoked from 1ms tick interrupt serivice routine
*
*/
void LedSegRefreshHandler(void)
{
	static uint32_t segLedDispTick = 0;

	if (COUNT_DOWN_OFF != countDownState)
	{
		if (CheckTimeElapsed(segLedDispTick, LED_SEG_DISP_INTERVAL))
		{
			ShowCounterOnLedSeg();
			segLedDispTick = osKernelSysTick();
		}
	}
}
#endif

/*!
*\brief show counter on led segment display
*
*/
static void ShowCounterOnLedSeg(void)
{
	static SEGMENT_POS pos = SEG_POS0;
	static char buffer[SEG_POS_MAX + 1];
	static uint16_t lastCounter = (uint16_t)0xffff;
	static uint16_t blinkTime = 0;
	static bool blinkPosOn = true;
	char character;

	if (lastCounter != counter)
	{
		lastCounter = counter;
		snprintf(buffer, SEG_POS_MAX + 1, "%04d", counter);
	}

	if (pos < SEG_POS_MAX)
	{
		if ((pos == blinkPos) && (!blinkPosOn))
		{
			//turn off the segment
			character = NULL;
		}
		else
		{
			character = buffer[pos];
		}

		LedSegmentDisplay(pos, character);
		pos++;
	}
	else
	{
		if (SEG_POS_MAX == pos)
		{
			//show '.'
			LedSegmentDisplay(SEG_POS0, '.');
		}

		pos = SEG_POS0;
	}

	blinkTime += LED_SEG_DISP_INTERVAL;

	if (blinkTime > BLINK_INTERVAL)
	{
		blinkPosOn = !blinkPosOn ;
		blinkTime = 0;
	}
}

/*!
*\brief cycle led segment blink position.
*\param [in] state - true to cycle to next position and false to disable blink.
*
*\retval none
*/
static void CycleBlinkPos(bool state)
{
	if (state)
	{
		//cycle to next position
		blinkPos++;

		if (blinkPos > SEG_POS_MAX)
		{
			//turn aournd to position 0
			blinkPos = 0;
		}
	}
	else
	{
		//remove selection
		blinkPos = SEG_POS_MAX;
	}
}

/*!
*\brief set count down state.
*\param [in] state - count down state
*\retval none
*/
static inline void SetCountDownState(COUNT_DOWN_STATE state)
{
	countDownState = state;
}

/*!
*\brief turn on led segment display.
*/
static inline void TurnOnLedSegDisplay(void)
{
#if (LED_SEGMENT_DISP_LEVEL == 0)

	if (COUNT_DOWN_OFF == countDownState)
	{
		//turn on display

		osTimerStart(segLedDispTimerHandle, LED_SEG_DISP_INTERVAL);
	}

#endif
}

/*!
*\brief cycle to next count down state.
*
*\retval none
*/
static void CycleCountDownState(void)
{
	switch (countDownState)
	{
		case COUNT_DOWN_ON:
			osTimerStop(counterDownTimerHandle);
			SetCountDownState(COUNT_DOWN_STOP);
			StartBeep(1);
			break;

		case COUNT_DOWN_STOP:
			SetCountDownState(COUNT_DOWN_CLEAR);

			if (0 != counter)
			{
				counter = 0;
				StartBeep(1);
			}
			else
			{
				//move to count down off state immediately if counter is already 0
				CycleCountDownState();
			}

			break;

		case COUNT_DOWN_CLEAR:
			//turn off all led segment
			ClearLedSegmentsDisplay();
			//disable adjustment selection
			CycleBlinkPos(false);
#if (LED_SEGMENT_DISP_LEVEL == 0)
			//stop led segment dispaly timer
			osTimerStop(segLedDispTimerHandle);
#endif
			SetCountDownState(COUNT_DOWN_OFF);
			StartBeep(1);
			break;

		default:
			break;
	}
}

/*!
*\brief Start counting down¡£
*
*\retval none
*/
static void StartCountingDown(void)
{
	if (COUNT_DOWN_ON != countDownState)
	{
		if (counter != 0)
		{
			//disable adjustment selection
			CycleBlinkPos(false);
			//start counting down
			osTimerStart(counterDownTimerHandle, LED_SEG_DISP_INTERVAL);
			SetCountDownState(COUNT_DOWN_ON);
			StartBeep(1);
		}
	}
}

/*!
*\brief update count down counter according to current adjustment position.
*\param [in] nPOS - current adjustment position
*\param [in] IsIncrease - true for incremental and false for decremental
*\retval none
*/
static void UpdateCounter(uint8_t nPOS, bool IsIncrease)
{
	int16_t temp;

	if (IsIncrease)
	{
		HAL_GPIO_WritePin(MCU_RotaryEncoder_LED_N_GPIO_Port, MCU_RotaryEncoder_LED_N_Pin, GPIO_PIN_SET );
//		HAL_GPIO_WritePin(MCU_RotaryEncoder_LED_P_GPIO_Port, MCU_RotaryEncoder_LED_P_Pin, GPIO_PIN_RESET );
	}
	else
	{
		HAL_GPIO_WritePin(MCU_RotaryEncoder_LED_N_GPIO_Port, MCU_RotaryEncoder_LED_N_Pin, GPIO_PIN_RESET );
//		HAL_GPIO_WritePin(MCU_RotaryEncoder_LED_P_GPIO_Port, MCU_RotaryEncoder_LED_P_Pin, GPIO_PIN_SET );
	}

	osTimerStart(rotaryLedTimerHandle, ROTARY_LED_DURATION);

	if (nPOS < SEG_POS_MAX)
	{
		switch (nPOS)
		{
			case SEG_POS0:
				temp = 1000;
				break;

			case SEG_POS1:
				temp = 100;
				break;

			case SEG_POS2:
				temp = 10;
				break;

			default:
				temp = 1;
				break;
		}

		temp = (IsIncrease) ? (counter + temp) : (counter - temp);

		if ((temp >= 0) && (temp <= MAX_COUNTER_VALUE))
		{
			counter = temp;
		}
	}

	if (COUNT_DOWN_ON == countDownState)
	{
		//stop counting down if it is going
		CycleCountDownState();
	}

	TurnOnLedSegDisplay();

	//force to COUNT_DOWN_STOP
	SetCountDownState(COUNT_DOWN_STOP);
}

/*!
*\brief task to display LCD & LED segment
*\param [in] argument - argument pointer
*\retval none
*/
void StartSegDisplayTask(void const *argument)
{
	osEvent event;

	osTimerDef(DiagInPollTimer, PollDiagInputTimerCallback);
	diagInPollTimerHandle = osTimerCreate(osTimer(DiagInPollTimer), osTimerPeriodic, NULL);
	assert_param(diagInPollTimerHandle != NULL);

	/* definition and creation of CounterDownTimer */
	osTimerDef(CounterDownTimer, CountDownTimerCallback);
	counterDownTimerHandle = osTimerCreate(osTimer(CounterDownTimer), osTimerPeriodic, NULL);
	assert_param(counterDownTimerHandle != NULL);

#if (LED_SEGMENT_DISP_LEVEL == 0)
	/* definition and creation of SegLedStaticDispTimer */
	osTimerDef(segLedDispTimer, LedSegDispTimerCallback);
	segLedDispTimerHandle = osTimerCreate(osTimer(segLedDispTimer), osTimerPeriodic, NULL);
	assert_param(segLedDispTimerHandle != NULL);
#endif

	osTimerDef(beepTimer, BeepTimerCallback);
	beepTimerHandle = osTimerCreate(osTimer(beepTimer), osTimerOnce, NULL);
	assert_param(beepTimerHandle != NULL);
	SetBeepState(BEEP_STOP);

	osTimerDef(rotaryLedTimer, RotaryLedTimerCallback);
	rotaryLedTimerHandle = osTimerCreate(osTimer(rotaryLedTimer), osTimerOnce, NULL);
	assert_param(rotaryLedTimerHandle != NULL);

	//init count down counter
	counter = 0;

	//start periodical timer
	osTimerStart(diagInPollTimerHandle, MECH_DIAG_IN_POLL_INTERVAL);

	//segDisplayTaskHandle is also a flag that task is ready
	segDisplayTaskHandle = osThreadGetId();

	/* Infinite loop */
	for (;;)
	{
		event = osSignalWait(0x7FFFFFFF, osWaitForever);

		if (event.status == osEventSignal)
		{
#if (LED_SEGMENT_DISP_LEVEL == 0)

			if (event.value.signals & TASK_SIGNAL_LEDDISP_MASK)
			{
				ShowCounterOnLedSeg();
			}

#endif

			if ((event.value.signals) & TASK_SIGNAL_POLL_INPUT_MASK)
			{
				PollMechDiagInput();
			}
		}
	}

	/* USER CODE END StartSegDisplayTask */
}

/*!
*\brief start beeping
*\param [in] repeat - beep pulse repeat time
*/
static void StartBeep(uint8_t repeat)
{
	beepCount = repeat;
	SetBeepState(BEEP_ON);
}

/*!
*\brief set beep state
*\param [in] state - beep state
*/
static void SetBeepState(BEEP_STATE state)
{
	if (BEEP_OFF == state)
	{
		beeper_off();
		osTimerStart(beepTimerHandle, BEEP_OFF_DURATION);
		beepState = BEEP_OFF;
	}
	else if (BEEP_ON == state)
	{
		beeper_on();
		osTimerStart(beepTimerHandle, BEEP_ON_DURATION);
		beepState = BEEP_ON;
	}
	else
	{
		beeper_off();
		beepState = BEEP_STOP;
	}
}


/*!
*\brief beep timer callback
*\param [in] argument - argument pointer
*/
static void BeepTimerCallback(void const *argument)
{
	if (BEEP_ON == beepState)
	{
		beepCount--;

		if (0 == beepCount)
		{
			SetBeepState(BEEP_STOP);
		}
		else
		{
			SetBeepState(BEEP_OFF);
		}
	}
	else if (BEEP_OFF == beepState)
	{
		SetBeepState(BEEP_ON);
	}
}

/*!
*\brief Poll Diag Input periodic timer callback
*\param [in] argument - argument pointer
*/
static void PollDiagInputTimerCallback(void const *argument)
{
	osSignalSet(segDisplayTaskHandle, TASK_SIGNAL_POLL_INPUT_MASK);
}

/*!
*\brief counting down timer callback
*\param [in] argument - argument pointer
*/
static void CountDownTimerCallback(void const *argument)
{
	counter--;

	if ( 0 == counter)
	{
		//counting down to zero
		osTimerStop(counterDownTimerHandle);
		SetCountDownState(COUNT_DOWN_STOP);
		StartBeep(4);
	}
}

#if (LED_SEGMENT_DISP_LEVEL == 0)
/*!
*\brief led segment display timer callback
*\param [in] argument - argument pointer
*/
static void LedSegDispTimerCallback(void const *argument)
{
	osSignalSet(segDisplayTaskHandle, TASK_SIGNAL_LEDDISP_MASK);
}
#endif

/*!
*\brief rotary led timer callback
*\param [in] argument - argument pointer
*/
static void RotaryLedTimerCallback(void const *argument)
{
	//turn off both direction leds
	HAL_GPIO_WritePin(MCU_RotaryEncoder_LED_N_GPIO_Port, MCU_RotaryEncoder_LED_N_Pin, GPIO_PIN_SET );
//	HAL_GPIO_WritePin(MCU_RotaryEncoder_LED_P_GPIO_Port, MCU_RotaryEncoder_LED_P_Pin, GPIO_PIN_SET );
}

/*!
*\brief GPIO pin interrupt handler
*\param [in] Pin - interrupt pin
*\retval none
*/
void mech_btn_eint(uint16_t Pin)
{
	if (!segDisplayTaskHandle)
	{
		//task is not initialized yet
		return;
	}

	switch (Pin)
	{
		case MECH_BTN1_Pin:

			//stop button
			if (!HAL_GPIO_ReadPin(MECH_BTN1_GPIO_Port, MECH_BTN1_Pin))
			{
				HAL_GPIO_WritePin(MECH_BTN1_LN_GPIO_Port, MECH_BTN1_LN_Pin, GPIO_PIN_RESET);
				CycleCountDownState();
			}
			else
			{
				HAL_GPIO_WritePin(MECH_BTN1_LN_GPIO_Port, MECH_BTN1_LN_Pin, GPIO_PIN_SET);
			}

			break;

		case MECH_BTN2_Pin:

			//start button
			if (!HAL_GPIO_ReadPin(MECH_BTN2_GPIO_Port, MECH_BTN2_Pin))
			{
				HAL_GPIO_WritePin(MECH_BTN2_LN_GPIO_Port, MECH_BTN2_LN_Pin, GPIO_PIN_RESET);
				StartCountingDown();
			}
			else
			{
				HAL_GPIO_WritePin(MECH_BTN2_LN_GPIO_Port, MECH_BTN2_LN_Pin, GPIO_PIN_SET);
			}

			break;

		case MCU_RotaryEncoder_PB_Pin:
			if (CheckTimeElapsed(pushBtnTick, 50))
			{
				//RotaryEncoder_PB
				if (!HAL_GPIO_ReadPin(MCU_RotaryEncoder_PB_GPIO_Port, MCU_RotaryEncoder_PB_Pin))
				{
					if ((COUNT_DOWN_ON != countDownState) && (COUNT_DOWN_OFF != countDownState) )
					{
						//move to the next blinking position
						CycleBlinkPos(true);
					}
				}
			}

			pushBtnTick = osKernelSysTick();

			break;

		case MCU_RotaryEncoder_A_Pin:
			//if (CheckTimeElapsed(rotaryATick, 1))
			{
				lastRotaryALevel = HAL_GPIO_ReadPin(MCU_RotaryEncoder_A_GPIO_Port, MCU_RotaryEncoder_A_Pin);

				//RotaryEncoder_A
				if (0 != rotaryBChgNum)
				{
					if (lastRotaryBLevel == lastRotaryALevel)
					{
						UpdateCounter(blinkPos, false); //the posation number decrease.
						rotaryAChgNum = 0;
						rotaryBChgNum = 0;
					}
					else
					{
						//discard previous rotatry B pin change
						rotaryBChgNum = 0;
						rotaryAChgNum = 1;
					}
				}
				else
				{
					rotaryAChgNum = 1;
				}
			}

			//rotaryATick = osKernelSysTick();

			break;

		case MCU_RotaryEncoder_B_Pin:
			//if (CheckTimeElapsed(rotaryBTick, 1))
			{
				lastRotaryBLevel = HAL_GPIO_ReadPin(MCU_RotaryEncoder_B_GPIO_Port, MCU_RotaryEncoder_B_Pin);

				//RotaryEncoder_B
				if (0 != rotaryAChgNum)
				{
					if (lastRotaryBLevel == lastRotaryALevel)
					{
						UpdateCounter(blinkPos, true); //the position number incremetnal.
						rotaryAChgNum = 0;
						rotaryBChgNum = 0;
					}
					else
					{
						//discard previous rotatry A pin change
						rotaryAChgNum = 0;
						rotaryBChgNum = 1;
					}
				}
				else
				{
					rotaryBChgNum = 1;
				}
			}

			//rotaryBTick = osKernelSysTick();

			break;

		default:
			break;
	}
}


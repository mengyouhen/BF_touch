/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		flex_common.c
 [Date]:
 		2017-12-20
 [Author]:
 		carson.qian
 [Reversion History]:
 		v1.0
*******************************************************************************/

/*========================================================================================================
										I N C L U D E
========================================================================================================*/
// external


// internal
#include "flex_common.h"

/*========================================================================================================
										E X T E R N
========================================================================================================*/






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
uint8_t Common_Calc_Bits(int32_t num)
{
	uint8_t bits = 0;

	if(0==num)
	{
		bits=1;
	}
	else
	{
		while (num)
		{
			num = num / 10;
			bits++;
		}
	}
	
	return bits;
}

//====================================================
uint8_t Find_MaxValue(uint8_t* pData, uint8_t count)
{	
	uint8_t max_value=*pData;
	uint8_t* p=pData;
	
	while(--count)
	{
		if(*++p>=max_value)
		{
			max_value=*p;
		}
	}

	return max_value;
}

uint8_t Find_MaxAbsValue_Index(int8_t* pData, uint8_t count)
{	
	uint8_t max_idx=0;
	uint8_t index=0;
	int8_t max_value=FLEX_ABS(*pData);

	for(index=0; index<count; index++)
	{
		if(FLEX_ABS(*(pData+index))>max_value)
		{
			max_value=FLEX_ABS(*(pData+index));
			max_idx=index;
		}
	}

	return max_idx;
}

//====================================================
osStatus Queue_MsgPut (osMessageQId queue_id, void *message, uint32_t millisec)
{
	portBASE_TYPE taskWoken = pdFALSE;
	TickType_t ticks;

	ticks = millisec / portTICK_PERIOD_MS;

	if (__get_IPSR() != 0)
	{
		if (xQueueSendFromISR(queue_id, message, &taskWoken) != pdTRUE)
		{
			return osErrorOS;
		}

		portEND_SWITCHING_ISR(taskWoken);
	}
	else
	{
		if (xQueueSend(queue_id, message, ticks) != pdTRUE)
		{
			return osErrorOS;
		}
	}

	return osOK;
}

//====================================================
uint16_t Find_Max_u16Value(uint16_t* pData, uint8_t count)
{	
	uint16_t max_value=*pData;
	uint16_t* p=pData;
	
	while(--count)
	{
		if(*++p>=max_value)
		{
			max_value=*p;
		}
	}

	return max_value;
}

uint8_t Find_Max_u16Value_Index(uint16_t* pData, uint8_t count)
{	
	uint8_t max_idx=0;
	uint8_t index=0;
	uint16_t max_value=*pData;

	for(index=0; index<count; index++)
	{
		if(*(pData+index)>max_value)
		{
			max_value=*(pData+index);
			max_idx=index;
		}
	}

	return max_idx;
}

//====================================================
/*!
*\brief Enhanced atol() to converts a string to a signed long integer
*\param [in] s - point to a string
*\param [in] value - point to hold integer
*\param [in] base -number base to use in conversion
*
*\return conversion status
*\retval 0 - invalid number or out of range
*\retval !0 - successful
*/
bool atolEx(char *s, int32_t *value, int base)
{
	char *endptr;
	int32_t temp;

	//If a range error occurs, errno will be set.
	temp = strtol((const char *)s, &endptr, base);

	//convert failed if endptr is not referred to '\0'
	if ((*endptr) != '\0')
	{
		return false;
	}

	*value =  temp;
	return true;
}


/*!
*\brief Enhanced atol() to converts a string to a unsigned long integer
*\param [in] s - point to a string
*\param [in] value - point to hold integer
*\param [in] base -number base to use in conversion
*
*\return conversion status
*\retval 0 - invalid number or out of range
*\retval !0 - successful
*/
bool atoulEx(char *s, uint32_t *value, int base)
{
	char *endptr;
	uint32_t temp;

	//If a range error occurs, errno will be set.
	temp = strtoul((const char *)s, &endptr, base);

	//convert failed if endptr is not referred to '\0'
	if ((*endptr) != '\0')
	{
		return false;
	}

	*value =  temp;
	return true;
}

/*
 *\brief locates the first occurence of character differ to c (converted to a char) in the string
 * pointed to by s. The terminating null character is considered part of
 * the string.
 *\param [in] s1 -source to be search
 *\param [in] c -character
 *\return - a pointer to the character, or a null pointer if non c does
 * not occur in the string.
 */
char *strnchr(const char *s, char c)
{
	char *p = (char *)s;

	while (*p)
	{
		if (c != *p)
		{
			return p;
		}

		p++;
	}

	return NULL;
}


/*!
*\brief check specific duration elapsed or not
*\param [in] preTick - last tick
*\param [in] duration - duration
*
*\retval true - specified duration has eplased
*\retval false - no,still inside duration window
*/
bool CheckTimeElapsed(uint32_t preTick, uint32_t duration)
{
	uint32_t tick;
	tick = osKernelSysTick();

	if (tick < preTick)
	{
		tick += ((uint32_t) - 1) - preTick;
	}
	else
	{
		tick = tick - preTick;
	}

	return (tick >= duration);
}


/*========================================================================================================
										End
========================================================================================================*/





/******************************************************************************
 Copyright (c) 2018 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		flex_drv_m24cxx.c
 [Date]:
 		2018-07-09
 [Author]:
 		Tim Lin
 [Reversion History]:
 		v1.0
*******************************************************************************/

/*========================================================================================================
										I N C L U D E
========================================================================================================*/
// external
#include <stdbool.h>
#include "flex_common.h"


// internal
#include "flex_drv_m24cxx.h"


/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME					"[M24CXX]--"
#endif

#if defined(__FLEX_TRACE_ENABLE__)
#define TRACE_F(fmt, arg...)				DBG_MSG(MOD_NAME "[F]-%s.\r\n", __func__)
#define TRACE_I(fmt, arg...)				DBG_MSG(MOD_NAME "[I]-" fmt, ##arg)
#define TRACE_W(fmt, arg...)			DBG_MSG(MOD_NAME "[W]-" fmt, ##arg)
#else
#define TRACE_F()
#define TRACE_I(fmt, arg...)
#define TRACE_W(fmt, arg...)
#endif
#define TRACE_E(fmt, arg...)				DBG_MSG(MOD_NAME "[E]-" fmt, ##arg)

/*========================================================================================================
										E X T E R N
========================================================================================================*/





/*========================================================================================================
										D E F I N E
========================================================================================================*/
#define M24CXX_I2C_CHANNEL		hi2c1
#define M24CXX_DEV_TYPE_ID		(0xA0)
#define M24CXX_I2C_TIMEOUT 		10
#define M24CXX_MUX_TIMEOUT 		1000

//task event notification bit field
#define WRITE_REQUEST_EVENT		_BIT0_
#define WRITE_TIMEOUT_EVENT		_BIT1_


/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
typedef enum
{
	M24CXX_STATE_IDLE,
	M24CXX_STATE_BUSY,
	M24CXX_STATE_MAX,
} M24CXX_STATE;

typedef struct
{
	M24CXX_STATE state;
	uint16_t offset;
	uint8_t *bufferPtr;
	uint16_t size;
} M24CXX_DATA;

typedef struct
{
	uint16_t offset;
	uint8_t *buffer;
	uint16_t size;
	M24CXX_CALLBACK cb;
	uint16_t context;
} M24CXX_REQ_DATA;

typedef struct _M24CXX_REQ_ITEM_
{
	M24CXX_REQ_DATA request;
	struct _M24CXX_REQ_ITEM_ *nextItem;
} M24CXX_REQ_ITEM;


/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
static osMutexId m24cxxMutexId = NULL;
static M24CXX_DATA mc24cxxData = {M24CXX_STATE_IDLE, 0, NULL, 0};

#ifdef M24CXX_NON_BLOCK_ACCESS
static osThreadId taskHandle = NULL;
static bool taskIsActive = false;
static osTimerId writeTimer = NULL;
static M24CXX_REQ_ITEM *writeItemHead = NULL;
#endif

/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
static M24CXX_STATUS M24CXX_PageWrite(void);

#ifdef M24CXX_NON_BLOCK_ACCESS
static void M24CXX_InitItemWrite(void);
static bool M24CXX_WriteProceed(void);
static bool M24CXX_ItemWriteDone(M24CXX_STATUS status);
static void M24CXX_WriteTimerCallback(void const *argument);


/*!
*\brief m24cxx write timer call back function
*\param [in] argument -argument pointer
*/
static void M24CXX_WriteTimerCallback(void const *argument)
{
	osSignalSet(taskHandle, WRITE_TIMEOUT_EVENT);
}


/*!
*\brief M24CXX task
*\param [in] argument - argument pointer
*/
static void M24CXX_Task(void const *argument)
{
	osEvent event;

	osTimerDef(m24cxxWriteTimer, M24CXX_WriteTimerCallback);
	writeTimer = osTimerCreate(osTimer(m24cxxWriteTimer), osTimerOnce, NULL);
	assert_param(writeTimer != NULL);

	/* Infinite loop */
	for (;;)
	{
		event = osSignalWait(0x7FFFFFFF, osWaitForever);

		if (event.status == osEventSignal)
		{
			if (event.value.signals & WRITE_REQUEST_EVENT)
			{
				M24CXX_InitItemWrite();

				if (!M24CXX_WriteProceed())
				{
					mc24cxxData.state = M24CXX_STATE_IDLE;
					osTimerDelete(writeTimer);
					writeTimer = NULL;
					taskIsActive = false;
					osThreadTerminate(taskHandle);
				}
			}

			if (event.value.signals & WRITE_TIMEOUT_EVENT)
			{
				if (!M24CXX_WriteProceed())
				{
					//all is done
					mc24cxxData.state = M24CXX_STATE_IDLE;
					osTimerDelete(writeTimer);
					writeTimer = NULL;
					taskIsActive = false;
					osThreadTerminate(taskHandle);
				}
			}
		}
	}
}

/*!
*\brief initialize write item
*/
static void M24CXX_InitItemWrite(void)
{
	mc24cxxData.state = M24CXX_STATE_BUSY;
	mc24cxxData.offset = writeItemHead->request.offset;
	mc24cxxData.size = writeItemHead->request.size;
	mc24cxxData.bufferPtr = writeItemHead->request.buffer;
}

/*!
*\brief item write completion handler
*\param [in] status - item write status
*
*\retval true - next item is ongoing
*\retval true - no item any more
*/
static bool M24CXX_ItemWriteDone(M24CXX_STATUS status)
{
	bool result;
	M24CXX_REQ_ITEM *item = writeItemHead;

	mc24cxxData.state = M24CXX_STATE_IDLE;
	//writeItemHead point to next item
	writeItemHead = writeItemHead->nextItem;

	if (item->request.cb)
	{
		item->request.cb(status, item->request.context);
	}

	//discard memory
	pf_free(item->request.buffer);
	pf_free(item);

	//check next item
	if (writeItemHead)
	{
		osSignalSet(taskHandle, WRITE_REQUEST_EVENT);
		result = true;
	}
	else
	{
		result = false;
	}

	return result;
}


/*!
*\brief proceed M24CXX write request
*\retval true - process is ongoing
*\retval true - process is done
*/
static bool M24CXX_WriteProceed(void)
{
	bool result = true;
	M24CXX_STATUS status;

	//check if current request itme is done
	if (0 != mc24cxxData.size)
	{
		//continue to proceed current item
		status = M24CXX_PageWrite();

		if (M24CXX_OK == status)
		{
			//write 10ms to let write done
			osTimerStart(writeTimer, 10);
		}
		else if ((M24CXX_BUSY == status) || (M24CXX_TIMEOUT == status))
		{
			//start 100ms timer to retry if i2c bus is busy now
			osTimerStart(writeTimer, 100);
		}
		else
		{
			//treat item done if error happened
			result = M24CXX_ItemWriteDone(status);
		}
	}
	else
	{
		//done
		result = M24CXX_ItemWriteDone(M24CXX_OK);
	}

	return result;
}

/*!
*\brief request to write M24CXX in non blocking way
*\param [in] offset - start memory offset to be writen
*\param [in] source - pointer to source buffer
*\param [in] size -size to write
*\param [in] cb - callback when write done
*\param [in] context - callback context
*
*\retval M24CXX_STATUS
*/
M24CXX_STATUS M24CXX_NonBlockWrite(uint16_t offset, uint8_t *source, uint16_t size, M24CXX_CALLBACK cb, uint16_t context)
{
	uint8_t *buffer;
	M24CXX_REQ_ITEM *item;
	M24CXX_STATUS status = M24CXX_OK;

	if (!source || (0 == size))
	{
		return M24CXX_INVALID_PARA;
	}

	if ( ( offset + size) > M24CXX_SIZE)
	{
		return M24CXX_INVALID_PARA;
	}

	item = (M24CXX_REQ_ITEM *)pf_malloc(sizeof(M24CXX_REQ_ITEM));

	if (!item)
	{
		goto ERROR;
	}

	//allocate memory buffer to save source
	buffer = (uint8_t *)pf_malloc(size);

	if (!buffer)
	{
		goto ERROR;
	}

	//copy source conent to buffer
	pf_memcpy(buffer, source, size);
	//save others to request
	item->request.buffer = buffer;
	item->request.offset = offset;
	item->request.size = size;
	item->request.cb = cb;
	item->request.context = context;
	item->nextItem = NULL;

	//put new reques to chain
	if (NULL == writeItemHead)
	{
		//first in list
		writeItemHead = item;
	}
	else
	{
		M24CXX_REQ_ITEM *list = writeItemHead;

		//search last one in list
		while (list->nextItem)
		{
			list = list->nextItem;
		}

		list->nextItem = item;
	}

	if (!taskIsActive)
	{
		osThreadDef(m24cxxTask, M24CXX_Task, osPriorityLow, 0, 128);
		taskHandle = osThreadCreate(osThread(m24cxxTask), NULL);

		if (!taskHandle)
		{
			status = M24CXX_ERROR;
		}
		else
		{
			taskIsActive = true;

			if (M24CXX_STATE_IDLE == mc24cxxData.state)
			{
				osSignalSet(taskHandle, WRITE_REQUEST_EVENT);
			}
		}
	}

	goto EXIT;
ERROR:

	if (buffer)
	{
		pf_free(buffer);
	}

	if (item)
	{
		pf_free(item);
	}

	status = M24CXX_ERROR;

EXIT:
	return status;
}
#endif //#ifdef M24CXX_NON_BLOCK_ACCESS

/*!
*\brief M24CXX i2c page write
*\return M24CXX_STATUS
*/
static M24CXX_STATUS M24CXX_PageWrite(void)
{
	HAL_StatusTypeDef ret;
	M24CXX_STATUS status = M24CXX_OK;
	uint8_t selectCode;
	uint8_t memAddr;

	/*
	8 bit address byte, for MC24C08, the capacity is 8Kb, 1024KB. Assume E2=0, then:
	Address 	   Memory range (Byte)
	0xA0			0~255
	0xA1			256~511
	0xA2			512 - 767
	0xA3			768 - 1023
	*/
	selectCode = M24CXX_DEV_TYPE_ID + (uint8_t)(mc24cxxData.offset >> 8);
	memAddr = mc24cxxData.offset & 0xff;

	//get current write page boundary
	uint16_t boundary = (mc24cxxData.offset & (uint16_t)~(M24CXX_PAGE_SIZE - 1)) + (M24CXX_PAGE_SIZE - 1);
	uint16_t chunkSize = ((mc24cxxData.offset + mc24cxxData.size) > boundary) ? \
						 (boundary - mc24cxxData.offset + 1) : mc24cxxData.size;

	//DBG_MSG("selectCode=%x,offset=%d, length = %d\n", selectCode, memAddr, chunkSize);

	ret = HAL_I2C_Mem_Write(&M24CXX_I2C_CHANNEL, selectCode, memAddr, I2C_MEMADD_SIZE_8BIT,
							mc24cxxData.bufferPtr, chunkSize, M24CXX_I2C_TIMEOUT);

	if (ret != HAL_OK)
	{
		TRACE_E("%s failed to write 0x%x:%d\n", __FUNCTION__, mc24cxxData.offset, ret);

		if (HAL_BUSY == ret)
		{
			status = M24CXX_BUSY;
		}
		else if (HAL_TIMEOUT == ret)
		{
			status = M24CXX_TIMEOUT;
		}
		else if (HAL_ERROR == ret)
		{
			status = M24CXX_ERROR;
		}
	}
	else
	{
		//update operation parameters
		mc24cxxData.offset += chunkSize;
		mc24cxxData.size -= chunkSize;
		mc24cxxData.bufferPtr += chunkSize;
	}

	return status;
}

/*!
*\brief request to write M24CXX in blocking way
*\param [in] offset - start memory offset to be writen
*\param [in] source - pointer to source buffer
*\param [in] size -size to write
*\param [in] cb - callback when write done
*\param [in] context - callback context
*
*\retval M24CXX_STATUS
*/
M24CXX_STATUS M24CXX_Write(uint16_t offset, uint8_t *source, uint16_t size)
{
	M24CXX_STATUS status = M24CXX_OK;
	uint8_t retry = 5;

	if (!source || (0 == size))
	{
		return M24CXX_INVALID_PARA;
	}

	if ( ( offset + size) > M24CXX_SIZE)
	{
		return M24CXX_INVALID_PARA;
	}

	//try to acquire mutex id
	if (osOK != osMutexWait(m24cxxMutexId, M24CXX_MUX_TIMEOUT))
	{
		return M24CXX_BUSY;
	}

	mc24cxxData.state = M24CXX_STATE_BUSY;
	mc24cxxData.offset = offset;
	mc24cxxData.size = size;
	mc24cxxData.bufferPtr = source;

	while (0 != mc24cxxData.size)
	{
		//continue to proceed current item
		status = M24CXX_PageWrite();

		if (M24CXX_OK == status)
		{
			//write 5 ms to let write done
			osDelay(5);
		}
		else if ((M24CXX_BUSY == status) || (M24CXX_TIMEOUT == status))
		{
			//start 20ms timer to retry if i2c bus is busy now
			if (--retry)
			{
				osDelay(20);
			}
			else
			{
				break;
			}
		}
		else
		{
			//treat item done if error happened
			break;
		}
	}

	//release mutex id
	osMutexRelease(m24cxxMutexId);

	return status;
}


/*!
*\brief read M24CXX eeprom in blocking way
*\param [in] offset - start memory offset to be writen
*\param [in] data - pointer to read buffer
*\param [in] size -size to read
*
*\retval M24CXX_STATUS
*/
M24CXX_STATUS M24CXX_Read(uint16_t offset, uint8_t *data, uint16_t size)
{
	HAL_StatusTypeDef ret;
	M24CXX_STATUS status = M24CXX_OK;
	uint8_t retry = 5;
	uint8_t selectCode;
	uint16_t boundary;
	uint16_t chunkSize;

	if (!data || (0 == size))
	{
		return M24CXX_INVALID_PARA;
	}

	if ( ( offset + size) > M24CXX_SIZE)
	{
		return M24CXX_INVALID_PARA;
	}

	//try to acquire mutex id
	if (osOK != osMutexWait(m24cxxMutexId, M24CXX_MUX_TIMEOUT))
	{
		return M24CXX_BUSY;
	}

	do
	{
		//bank boundary
		boundary = (offset & (uint16_t)~(M24CXX_BANK_SIZE - 1)) + (M24CXX_BANK_SIZE - 1);
		chunkSize = ((offset + size) > boundary) ? (boundary - offset + 1) : size;

		/*
		8 bit address byte, for MC24C08, the capacity is 8Kb, 1024KB. Assume E2=0, then:
		Address 	   Memory range (Byte)
		0xA0		 	0~255
		0xA1			256~511
		0xA2			512 - 767
		0xA3			768 - 1023
		*/
		selectCode = M24CXX_DEV_TYPE_ID + (uint8_t)(offset >> 8);

		ret = HAL_I2C_Mem_Read(&M24CXX_I2C_CHANNEL, selectCode, offset & 0xff, I2C_MEMADD_SIZE_8BIT, data, chunkSize, M24CXX_I2C_TIMEOUT);

		if (ret == HAL_OK)
		{
			offset += chunkSize;
			size -= chunkSize;
			data += chunkSize;
		}
		else
		{
			TRACE_E("%s failed to read 0x%x:%d\n", __FUNCTION__, offset, ret);

			if ((HAL_BUSY == ret) || (HAL_TIMEOUT == ret))
			{
				//start 20ms timer to retry if i2c bus is busy now
				if (--retry)
				{
					osDelay(20);
				}
				else
				{
					status = M24CXX_BUSY;
					break;
				}
			}
			else
			{
				//treat item done if error happened
				status = M24CXX_ERROR;
				break;
			}
		}
	}
	while (0 != size);

	//release mutex id
	osMutexRelease(m24cxxMutexId);

	return status;
}

/*!
*\brief Initialize m24cxx i2c eeprom
*
*/
void M24CXX_Init(void)
{
	osMutexDef(m24cxxMute);
	m24cxxMutexId = osMutexCreate (osMutex(m24cxxMute));
	assert_param(m24cxxMutexId != NULL);
}


/*========================================================================================================
										End
========================================================================================================*/

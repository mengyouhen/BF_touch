/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_drv_tsc_cap1214.c
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
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_conf.h"
#if defined(HAL_I2C_MODULE_ENABLED)
#include "i2c.h"
#endif
#include "flex_drv_button.h"
#include "flex_drv_led.h"

// internal
#include "flex_drv_captouch_cap1214.h"


#if (TSC_TYPE_SELECT&TSC_TYPE_MICROCHIP_CA1214_I2C)
/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME								"[CAP1214]--"
#endif

#if 0//defined(__FLEX_TRACE_ENABLE__)
#define TRACE_F(fmt, arg...)				DBG_MSG(MOD_NAME "[F]--%s.\r\n", __func__)
#define TRACE_I(fmt, arg...)				DBG_MSG(MOD_NAME "[I]--" fmt, ##arg)
#define TRACE_W(fmt, arg...)				DBG_MSG(MOD_NAME "[W]--" fmt, ##arg)
#else
#define TRACE_F()										//do{}while(0)
#define TRACE_I(fmt, arg...)				//do{}while(0)	
#define TRACE_W(fmt, arg...)				//do{}while(0)	
#endif
#define TRACE_E(fmt, arg...)				DBG_MSG(MOD_NAME "[E]--" fmt, ##arg)

//========================================
// DEBUG
//========================================
#if 0//defined(__FLEX_TRACE_ENABLE__)
#define DEBUG_TRACE				
#endif
/*========================================================================================================
										E X T E R N 
========================================================================================================*/


/*========================================================================================================
										D E F I N E
========================================================================================================*/
//========================================
// I2C 
//========================================
#define CAP1214_I2C_CHANNEL		hi2c1
#define CAP1214_I2C_ADDR			0x50  // 8bit
#define CAP1214_I2C_TIMEOUT 	50		// 0x10

#define CAPxxxx_ID  0x5A

//========================================
// reg 
//========================================
#define CAP_NUM_WRITABLE_REGISTERS          48

#define CS_1_THRESHOLD       30		
#define CS_2_THRESHOLD       30		
#define CS_3_THRESHOLD       30		
#define CS_4_THRESHOLD       40		
#define CS_5_THRESHOLD       30		
#define CS_6_THRESHOLD       40		
#define CS_7_THRESHOLD       30		
#define CS_GROUP_THRESHOLD   25		

#define CS1_BIT   _BIT0_
#define CS2_BIT   _BIT1_
#define CS3_BIT   _BIT2_
#define CS4_BIT   _BIT3_
#define CS5_BIT   _BIT4_
#define CS6_BIT   _BIT5_
#define CS7_BIT   _BIT6_
#define CS8_BIT   _BIT7_
#define CS9_BIT   _BIT8_
#define CS10_BIT  _BIT9_
#define CS11_BIT  _BIT10_
#define CS12_BIT  _BIT11_
#define CS13_BIT  _BIT12_
#define CS14_BIT  _BIT13_

//========================================
// KEY SORT 
//========================================
#define NUMBER_OF_SENSORS   14

//========================================
// PATCH
//========================================
#define PATCH_FOR_CHARGE_RESIDUAL   true

//========================================
// ALGORITHM
//========================================
#define ALGORITHM_A                 false
#define ALGORITHM_B                 false
#define ALGORITHM_C                 false
#define ALGORITHM_D                 true
#define ALGORITHM_FILTER            true

#if ALGORITHM_FILTER
#define FILTER_COUNT      3
#endif

/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
typedef enum
{
	CAP_CS_1,
	CAP_CS_2,
	CAP_CS_3,
	CAP_CS_4,
	CAP_CS_5,
	CAP_CS_6,
	CAP_CS_7,
	CAP_CS_8,
	CAP_CS_9,
	CAP_CS_10,
	CAP_CS_11,
	CAP_CS_12,
	CAP_CS_13,
	CAP_CS_14,
	CAP_CS_MAX,
	CAP_CS_INVALID=0xff
}CAP_CS_ENUM;

typedef enum
{
	CAP_LED_1,
	CAP_LED_2,
	CAP_LED_3,
	CAP_LED_4,
	CAP_LED_5,
	CAP_LED_6,
	CAP_LED_7,
	CAP_LED_8,
	CAP_LED_9,
	CAP_LED_10,
	CAP_LED_11,
	CAP_LED_MAX,
	CAP_LED_INVALID=0xff
}CAP_LED_ENUM;

typedef struct
{
	uint8_t addr;
	uint8_t value;
}CAP_REG_DATA_ST;


//========================================
// ALGORITHM
//========================================
#if ALGORITHM_A
typedef struct
{
	const CAP_CS_ENUM* npAdjacentGroup;
	uint8_t nCount;
}CS_ALGORITHM_ADJACENT_ST;
#endif

#if ALGORITHM_B
typedef struct
{
	CAP_CS_ENUM nDetect;
	CAP_CS_ENUM nFarther;
	CAP_CS_ENUM nCloser;
	CAP_CS_ENUM nBackup;
}CS_ALGORITHM_COLUMN_ST;
#endif

/*========================================================================================================
										D E S C R I P T I O N
========================================================================================================*/
/*
	----------------------------------------------------------------
	|   < KEY >   |   < CS  >  |  < CAP_LED >  |  < DELTAS_ARRAY > |
	----------------------------------------------------------------
	|    KEY_1    |     CS3    |      LED3     |       data[2]     |
	----------------------------------------------------------------
	|    KEY_2    |     CS2    |      LED2     |       data[1]     |
	----------------------------------------------------------------
	|    KEY_3    |     CS1    |      LED1     |       data[0]     |
	----------------------------------------------------------------
	|    KEY_4    |     CS4    |      LED6     |       data[3]     |
	----------------------------------------------------------------
	|    KEY_5    |     CS5    |      LED5     |       data[4]     |
	----------------------------------------------------------------
	|    KEY_6    |     CS6    |      LED4     |       data[5]     |
	----------------------------------------------------------------
	|    KEY_7    |     CS7    |      LED9     |       data[6]     |
	----------------------------------------------------------------
	|    KEY_8    |     CS8    |      LED8     |       data[7]     |
	----------------------------------------------------------------
	|    KEY_9    |     CS9    |      LED7     |       data[8]     |
	----------------------------------------------------------------
	|    KEY_OK   |     CS10   |    GPIO(PD7)  |       data[9]     |
	----------------------------------------------------------------
	|    KEY_0    |     CS11   |      LED11    |       data[10]    |
	----------------------------------------------------------------
	|    KEY_DEL  |     CS12   |      LED10    |       data[11]    |
	----------------------------------------------------------------
	|    KEY_LEFT |     CS13   |    GPIO(PF8)  |       data[12]    |
	----------------------------------------------------------------
	|    KEY_DOWN |     CS14   |    GPIO(PC2)  |       data13]     |
	----------------------------------------------------------------
*/
	
/*
	CS1  ----> MOC_TS1_IN1	----> S41(silk-screen) ----> KEY3
	CS2  ----> MOC_TS1_IN2	----> S42(silk-screen) ----> KEY2
	CS3  ----> MOC_TS1_IN3	----> S43(silk-screen) ----> KEY1
	CS4  ----> MOC_TS1_IN4	----> S44(silk-screen) ----> KEY4
	CS5  ----> MOC_TS1_IN5	----> S45(silk-screen) ----> KEY5
	CS6  ----> MOC_TS1_IN6	----> S46(silk-screen) ----> KEY6
	CS7  ----> MOC_TS1_IN7	----> S47(silk-screen) ----> KEY7
	CS8  ----> MOC_TS1_IN8	----> S48(silk-screen) ----> KEY8
	CS9  ----> MOC_TS1_IN9	----> S49(silk-screen) ----> KEY9
	CS10 ----> MOC_TS1_IN10 ----> S50(silk-screen) ----> DEL
	CS11 ----> MOC_TS1_IN11 ----> S51(silk-screen) ----> KEY0
	CS12 ----> MOC_TS1_IN12 ----> S52(silk-screen) ----> OK
	CS13 ----> MCU_TB3_IN3	----> S11(silk-screen) ----> LEFT
	CS14 ----> MCU_TB4_IN4	----> S13(silk-screen) ----> DOWN
*/

/*
	LIST_LED_KEY1 ----> LED40(silk-screen) ----> MOC_TS3_LED3 	----> LED3/GPIO3 
	LIST_LED_KEY2 ----> LED39(silk-screen) ----> MOC_TS2_LED2 	----> LED2/GPIO2 
	LIST_LED_KEY3 ----> LED38(silk-screen) ----> MOC_TS1_LED1 	----> LED1/GPIO1 
	LIST_LED_KEY4 ----> LED43(silk-screen) ----> MOC_TS6_LED6 	----> LED6/GPIO6 
	LIST_LED_KEY5 ----> LED42(silk-screen) ----> MOC_TS5_LED5 	----> LED5/GPIO5 
	LIST_LED_KEY6 ----> LED41(silk-screen) ----> MOC_TS4_LED4 	----> LED4/GPIO4 
	LIST_LED_KEY7 ----> LED46(silk-screen) ----> MOC_TS9_LED9 	----> LED9			 
	LIST_LED_KEY8 ----> LED45(silk-screen) ----> MOC_TS8_LED8 	----> LED8/GPIO8 
	LIST_LED_KEY9 ----> LED44(silk-screen) ----> MOC_TS7_LED7 	----> LED7/GPIO7 
	LIST_LED_OK 	----> LED49(silk-screen) ----> MOC_TS12_LED12 ----> PD7 			 
	LIST_LED_KEY0 ----> LED48(silk-screen) ----> MOC_TS11_LED11 ----> LED11 		 
	LIST_LED_DEL	----> LED47(silk-screen) ----> MOC_TS10_LED10 ----> LED10 		 
*/

/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
static uint8_t g_cap_error; 
static bool g_cap_inited=false;
static int8_t g_cap_raw_deltas[NUMBER_OF_SENSORS];

static fpDrvBtnEvent g_cap_button_cb=NULL;
static DRV_BTN_ENUM g_cap_button=LIST_BTN_NONE;

static const CAP_REG_DATA_ST cap1214_init_data[]=
{   //  Address     Value
  {   0x00,       0x00    },      // Main Control
  {   0x1F,       0x6F    },      // Data Sensitivity
#if PATCH_FOR_CHARGE_RESIDUAL // carson 
	// Enable re-calibration for button and group 
  {   0x20,       0x2A    },			// General Configuration
#else
  //no re-calibration is performed when hold on non-grouped sensors
  {   0x20,       0x20    },      // General Configuration
#endif
  //enable CS1~CS7 channels
  {   0x21,       0xFF    },      // Sensor Enable
  {   0x22,       0xA4    },      // Button Configuration
  {   0x23,       0x07    },      // Group Configuration 1
#if PATCH_FOR_CHARGE_RESIDUAL // carson 
  // set duration time for group, as same as button' defult:5600ms
  {   0x24,       0xA4    },			// Group Configuration 2
#else
  {   0x24,       0x39    },      // Group Configuration 2
#endif
  {   0x26,       0x00    },      // Calibration Activate
  {   0x46,       0x00    },      // Group Calibration Activate
  {   0x27,       0xFF    },      // Interrupt Enable 1
   //no interrupt for GPIO1~GPIO8
  {   0x28,       0x00    },      // Interrupt Enable 2
  #if 1
  //only 1 touch allowed in both non-group and gourp sensors
  {   0x2A,       0x83    },      // Multiple Touch Configuration
  #else
  //{   0x2A,       0x00    },      // Multiple Touch Configuration
  #endif
  {   0x2F,       0x8A    },      // Recalibration Configuration
  {   0x30,       CS_1_THRESHOLD    },      // CS1(KEY_3) Threshold
  {   0x31,       CS_2_THRESHOLD    },      // CS2(KEY_2) Threshold
  {   0x32,       CS_3_THRESHOLD    },      // CS3(KEY_1) Threshold
  {   0x33,       CS_4_THRESHOLD    },      // CS4(KEY_4) Threshold
  {   0x34,       CS_5_THRESHOLD    },      // CS5(KEY_5) Threshold
  {   0x35,       CS_6_THRESHOLD    },      // CS6(KEY_6) Threshold
  {   0x36,       CS_7_THRESHOLD    },      // CS7(KEY_7) Threshold
  {   0x37,       CS_GROUP_THRESHOLD    },      // Group Threshold
//  {   0x38,       0xAA    },      // Sensor Input Noise Threshold
  //group sensor (CS8 ~ CS14) are used as separate sensors and not grouped
  {   0x40,       0x02    },      // configuration 2
  //enable CS8 ~ CS14 channels in group sensors
  {   0x41,       0x7f    },      // Group sensor channel enable
  //{   0x42,       0x02    },      // proximity control
  //sampling configuration setting
  //{   0x4E,       0x00    },        //sampling Chnanel Select
  //{   0x4F,       0x01    },        //sampling Configuration
  {   0x70,       0xff    },      // LED/GPIO Direction 
  //{   0x71,       0x00   },      // LED/GPIO Output Type
  {   0x73,       0x00     },      // LED1~LED8 off
  {   0x74,       0x00     },      // LED9~LED11 off
  //{   0x75,       0x00    },      // LED1 Polarity 1
  //{   0x76,       0x00    },      // LED1 Polarity 1

  {   0x77,       0x00    },      // LED Linked Transition Control
  //{   0x79,       0x00    },      // LED Mirror Control 1
  //{   0x7A,       0x00    },      // LED Mirror Control 2
  //sensor LED linking setting
  {   0x80,       0x00    },      // Sensor LED Linking
  //{   0x81,       0x00    },      // LED Behavior 1
  //{   0x82,       0x00    },      // LED Behavior 2
  //{   0x83,       0x00    },      // LED Behavior 3
  //{   0x84,       0x20    },      // LED Pulse 1 Period
  //{   0x85,       0x14    },      // LED Pulse 2 Period
  //{   0x86,       0x5D    },      // LED Breathe Period
  {   0x88,       0x04    },      // LED Config
  //{   0x90,       0xF0    },      // LED Pulse 1 Duty Cycle
  //{   0x91,       0xF0    },      // LED Pulse 2 Duty Cycle
  //{   0x92,       0xF0    },      // LED Breathe Duty Cycle
  //{   0x93,       0xF0    },      // LED Direct Duty Cycle
  //{   0x94,       0x00    },      // LED Direct Ramp Rates
  //{   0x95,       0x00    }       // LED Off Delay
};

static const DRV_BTN_ENUM csToBtnTable[CAP_CS_MAX] = 
{
	LIST_BTN_3,
	LIST_BTN_2,
	LIST_BTN_1,
	LIST_BTN_4,
	LIST_BTN_5,
	LIST_BTN_6,
	LIST_BTN_7,
	LIST_BTN_8,
	LIST_BTN_9,
	LIST_BTN_DEL,
	LIST_BTN_0,
	LIST_BTN_OK,
	LIST_BTN_LEFT,
	LIST_BTN_DOWN,
};

//========================================
// ALGORITHM
//========================================
#if ALGORITHM_A
/*-----------------------------------------------

   |-----------|-------------|-----------|
   | KEY_1     |    KEY_2    |   KEY_3   |
   | (CS3)     |    (CS2)    |   (CS1)   |
   |-----------|-------------|-----------|
   | KEY_4     |    KEY_5    |   KEY_6   |
   | (CS4)     |    (CS5)    |   (CS6)   |
   |-----------|-------------|-----------|
   | KEY_7     |    KEY_8    |   KEY_9   |
   | (CS7)     |    (CS8)    |   (CS9)   |
   |-----------|-------------|-----------|
   | KEY_OK    |    KEY_0    |  KEY_DEL  |
   | (CS12)    |   (CS11)    |   (CS10)  |
   |-------------------------------------|

   KEY_LEFT
    (CS13)

            KEY_DOWN
             (CS14)
   
------------------------------------------------*/

static CAP_CS_ENUM const cs1_adjacent_group[] =
{
	CAP_CS_2, CAP_CS_6
};

static CAP_CS_ENUM const cs2_adjacent_group[] =
{
	CAP_CS_1, CAP_CS_3, CAP_CS_5
};

static CAP_CS_ENUM const cs3_adjacent_group[] =
{
	CAP_CS_2, CAP_CS_4
};

static CAP_CS_ENUM const cs4_adjacent_group[] =
{
	CAP_CS_3, CAP_CS_5, CAP_CS_7
};

static CAP_CS_ENUM const cs5_adjacent_group[] =
{
	CAP_CS_2, CAP_CS_4, CAP_CS_6, CAP_CS_8
};

static CAP_CS_ENUM const cs6_adjacent_group[] =
{
	CAP_CS_1, CAP_CS_5, CAP_CS_9
};

static CAP_CS_ENUM const cs7_adjacent_group[] =
{
	CAP_CS_4, CAP_CS_8, CAP_CS_12
};

static CAP_CS_ENUM const cs8_adjacent_group[] =
{
	CAP_CS_5, CAP_CS_7, CAP_CS_9, CAP_CS_11
};

static CAP_CS_ENUM const cs9_adjacent_group[] =
{
	CAP_CS_6, CAP_CS_8, CAP_CS_10
};

static CAP_CS_ENUM const cs10_adjacent_group[] =
{
	CAP_CS_9, CAP_CS_11
};

static CAP_CS_ENUM const cs11_adjacent_group[] =
{
	CAP_CS_8, CAP_CS_10, CAP_CS_12
};

static CAP_CS_ENUM const cs12_adjacent_group[] =
{
	CAP_CS_7, CAP_CS_11
};

static CS_ALGORITHM_ADJACENT_ST const g_cap_AajacentArray[12]=
{
	{cs1_adjacent_group,    2},
	{cs2_adjacent_group,    3},
	{cs3_adjacent_group,    2},
	{cs4_adjacent_group,    3},
	{cs5_adjacent_group,    4},
	{cs6_adjacent_group,    3},
	{cs7_adjacent_group,    3},
	{cs8_adjacent_group,    4},
	{cs9_adjacent_group,    3},
	{cs10_adjacent_group,   2},
	{cs11_adjacent_group,   3},
	{cs12_adjacent_group,   2},
};
#endif
//---------------------------------------
#if ALGORITHM_B
static CS_ALGORITHM_COLUMN_ST const g_cap_ColumnArray[6]=
{
// nDetect   nFarther   nCloser    nBackup
	{CAP_CS_4, CAP_CS_12, CAP_CS_3,  CAP_CS_7},  
	{CAP_CS_5, CAP_CS_11, CAP_CS_2,  CAP_CS_8},
	{CAP_CS_6, CAP_CS_10, CAP_CS_1,  CAP_CS_9},
	
	{CAP_CS_7, CAP_CS_3,  CAP_CS_12, CAP_CS_4},
	{CAP_CS_8, CAP_CS_2,  CAP_CS_11, CAP_CS_5},
	{CAP_CS_9, CAP_CS_1,  CAP_CS_10, CAP_CS_6},
};
#endif

/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
//========================================
// I2C 
//========================================
static HAL_StatusTypeDef Cap_I2c_Read_Bytes(uint8_t reg, uint8_t *data, uint8_t count)
{
	HAL_StatusTypeDef ret;
	ret = HAL_I2C_Mem_Read(&CAP1214_I2C_CHANNEL, CAP1214_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, count, CAP1214_I2C_TIMEOUT);

	if(ret != HAL_OK)
	{
		TRACE_E("%s,read (0x%x):%d.\n", __func__, reg, ret);
	}	
	
	return ret;
}

static HAL_StatusTypeDef Cap_I2c_Write_Bytes(uint8_t reg, uint8_t data, uint8_t count)
{
	HAL_StatusTypeDef ret;
	
	ret = HAL_I2C_Mem_Write(&CAP1214_I2C_CHANNEL, CAP1214_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &data, count, CAP1214_I2C_TIMEOUT);
	if(ret != HAL_OK)
	{
		TRACE_E("%s,write (0x%x):%d.\n",  __func__, reg, ret);
	}

	return ret;
}
//----------------------------------------------------
static HAL_StatusTypeDef Cap_I2c_Read(uint8_t reg, uint8_t *data)
{
	return Cap_I2c_Read_Bytes(reg, data,1);
}

static HAL_StatusTypeDef Cap_I2c_Write(uint8_t reg, uint8_t data)
{
	return Cap_I2c_Write_Bytes(reg,data,1);
}

//========================================
// READ ID 
//========================================
static HAL_StatusTypeDef Cap_Read_ProId(void)
{
	HAL_StatusTypeDef ret=HAL_OK;
  uint8_t timeout = 3;
	uint8_t data[3]={0};
	uint8_t product_id=0;
	
  do {
		ret = Cap_I2c_Read_Bytes(0xFD, data, 3);   // default id=0X5A
    timeout--;
  } while((ret != HAL_OK) && (timeout != 0));

  if (timeout == 0)     
  {    
		TRACE_E("%s, timeout. \r\n", __func__);	
    return ret;
  }

  product_id=data[0];
	
	//TRACE_I("%s,product_id = 0x%X (expect 0x5A). \r\n", __func__, product_id);	
	//TRACE_I("%s,manufacturer = 0x%X (expect 0x5D). \r\n", __func__, data[1]);	
	//TRACE_I("%s,version = 0x%X (expect 0x80). \r\n", __func__, data[2]);		

  if (product_id != CAPxxxx_ID)      
  {    
		// Invalid Device ID
		TRACE_E("%s, read ID failure. \r\n", __func__);	
    return HAL_ERROR;
  }
  
  return ret;
}

//========================================
// INIT
//========================================
static bool Cap_Init(void)
{
	uint16_t index;
	HAL_StatusTypeDef ret=HAL_OK;
	
	TRACE_F();
	
	ret=Cap_Read_ProId();
	if (ret!= HAL_OK) 
	{         
		return false;
	}

	for(index=0; index<sizeof(cap1214_init_data)/sizeof(CAP_REG_DATA_ST); index++)
	{
		ret |= Cap_I2c_Write(cap1214_init_data[index].addr, cap1214_init_data[index].value);
	}

	if(HAL_OK==ret)
	{
		g_cap_inited = true;
	}

	TRACE_I("g_cap_inited=%d.\r\n", g_cap_inited);
	
	return g_cap_inited;
}

//========================================
// LED
//========================================
static void cap_led_set_output(CAP_LED_ENUM led, bool level)
{
	uint8_t reg;
	uint8_t value;
	uint8_t mask;

	if(!g_cap_inited)
	{
		Cap_Init();
	}
	
	if(led < CAP_LED_MAX)
	{
		if(led <= CAP_LED_8) 
		{
			reg = 0x73;
			mask = 1 << (led - CAP_LED_1);
		}
		else
		{
			reg = 0x74;
			mask = 1 << (led - CAP_LED_9);
		}

		Cap_I2c_Read(reg, &value);

		if(level)
		{
			value |= mask;
		}
		else
		{
			value &= ~mask;
		}

		Cap_I2c_Write(reg,value);
	}
}

static bool cap_led_get_status(CAP_LED_ENUM led)
{
	uint8_t reg;
	uint8_t value;
	uint8_t mask;
	bool bRet=false;

	if(!g_cap_inited)
	{
		Cap_Init();
	}
	
	if(led < CAP_LED_MAX)
	{
		if(led <= CAP_LED_8) 
		{
			reg = 0x73;
			mask = 1 << (led - CAP_LED_1);
		}
		else
		{
			reg = 0x74;
			mask = 1 << (led - CAP_LED_9);
		}
		
		Cap_I2c_Read(reg, &value);
		bRet= (value & mask)? true:false;
	}

	return bRet;
}

//========================================
// KEY 
//========================================
void CAP_getSensorDeltas(int8_t* deltas)
{
	g_cap_error = Cap_I2c_Read_Bytes(0x10, (uint8_t*)deltas, NUMBER_OF_SENSORS);
}

uint8_t CAP_getMainControl(void)
{
	uint8_t value = 0;
	g_cap_error = Cap_I2c_Read_Bytes(0x02, &value, 1);
	return value;
}

uint8_t CAP_getConfiguration2(void)
{
	uint8_t value = 0;
	g_cap_error = Cap_I2c_Read_Bytes(0x44, &value, 1);
	return value;
}
    
uint8_t CAP_getGeneralStatus(void)
{
	uint8_t value = 0;
	g_cap_error = Cap_I2c_Read_Bytes(0x02, &value, 1);
	return value;
}

uint16_t CAP_getSensorStatus(void)
{
	uint8_t value[2] = {0};
	g_cap_error = Cap_I2c_Read_Bytes(0x03, &value[0], 2);

	// reg_0x03 bit6 and bit7 is used for gesture UP and Down,
	// we not use slide function, so ignore this two bits
	return (value[1]<<6) | (value[0]&0x3f);  
}

void CAP_resetSensorStatus(void)// clear sensor interrupt status
{
	uint8_t value = 0;

	g_cap_error = Cap_I2c_Read_Bytes(0x00, &value, 1);
	if (g_cap_error)
	{
		return;
	}
  
	//Clear the INT bit (bit 0)
	value &= 0xFE;  
	g_cap_error = Cap_I2c_Write_Bytes(0x00, value, 1);
}

void CAP_enterStandby(void)
{
	uint8_t value = 0;
  
	//WAKE_TRIS = 1;
  
	// Get the current value of the general status register
	g_cap_error = Cap_I2c_Read_Bytes(0x00, &value, 1);
	if (g_cap_error)
	{
		return;
	}
  
	value |= 0x20; //0b00100000;    // Set the Standby bit 
	g_cap_error = Cap_I2c_Write_Bytes(0x00, value, 1);
}

void CAP_exitStandby(void)
{
	uint8_t value = 0;
  
	// Get the current value of the general status register
	g_cap_error = Cap_I2c_Read_Bytes(0x00, &value, 1);
	if (g_cap_error)
	{
		return;
	}
  
	value &= 0xDF; //0b11011111; 
	g_cap_error = Cap_I2c_Write_Bytes(0x00, value, 1);
}

void CAP_exitDeepSleep(void)
{
	uint8_t value = 0;

	//WAKE_LAT    = 1;
	//WAKE_TRIS   = 0;
  
	g_cap_error = Cap_I2c_Write_Bytes(0x00, value, 1);
	//TRACE_I("%s, W----REG_0x00 = 0x%x . \r\n", __func__, value);	
	//WAKE_TRIS   = 1;
	//I2C_idle();
}

void CAP_enterDeepSleep(void)
{
	uint8_t value = 0;
  
	//WAKE_LAT  = 0;
	//WAKE_TRIS = 0;
  
	// Get the current value of the general status register
	g_cap_error = Cap_I2c_Read_Bytes(0x00, &value, 1);
	//TRACE_I("%s, R----REG_0x00 = 0x%x . \r\n", __func__, value);	
	if (g_cap_error)
	{
		return;
	}
  
	value |= 0x10;//0b00010000;
	g_cap_error = Cap_I2c_Write_Bytes(0x00, value, 1);
	//TRACE_I("%s, W----REG_0x00 = 0x%x . \r\n", __func__, value);	
	//I2C_idle();
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//========================================
// MAPPING
//========================================
// cap1214 cs ---> logic button
static DRV_BTN_ENUM CS_TO_BTN(CAP_CS_ENUM cs) 
{
	if(cs>=CAP_CS_MAX)
	{
		return LIST_BTN_NONE;
	}

	return csToBtnTable[cs-CAP_CS_1];
}

static CAP_CS_ENUM BTN_TO_CS(DRV_BTN_ENUM button) 
{
	CAP_CS_ENUM cs=CAP_CS_INVALID;
	
	if((button>=LIST_BTN_1)&&(button<=LIST_BTN_DOWN))
	{
		switch(button)
		{
			case LIST_BTN_1:    cs=CAP_CS_3; 	break;
			case LIST_BTN_2:    cs=CAP_CS_2; 	break;
			case LIST_BTN_3:    cs=CAP_CS_1; 	break;
			case LIST_BTN_4:    cs=CAP_CS_4; 	break;
			case LIST_BTN_5:    cs=CAP_CS_5; 	break;
			case LIST_BTN_6:    cs=CAP_CS_6; 	break;
			case LIST_BTN_7:    cs=CAP_CS_7; 	break;
			case LIST_BTN_8:    cs=CAP_CS_8; 	break;
			case LIST_BTN_9:    cs=CAP_CS_9; 	break;
			case LIST_BTN_DEL:  cs=CAP_CS_10;	break;
			case LIST_BTN_0:    cs=CAP_CS_11;	break;
			case LIST_BTN_OK:   cs=CAP_CS_12;	break;
			case LIST_BTN_LEFT: cs=CAP_CS_13;	break;
			case LIST_BTN_DOWN: cs=CAP_CS_14;	break;
			default:
				break;
		}
	}

	return cs;
}

static CAP_LED_ENUM LED_TO_CAPLED(flex_led_name_enum led) 	// logic led ---> logic led
{
	CAP_LED_ENUM cap_led;

	switch(led)
	{
		case LIST_LED_KEY1:    cap_led = CAP_LED_3;     break;
		case LIST_LED_KEY2:    cap_led = CAP_LED_2;     break;
		case LIST_LED_KEY3:    cap_led = CAP_LED_1;     break;
		case LIST_LED_KEY4:    cap_led = CAP_LED_6;     break;
		case LIST_LED_KEY5:    cap_led = CAP_LED_5;     break;
		case LIST_LED_KEY6:    cap_led = CAP_LED_4;     break;
		case LIST_LED_KEY7:    cap_led = CAP_LED_9;     break;
		case LIST_LED_KEY8:    cap_led = CAP_LED_8;     break;
		case LIST_LED_KEY9:    cap_led = CAP_LED_7;     break;
		case LIST_LED_KEY0:    cap_led = CAP_LED_11;    break;
		case LIST_LED_DEL:     cap_led = CAP_LED_10;    break;
		default:    
			cap_led = CAP_LED_INVALID; 
      break;
	}

	return cap_led;
}

//===================================================================
// process key
//===================================================================
#if 1
static CAP_CS_ENUM FindFirstCsTouch(uint16_t stateMask)
{
	CAP_CS_ENUM cs=CAP_CS_INVALID;

	//find first touched channel in below priority:
	//CS1 > CS2 >CS3......>CS14
	if (_bit0_(stateMask))        cs = CAP_CS_1;		
	else if (_bit1_(stateMask))   cs = CAP_CS_2;		
	else if (_bit2_(stateMask))   cs = CAP_CS_3;		
	else if (_bit3_(stateMask))   cs = CAP_CS_4;		
	else if (_bit4_(stateMask))   cs = CAP_CS_5;		
	else if (_bit5_(stateMask))   cs = CAP_CS_6;
	else if (_bit6_(stateMask))   cs = CAP_CS_7;		
	else if (_bit7_(stateMask))   cs = CAP_CS_8;		
	else if (_bit8_(stateMask))   cs = CAP_CS_9;		
	else if (_bit9_(stateMask))   cs = CAP_CS_10;		
	else if (_bit10_(stateMask))  cs = CAP_CS_11;		
	else if (_bit11_(stateMask))  cs = CAP_CS_12;		
	else if (_bit12_(stateMask))  cs = CAP_CS_13;		
	else if (_bit13_(stateMask))  cs = CAP_CS_14;	
	else cs = CAP_CS_INVALID;

	return cs;
}

static void Drv_CapTouch_Key_Report(DRV_BTN_ENUM button, bool pressed)
{
	DRV_BTN_INFO_ST report;

#if defined(DEBUG_TRACE)
	switch(button)
	{
		case LIST_BTN_0:    TRACE_I("Report: BTN_0--- %s.\r\n",pressed?"press":"release"); break;			
		case LIST_BTN_1:    TRACE_I("Report: BTN_1--- %s.\r\n",pressed?"press":"release"); break;		
		case LIST_BTN_2:    TRACE_I("Report: BTN_2--- %s.\r\n",pressed?"press":"release"); break;		
		case LIST_BTN_3:    TRACE_I("Report: BTN_3--- %s.\r\n",pressed?"press":"release"); break;		
		case LIST_BTN_4:    TRACE_I("Report: BTN_4--- %s.\r\n",pressed?"press":"release"); break;		
		case LIST_BTN_5:    TRACE_I("Report: BTN_5--- %s.\r\n",pressed?"press":"release"); break;		
		case LIST_BTN_6:    TRACE_I("Report: BTN_6--- %s.\r\n",pressed?"press":"release"); break;		
		case LIST_BTN_7:    TRACE_I("Report: BTN_7--- %s.\r\n",pressed?"press":"release"); break;		
		case LIST_BTN_8:    TRACE_I("Report: BTN_8--- %s.\r\n",pressed?"press":"release"); break;		
		case LIST_BTN_9:    TRACE_I("Report: BTN_9--- %s.\r\n",pressed?"press":"release"); break;		
		case LIST_BTN_OK:   TRACE_I("Report: BTN_OK  --- %s.\r\n",pressed?"press":"release"); break;		
		case LIST_BTN_DEL:  TRACE_I("Report: BTN_DEL --- %s.\r\n",pressed?"press":"release"); break;		
		//----------------
		case LIST_BTN_DOWN: TRACE_I("Report: BTN_DOWN--- %s.\r\n",pressed?"press":"release"); break;		
		case LIST_BTN_LEFT: TRACE_I("Report: BTN_LEFT--- %s.\r\n",pressed?"press":"release"); break;
		default:
			TRACE_W("Report: BTN(%d) out of range.\r\n", button);
			break;
	}
#endif

	report.nBtn = button;
	report.nAct = pressed? LIST_BTN_ACT_PRESS:LIST_BTN_ACT_RELEASE;
	
	if(g_cap_button_cb!=NULL)
	{
		g_cap_button_cb(report); 
	}
}

//========================================
// ALGORITHM
//========================================
#if ALGORITHM_A
static CAP_CS_ENUM Algorithm_Find_Adjacent_MaxValue_CS(CAP_CS_ENUM cs)
{
	CAP_CS_ENUM ret=CAP_CS_INVALID;
	
	if(cs<=CAP_CS_12)
	{
		uint8_t i;
		uint8_t adjacent_counts=g_cap_AajacentArray[cs].nCount; 
		uint8_t max_value=0;
		CAP_CS_ENUM adjacent_cs;
		
		for(i=0; i<adjacent_counts; i++)
		{
			adjacent_cs=g_cap_AajacentArray[cs].npAdjacentGroup[i];

			if(FLEX_ABS(g_cap_raw_deltas[adjacent_cs])>max_value) //cs:1~12, array: start from 0
			{
				max_value=FLEX_ABS(g_cap_raw_deltas[adjacent_cs]);
				ret=adjacent_cs;
			}
		}
	}

	return ret;
}

static uint8_t Algorithm_Calc_Adjacent_AvergValue(CAP_CS_ENUM cs)
{
	uint8_t value=0;
	
	if(cs<=CAP_CS_12)
	{
		uint8_t i;
		uint8_t adjacent_counts=g_cap_AajacentArray[cs].nCount;
		CAP_CS_ENUM adjacent_cs;
		
		for(i=0; i<adjacent_counts; i++)
		{
			adjacent_cs=g_cap_AajacentArray[cs].npAdjacentGroup[i];
			value+=FLEX_ABS(g_cap_raw_deltas[adjacent_cs]);
		}
		value=value/adjacent_counts;
	}

	return value;
}

static bool Algorithm_A(CAP_CS_ENUM detect_cs, CAP_CS_ENUM* optimize_cs)
{
	bool bOptimized=false;
	CAP_CS_ENUM compare_cs;
	uint8_t detect_cs_delta; 
	uint8_t compare_cs_delta;

	compare_cs=Algorithm_Find_Adjacent_MaxValue_CS(detect_cs);
	detect_cs_delta = FLEX_ABS(g_cap_raw_deltas[detect_cs]);
	compare_cs_delta = FLEX_ABS(g_cap_raw_deltas[compare_cs]);
	TRACE_I("[A]:det_delta=%d, cmp_delta=%d.\r\n", detect_cs_delta, compare_cs_delta);

	*optimize_cs=detect_cs; //init

	if(compare_cs_delta>detect_cs_delta) // 
	{
		TRACE_I("[A]:optimized (adjacent max delta'abs > det delta'abs).\r\n");
		bOptimized=true;
		*optimize_cs=compare_cs;
	}
	else if(compare_cs_delta<=detect_cs_delta)
	{
		uint8_t detect_cs_adjacent_delta;
		uint8_t compare_cs_adjacent_delta; 
	
		detect_cs_adjacent_delta = Algorithm_Calc_Adjacent_AvergValue(detect_cs);
		compare_cs_adjacent_delta = Algorithm_Calc_Adjacent_AvergValue(compare_cs);
		
		if(compare_cs_adjacent_delta>detect_cs_adjacent_delta)
		{
			TRACE_I("[A]:optimized (adjacent max delta's aver > det delta's aver).\r\n");
			bOptimized=true;
			*optimize_cs=compare_cs;
		}
	}

	return bOptimized;
}
#endif

#if ALGORITHM_B
static bool Algorithm_B(CAP_CS_ENUM detect_cs, CAP_CS_ENUM* optimize_cs)
{
	bool bOptimized=false;
	uint8_t index=0;
	uint8_t count=sizeof(g_cap_ColumnArray)/sizeof(CS_ALGORITHM_COLUMN_ST);


	*optimize_cs=detect_cs;//init

// find which index	
	for(index=0; index<count; index++)
	{
		if(detect_cs==g_cap_ColumnArray[index].nDetect)
		{
			if(FLEX_ABS(g_cap_raw_deltas[g_cap_ColumnArray[index].nFarther])>=FLEX_ABS(g_cap_raw_deltas[g_cap_ColumnArray[index].nCloser])) 
			{
				TRACE_I("[B]:optimized (farther's delta>closer's delta.\r\n");
				bOptimized=true;
				*optimize_cs = g_cap_ColumnArray[index].nBackup; // Backup is the king
			}
			break;
		}
	}

	return bOptimized;
}
#endif

#if ALGORITHM_C
/*
	there is another CS'delta is larger than detected CS'delta,
	select the king cs
*/
static bool Algorithm_C(CAP_CS_ENUM detect_cs, CAP_CS_ENUM* optimize_cs)
{
	bool bOptimized=false;
	
	uint8_t cmp_cs_index=0;
	uint8_t cmp_cs_delta_abs;
	uint16_t stateMask=0;
	
	cmp_cs_index=Find_MaxAbsValue_Index(g_cap_raw_deltas,NUMBER_OF_SENSORS);

	if(cmp_cs_index !=detect_cs)
	{
		TRACE_I("[C]:det_delta=%d, cmp_delta=%d.\r\n", g_cap_raw_deltas[detect_cs], g_cap_raw_deltas[cmp_cs_index]);
		
		cmp_cs_delta_abs=FLEX_ABS(g_cap_raw_deltas[cmp_cs_index]);
		
		switch(cmp_cs_index)
		{
			case 0: 	 if(cmp_cs_delta_abs>=CS_1_THRESHOLD)     {bOptimized=true; stateMask=CS1_BIT;}  break;
			case 1: 	 if(cmp_cs_delta_abs>=CS_2_THRESHOLD)     {bOptimized=true; stateMask=CS2_BIT;}  break;
			case 2: 	 if(cmp_cs_delta_abs>=CS_3_THRESHOLD)     {bOptimized=true; stateMask=CS3_BIT;}  break;
			case 3: 	 if(cmp_cs_delta_abs>=CS_4_THRESHOLD)     {bOptimized=true; stateMask=CS4_BIT;}  break;
			case 4: 	 if(cmp_cs_delta_abs>=CS_5_THRESHOLD)     {bOptimized=true; stateMask=CS5_BIT;}  break;
			case 5: 	 if(cmp_cs_delta_abs>=CS_6_THRESHOLD)     {bOptimized=true; stateMask=CS6_BIT;}  break;
			case 6: 	 if(cmp_cs_delta_abs>=CS_7_THRESHOLD)     {bOptimized=true; stateMask=CS7_BIT;}  break;
			case 7: 	 if(cmp_cs_delta_abs>=CS_GROUP_THRESHOLD) {bOptimized=true; stateMask=CS8_BIT;}  break;
			case 8: 	 if(cmp_cs_delta_abs>=CS_GROUP_THRESHOLD) {bOptimized=true; stateMask=CS9_BIT;}  break;
			case 9: 	 if(cmp_cs_delta_abs>=CS_GROUP_THRESHOLD) {bOptimized=true; stateMask=CS10_BIT;} break;
			case 10:	 if(cmp_cs_delta_abs>=CS_GROUP_THRESHOLD) {bOptimized=true; stateMask=CS11_BIT;} break;
			case 11:	 if(cmp_cs_delta_abs>=CS_GROUP_THRESHOLD) {bOptimized=true; stateMask=CS12_BIT;} break;
			case 12:	 if(cmp_cs_delta_abs>=CS_GROUP_THRESHOLD) {bOptimized=true; stateMask=CS13_BIT;} break;
			case 13:	 if(cmp_cs_delta_abs>=CS_GROUP_THRESHOLD) {bOptimized=true; stateMask=CS14_BIT;} break;
			default:
				TRACE_W("[C]:goto default(%d)\r\n", cmp_cs_index);
				break;
		}
		//TRACE_I("Algorithm_C:stateMask=%d\r\n", stateMask);

		if(bOptimized)
		{
			TRACE_I("[C]:optimized.\r\n");
			*optimize_cs = FindFirstCsTouch(stateMask);
		}
	}

	return bOptimized;
}
#endif

#if ALGORITHM_D
/*
	if in the same column, there is another CS'delta is larger than detected CS'delta,
	select the king cs
*/
static bool Algorithm_D(CAP_CS_ENUM detect_cs, CAP_CS_ENUM* optimize_cs)
{
	bool bOptimized=false;
	uint8_t cmp_cs_index=0;
	uint8_t cmp_cs_delta_abs;
	uint16_t stateMask=0;
	int8_t cmp_data_array[4]={0};

	switch(detect_cs)
	{
		case CAP_CS_3:  // BTN_1
		case CAP_CS_4:  // BTN_4
		case CAP_CS_7:  // BTN_7
		case CAP_CS_12: // BTN_OK
			cmp_data_array[0]=g_cap_raw_deltas[2];  // CS3's delta
			cmp_data_array[1]=g_cap_raw_deltas[3];  // CS4's delta
			cmp_data_array[2]=g_cap_raw_deltas[6];  // CS7's delta
			cmp_data_array[3]=g_cap_raw_deltas[11]; // CS12's delta
			/*
			TRACE_I("[D]:BTN_1=%d\r\n",cmp_data_array[0]);
			TRACE_I("[D]:BTN_4=%d\r\n",cmp_data_array[1]);
			TRACE_I("[D]:BTN_7=%d\r\n",cmp_data_array[2]);
			TRACE_I("[D]:BTN_OK=%d\r\n",cmp_data_array[3]);
			*/
			switch(Find_MaxAbsValue_Index(cmp_data_array,4))
			{
				case 0: cmp_cs_index=CAP_CS_3;	TRACE_I("[D]:KING=BTN_1\r\n"); break; 
				case 1: cmp_cs_index=CAP_CS_4;	TRACE_I("[D]:KING=BTN_4\r\n"); break; 
				case 2: cmp_cs_index=CAP_CS_7;	TRACE_I("[D]:KING=BTN_7\r\n"); break; 
				case 3: cmp_cs_index=CAP_CS_12;	TRACE_I("[D]:KING=BTN_OK\r\n"); break; 
				default:
					break;
			}
			break;

		case CAP_CS_2:  // BTN_2
		case CAP_CS_5:  // BTN_5
		case CAP_CS_8:  // BTN_8
		case CAP_CS_11: // BTN_0
			cmp_data_array[0]=g_cap_raw_deltas[1];  // CS2's delta
			cmp_data_array[1]=g_cap_raw_deltas[4];  // CS5's delta
			cmp_data_array[2]=g_cap_raw_deltas[7];  // CS8's delta
			cmp_data_array[3]=g_cap_raw_deltas[10]; // CS11's delta
			/*
			TRACE_I("[D]:BTN_2=%d\r\n",cmp_data_array[0]);
			TRACE_I("[D]:BTN_5=%d\r\n",cmp_data_array[1]);
			TRACE_I("[D]:BTN_8=%d\r\n",cmp_data_array[2]);
			TRACE_I("[D]:BTN_0=%d\r\n",cmp_data_array[3]);
			*/
			switch(Find_MaxAbsValue_Index(cmp_data_array,4))
			{
				case 0: cmp_cs_index=CAP_CS_2;	TRACE_I("[D]:KING=BTN_2\r\n"); break; 
				case 1: cmp_cs_index=CAP_CS_5;	TRACE_I("[D]:KING=BTN_5\r\n"); break; 
				case 2: cmp_cs_index=CAP_CS_8;	TRACE_I("[D]:KING=BTN_8\r\n"); break; 
				case 3: cmp_cs_index=CAP_CS_11;	TRACE_I("[D]:KING=BTN_0\r\n"); break; 
				default:
					break;
			}
			break;

		case CAP_CS_1:  // BTN_3
		case CAP_CS_6:  // BTN_6
		case CAP_CS_9:  // BTN_9
		case CAP_CS_10: // BTN_DEL
			cmp_data_array[0]=g_cap_raw_deltas[0];  // CS1's delta
			cmp_data_array[1]=g_cap_raw_deltas[5];  // CS6's delta
			cmp_data_array[2]=g_cap_raw_deltas[8];  // CS9's delta
			cmp_data_array[3]=g_cap_raw_deltas[9];  // CS10's delta
			/*
			TRACE_I("[D]:BTN_3=%d\r\n",cmp_data_array[0]);
			TRACE_I("[D]:BTN_6=%d\r\n",cmp_data_array[1]);
			TRACE_I("[D]:BTN_9=%d\r\n",cmp_data_array[2]);
			TRACE_I("[D]:BTN_DEL=%d\r\n",cmp_data_array[3]);
			*/
			switch(Find_MaxAbsValue_Index(cmp_data_array,4))
			{
				case 0: cmp_cs_index=CAP_CS_1;	TRACE_I("[D]:KING=BTN_3\r\n"); break; 
				case 1: cmp_cs_index=CAP_CS_6;	TRACE_I("[D]:KING=BTN_6\r\n"); break; 
				case 2: cmp_cs_index=CAP_CS_9;	TRACE_I("[D]:KING=BTN_9\r\n"); break; 
				case 3: cmp_cs_index=CAP_CS_10;	TRACE_I("[D]:KING=BTN_DEL\r\n"); break; 
				default:
					break;
			}
			break;

		default:
			break;
	}

	if(cmp_cs_index !=detect_cs)
	{
		TRACE_I("[D]:detect_cs=%d, cmp_cs=%d.\r\n", detect_cs, cmp_cs_index);
		TRACE_I("[D]:detect_cs_delta=%d, cmp_cs_delta=%d.\r\n", g_cap_raw_deltas[detect_cs], g_cap_raw_deltas[cmp_cs_index]);
		
		cmp_cs_delta_abs=FLEX_ABS(g_cap_raw_deltas[cmp_cs_index]);
		
		switch(cmp_cs_index)
		{
			case 0: 	 if(cmp_cs_delta_abs>=CS_1_THRESHOLD)     {bOptimized=true; stateMask=CS1_BIT;}  break;
			case 1: 	 if(cmp_cs_delta_abs>=CS_2_THRESHOLD)     {bOptimized=true; stateMask=CS2_BIT;}  break;
			case 2: 	 if(cmp_cs_delta_abs>=CS_3_THRESHOLD)     {bOptimized=true; stateMask=CS3_BIT;}  break;
			case 3: 	 if(cmp_cs_delta_abs>=CS_4_THRESHOLD)     {bOptimized=true; stateMask=CS4_BIT;}  break;
			case 4: 	 if(cmp_cs_delta_abs>=CS_5_THRESHOLD)     {bOptimized=true; stateMask=CS5_BIT;}  break;
			case 5: 	 if(cmp_cs_delta_abs>=CS_6_THRESHOLD)     {bOptimized=true; stateMask=CS6_BIT;}  break;
			case 6: 	 if(cmp_cs_delta_abs>=CS_7_THRESHOLD)     {bOptimized=true; stateMask=CS7_BIT;}  break;
			case 7: 	 if(cmp_cs_delta_abs>=CS_GROUP_THRESHOLD) {bOptimized=true; stateMask=CS8_BIT;}  break;
			case 8: 	 if(cmp_cs_delta_abs>=CS_GROUP_THRESHOLD) {bOptimized=true; stateMask=CS9_BIT;}  break;
			case 9: 	 if(cmp_cs_delta_abs>=CS_GROUP_THRESHOLD) {bOptimized=true; stateMask=CS10_BIT;} break;
			case 10:	 if(cmp_cs_delta_abs>=CS_GROUP_THRESHOLD) {bOptimized=true; stateMask=CS11_BIT;} break;
			case 11:	 if(cmp_cs_delta_abs>=CS_GROUP_THRESHOLD) {bOptimized=true; stateMask=CS12_BIT;} break;
			case 12:	 if(cmp_cs_delta_abs>=CS_GROUP_THRESHOLD) {bOptimized=true; stateMask=CS13_BIT;} break;
			case 13:	 if(cmp_cs_delta_abs>=CS_GROUP_THRESHOLD) {bOptimized=true; stateMask=CS14_BIT;} break;
			default:
				TRACE_W("[D]:goto default(%d)\r\n", cmp_cs_index);
				break;
		}
		//TRACE_I("[D]:stateMask=%d\r\n", stateMask);

		if(bOptimized)
		{
			TRACE_I("[D]:optimized.\r\n");
			*optimize_cs = FindFirstCsTouch(stateMask);
		}
	}
	
	return bOptimized;
}
#endif

static DRV_BTN_ENUM Drv_CapTouch_Get_Current_Button(uint16_t status_mask)
{
	DRV_BTN_ENUM btn;
#if (ALGORITHM_A||ALGORITHM_B||ALGORITHM_C||ALGORITHM_D)
	CAP_CS_ENUM detected_cs;
	CAP_CS_ENUM optimized_cs;
#endif

	//TRACE_I("=========================================.\r\n");
//------------------------------------------------
	detected_cs = FindFirstCsTouch(status_mask);
	btn = CS_TO_BTN(detected_cs);
	switch(btn)
	{
		case LIST_BTN_0:    TRACE_I("detected->BTN_0.\r\n");	 break;
		case LIST_BTN_1:    TRACE_I("detected->BTN_1.\r\n");	 break;
		case LIST_BTN_2:    TRACE_I("detected->BTN_2.\r\n");	 break;
		case LIST_BTN_3:    TRACE_I("detected->BTN_3.\r\n");	 break;
		case LIST_BTN_4:    TRACE_I("detected->BTN_4.\r\n");	 break;
		case LIST_BTN_5:    TRACE_I("detected->BTN_5.\r\n"); 	 break;
		case LIST_BTN_6:    TRACE_I("detected->BTN_6.\r\n");	 break;
		case LIST_BTN_7:    TRACE_I("detected->BTN_7.\r\n");	 break;
		case LIST_BTN_8:    TRACE_I("detected->BTN_8.\r\n");	 break;
		case LIST_BTN_9:    TRACE_I("detected->BTN_9.\r\n");	 break;
		case LIST_BTN_OK: 	TRACE_I("detected->BTN_OK.\r\n");  break;
		case LIST_BTN_DEL:	TRACE_I("detected->BTN_DEL.\r\n"); break;
		case LIST_BTN_LEFT: TRACE_I("detected->BTN_LEFT.\r\n");break;
		case LIST_BTN_DOWN: TRACE_I("detected->BTN_DOWN.\r\n");break;
		default:
			TRACE_W("detected->BTN(%d) out of range.\r\n", btn);
			break;
	}
	/*
	if(LIST_BTN_5==btn)
	{
		TRACE_I("BTN_5=%d, BTN_8=%d.\r\n", g_cap_raw_deltas[CAP_CS_5], g_cap_raw_deltas[CAP_CS_8]);
		TRACE_I("BTN_2=%d, BTN_0=%d.\r\n", g_cap_raw_deltas[CAP_CS_2], g_cap_raw_deltas[CAP_CS_11]);
	}
	else if(LIST_BTN_9==btn)
	{
		TRACE_I("BTN_9=%d, BTN_R=%d.\r\n", g_cap_raw_deltas[CAP_CS_9], g_cap_raw_deltas[CAP_CS_10]);
		TRACE_I("BTN_6=%d, BTN_0=%d.\r\n", g_cap_raw_deltas[CAP_CS_6], g_cap_raw_deltas[CAP_CS_11]);
	}
	else if(LIST_BTN_0==btn)
	{
		TRACE_I("BTN_0=%d, BTN_R=%d.\r\n", g_cap_raw_deltas[CAP_CS_11], g_cap_raw_deltas[CAP_CS_10]);
		TRACE_I("BTN_8=%d, BTN_9=%d.\r\n", g_cap_raw_deltas[CAP_CS_8],  g_cap_raw_deltas[CAP_CS_9]);
	}
	*/
//------------------------------------------------
#if ALGORITHM_C
	if(detected_cs<=CAP_CS_12)
	{
		if(Algorithm_C(detected_cs,&optimized_cs))
		{
			btn = CS_TO_BTN(optimized_cs);
			switch(btn)
			{
				case LIST_BTN_0:			TRACE_I("[C]->BTN_0.\r\n"); 		break;
				case LIST_BTN_1:			TRACE_I("[C]->BTN_1.\r\n"); 		break;
				case LIST_BTN_2:			TRACE_I("[C]->BTN_2.\r\n"); 		break;
				case LIST_BTN_3:			TRACE_I("[C]->BTN_3.\r\n"); 		break;
				case LIST_BTN_4:			TRACE_I("[C]->BTN_4.\r\n"); 		break;
				case LIST_BTN_5:			TRACE_I("[C]->BTN_5.\r\n"); 		break;
				case LIST_BTN_6:			TRACE_I("[C]->BTN_6.\r\n"); 		break;
				case LIST_BTN_7:			TRACE_I("[C]->BTN_7.\r\n"); 		break;
				case LIST_BTN_8:			TRACE_I("[C]->BTN_8.\r\n"); 		break;
				case LIST_BTN_9:			TRACE_I("[C]->BTN_9.\r\n"); 		break;
				case LIST_BTN_OK: 		TRACE_I("[C]->BTN_OK.\r\n");		break;
				case LIST_BTN_DEL:		TRACE_I("[C]->BTN_DEL.\r\n"); 	break;
				case LIST_BTN_LEFT: 	TRACE_I("[C]->BTN_LEFT.\r\n");	break;
				case LIST_BTN_DOWN: 	TRACE_I("[C]->BTN_DOWN.\r\n");	break;
				default:
					TRACE_W("[C]->BTN(%d) out of range.\r\n", btn);
					break;
			}
		}
	}
#endif

//------------------------------------------------
#if ALGORITHM_D
	if(detected_cs<=CAP_CS_12)
	{
		if(Algorithm_D(detected_cs,&optimized_cs))
		{
			btn = CS_TO_BTN(optimized_cs);
			switch(btn)
			{
				case LIST_BTN_0:			TRACE_I("[D]->BTN_0.\r\n"); 		break;
				case LIST_BTN_1:			TRACE_I("[D]->BTN_1.\r\n"); 		break;
				case LIST_BTN_2:			TRACE_I("[D]->BTN_2.\r\n"); 		break;
				case LIST_BTN_3:			TRACE_I("[D]->BTN_3.\r\n"); 		break;
				case LIST_BTN_4:			TRACE_I("[D]->BTN_4.\r\n"); 		break;
				case LIST_BTN_5:			TRACE_I("[D]->BTN_5.\r\n"); 		break;
				case LIST_BTN_6:			TRACE_I("[D]->BTN_6.\r\n"); 		break;
				case LIST_BTN_7:			TRACE_I("[D]->BTN_7.\r\n"); 		break;
				case LIST_BTN_8:			TRACE_I("[D]->BTN_8.\r\n"); 		break;
				case LIST_BTN_9:			TRACE_I("[D]->BTN_9.\r\n"); 		break;
				case LIST_BTN_OK: 		TRACE_I("[D]->BTN_OK.\r\n");		break;
				case LIST_BTN_DEL:		TRACE_I("[D]->BTN_DEL.\r\n"); 	break;
				case LIST_BTN_LEFT: 	TRACE_I("[D]->BTN_LEFT.\r\n");	break;
				case LIST_BTN_DOWN: 	TRACE_I("[D]->BTN_DOWN.\r\n");	break;
				default:
					TRACE_W("[D]->BTN(%d) out of range.\r\n", btn);
					break;
			}
		}
	}
#endif

//------------------------------------------------
#if ALGORITHM_A
	if(Algorithm_A(detected_cs, &optimized_cs))
	{
		btn = CS_TO_BTN(optimized_cs);
		switch(btn)
		{
			case LIST_BTN_0:    TRACE_I("[A]->BTN_0.\r\n"); 	break;
			case LIST_BTN_1:    TRACE_I("[A]->BTN_1.\r\n"); 	break;
			case LIST_BTN_2:    TRACE_I("[A]->BTN_2.\r\n"); 	break;
			case LIST_BTN_3:    TRACE_I("[A]->BTN_3.\r\n"); 	break;
			case LIST_BTN_4:    TRACE_I("[A]->BTN_4.\r\n"); 	break;
			case LIST_BTN_5:    TRACE_I("[A]->BTN_5.\r\n"); 	break;
			case LIST_BTN_6:    TRACE_I("[A]->BTN_6.\r\n"); 	break;
			case LIST_BTN_7:    TRACE_I("[A]->BTN_7.\r\n"); 	break;
			case LIST_BTN_8:    TRACE_I("[A]->BTN_8.\r\n"); 	break;
			case LIST_BTN_9:    TRACE_I("[A]->BTN_9.\r\n"); 	break;
			case LIST_BTN_OK:   TRACE_I("[A]->BTN_OK.\r\n");	break;
			case LIST_BTN_DEL:  TRACE_I("[A]->BTN_DEL.\r\n"); break;
			case LIST_BTN_LEFT: TRACE_I("[A]->BTN_LEFT.\r\n");break;
			case LIST_BTN_DOWN: TRACE_I("[A]->BTN_DOWN.\r\n");break;
			default:
				TRACE_W("[A]->BTN(%d) out of range.\r\n",btn);
				break;
		}
	}
#endif

//------------------------------------------------
#if ALGORITHM_B
	if(Algorithm_B(detected_cs, &optimized_cs))
	{
		btn = CS_TO_BTN(optimized_cs);
		switch(btn)
		{
			case LIST_BTN_0:    TRACE_I("[B]->BTN_0.\r\n"); 	break;
			case LIST_BTN_1:    TRACE_I("[B]->BTN_1.\r\n"); 	break;
			case LIST_BTN_2:    TRACE_I("[B]->BTN_2.\r\n"); 	break;
			case LIST_BTN_3:    TRACE_I("[B]->BTN_3.\r\n"); 	break;
			case LIST_BTN_4:    TRACE_I("[B]->BTN_4.\r\n"); 	break;
			case LIST_BTN_5:    TRACE_I("[B]->BTN_5.\r\n"); 	break;
			case LIST_BTN_6:    TRACE_I("[B]->BTN_6.\r\n"); 	break;
			case LIST_BTN_7:    TRACE_I("[B]->BTN_7.\r\n"); 	break;
			case LIST_BTN_8:    TRACE_I("[B]->BTN_8.\r\n"); 	break;
			case LIST_BTN_9:    TRACE_I("[B]->BTN_9.\r\n"); 	break;
			case LIST_BTN_OK:   TRACE_I("[B]->BTN_OK.\r\n");	break;
			case LIST_BTN_DEL:  TRACE_I("[B]->BTN_DEL.\r\n"); break;
			case LIST_BTN_LEFT: TRACE_I("[B]->BTN_LEFT.\r\n");break;
			case LIST_BTN_DOWN: TRACE_I("[B]->BTN_DOWN.\r\n");break;
			default:
				TRACE_W("[B]->BTN(%d) out of range.\r\n",btn);
				break;
		}
	}
#endif

//------------------------------------------------
	return btn;
}

#endif

//========================================
// FUNCS
//========================================
static bool Drv_CapTouch_Init(void)
{
	bool ret=false;
	
	ret=Cap_Init();
	
	return ret;
}

static bool Drv_CapTouch_Get_Led(flex_led_name_enum led_name)
{
	return cap_led_get_status(LED_TO_CAPLED(led_name));
}

static void Drv_CapTouch_Set_Led(flex_led_name_enum led_name, bool on)
{
	cap_led_set_output(LED_TO_CAPLED(led_name),on);	
}

static bool Drv_CapTouch_Polling_Supported(void)
{
	return true;
}

/*
	There are still 5 cases can be well processed
	
	1: sometimes, when press BTN_8, it only detect BTN_5
		trace is as folloing:
		[CAP1214]--[I]--########################## stateMask=0x40, Num=1, g_cap_button=8.
		[CAP1214]--[I]--########################## stateMask=0x40, Num=1, g_cap_button=8.
		[CAP1214]--[I]--########################## stateMask=0x0, Num=0, g_cap_button=8.
		[CAP1214]--[W]--report: [RELEASE]
		[CAP1214]--[I]--============================report: BTN_7--- release.
		[INPUT]--[I]--Parse: BTN_KEY7 release.
		[APP]--[I]--KEY_7---OFF
		[CAP1214]--[I]--########################## stateMask=0x10, Num=1, g_cap_button=0.
		[CAP1214]--[I]--detected->BTN_5.
		[CAP1214]--[W]--report: [PRESS]
		[CAP1214]--[I]--============================report: BTN_5--- press.
		[INPUT]--[I]--Parse: BTN_KEY5 press.
		[APP]--[I]--KEY_5---ON
		[CAP1214]--[I]--########################## stateMask=0x10, Num=1, g_cap_button=6.
		[CAP1214]--[I]--########################## stateMask=0x10, Num=1, g_cap_button=6.
		[CAP1214]--[I]--########################## stateMask=0x10, Num=1, g_cap_button=6.
		[CAP1214]--[I]--########################## stateMask=0x10, Num=1, g_cap_button=6.

	2: we can not well done about the following 2 cases:
	 (1):BUTTON_1 is pressed, BUTTON_5 and BUTTON_8 are both pressed, then BUTTON is released, should be report?
	 (2):BUTTON_1 is pressed, BUTTON_5 or BUTTON_8 is pressed, but chip detectd both 5&8 is detected, then BUTTON is released, should be report?

	3: BUTTON_A is pressed,whether it is detected or not, BUTTON_B reach the threshold, and BUTTON_B's influence to adjacnet is large than BUTTON_A
	   it sounds impossible, but that's the way it is
		trace is as folloing:(press 7-->release 7--->press 8-->release 8--->press 9)
		[CAP1214]--[I]--=========================================.
		[CAP1214]--[I]--detected->BTN_7.
		[CAP1214]--[I]--########################## stateMask=0x40.
		[CAP1214]--[I]--########################## stateMask=0x40.
		[CAP1214]--[I]--########################## stateMask=0x40.
		[CAP1214]--[I]--########################## stateMask=0x40.
		[CAP1214]--[I]--########################## stateMask=0x40.
		[CAP1214]--[I]--########################## stateMask=0x40.
		[CAP1214]--[I]--########################## stateMask=0x10.
		[CAP1214]--[I]--=========================================.
		[CAP1214]--[I]--detected->BTN_5.
		[CAP1214]--[I]--BTN_5=51, BTN_8=-2.
		[CAP1214]--[I]--BTN_2=-2, BTN_0=-1.
		[CAP1214]--[I]--########################## stateMask=0x10.
		[CAP1214]--[I]--########################## stateMask=0x10.	  
		[CAP1214]--[I]--########################## stateMask=0x10.
		[CAP1214]--[I]--########################## stateMask=0x10.
		[CAP1214]--[I]--########################## stateMask=0x10.
		[CAP1214]--[I]--########################## stateMask=0x100.
		[CAP1214]--[I]--=========================================.
		[CAP1214]--[I]--detected->BTN_9.
		[CAP1214]--[I]--BTN_9=126, BTN_RET=1.
		[CAP1214]--[I]--BTN_6=17, BTN_0=0.
		[CAP1214]--[I]--########################## stateMask=0x100.		

	4: when BUTTON_8 is pressed, LED_8 turn on,  then BUTTON_8 release, LED_8 OFF, but BUTTON_5's LED blink once 

	5: BUTTON_DEL is pressed , but BUTTON_9 is detected
		[CAP1214]--[I]--111111111111111111111111111111111111111111--> TOUCH, stateMask=0x100, Num=1, g_cap_button=0.
		[CAP1214]--[I]--KEY1(000) 			 KEY2(001)			 KEY3(000)
		[CAP1214]--[I]--KEY4(000) 			 KEY5(003)			 KEY6(001)
		[CAP1214]--[I]--KEY7(-02) 			 KEY8(002)			 KEY9(059)
		[CAP1214]--[I]--OK	(-02) 			 KEY0(-08)			 DEL (-01)
		[CAP1214]--[I]--detected->BTN_9.
		[CAP1214]--[I]--[D]:KING=BTN_9
		[CAP1214]--[I]--Report: BTN_9--- press.
		[CAP1214]--[I]--000000000000000000000000000000000000000000--> RELEASE, stateMask=0x0, Num=0, g_cap_button=10.
		[CAP1214]--[I]--KEY1(000) 			 KEY2(000)			 KEY3(-01)
		[CAP1214]--[I]--KEY4(-01) 			 KEY5(000)			 KEY6(000)
		[CAP1214]--[I]--KEY7(000) 			 KEY8(-02)			 KEY9(007)
		[CAP1214]--[I]--OK	(000) 			 KEY0(-09)			 DEL (-88)
		[CAP1214]--[I]--Report: BTN_9--- release.
*/
static void Drv_CapTouch_Polling(void)
{
	uint16_t stateMask=0;
	uint8_t Num = 0;
#if ALGORITHM_FILTER
	static uint8_t filter_data_count=FILTER_COUNT;
#endif
	static uint16_t last_stateMask=0;
	
	static bool flag_Step2=false;
	static DRV_BTN_ENUM  Step2_btn=LIST_BTN_NONE;

	if(flag_Step2)
	{
		//TRACE_I("A->A+B+...->B----step2.\r\n");
		flag_Step2=false;
		Drv_CapTouch_Key_Report(Step2_btn, true);
		g_cap_button = Step2_btn;
		Step2_btn=LIST_BTN_NONE;
		return;
	}
	
//-----------------------------------------
//skip if i2c is currently busy
//-----------------------------------------
	if(HAL_I2C_STATE_READY != HAL_I2C_GetState(&CAP1214_I2C_CHANNEL))
	{
		return;
	}	

//-----------------------------------------
// Read the state mask value & sensor values 
//-----------------------------------------
  stateMask = CAP_getSensorStatus();
//-----------------------------------------
// ignore same status
//-----------------------------------------
	if(last_stateMask==stateMask) 
	{
	/* read delta */
		//CAP_getSensorDeltas(g_cap_raw_deltas);
	/* clear sensor interrupt status */
		CAP_resetSensorStatus();
		return;
	}
	else
	{
	#if ALGORITHM_FILTER
		if((stateMask>0)&&(stateMask!=0x1000)&&(stateMask!=0x2000)&&(filter_data_count>0))
		{
			// when press button(except BTN_LEFT/DOWN, first data be ignored
			filter_data_count--;
			
		#if defined(DEBUG_TRACE)
			TRACE_I("$$$$$$$$$$$$$$$$$$$$$--> filter, stateMask=0x%x.\r\n",stateMask);
			switch(CS_TO_BTN(FindFirstCsTouch(stateMask)))
			{
				case LIST_BTN_0:		TRACE_I("filter->BTN_0.\r\n");	 break;
				case LIST_BTN_1:		TRACE_I("filter->BTN_1.\r\n");	 break;
				case LIST_BTN_2:		TRACE_I("filter->BTN_2.\r\n");	 break;
				case LIST_BTN_3:		TRACE_I("filter->BTN_3.\r\n");	 break;
				case LIST_BTN_4:		TRACE_I("filter->BTN_4.\r\n");	 break;
				case LIST_BTN_5:		TRACE_I("filter->BTN_5.\r\n");	 break;
				case LIST_BTN_6:		TRACE_I("filter->BTN_6.\r\n");	 break;
				case LIST_BTN_7:		TRACE_I("filter->BTN_7.\r\n");	 break;
				case LIST_BTN_8:		TRACE_I("filter->BTN_8.\r\n");	 break;
				case LIST_BTN_9:		TRACE_I("filter->BTN_9.\r\n");	 break;
				case LIST_BTN_OK: 	TRACE_I("filter->BTN_OK.\r\n");  break;
				case LIST_BTN_DEL:	TRACE_I("filter->BTN_DEL.\r\n"); break;
				case LIST_BTN_LEFT: TRACE_I("filter->BTN_LEFT.\r\n");break;
				case LIST_BTN_DOWN: TRACE_I("filter->BTN_DOWN.\r\n");break;
				default:
					TRACE_W("filter->BTN out of range.\r\n");
					break;
			}
		#endif
		/* clear sensor interrupt status */
			CAP_resetSensorStatus();
			
			return;
		}
		else
		{
			last_stateMask=stateMask; 

			// when release button, reset count
			if((0==stateMask)&&(0==filter_data_count))
			{
				filter_data_count=FILTER_COUNT;
				TRACE_I("@@@reset FILTER_COUNT\r\n");
			}
		}
	#else
		last_stateMask=stateMask; 
	#endif
	}

//---------------------------------------------
// process button
//---------------------------------------------
	if(0==stateMask)
	{
		Num=0;
		TRACE_I("000000000000000000000000000000000000000000--> stateMask=0x%x, Num=%d, g_cap_button=%d.\r\n",stateMask, Num, g_cap_button);
	/* read delta */
		CAP_getSensorDeltas(g_cap_raw_deltas);
	#if defined(DEBUG_TRACE)
		if(HAL_OK == g_cap_error)
		{
			TRACE_I("KEY1(%3d) 	 KEY2(%3d) 	 KEY3(%3d)\r\n",g_cap_raw_deltas[2], g_cap_raw_deltas[1],	g_cap_raw_deltas[0]);
			TRACE_I("KEY4(%3d) 	 KEY5(%3d) 	 KEY6(%3d)\r\n",g_cap_raw_deltas[3], g_cap_raw_deltas[4],	g_cap_raw_deltas[5]);
			TRACE_I("KEY7(%3d) 	 KEY8(%3d) 	 KEY9(%3d)\r\n",g_cap_raw_deltas[6], g_cap_raw_deltas[7],	g_cap_raw_deltas[8]);
			TRACE_I("OK  (%3d) 	 KEY0(%3d) 	 DEL (%3d)\r\n",g_cap_raw_deltas[11],g_cap_raw_deltas[10], g_cap_raw_deltas[9]);
			//TRACE_I("LEFT(%3d)\r\n",g_cap_raw_deltas[12]);
			//TRACE_I("DOWN(%3d)\r\n",g_cap_raw_deltas[13]);
		} 
	#endif
	
	/* clear sensor interrupt status */
		CAP_resetSensorStatus();
			
	/* report */
		if(g_cap_button!=LIST_BTN_NONE)
		{
			//TRACE_I("report: [RELEASE]\r\n");
			Drv_CapTouch_Key_Report(g_cap_button, false);
			g_cap_button=LIST_BTN_NONE;
		}
	}
	else 
	{
		for(uint8_t i=0;i<(sizeof(stateMask) * 8);i++)
		{
			if(stateMask & (1<<i))
			{
				Num++;
			}
		}
	
		TRACE_I("111111111111111111111111111111111111111111--> stateMask=0x%x, Num=%d, g_cap_button=%d.\r\n",stateMask, Num, g_cap_button);
	/* read delta */
		CAP_getSensorDeltas(g_cap_raw_deltas);
	#if defined(DEBUG_TRACE)
		if(HAL_OK == g_cap_error)
		{
			TRACE_I("KEY1(%3d) 	 KEY2(%3d) 	 KEY3(%3d)\r\n",g_cap_raw_deltas[2], g_cap_raw_deltas[1],	g_cap_raw_deltas[0]);
			TRACE_I("KEY4(%3d) 	 KEY5(%3d) 	 KEY6(%3d)\r\n",g_cap_raw_deltas[3], g_cap_raw_deltas[4],	g_cap_raw_deltas[5]);
			TRACE_I("KEY7(%3d) 	 KEY8(%3d) 	 KEY9(%3d)\r\n",g_cap_raw_deltas[6], g_cap_raw_deltas[7],	g_cap_raw_deltas[8]);
			TRACE_I("OK  (%3d) 	 KEY0(%3d) 	 DEL (%3d)\r\n",g_cap_raw_deltas[11],g_cap_raw_deltas[10], g_cap_raw_deltas[9]);
			//TRACE_I("LEFT(%3d)\r\n",g_cap_raw_deltas[12]);
			//TRACE_I("DOWN(%3d)\r\n",g_cap_raw_deltas[13]);
		}	
	#endif
	/* clear sensor interrupt status */
		CAP_resetSensorStatus();
	
	/* report */
		if(LIST_BTN_NONE==g_cap_button)
		{
			/* 
				case 1:   no touch --> one button is detected
				case 2:   no touch --> multi button is detected
			*/
			DRV_BTN_ENUM btn=Drv_CapTouch_Get_Current_Button(stateMask);
			
			//TRACE_I("report: [PRESS]\r\n");
			
			if(btn!=LIST_BTN_NONE)
			{
				Drv_CapTouch_Key_Report(btn, true);
				g_cap_button = btn;
			}
		}
		else
		{		
			if(1==Num)
			{
				/*
					report the previous button release
				*/
				
				DRV_BTN_ENUM btn=Drv_CapTouch_Get_Current_Button(stateMask);

				if(btn!=g_cap_button)
				{
				/*
					A ---> A+B ---> B
				*/
					//TRACE_I("report: [prev RELEASE]----caused by A ---> A+B ---> B\r\n"); 
					
					if(g_cap_button!=LIST_BTN_NONE)
					{
						Drv_CapTouch_Key_Report(g_cap_button, false);
					}

					flag_Step2=true;
					Step2_btn=Drv_CapTouch_Get_Current_Button(stateMask);
				}
				else
				{
					/*
						A ---> A+B+... ---> A
					*/
				}
			}
			else
			{
				/*
					when one button is active , other buttons are pressed
					case 1: A---> A+B         ignore(for A is active)
					case 2: A---> A+B+C+....  ignore(for A is active)
					case 3: A---> B+C+....    report(fot A is released)
				*/
				CAP_CS_ENUM active_cs=BTN_TO_CS(g_cap_button);
				bool active=true;
				
				switch(active_cs)
				{
					case CAP_CS_1: 	 active=stateMask&CS1_BIT;	break;
					case CAP_CS_2: 	 active=stateMask&CS2_BIT;	break;
					case CAP_CS_3: 	 active=stateMask&CS3_BIT;	break;
					case CAP_CS_4: 	 active=stateMask&CS4_BIT;	break;
					case CAP_CS_5: 	 active=stateMask&CS5_BIT;	break;
					case CAP_CS_6: 	 active=stateMask&CS6_BIT;	break;
					case CAP_CS_7: 	 active=stateMask&CS7_BIT;	break;
					case CAP_CS_8: 	 active=stateMask&CS8_BIT;	break;
					case CAP_CS_9: 	 active=stateMask&CS9_BIT;	break;
					case CAP_CS_10:  active=stateMask&CS10_BIT; break;
					case CAP_CS_11:	 active=stateMask&CS11_BIT; break;
					case CAP_CS_12:	 active=stateMask&CS12_BIT; break;
					case CAP_CS_13:	 active=stateMask&CS13_BIT; break;
					case CAP_CS_14:	 active=stateMask&CS14_BIT; break;
					default:
						TRACE_W("active_cs(%d):goto default\r\n",active_cs);
						break;
				}

				if(false==active)
				{
					//TRACE_I("multi touch, but active button is released\r\n");
					//TRACE_I("report: [prev RELEASE]----caused by A ---> B+C --->B/C\r\n"); 
					
					if(g_cap_button!=LIST_BTN_NONE)
					{
						Drv_CapTouch_Key_Report(g_cap_button, false);
					}

					flag_Step2=true;
					Step2_btn=Drv_CapTouch_Get_Current_Button(stateMask);
				}
			}
		}
	}
}

static void Drv_CapTouch_Set_Callback(fpDrvBtnEvent func_ptr)
{   
	if(func_ptr!=NULL)
	{
		g_cap_button_cb = func_ptr;
		
		if(g_cap_button!=LIST_BTN_NONE)
		{
			//TRACE_I("REPORT BUTTON WHEN REGISTERED CALLBACK.\r\n");
			Drv_CapTouch_Key_Report(g_cap_button, true);
		}
	}
}

const DRV_CAPTOUCH_FUNCS_ST cap1214_funcs =
{
	.nInit             = Drv_CapTouch_Init,
	.nGetLedOnOff      = Drv_CapTouch_Get_Led,
	.nSetLedOnOff      = Drv_CapTouch_Set_Led,
	.nSupportPolling   = Drv_CapTouch_Polling_Supported,
	.nPolling          = Drv_CapTouch_Polling,
	.nSetCallback      = Drv_CapTouch_Set_Callback,
};

/*========================================================================================================
										End
========================================================================================================*/
#endif



/******************************************************************************
 Copyright (c) 2017 Flex(Shanghai) Co.,Ltd. 
 All rights reserved.

 [Module Name]:		
 		flex_drv_captouch_mpr121.c
 [Date]:       
 		2018-09-12
 [Author]:   	
 		Joanna.wang 
 [Reversion History]:
 		v1.0
*******************************************************************************/

/*========================================================================================================
										I N C L U D E
========================================================================================================*/
// external
#include "i2c.h"


// internal
#include "flex_drv_captouch_mpr121.h"


/*========================================================================================================
										D E B U G
========================================================================================================*/
#ifndef MOD_NAME
#define MOD_NAME								"[MPR121]--"
#endif

#if defined(__FLEX_TRACE_ENABLE__)
#define TRACE_F(fmt, arg...)				DBG_MSG(MOD_NAME "[F]--%s.\r\n", __func__)
#define TRACE_I(fmt, arg...)				DBG_MSG(MOD_NAME "[I]--" fmt, ##arg)
#define TRACE_W(fmt, arg...)				DBG_MSG(MOD_NAME "[W]--" fmt, ##arg)
#else
#define TRACE_F()										//do{}while(0)
#define TRACE_I(fmt, arg...)				//do{}while(0)	
#define TRACE_W(fmt, arg...)				//do{}while(0)	
#endif
#define TRACE_E(fmt, arg...)				DBG_MSG(MOD_NAME "[E]--" fmt, ##arg)





/*========================================================================================================
										E X T E R N 
========================================================================================================*/







/*========================================================================================================
										D E F I N E
========================================================================================================*/
#define TouchThre 11//15//30//10 Touch threshold
#define ReleaThre 7//8//25//8 Release threshold
#define Prox_TouchThre 6 // Proximity threshold
#define Prox_ReleaThre 4 // Proximity release threshold
#define MPR121_I2C_CHANNEL		hi2c1
#define MPR121_I2C_ADDR			0x5A<<1  // 8bit
#define MPR121_I2C_TIMEOUT 	50		// 0x10
#define MPRxxxx_ID  0x5A
#define FILTER G
#define ELETRODE_NUM 12
#define VERT_NUM 7
#define HORI_NUM 5

// MPR121 Register Defines
#define MHD_R	0x2B
#define NHD_R	0x2C
#define	NCL_R 	0x2D
#define	FDL_R	0x2E
#define	MHD_F	0x2F
#define	NHD_F	0x30
#define	NCL_F	0x31
#define	FDL_F	0x32
#define	ELE0_T	0x41
#define	ELE0_R	0x42
#define	ELE1_T	0x43
#define	ELE1_R	0x44
#define	ELE2_T	0x45
#define	ELE2_R	0x46
#define	ELE3_T	0x47
#define	ELE3_R	0x48
#define	ELE4_T	0x49
#define	ELE4_R	0x4A
#define	ELE5_T	0x4B
#define	ELE5_R	0x4C
#define	ELE6_T	0x4D
#define	ELE6_R	0x4E
#define	ELE7_T	0x4F
#define	ELE7_R	0x50
#define	ELE8_T	0x51
#define	ELE8_R	0x52
#define	ELE9_T	0x53
#define	ELE9_R	0x54
#define	ELE10_T	0x55
#define	ELE10_R	0x56
#define	ELE11_T	0x57
#define	ELE11_R	0x58
#define	FIL_CFG	0x5D
#define	ELE_CFG	0x5E
#define GPIO_CTRL0	0x73
#define	GPIO_CTRL1	0x74
#define GPIO_DATA	0x75
#define	GPIO_DIR	0x76
#define	GPIO_EN		0x77
#define	GPIO_SET	0x78
#define	GPIO_CLEAR	0x79
#define	GPIO_TOGGLE	0x7A
#define	ATO_CFG0	0x7B
#define	ATO_CFGU	0x7D
#define	ATO_CFGL	0x7E
#define	ATO_CFGT	0x7F
#define	SW_RST	0x80


//Status Register bit value
#define	ELETRODE_CHL0  0x01
#define	ELETRODE_CHL1  0x02
#define	ELETRODE_CHL2  0x04
#define	ELETRODE_CHL3  0x08
#define	ELETRODE_CHL4  0x10
#define	ELETRODE_CHL5  0x20
#define	ELETRODE_CHL6  0x40
#define	ELETRODE_CHL7	 0x80
#define	ELETRODE_CHL8  0x01
#define	ELETRODE_CHL9  0x02
#define	ELETRODE_CHL10 0x04
#define	ELETRODE_CHL11 0x08
#define	ELETRODE_CHL12 0x10	

/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
typedef struct
{
	uint8_t Reg0;
	uint8_t Reg1;
	uint8_t Touched;
}TouchStatus_st;

TouchStatus_st CurrTouchStatus = {.Reg0=0, .Reg1=0, .Touched=0};
TouchStatus_st PreTouchStatus = {.Reg0=0, .Reg1=0, .Touched=0};

typedef enum
{
	ELE_CHL_0,
	ELE_CHL_1,
	ELE_CHL_2,	
	ELE_CHL_3,
	ELE_CHL_4,
	ELE_CHL_5,
	ELE_CHL_6,
	ELE_CHL_7,
	ELE_CHL_8,
	ELE_CHL_9,
	ELE_CHL_10,
	ELE_CHL_11,
	ELE_CHL_12,
	ELE_CHL_MAX,	
}ELE_CHL;

/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/
uint8_t readingArray[43];
uint8_t ele_delta[14];
uint8_t SampSumX=0;
uint8_t SampSX=0;
uint8_t SampSumY=0;
uint8_t SampSY=0;




/*========================================================================================================
										F U N C T I O N
========================================================================================================*/
//========================================
// I2C 
//========================================
static HAL_StatusTypeDef MPR_I2c_Read_Bytes(uint8_t reg, uint8_t *data, uint8_t count)
{
	HAL_StatusTypeDef ret;
	ret = HAL_I2C_Mem_Read(&MPR121_I2C_CHANNEL, MPR121_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, count, MPR121_I2C_TIMEOUT);

	if(ret != HAL_OK)
	{
		TRACE_E("%s,read (0x%x):%d.\n", __func__, reg, ret);
	}	
	
	return ret;
}

static HAL_StatusTypeDef MPR_I2c_Write_Bytes(uint8_t reg, uint8_t data, uint8_t count)
{
	HAL_StatusTypeDef ret;
	
	ret = HAL_I2C_Mem_Write(&MPR121_I2C_CHANNEL, MPR121_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &data, count, MPR121_I2C_TIMEOUT);
	if(ret != HAL_OK)
	{
		TRACE_E("%s,write (0x%x):%d.\n",  __func__, reg, ret);
	}

	return ret;
}
//----------------------------------------------------
static HAL_StatusTypeDef MPR_I2c_Read(uint8_t reg, uint8_t *data)
{
	return MPR_I2c_Read_Bytes(reg, data,1);
}

static HAL_StatusTypeDef MPR_I2c_Write(uint8_t reg, uint8_t data)
{
	return MPR_I2c_Write_Bytes(reg,data,1);
}
bool MPR121_init(void)
{
	//Reset MPR121 if not reset correctly
	MPR_I2c_Write(SW_RST,0x63); //Soft reset
	MPR_I2c_Write(ELE_CFG,0x00); //Stop mode
	//touch pad baseline filter
	//rising
	MPR_I2c_Write(0x2B,0x01); //0xFF// MAX HALF DELTA Rising
	MPR_I2c_Write(0x2C,0x01); //0xFF// NOISE HALF DELTA Rising
	MPR_I2c_Write(0x2D,0x00); // //0 NOISE COUNT LIMIT Rising
	MPR_I2c_Write(0x2E,0x00); // DELAY LIMIT Rising
	//falling
	MPR_I2c_Write(0x2F,0x01); // MAX HALF DELTA Falling
	MPR_I2c_Write(0x30,0x01); // NOISE HALF DELTA Falling
	MPR_I2c_Write(0x31,0xFF); // NOISE COUNT LIMIT Falling
	MPR_I2c_Write(0x32,0x02); // //2//DELAY LIMIT Falling
	//touched
	MPR_I2c_Write(0x33,0x00); // Noise half delta touched
	MPR_I2c_Write(0x34,0x00); // Noise counts touched
	MPR_I2c_Write(0x35,0x00); //Filter delay touched
	//Touch pad threshold
	MPR_I2c_Write(0x41,TouchThre); // ELE0 TOUCH THRESHOLD
	MPR_I2c_Write(0x42,ReleaThre); // ELE0 RELEASE THRESHOLD
	MPR_I2c_Write(0x43,TouchThre); // ELE1 TOUCH THRESHOLD
	MPR_I2c_Write(0x44,ReleaThre); // ELE1 RELEASE THRESHOLD
	MPR_I2c_Write(0x45,TouchThre); // ELE2 TOUCH THRESHOLD
	MPR_I2c_Write(0x46,ReleaThre); // ELE2 RELEASE THRESHOLD
	MPR_I2c_Write(0x47,TouchThre); // ELE3 TOUCH THRESHOLD
	MPR_I2c_Write(0x48,ReleaThre); // ELE3 RELEASE THRESHOLD
	MPR_I2c_Write(0x49,TouchThre); // ELE4 TOUCH THRESHOLD
	MPR_I2c_Write(0x4A,ReleaThre); // ELE4 RELEASE THRESHOLD
	MPR_I2c_Write(0x4B,TouchThre); // ELE5 TOUCH THRESHOLD
	MPR_I2c_Write(0x4C,ReleaThre); // ELE5 RELEASE THRESHOLD
	MPR_I2c_Write(0x4D,TouchThre); // ELE6 TOUCH THRESHOLD
	MPR_I2c_Write(0x4E,ReleaThre); // ELE6 RELEASE THRESHOLD
	MPR_I2c_Write(0x4F,TouchThre); // ELE7 TOUCH THRESHOLD
	MPR_I2c_Write(0x50,ReleaThre); // ELE7 RELEASE THRESHOLD
	MPR_I2c_Write(0x51,TouchThre); // ELE8 TOUCH THRESHOLD
	MPR_I2c_Write(0x52,ReleaThre); // ELE8 RELEASE THRESHOLD
	MPR_I2c_Write(0x53,TouchThre); // ELE9 TOUCH THRESHOLD
	MPR_I2c_Write(0x54,ReleaThre); // ELE9 RELEASE THRESHOLD
	MPR_I2c_Write(0x55,TouchThre); // ELE10 TOUCH THRESHOLD
	MPR_I2c_Write(0x56,ReleaThre); // ELE10 RELEASE THRESHOLD
	MPR_I2c_Write(0x57,TouchThre); // ELE11 TOUCH THRESHOLD
	MPR_I2c_Write(0x58,ReleaThre); // ELE11 RELEASE THRESHOLD
	//AFE configuration
	MPR_I2c_Write(0x5D,0x00);
	MPR_I2c_Write(0x5C,0xC0);
//	//Auto configuration
	MPR_I2c_Write(0x7B,0xCB);
	MPR_I2c_Write(0x7D,0xE4);
	MPR_I2c_Write(0x7E,0x94);
	MPR_I2c_Write(0x7F,0xCD);
//	MPR_I2c_Write(0x5F,0x0b);
//	MPR_I2c_Write(0x60,0x82);
//	MPR_I2c_Write(0x61,0xb5);
	MPR_I2c_Write(ELE_CFG,0x0C);
	DBG_MSG("Mpr Init OK\n");
	//mpr_press_detect();
	return true;
}
uint8_t Analy_MPR121_vert_status_register(void)
{
	uint8_t ele_channel_vert;
//	for(uint8_t i =0; i<0x2; i++)
//	  DBG_MSG("readingArray[%d]:0x%02x\n",i,readingArray[i]);
	if(CurrTouchStatus.Reg0 & ELETRODE_CHL0)
		ele_channel_vert = ELE_CHL_0;
	if(CurrTouchStatus.Reg0 & ELETRODE_CHL1)
		ele_channel_vert = ELE_CHL_1;
	if(CurrTouchStatus.Reg0 & ELETRODE_CHL2)
		ele_channel_vert = ELE_CHL_2;
	if(CurrTouchStatus.Reg0 & ELETRODE_CHL3)
		ele_channel_vert = ELE_CHL_3;
	if(CurrTouchStatus.Reg0 & ELETRODE_CHL4)
		ele_channel_vert = ELE_CHL_4;
	if(CurrTouchStatus.Reg0 & ELETRODE_CHL5)
		ele_channel_vert = ELE_CHL_5;
	if(CurrTouchStatus.Reg0 & ELETRODE_CHL6)
		ele_channel_vert = ELE_CHL_6;
	if(CurrTouchStatus.Reg0 & ELETRODE_CHL7)
		ele_channel_vert = ELE_CHL_7;
  return ele_channel_vert;
}
uint8_t Analy_MPR121_hori_status_register(void)
{
	uint8_t ele_channel_hori;	
//	for(uint8_t i =0; i<0x2; i++)
//	  DBG_MSG("readingArray[%d]:0x%02x\n",i,readingArray[i]);
	if(CurrTouchStatus.Reg1 & ELETRODE_CHL8)
		ele_channel_hori = ELE_CHL_8;		
	if(CurrTouchStatus.Reg1 & ELETRODE_CHL9)
		ele_channel_hori = ELE_CHL_9;	
	if(CurrTouchStatus.Reg1 & ELETRODE_CHL10)
		ele_channel_hori = ELE_CHL_10;		
	if(CurrTouchStatus.Reg1 & ELETRODE_CHL11)
		ele_channel_hori = ELE_CHL_11;
  if(CurrTouchStatus.Reg1 & ELETRODE_CHL12)
		ele_channel_hori = ELE_CHL_12;
  return ele_channel_hori;	
}
void Read_MPR121_ele_register(void)
{
	// read the register before 0x2B
	MPR_I2c_Read_Bytes(0x00, (uint8_t *)(&readingArray), 0x2B);
	for(uint8_t i =0; i<0x2; i++)
	  DBG_MSG("readingArray[%d]:0x%02x\n",i,readingArray[i]);	
}
void Get_ele_data(void)
{
	uint8_t i;
	uint16_t tmp_sig,tmp_bas;
	for (i=0; i<ELETRODE_NUM+1; i++)
	{
		tmp_sig=(((uint16_t)readingArray[0x04+2*i])|(((uint16_t)readingArray[0x04+1+2*i])<<8))&0xFFFC;
		tmp_bas=((uint16_t)readingArray[0x1e + i])<<2;
		ele_delta[i]=abs((int16_t)(tmp_sig-tmp_bas));
		//DBG_MSG("ele_delta[%d]:0x%02x\n",i,ele_delta[i]);
	}
}
void Get_touch_status(void)
{
	uint16_t touchstatus;
	CurrTouchStatus.Reg0=readingArray[0x00];
	CurrTouchStatus.Reg1=readingArray[0x01];
	touchstatus = CurrTouchStatus.Reg1 << 8;
	touchstatus |= CurrTouchStatus.Reg0;
	for (uint8_t j=0; j<12; j++)  // Check how many electrodes were pressed
	{
		if ((touchstatus & (1<<j)))
				DBG_MSG("%x %d\n",touchstatus,j);
	}	
	if (((CurrTouchStatus.Reg0 & 0xff) != 0) || ((CurrTouchStatus.Reg1 & 0x1f)!= 0) )
	{
		CurrTouchStatus.Touched=1;
	}else
	{
		CurrTouchStatus.Touched=0;
	}
	DBG_MSG("%d\n",CurrTouchStatus.Touched);
}
void Intp5x7(void)
{
	uint8_t i;
	SampSumX=0;
	SampSX=0;
	SampSumY=0;
	SampSY=0;
	for(i=0;i<ELETRODE_NUM;i++)
	{
		if(i<VERT_NUM)
		{
			SampSumX+=(i+1)*ele_delta[i];
			SampSX+=ele_delta[i];
		}
		else
		{
			SampSumY+=(i-HORI_NUM-1)*ele_delta[i];
			SampSY+=ele_delta[i];
		}
	}
	//DBG_MSG("%d %d %d %d\n",SampSumX,SampSX,SampSumY,SampSY);
}
int32_t FilterXY(int32_t prev,int32_t spl,uint8_t m)
{
	//X=6/8*X'+2/8*X''
	if(m==1) 
		return prev-(prev>>2)+(spl>>2);
	// 4/8 4/8
	else if(m==2) 
		return (prev>>1)+(spl>>1);
	// 5/8 3/8
	else if(m==3) 
		return prev-(prev>>1)+(prev>>3)+(spl>>2)+(spl>>3);
	// 7/8 1/8
	else if(m==4) 
		return prev-(prev>>3)+(spl>>3);
	//return -1;
}
// divider
int32_t GetPosXY(int32_t fz,int32_t fm)
{
	uint8_t i;
	uint32_t w=0;
	uint16_t q=0,b=0;
	uint8_t s=0,g=0;
	if(fz==0||fm==0) return 0;
	for(i=0;i<5;i++)
	{
		if(fz<fm)
		{
			if(i==0) w=0;
			if(i==1) q=0;
			if(i==2) b=0;
			if(i==3) s=0;
			if(i==4) g=0;
			fz=(fz<<3)+(fz<<1);
			continue;
		}
		while(1)
		{
			fz-=fm;
			if(i==0) ++w;
			if(i==1) ++q;
			if(i==2) ++b;
			if(i==3) ++s;
			if(i==4) ++g;
			if(fz<fm)
			{
			fz=(fz<<3)+(fz<<1);
			break;
			}
		}
	}
	// y.yyyy*10000
	w=(w<<13)+(w<<10)+(w<<9)+(w<<8)+(w<<4);
	q=(q<<9)+(q<<8)+(q<<7)+(q<<6)+(q<<5)+(q<<3);
	b=(b<<6)+(b<<5)+(b<<2);
	s=(s<<3)+(s<<1);
	return w+q+b+s+g;
}


void Pol_mouse_dat(void)
{
	int32_t CurSumX,CurSX,CurSumY,CurSY,CurPosX,CurPosY,PrevSumX,PrevSX,PrevSumY,PrevSY,PrevPosX,PrevPosY,CurDX,CurDY;
  int32_t SamDX,SamDY,PrevDX,PrevDY,SndFlg,FstFlg,S_X,DX,S_Y,DY;	
	if(CurrTouchStatus.Touched==1) // pressed
	{
		// get CurSumX,CurSumY,SX,SY
		CurSumX = FilterXY(PrevSumX,SampSumX,1);
		CurSX = FilterXY(PrevSX,SampSX,1);
		CurSumY = FilterXY(PrevSumY,SampSumY,1);
		CurSY = FilterXY(PrevSY,SampSY,1);
		CurPosX = GetPosXY(CurSumX,CurSX);
		CurPosY = GetPosXY(CurSumY,CurSY);	
		// G Filter
#if FILTER==G
		CurPosX = FilterXY(PrevPosX,CurPosX,2);
		CurPosY = FilterXY(PrevPosY,CurPosY,2);
		CurDX = CurPosX-PrevPosX;
		CurDY = CurPosY-PrevPosY;
#endif
		// D Filter
#if FILTER==D
		SamDX = CurPosX-PrevPosX;
		SamDY = CurPosY-PrevPosY;
		CurDX = FilterXY(PrevDX,SamDX,1);
		CurDY = FilterXY(PrevDY,SamDY,1);
#endif
		// A Filter
#if FILTER==A
		CurPosX = FilterXY(PrevPosX,CurPosX,3);
		CurPosY = FilterXY(PrevPosY,CurPosY,3);
		SamDX = CurPosX-PrevPosX;
		SamDY = CurPosY-PrevPosY;
		CurDX = FilterXY(PrevDX,SamDX,3);
		CurDY = FilterXY(PrevDY,SamDY,3);
#endif
		//DBG_MSG("CurrTouchStatus.Touched:%d CurDX:%d CurDY:%d CurSumX:%d CurSumY:%d CurPosX:%d CurPosY:%d\n",CurrTouchStatus.Touched,CurDX,CurDY,CurSumX,CurSumY,CurPosX,CurPosY);
		if(PreTouchStatus.Touched==0) // fast track when finger just pressed
		{
#if FILTER==D||FILTER==A
			SndFlg=1;
#endif
			PrevSumX=SampSumX;
			PrevSX=SampSX;
			PrevSumY=SampSumY;
			PrevSY=SampSY;
			PrevPosX=GetPosXY(PrevSumX,PrevSX);
			PrevPosY=GetPosXY(PrevSumY,PrevSY);
			//DBG_MSG("fast track when finger just pressed\n");	
		}
		else // when finger pressed for some time
		{
			if(FstFlg++>=3) // ingored first three samples for avoid cursor's tremble
			{
				FstFlg=3;
				// for debug
				/*
				if((CurSumX!=PrevSumX)||(CurSX!=PrevSX)||(CurSumY!=PrevSumY)||(CurSY!=PrevSY))
				{
				SendChar((char)(CurSumX>>8));
				SendChar((char)(CurSumX&0x00ff));
				SendChar((char)(CurSX>>8));
				SendChar((char)(CurSX&0x00ff));
				SendChar((char)(CurSumY>>8));
				SendChar((char)(CurSumY&0x00ff));
				SendChar((char)(CurSY>>8));
				SendChar((char)(CurSY&0x00ff));
				SendChar('\r');
				SendChar('\n');
				} */
				if((CurPosX!=PrevPosX)||(CurPosY!=PrevPosY))
				{
					// for debug
					/*
					SendChar((char)(CurPosX>>24));
					SendChar((char)((CurPosX&0x00ff0000)>>16));
					SendChar((char)((CurPosX&0x0000ff00)>>8));
					SendChar((char)(CurPosX&0x000000ff));
					SendChar((char)(CurPosY>>24));
					SendChar((char)((CurPosY&0x00ff0000)>>16));
					SendChar((char)((CurPosY&0x0000ff00)>>8));
					SendChar((char)(CurPosY&0x000000ff));
					SendChar('\r');
					SendChar('\n'); */
					// delta counts < threshold?ingored
					if(((CurDX<S_X)&&(CurDX>=0))||((CurDX>-S_X)&&(CurDX<=0)))
					{
						// send 0
						DX=0;
						//DBG_MSG("delta counts < threshold?ingored:X\n");
					}
					else
					{
						// every 50 delta counts corresponds to 1 amplify factor
						// amplify factors could be adjusted by cursor's move performance
						if(CurDX>0)
						{
							if(CurDX>=S_X&&CurDX<S_X+50) DX=2; // 2
							else if(CurDX>=S_X+50&&CurDX<S_X+100) DX=2; // 2
							else if(CurDX>=S_X+100&&CurDX<S_X+150) DX=2; // 2
							else if(CurDX>=S_X+150&&CurDX<S_X+200) DX=2; // 2
							else if(CurDX>=S_X+200&&CurDX<S_X+250) DX=3; // 3
							else if(CurDX>=S_X+250&&CurDX<S_X+300) DX=3; // 3
							else if(CurDX>=S_X+300&&CurDX<S_X+350) DX=3; // 3
							else if(CurDX>=S_X+350&&CurDX<S_X+400) DX=3; // 3
							else if(CurDX>=S_X+400&&CurDX<S_X+450) DX=4; // 4
							else if(CurDX>=S_X+450&&CurDX<S_X+500) DX=4; // 4
							else if(CurDX>=S_X+500&&CurDX<S_X+550) DX=4; // 4
							else if(CurDX>=S_X+550&&CurDX<S_X+600) DX=4; // 5
							else if(CurDX>=S_X+600&&CurDX<S_X+650) DX=5; // 5
							else if(CurDX>=S_X+650&&CurDX<S_X+700) DX=5; // 5
							else if(CurDX>=S_X+700&&CurDX<S_X+750) DX=5; // 6
							else if(CurDX>=S_X+750&&CurDX<S_X+800) DX=5; // 6
							else if(CurDX>=S_X+800&&CurDX<S_X+850) DX=6; // 6
							else if(CurDX>=S_X+850&&CurDX<S_X+900) DX=6; // 7
							else if(CurDX>=S_X+900&&CurDX<S_X+950) DX=6; // 7
							else if(CurDX>=S_X+950&&CurDX<S_X+1000) DX=6; // 8
							else if(CurDX>=S_X+1000&&CurDX<S_X+1050) DX=7; // 8
							else if(CurDX>=S_X+1050&&CurDX<S_X+1100) DX=7; // 9
							else if(CurDX>=S_X+1100&&CurDX<S_X+1150) DX=7; // 10
							else if(CurDX>=S_X+1150&&CurDX<S_X+1200) DX=8; // 11
							else if(CurDX>=S_X+1200&&CurDX<S_X+1250) DX=8; // 12
							else if(CurDX>=S_X+1250&&CurDX<S_X+1300) DX=9; // 13
							else if(CurDX>=S_X+1300&&CurDX<S_X+1350) DX=9; // 14
							else if(CurDX>=S_X+1350&&CurDX<S_X+1400) DX=10; // 15
							else if(CurDX>=S_X+1400&&CurDX<S_X+1450) DX=10; // 16
							else if(CurDX>=S_X+1450&&CurDX<S_X+1500) DX=11; // 17
							else if(CurDX>=S_X+1500&&CurDX<S_X+1550) DX=11; // 18
							else if(CurDX>=S_X+1550&&CurDX<S_X+1600) DX=12; // 19
							else if(CurDX>=S_X+1600&&CurDX<S_X+1650) DX=13; // 20
							else if(CurDX>=S_X+1650&&CurDX<S_X+1700) DX=14; // 21
							else if(CurDX>=S_X+1700&&CurDX<S_X+1750) DX=15; // 22
							else DX=18; // 25
						}
						else
						{
							if(CurDX+S_X<=0&&CurDX+S_X+50>0) DX=-2; // -2 2
							else if(CurDX+S_X+50<=0&&CurDX+S_X+100>0) DX=-2; // -2 2
							else if(CurDX+S_X+100<=0&&CurDX+S_X+150>0) DX=-2; // -2 2
							else if(CurDX+S_X+150<=0&&CurDX+S_X+200>0) DX=-2; // -3 3
							else if(CurDX+S_X+200<=0&&CurDX+S_X+250>0) DX=-3; // -3 3
							else if(CurDX+S_X+250<=0&&CurDX+S_X+300>0) DX=-3; // -3 3
							else if(CurDX+S_X+300<=0&&CurDX+S_X+350>0) DX=-3; // -3 3
							else if(CurDX+S_X+350<=0&&CurDX+S_X+400>0) DX=-3; // -3 4
							else if(CurDX+S_X+400<=0&&CurDX+S_X+450>0) DX=-4; // -4 4
							else if(CurDX+S_X+450<=0&&CurDX+S_X+500>0) DX=-4; // -4 4
							else if(CurDX+S_X+500<=0&&CurDX+S_X+550>0) DX=-4; // -4 4
							else if(CurDX+S_X+550<=0&&CurDX+S_X+600>0) DX=-4; // -5
							else if(CurDX+S_X+600<=0&&CurDX+S_X+650>0) DX=-5; // -5
							else if(CurDX+S_X+650<=0&&CurDX+S_X+700>0) DX=-5; // -5
							else if(CurDX+S_X+700<=0&&CurDX+S_X+750>0) DX=-5; // -6
							else if(CurDX+S_X+750<=0&&CurDX+S_X+800>0) DX=-5; // -6
							else if(CurDX+S_X+800<=0&&CurDX+S_X+850>0) DX=-6; // -6
							else if(CurDX+S_X+850<=0&&CurDX+S_X+900>0) DX=-6; // -7
							else if(CurDX+S_X+900<=0&&CurDX+S_X+950>0) DX=-6; // -7
							else if(CurDX+S_X+950<=0&&CurDX+S_X+1000>0) DX=-6; // -8
							else if(CurDX+S_X+1000<=0&&CurDX+S_X+1050>0) DX=-7; // -8
							else if(CurDX+S_X+1050<=0&&CurDX+S_X+1100>0) DX=-7; // -9
							else if(CurDX+S_X+1100<=0&&CurDX+S_X+1150>0) DX=-7; // -10
							else if(CurDX+S_X+1150<=0&&CurDX+S_X+1200>0) DX=-8; // -11
							else if(CurDX+S_X+1200<=0&&CurDX+S_X+1250>0) DX=-8; // -12
							else if(CurDX+S_X+1250<=0&&CurDX+S_X+1300>0) DX=-9; // -13
							else if(CurDX+S_X+1300<=0&&CurDX+S_X+1350>0) DX=-9; // -14
							else if(CurDX+S_X+1350<=0&&CurDX+S_X+1400>0) DX=-10; // -15
							else if(CurDX+S_X+1400<=0&&CurDX+S_X+1450>0) DX=-10; // -16
							else if(CurDX+S_X+1450<=0&&CurDX+S_X+1500>0) DX=-11; // -17
							else if(CurDX+S_X+1500<=0&&CurDX+S_X+1550>0) DX=-11; // -18
							else if(CurDX+S_X+1550<=0&&CurDX+S_X+1600>0) DX=-12; // -19
							else if(CurDX+S_X+1600<=0&&CurDX+S_X+1650>0) DX=-13; // -20
							else if(CurDX+S_X+1650<=0&&CurDX+S_X+1700>0) DX=-14; // -21
							else if(CurDX+S_X+1700<=0&&CurDX+S_X+1750>0) DX=-15; // -22
							else DX=-18; // -25
						}
						//DBG_MSG("every 50 delta counts corresponds to 1 amplify factor:X\n");
					}
					if(((CurDY<S_Y)&&(CurDY>=0))||((CurDY>-S_Y)&&(CurDY<=0)))
					{
						// send 0
						DY=0;
						//DBG_MSG("delta counts < threshold?ingored:Y\n");
					}
					else
					{
						if(CurDY>0)
						{
							if(CurDY>=S_Y&&CurDY<S_Y+50) DY=2; // 2
							else if(CurDY>=S_Y+50&&CurDY<S_Y+100) DY=2; // 2
							else if(CurDY>=S_Y+100&&CurDY<S_Y+150) DY=2; // 2
							else if(CurDY>=S_Y+150&&CurDY<S_Y+200) DY=2; // 2
							else if(CurDY>=S_Y+200&&CurDY<S_Y+250) DY=3; // 3
							else if(CurDY>=S_Y+250&&CurDY<S_Y+300) DY=3; // 3
							else if(CurDY>=S_Y+300&&CurDY<S_Y+350) DY=3; // 3
							else if(CurDY>=S_Y+350&&CurDY<S_Y+400) DY=3; // 3
							else if(CurDY>=S_Y+400&&CurDY<S_Y+450) DY=4; // 4
							else if(CurDY>=S_Y+450&&CurDY<S_Y+500) DY=4; // 4
							else if(CurDY>=S_Y+500&&CurDY<S_Y+550) DY=4; // 4
							else if(CurDY>=S_Y+550&&CurDY<S_Y+600) DY=4; // 5
							else if(CurDY>=S_Y+600&&CurDY<S_Y+650) DY=5; // 5
							else if(CurDY>=S_Y+650&&CurDY<S_Y+700) DY=5; // 5
							else if(CurDY>=S_Y+700&&CurDY<S_Y+750) DY=5; // 6
							else if(CurDY>=S_Y+750&&CurDY<S_Y+800) DY=5; // 6
							else if(CurDY>=S_Y+800&&CurDY<S_Y+850) DY=6; // 6
							else if(CurDY>=S_Y+850&&CurDY<S_Y+900) DY=6; // 7
							else if(CurDY>=S_Y+900&&CurDY<S_Y+950) DY=6; // 7
							else if(CurDY>=S_Y+950&&CurDY<S_Y+1000) DY=6; // 8
							else if(CurDY>=S_Y+1000&&CurDY<S_Y+1050) DY=7; // 8
							else if(CurDY>=S_Y+1050&&CurDY<S_Y+1100) DY=7; // 9
							else if(CurDY>=S_Y+1100&&CurDY<S_Y+1150) DY=7; // 10
							else if(CurDY>=S_Y+1150&&CurDY<S_Y+1200) DY=8; // 11
							else if(CurDY>=S_Y+1200&&CurDY<S_Y+1250) DY=8; // 12
							else if(CurDY>=S_Y+1250&&CurDY<S_Y+1300) DY=9; // 13
							else if(CurDY>=S_Y+1300&&CurDY<S_Y+1350) DY=9; // 14
							else if(CurDY>=S_Y+1350&&CurDY<S_Y+1400) DY=10; // 15
							else if(CurDY>=S_Y+1400&&CurDY<S_Y+1450) DY=10; // 16
							else if(CurDY>=S_Y+1450&&CurDY<S_Y+1500) DY=11; // 17
							else if(CurDY>=S_Y+1500&&CurDY<S_Y+1550) DY=11; // 18
							else if(CurDY>=S_Y+1550&&CurDY<S_Y+1600) DY=12; // 19
							else if(CurDY>=S_Y+1600&&CurDY<S_Y+1650) DY=13; // 20
							else if(CurDY>=S_Y+1650&&CurDY<S_Y+1700) DY=14; // 21
							else if(CurDY>=S_Y+1700&&CurDY<S_Y+1750) DY=15; // 22
							else DY=18; // 25
						}
						else
						{
							if(CurDY+S_Y<=0&&CurDY+S_Y+50>0) DY=-2; // -2
							else if(CurDY+S_Y+50<=0&&CurDY+S_Y+100>0) DY=-2; // -2
							else if(CurDY+S_Y+100<=0&&CurDY+S_Y+150>0) DY=-2; // -2
							else if(CurDY+S_Y+150<=0&&CurDY+S_Y+200>0) DY=-2; // -2
							else if(CurDY+S_Y+200<=0&&CurDY+S_Y+250>0) DY=-3; // -3
							else if(CurDY+S_Y+250<=0&&CurDY+S_Y+300>0) DY=-3; // -3
							else if(CurDY+S_Y+300<=0&&CurDY+S_Y+350>0) DY=-3; // -3
							else if(CurDY+S_Y+350<=0&&CurDY+S_Y+400>0) DY=-3; // -3
							else if(CurDY+S_Y+400<=0&&CurDY+S_Y+450>0) DY=-4; // -4
							else if(CurDY+S_Y+450<=0&&CurDY+S_Y+500>0) DY=-4; // -4
							else if(CurDY+S_Y+500<=0&&CurDY+S_Y+550>0) DY=-4; // -4
							else if(CurDY+S_Y+550<=0&&CurDY+S_Y+600>0) DY=-4; // -5
							else if(CurDY+S_Y+600<=0&&CurDY+S_Y+650>0) DY=-5; // -5
							else if(CurDY+S_Y+650<=0&&CurDY+S_Y+700>0) DY=-5; // -5
							else if(CurDY+S_Y+700<=0&&CurDY+S_Y+750>0) DY=-5; // -6
							else if(CurDY+S_Y+750<=0&&CurDY+S_Y+800>0) DY=-5; // -6
							else if(CurDY+S_Y+800<=0&&CurDY+S_Y+850>0) DY=-6; // -6
							else if(CurDY+S_Y+850<=0&&CurDY+S_Y+900>0) DY=-6; // -7
							else if(CurDY+S_Y+900<=0&&CurDY+S_Y+950>0) DY=-6; // -7
							else if(CurDY+S_Y+950<=0&&CurDY+S_Y+1000>0) DY=-6; // -8
							else if(CurDY+S_Y+1000<=0&&CurDY+S_Y+1050>0) DY=-7; // -8
							else if(CurDY+S_Y+1050<=0&&CurDY+S_Y+1100>0) DY=-7; // -9
							else if(CurDY+S_Y+1100<=0&&CurDY+S_Y+1150>0) DY=-7; // -10
							else if(CurDY+S_Y+1150<=0&&CurDY+S_Y+1200>0) DY=-8; // -11
							else if(CurDY+S_Y+1200<=0&&CurDY+S_Y+1250>0) DY=-8; // -12
							else if(CurDY+S_Y+1250<=0&&CurDY+S_Y+1300>0) DY=-9; // -13
							else if(CurDY+S_Y+1300<=0&&CurDY+S_Y+1350>0) DY=-9; // -14
							else if(CurDY+S_Y+1350<=0&&CurDY+S_Y+1400>0) DY=-10; // -15
							else if(CurDY+S_Y+1400<=0&&CurDY+S_Y+1450>0) DY=-10; // -16
							else if(CurDY+S_Y+1450<=0&&CurDY+S_Y+1500>0) DY=-11; // -17
							else if(CurDY+S_Y+1500<=0&&CurDY+S_Y+1550>0) DY=-11; // -18
							else if(CurDY+S_Y+1550<=0&&CurDY+S_Y+1600>0) DY=-12; // -19
							else if(CurDY+S_Y+1600<=0&&CurDY+S_Y+1650>0) DY=-13; // -20
							else if(CurDY+S_Y+1650<=0&&CurDY+S_Y+1700>0) DY=-14; // -21
							else if(CurDY+S_Y+1700<=0&&CurDY+S_Y+1750>0) DY=-15; // -22
							else DY=-18; // -25
						}
						//DBG_MSG("every 50 delta counts corresponds to 1 amplify factor:Y\n");
					}
					//DBG_MSG("(CurPosX!=PrevPosX)||(CurPosY!=PrevPosY)\n");
				}
				else
				{
					// send 0
					DX=0;
					DY=0;
				}
				//DBG_MSG("The forth samples for avoid cursor's tremble.DX:%d  DY:%d\n",DX,DY);
			}
			else
			{
				//send 0
				DX=0;
				DY=0;
				//DBG_MSG("ingored first three samples for avoid cursor's tremble.DX:%d  DY:%d\n",DX,DY);
			}
			PrevSumX=CurSumX;
			PrevSX=CurSX;
			PrevSumY=CurSumY;
			PrevSY=CurSY;
			PrevPosX=CurPosX;
			PrevPosY=CurPosY;
#if FILTER==D||FILTER==A
			if(SndFlg==1)
			{
				SndFlg=0;
				PrevDX=SamDX;
				PrevDY=SamDY;
			}
			else
			{
				PrevDX=CurDX;
				PrevDY=CurDY;
			}
#endif
			//DBG_MSG("when finger pressed for some time.DX:%d  DY:%d\n",DX,DY);
		}
		PreTouchStatus.Touched=1;
		DBG_MSG("Press£ºDX:%d  DY:%d\n",DX,DY);
	}
	else // unpressed
	{
		FstFlg=0;
		PreTouchStatus.Touched=0;
		DX=0;
		DY=0;
	  DBG_MSG("Release£ºDX:%d  DY:%d\n",DX,DY);
	}
}


void mpr_press_detect(void)
{
	Read_MPR121_ele_register(); // read reg 0x00 - 0x2b value to readingArray[]
	Get_ele_data(); // get signal, baseline, delta
	Get_touch_status(); // read touch status reg 0x00, 0x01, determin touched or not
//	Intp5x7(); // interpolation algorithm
//  Pol_mouse_dat();
}



const DRV_CAPTOUCH_FUNCS_ST mpr121_funcs =
{
	.nInit             = MPR121_init,
	.nGetLedOnOff      = NULL,
	.nSetLedOnOff      = NULL,
	.nSupportPolling   = NULL,
	.nPolling          = NULL,
	.nSetCallback      = NULL,
};




/*========================================================================================================
										End
========================================================================================================*/



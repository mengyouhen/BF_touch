/*************************************************** 
  This is a library for our Adafruit 24-channel PWM/LED driver

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/1429

  These drivers uses SPI to communicate, 3 pins are required to  
  interface: Data, Clock and Latch. The boards are chainable

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/


#include "flex_drv_tlc5947.h"

#define NUM_TLC5974 2

#define data   4
#define clock   5
#define latch   6
#define oe  -1  // set to -1 to not use the enable pin (its optional)

uint16_t *pwmbuffer;
uint16_t numdrivers;
uint8_t _clk, _dat, _lat;

void Adafruit_TLC5947(uint16_t n, uint8_t c, uint8_t d, uint8_t l) 
{
  numdrivers = n;
  _clk = c;
  _dat = d;
  _lat = l;

  //pwmbuffer = (uint16_t *)calloc(2, 24*n);
  pwmbuffer = (uint16_t *)pf_malloc(2 * 24*n);
  memset(pwmbuffer, 0, 2*24*n);
}

void write(void) {
  //digitalWrite(_lat, LOW);
	HAL_GPIO_WritePin(TLC_LAT_GPIO_Port, TLC_LAT_Pin, GPIO_PIN_RESET);
  // 24 channels per TLC5974
  for (int16_t c=24*numdrivers - 1; c >= 0 ; c--) 
	{
    // 12 bits per channel, send MSB first
    for (int8_t b=11; b>=0; b--) 
		{
      //digitalWrite(_clk, LOW);
      HAL_GPIO_WritePin(TLC_SCLK_GPIO_Port, TLC_SCLK_Pin, GPIO_PIN_RESET);
			//DBG_MSG("write:%d\n",pwmbuffer[c] & (1 << b));
      if (pwmbuffer[c] & (1 << b))  
        HAL_GPIO_WritePin(TLC_SIN_GPIO_Port, TLC_SIN_Pin, GPIO_PIN_SET);//digitalWrite(_dat, HIGH);
      else
        HAL_GPIO_WritePin(TLC_SIN_GPIO_Port, TLC_SIN_Pin, GPIO_PIN_RESET);//digitalWrite(_dat, LOW);

      //digitalWrite(_clk, HIGH);
			HAL_GPIO_WritePin(TLC_SCLK_GPIO_Port, TLC_SCLK_Pin, GPIO_PIN_SET);
    }
  }
  //digitalWrite(_clk, LOW);
  HAL_GPIO_WritePin(TLC_SCLK_GPIO_Port, TLC_SCLK_Pin, GPIO_PIN_RESET);
  //digitalWrite(_lat, HIGH);  
  //digitalWrite(_lat, LOW);
	HAL_GPIO_WritePin(TLC_LAT_GPIO_Port, TLC_LAT_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(TLC_LAT_GPIO_Port, TLC_LAT_Pin, GPIO_PIN_RESET);
}



void setPWM(uint16_t chan, uint16_t pwm) {
  if (pwm > 4095) pwm = 4095;
  if (chan > 24*numdrivers) return;
  pwmbuffer[chan] = pwm;  
}


void setLED(uint16_t lednum, uint16_t r, uint16_t g, uint16_t b) {
  setPWM(lednum*3, r);
  setPWM(lednum*3+1, g);
  setPWM(lednum*3+2, b);
}


bool begin() {	
  if (!pwmbuffer) return false;
	HAL_GPIO_WritePin(TLC_BLANK_GPIO_Port, TLC_BLANK_Pin, GPIO_PIN_RESET);

  return true;
}

void setup() 
{
	Adafruit_TLC5947(NUM_TLC5974, clock, data, latch);
  begin();
  if (oe >= 0) {
    //pinMode(oe, OUTPUT);
    //digitalWrite(oe, LOW);
  }
}

void loop() 
{
	setup();
	DBG_MSG("loop\n");
  colorWipe(4095, 0, 0, 200); // "Red" (depending on your LED wiring)
  HAL_Delay(2000);
  colorWipe(0, 4095, 0, 200); // "Green" (depending on your LED wiring)
  HAL_Delay(2000);
  colorWipe(0, 0, 4095, 200); // "Blue" (depending on your LED wiring)
  HAL_Delay(2000);
  rainbowCycle(100);
}


// Fill the dots one after the other with a color
void colorWipe(uint16_t r, uint16_t g, uint16_t b, uint8_t wait) 
{
  for(uint16_t i=0; i<8*NUM_TLC5974; i++) {
      setLED(i, r, g, b);
      write();
      HAL_Delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) 
{
  uint32_t i, j;

  for(j=0; j<4096; j++) { // 1 cycle of all colors on wheel
    for(i=0; i< 8*NUM_TLC5974; i++) {
      Wheel(i, ((i * 4096 / (8*NUM_TLC5974)) + j) & 4095);
    }
    write();
    HAL_Delay(wait);
  }
}

// Input a value 0 to 4095 to get a color value.
// The colours are a transition r - g - b - back to r.
void Wheel(uint8_t ledn, uint16_t WheelPos) 
{
  if(WheelPos < 1365) {
    setLED(ledn, 3*WheelPos, 4095 - 3*WheelPos, 0);
  } else if(WheelPos < 2731) {
    WheelPos -= 1365;
    setLED(ledn, 4095 - 3*WheelPos, 0, 3*WheelPos);
  } else {
    WheelPos -= 2731;
    setLED(ledn, 0, 3*WheelPos, 4095 - 3*WheelPos);
  }
}

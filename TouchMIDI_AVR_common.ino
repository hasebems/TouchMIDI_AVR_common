/* ========================================
 *
 *  TouchMIDI Common Platform for AVR
 *  TouchMIDI_AVR_common.ino
 *    description: TouchMidi Main Functions
 *
 *  Copyright(c)2018- Masahiko Hasebe at Kigakudoh
 *  This software is released under the MIT License, see LICENSE.txt
 *
 * ========================================
 */
#include  <MsTimer2.h>
#include  <Adafruit_NeoPixel.h>
#include  "configuration.h"
#include  "TouchMIDI_AVR_if.h"

#include  "i2cdevice.h"
#include  "magicflute.h"
#include  "honeycombbell.h"

#ifdef __AVR__
  #include <avr/power.h>
#endif

/*----------------------------------------------------------------------------*/
//
//     Global Variables
//
/*----------------------------------------------------------------------------*/
#define PIN 2
Adafruit_NeoPixel led = Adafruit_NeoPixel(MAX_LED, PIN, NEO_GRB + NEO_KHZ800);

//  Touch Sensor
uint16_t swState[2];
GlobalTimer gt;

/*----------------------------------------------------------------------------*/
#ifdef USE_SIX_TOUCH_SENS
static MagicFlute st;
#endif

/*----------------------------------------------------------------------------*/
#ifdef USE_TWELVE_TOUCH_SENS
static HoneycombBell hcb;
#endif

/*----------------------------------------------------------------------------*/
void flash()
{
  gt.incGlobalTime();
}
/*----------------------------------------------------------------------------*/
void setup()
{
  int err;

	//  Initialize Variables
  for ( int j=0; j<2; j++ ){
	  swState[j] = 0;
  }

  //  Initialize Hardware
  pinMode(6, OUTPUT);   // LED
  pinMode(7, OUTPUT);   // LED
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  wireBegin();
  Serial.begin(31250);

#ifdef USE_ADA88
 #ifdef USE_PCA9544A
  pca9544_changeI2cBus(0);
 #endif
  ada88_init();
  ada88_write(1);
#endif

  //  Initialize MBR
#ifdef USE_CY8CMBR3110
 #ifdef USE_PCA9544A
  //  Two MBR3110
  pca9544_changeI2cBus(1);
  err = MBR3110_init(0);
  digitalWrite(7, HIGH);
  if ( err ){ while(1){ setAda88_Number(err);}}

  pca9544_changeI2cBus(2);
  err = MBR3110_init(1);
  digitalWrite(7, LOW);
  if ( err ){ while(1){ setAda88_Number(err+1000);}}
 #else
  //  One MBR3110
  err = MBR3110_init();
  if ( err ){ while(1){ digitalWrite(7, HIGH); setAda88_Number(err);}}
 #endif
#else
  //  No CY8CMBR3110
#endif

  setAda88_Number(888);

  //  Set NeoPixel Library 
  led.begin();
  led.show(); // Initialize all pixels to 'off'

  //  Set Interrupt
  MsTimer2::set(10, flash);     // 10ms Interval Timer Interrupt
  MsTimer2::start();
}
/*----------------------------------------------------------------------------*/
void loop()
{
  //  Global Timer 
  generateTimer();

  //  Touch Sensor
#ifdef USE_SIX_TOUCH_SENS
  st.checkSixTouch();
#endif

#ifdef USE_TWELVE_TOUCH_SENS
  hcb.checkTwelveTouch(0);
  hcb.checkTwelveTouch(1);
#endif

	delay(2);
}
/*----------------------------------------------------------------------------*/
//
//     Arduino API
//
/*----------------------------------------------------------------------------*/
int analogDataRead( void )
{
  return analogRead(0);
}
/*----------------------------------------------------------------------------*/
//
//     Global Timer
//
/*----------------------------------------------------------------------------*/
void generateTimer( void )
{
  long diff = gt.globalTime() - gt.gtOld();
  gt.setGtOld(gt.globalTime());
  if ( diff < 0 ){ diff += 0x10000; }

  gt.clearAllTimerEvent();
  gt.updateTimer(diff);

  if ( gt._timer100msec_event == true ){
#ifdef USE_SIX_TOUCH_SENS    
    st.periodic100msec();
#endif
#ifdef USE_TWELVE_TOUCH_SENS
    hcb.periodic100msec();   
#endif
  }
}
/*----------------------------------------------------------------------------*/
//
//     MIDI Command & UI
//
/*----------------------------------------------------------------------------*/
void setMidiBuffer( uint8_t dt0, uint8_t dt1, uint8_t dt2 )
{
  Serial.write(dt0);
  Serial.write(dt1);
  Serial.write(dt2);
}
/*----------------------------------------------------------------------------*/
//
//     Hardware Access Functions
//
/*----------------------------------------------------------------------------*/
void setAda88_Number( int number )
{
#ifdef USE_PCA9544A
  pca9544_changeI2cBus(0);  
#endif
#ifdef USE_ADA88
  ada88_writeNumber(number);  // -1999 - 1999
#endif
}
/*----------------------------------------------------------------------------*/
//
//     Blink LED by NeoPixel Library
//
/*----------------------------------------------------------------------------*/
const uint8_t colorTable[16][3] = {
  { 200,   0,   0 },//  C
  { 175,  30,   0 },
  { 155,  50,   0 },//  D
  { 135,  70,   0 },
  { 110,  90,   0 },//  E
  {   0, 160,   0 },//  F
  {   0, 100, 100 },
  {   0,   0, 250 },//  G
  {  30,   0, 230 },
  {  60,   0, 190 },//  A
  { 100,   0, 140 },
  { 140,   0,  80 },//  B

  { 100, 100, 100 },
  { 100, 100, 100 },
  { 100, 100, 100 },
  { 100, 100, 100 }
 };
/*----------------------------------------------------------------------------*/
uint8_t colorTbl( uint8_t doremi, uint8_t rgb ){ return colorTable[doremi][rgb];}
void setLed( int ledNum, uint8_t red, uint8_t green, uint8_t blue )
{
  led.setPixelColor(ledNum,led.Color(red, green, blue));
}
void lightLed( void )
{
  led.show();
}

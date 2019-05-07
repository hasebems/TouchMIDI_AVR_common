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

#define RED_LED   6
#define GREEN_LED 7

//  Touch Sensor
uint16_t swState[2];
GlobalTimer gt;

/*----------------------------------------------------------------------------*/
#ifdef USE_SIX_TOUCH_SENS
static MagicFlute mf;
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
  pinMode(RED_LED, OUTPUT);   // LED
  pinMode(GREEN_LED, OUTPUT);   // LED
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  wireBegin();
  Serial.begin(31250);

  pinMode(5,OUTPUT);    //
  digitalWrite(5,HIGH);  //  Mute

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
  digitalWrite(RED_LED, HIGH);
  if ( err ){ while(1){ setAda88_Number(err);}}

  pca9544_changeI2cBus(2);
  err = MBR3110_init(1);
  digitalWrite(RED_LED, LOW);
  if ( err ){ while(1){ setAda88_Number(err+1000);}}
 #else
  //  One MBR3110
  err = MBR3110_init();
  if ( err ){ while(1){ digitalWrite(RED_LED, HIGH); setAda88_Number(err);}}
 #endif
#else
  //  No CY8CMBR3110
#endif

  //  Set NeoPixel Library 
  led.begin();
  led.show(); // Initialize all pixels to 'off'

  digitalWrite(5,LOW);  //  Mute Off

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
  int prs = mf.checkSixTouch_AndAirPressure();
  setAda88_Number(prs);
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

  if ( gt.timer100msecEvent() == true ){
#ifdef USE_SIX_TOUCH_SENS    
    mf.periodic100msec();
#endif
#ifdef USE_TWELVE_TOUCH_SENS
    hcb.periodic100msec();   
#endif
  }

  if ( gt.timer100msecEvent() == true ){
    // blink LED
    (gt.timer100ms() & 0x0002)? digitalWrite(GREEN_LED, HIGH):digitalWrite(GREEN_LED, LOW);
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

/* ========================================
 *
 *	configuration.h
 *		description: TouchMidi Configuration
 *
 *	Copyright(c)2017- Masahiko Hasebe at Kigakudoh
 *  This software is released under the MIT License, see LICENSE.txt.
 *
 * ========================================
*/
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

//---------------------------------------------------------
//    cofiguration type
//---------------------------------------------------------
#define TOUCHMIDI_EXP       0
#define TOUCHMIDI_OCARINA   1
#define TOUCHMIDI_HONEYCOMB 2

#define TOUCHMIDI_INST      TOUCHMIDI_HONEYCOMB


#if ( TOUCHMIDI_INST == TOUCHMIDI_OCARINA )
//---------------------------------------------------------
//    Ocarina Setting
//---------------------------------------------------------
#define   USE_AIR_PRESSURE
#define   USE_SIX_TOUCH_SENS
#define   MAX_LED       8

//---------------------------------------------------------
//		I2C Device Configuration
//---------------------------------------------------------
#define		USE_CY8CMBR3110
#define		USE_ADA88
//#define		USE_LPS22HB
//#define		USE_LPS25H
//#define		USE_AQM1602XA
//#define		USE_ADXL345
//#define		USE_PCA9685
//#define		USE_ATTINY
//#define		USE_PCA9544A
#endif


#if ( TOUCHMIDI_INST == TOUCHMIDI_HONEYCOMB )
//---------------------------------------------------------
//    HoneycombBell Setting
//---------------------------------------------------------
#define   USE_TWELVE_TOUCH_SENS
#define   MAX_LED       12

//---------------------------------------------------------
//    I2C Device Configuration
//---------------------------------------------------------
#define   USE_CY8CMBR3110
#define   USE_ADA88
//#define   USE_LPS22HB
//#define   USE_LPS25H
//#define   USE_AQM1602XA
//#define   USE_ADXL345
//#define   USE_PCA9685
//#define   USE_ATTINY
#define   USE_PCA9544A
#endif



#endif

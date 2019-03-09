/* ========================================
 *
 *  TouchMIDI Common Platform for AVR
 *  honeycombbell.h
 *    description: HoneycombBell 
 *
 *  Copyright(c)2019- Masahiko Hasebe at Kigakudoh
 *  This software is released under the MIT License, see LICENSE.txt
 *
 * ========================================
 */
#ifndef HONEYCOMBBELL_H
#define HONEYCOMBBELL_H

#include <Arduino.h>

class HoneycombBell {

public:
  HoneycombBell( void ) : honeycombKeyboard(0) {}
  
  void periodic100msec( void );
  void checkTwelveTouch( int device );
  void setNeoPixelTwelve( uint8_t locate, uint8_t sw );

private:
  uint16_t   honeycombKeyboard;

};
#endif

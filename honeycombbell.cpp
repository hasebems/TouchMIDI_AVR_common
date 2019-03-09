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
#include "honeycombbell.h"
#include "TouchMIDI_AVR_if.h"
#include "configuration.h"
#include "i2cdevice.h"

/*----------------------------------------------------------------------------*/
extern uint16_t swState[2];
extern GlobalTimer gt;

/*----------------------------------------------------------------------------*/
void HoneycombBell::periodic100msec( void )
{
  uint8_t block = (gt.timer100ms()%4)*3;
  setNeoPixelTwelve( block, 2 );
  setNeoPixelTwelve( block+1, 2 );
  setNeoPixelTwelve( block+2, 2 );
  if ( block == 0 ){ block = 9;}
  else { block-=3; }
  setNeoPixelTwelve( block, 0 );
  setNeoPixelTwelve( block+1, 0 );
  setNeoPixelTwelve( block+2, 0 ); 
}
/*----------------------------------------------------------------------------*/
void HoneycombBell::checkTwelveTouch( int device )
{
  uint8_t swb[2];
  int err = 1;
  int blk = (device!=0)?0:1;  //  inverse of device

#ifdef USE_PCA9544A
  pca9544_changeI2cBus(device+1);
#endif

#ifdef USE_CY8CMBR3110
  err = MBR3110_readTouchSw(swb,device);
#endif

  if ( err == 0 ){
    uint16_t sw = ((uint16_t)swb[0]) | ((uint16_t)swb[1]<<8);
    for ( int i=0; i<6; i++ ){
      uint16_t  bitPtn = 0x0001 << i;
      if ( (swState[blk]&bitPtn)^(sw&bitPtn) ){
        if ( sw & bitPtn ){
          setMidiBuffer( 0x90, (blk*6)+i, 0x7f );
          honeycombKeyboard |= 0x0001<<((blk*6)+i);
          setNeoPixelTwelve((blk*6)+i,1);
        }
        else {
          setMidiBuffer( 0x90, (blk*6)+i, 0 );
          honeycombKeyboard &= ~(0x0001<<((blk*6)+i));
          setNeoPixelTwelve((blk*6)+i,0);
        }
      }
    }
    swState[blk] = sw;
    setAda88_Number((swState[0]<<6) | (swState[1]&0x3f));
  }
}
/*----------------------------------------------------------------------------*/
void HoneycombBell::setNeoPixelTwelve( uint8_t locate, uint8_t sw )
{
  if ( (0x0001<<locate) & honeycombKeyboard ){
    if ( sw == 1 ){
      setLed(locate,colorTbl(locate%16,0),colorTbl(locate%16,1),colorTbl(locate%16,2));
    }
  }
  else {
    if ( sw == 0 ){
      setLed(locate,0,0,0);
    }
    else if ( sw == 2 ){
      setLed(locate,100,100,100);
    }
  }
  lightLed();
}

/* ========================================
 *
 *  TouchMIDI Common Platform for AVR
 *  air_pressure.cpp
 *    description: Air Pressure Functions
 *
 *  Copyright(c)2019- Masahiko Hasebe at Kigakudoh
 *  This software is released under the MIT License, see LICENSE.txt
 *
 * ========================================
 */
#include "air_pressure.h"
#include "configuration.h"
#include "i2cdevice.h"
#include "TouchMIDI_AVR_if.h"

/*----------------------------------------------------------------------------*/
//
//     Get Air Presure
//
/*----------------------------------------------------------------------------*/
int AirPressure::getPressure( void )
{
  //  Pressure Sensor Moving Avarage
  for ( int i=0; i<MOVING_AV_MAX-1; i++ ){
    _movingAv[i] = _movingAv[i+1];
  }
  _movingAv[MOVING_AV_MAX-1] = analogDataRead();
  int total = 0;
  for ( int i=0; i<MOVING_AV_MAX; i++ ){
    total += _movingAv[i];
  }
  _lastPressure = total/MOVING_AV_MAX;
  return _lastPressure;
}
/*----------------------------------------------------------------------------*/
//
//     Generate MIDI Event
//
/*----------------------------------------------------------------------------*/
bool AirPressure::generateExpEvent( uint8_t* midiValue )
{
    bool    ret = false;

  if ( _afterStartCounter < START_DEAD_BAND_TIME ){
    //  3sec. of the begining
    *midiValue = 0;
    _afterStartCounter++;
    _currentStandard = _lastPressure;
    return false;
  }

  int currentPrs = _lastPressure;

  //  Analyse & Generate Standard Pressure
  analyseStandardPressure(currentPrs);

  //  Generate MIDI Value
  int diff = currentPrs - _currentStandard;
  if ( diff < ZERO_OFFSET ){ diff = 0;}
  else if ( diff > 127 + ZERO_OFFSET ){ diff = 127;}
  else { diff -= ZERO_OFFSET;}

  uint8_t md = (uint8_t)diff;
  uint8_t expr = _lastMidiValue;

  if ( md != _lastMidiValue ){
    //  interpolate MIDI value
    expr = interpolateMidiExp(md);
    _lastMidiValue = expr;
    ret = true;
  }

set_ret:
  //  output
  *midiValue = expr;
  return ret;
}
//-------------------------------------------------------------------------
void AirPressure::analyseStandardPressure( int crntPrs )
{
  //  far from current standard (more than 1hPa)
  if (( _currentStandard+10 < crntPrs ) ||
    ( _currentStandard-10 > crntPrs )){
        _samePressureCounter = 0;   
    return;
  }

  //  Analize Standard Pressure
  if (( _newProposedPressure+NOISE_WIDTH < crntPrs ) ||
      ( _newProposedPressure-NOISE_WIDTH > crntPrs )){
    _samePressureCounter = 0;
    _newProposedPressure = crntPrs;
  }
  else {
    _samePressureCounter++;
    if ( _samePressureCounter > STABLE_COUNT ){
        _samePressureCounter = 0;
      if ( _currentStandard != _newProposedPressure ){
        //if (_dbg) _dbg->printf("<<Change Standard Pressure=%d>>\n",_newProposedPressure);
        _currentStandard = _newProposedPressure;
      }
    }
  }
}
//-------------------------------------------------------------------------
uint8_t AirPressure::interpolateMidiExp( uint8_t realExp )
{
  if ( realExp > _lastMidiValue ){
    if ( _lastMidiValue < 127 - MIDI_EXP_ITP_STEP ){
      if ( realExp > _lastMidiValue + MIDI_EXP_ITP_STEP ){
        realExp = _lastMidiValue + MIDI_EXP_ITP_STEP;
      }
    }
  }
  else {  // realExp < _lastMidiValue
    if ( _lastMidiValue > MIDI_EXP_ITP_STEP ){
      if ( realExp < _lastMidiValue - MIDI_EXP_ITP_STEP ){
        realExp = _lastMidiValue - MIDI_EXP_ITP_STEP;
      }
    }
  }
  //  never come when realExp == _lastMidiValue

  _lastMidiValue = realExp;
  return realExp;
}


/* -*- mode: c++; tab-width: 8 -*- */
/*
  Copyright (C) 2018 by Roger E Critchlow Jr, Charlestown, MA, USA.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/
#ifndef touch_pads_h
#define touch_pads_h

#include "Teensy3Touch.h"
#include "debouncer.h"

// this might be improved if it made an instance
// with npads and pins as constructor parameters
// and let everything be instance variables of
// the correct size

#ifndef NPADS
#define NPADS 16
#endif

namespace TouchPads {
  static int _npads;
  static uint8_t _pads[NPADS];
  static uint8_t _channels[NPADS];

  static uint16_t _touch[NPADS];
  static uint16_t _avgTouch[NPADS];
  static uint8_t _electrodes[NPADS];
  static uint16_t _minTouch[NPADS];
  static uint16_t _maxTouch[NPADS];
  static uint8_t _normTouch[NPADS];
  static uint8_t _threshold[NPADS];
  static uint8_t _expo;
  static uint16_t _last_touch;

  static uint8_t _callbackFlag;
  static uint32_t _callbackCount;

  static debouncer _debouncer[NPADS];
  
  static void reset() {
    for (int i = 0; i < _npads; i += 1) {
      _avgTouch[i] = 0;
      _maxTouch[i] = 0;
      _minTouch[i] = 65535;
    }
  }

  // compute exponential average
  // return (avg + val) / 2
  // return (avg + 2*avg + val) / 4;				// 3/4 + 1/4
  // return (avg + 2*avg + 4*avg + val) / 8;			// 7/8 + 1/8
  // return (avg + 2*avg + 4*avg + 8*avg + val) / 16;		// 15/16 + 1/16
  // return (avg + 2*avg + 4*avg + 8*avg + 16*avg + val) / 32;	// 31/32 + 1/32
  static uint16_t average(uint16_t avg, uint16_t val) {
    uint16_t acc = avg;
    switch (_expo) {
    case 0: return val;
    case 8: acc <<= 1; acc += avg; // fall through
    case 7: acc <<= 1; acc += avg; // fall through
    case 6: acc <<= 1; acc += avg; // fall through
    case 5: acc <<= 1; acc += avg; // fall through
    case 4: acc <<= 1; acc += avg; // fall through
    case 3: acc <<= 1; acc += avg; // fall through
    case 2: acc <<= 1; acc += avg; // fall through
    case 1: break;
    }
    acc += val;
    return acc >> _expo;
  }

  static void _callback(uint16_t *value) {
    _callbackFlag += 1;
    _callbackCount += 1;
    if (_callbackCount == 256) reset();
    for (int i = 0; i < _npads; i += 1) {
      uint16_t val = value[_channels[i]];
      if (val == 0 || val == 65535) {
	_electrodes[i] |= 1; 
	val = _minTouch[i];
      }
      _touch[i] = val;		// maybe average here, too, a little?
      if (_avgTouch[i] == 0) _avgTouch[i] = val;
      _avgTouch[i] = average(_avgTouch[i], val);
      _maxTouch[i] = max(_maxTouch[i], _avgTouch[i]);
      _minTouch[i] = min(_minTouch[i], _avgTouch[i]);
    }
  }

  // set the off/on threshold for normalized touch values
  static void set_threshold(uint8_t threshold) {
    for (int i = 0; i < _npads; i += 1) _threshold[i] = threshold;
  }
  static void set_threshold(uint8_t threshold, int i) {
    _threshold[i] = threshold;
  }
  static void set_steps(uint8_t steps) {
    for (int i = 0; i < _npads; i += 1) _debouncer[i].setSteps(steps);
  }
  static void set_steps(uint8_t steps, int i) {
    _debouncer[i].setSteps(steps);
  }
  static void set_average(uint8_t expo) {
    _expo = expo;
  }
  // see if a new touch configuration is available
  static bool available() {
    if ( ! _callbackFlag)
      return false;
    uint16_t new_touch = 0;
    for (int i = 0; i < _npads; i += 1) {
      uint16_t value = _touch[i];
      uint16_t range = _maxTouch[i]-_minTouch[i];
      uint16_t scale = 0xff / range;
      uint16_t excess = value-_minTouch[i];
      _normTouch[i] = range < 5 ? 0 : excess >= range ? 255 : scale * excess;
      if (_debouncer[i].debounce(_normTouch[i] > _threshold[i] ? 1 : 0)) new_touch |= 1<<i;
    }
    if (new_touch != _last_touch) {
      _last_touch = new_touch;
      return true;
    }
    return false;
  }

  static uint32_t clock() { return _callbackCount; }
  static uint16_t last_touch() { return _last_touch; }
  static uint16_t touch(int i) { return _touch[i]; }
  static uint16_t avgTouch(int i) { return _avgTouch[i]; }
  static uint16_t minTouch(int i) { return _minTouch[i]; }
  static uint16_t maxTouch(int i) { return _maxTouch[i]; }
  static uint16_t rangeTouch(int i) { return _maxTouch[i]-_minTouch[i]; }
  static uint16_t exTouch(int i) { return _touch[i]-_minTouch[i]; }
  static uint8_t normTouch(int i) { return _normTouch[i]; }

  static void begin(int npads, uint8_t *pads) {
    // if (npads > NPADS) abort();
    _npads = npads;
    uint16_t maskpins = 0;
    for (int i = 0; i < _npads; i += 1) {
      _pads[i] = pads[i];
      _channels[i] = Teensy3Touch::pinChannel(_pads[i]);
      maskpins |= (1<<_channels[i]);
      set_steps(DEBOUNCER_STEPS, i);
      set_threshold(TOUCH_THRESHOLD, i);
      set_average(SOFTWARE_AVERAGING);
    }
    reset();
    Teensy3Touch::start(maskpins,3,2,HARDWARE_AVERAGING,2,_callback);// 1 scan
  }

};

// allocate static data
//int TouchPads::_npads;
//uint8_t TouchPads::_pads[NPADS];
//uint8_t TouchPads::_channels[NPADS];

//uint16_t TouchPads::_touch[NPADS];
//uint16_t TouchPads::_avgTouch[NPADS];
//uint8_t TouchPads::_electrodes[NPADS];
//uint16_t TouchPads::_minTouch[NPADS];
//uint16_t TouchPads::_maxTouch[NPADS];
//uint8_t TouchPads::_normTouch[NPADS];
//uint8_t TouchPads::_threshold[NPADS];
//uint16_t TouchPads::_last_touch;
//uint8_t TouchPads::_callbackFlag;
//uint32_t TouchPads::_callbackCount;

//debouncer TouchPads::_debouncer[NPADS];

#endif // touch_pads_h

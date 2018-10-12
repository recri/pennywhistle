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
#ifndef AudioOut_h
#define AudioOut_h

/*
** The amp breakout is either of
**  https://www.adafruit.com/product/3006
**  or https://www.sparkfun.com/products/14809
** They have the same pinout with different labels
**
** The amp breakout is connected:
**  Teensy I2S LRCLK  (23) to breakout LRC or LRCLK
**  Teensy I2S BCLK   (9)  to breakout BCLK or BCLK
**  Teensy I2S TXDATA (13) to breakout DIN or DIN
**  Teensy            (31) to breakout SD or /SD
**  Teensy GND             to breakout GND or GND
**  Teensy Vin             to breakout Vin or 2.5-5.5V
**  breakout out + to Tip and Ring of TRS jack
**  breakout out - to Shield of TRS jack
**  breakout GAIN or GAIN  to 100k resistor back to Vin or 2.5-5.5V
**    setting 3dB gain
**
** the /SD and GAIN controls are controlled by logic
**  /SD pin 31 output HIGH -> amplifier enabled
**  /SD pin 31 output LOW -> amplifier shut down
**
*/
namespace AudioOut {
  static const uint8_t PIN_SD = 31;
  static const uint8_t PIN_SD_ENABLE = HIGH;
  static const uint8_t PIN_SD_SHUTDOWN = LOW;
  static int enabled = 1;
  
  void set_enabled(int onoff) { digitalWrite(PIN_SD, enabled = onoff != 0 ? 1 : 0); }
  int is_enabled() { return enabled; }
  int begin() { 
    pinMode(PIN_SD, OUTPUT); set_enabled(enabled);
    return 1;
  }

};

#endif

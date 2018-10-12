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
#ifndef AudioIn_h
#define AudioIn_h

/*
** The microphone breakout is 
** https://www.tindie.com/products/onehorse/ics43434-i2s-digital-microphone/
** It is connected:
**
**  Teensy I2S RXDATA (13) to breakout SD
**  Teensy I2S LRCLK  (23) to breakout WS
**  Teensy I2S BCLK   (9)  to breakout SCK
**  3.3V                   to breakout 3V3
**  GND                    to breakout GND
**  GND                    to breakout L/R
**
** Nothing else is required, 
** other than soft wiring the I2S input to the Teensy Audio library.
** and enabling the mixer that combines the left and right channels.
*/
namespace AudioIn {
  int begin() { return 1; }
};

#endif

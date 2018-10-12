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
/*
** Scan touch inputs and maintain touch state.
*/
#include "WProgram.h"
#include "Teensy3Touch.h"

uint32_t Teensy3Touch::_clock;
uint16_t Teensy3Touch::_value[16];
uint8_t Teensy3Touch::_active[16];
uint8_t Teensy3Touch::_nactive;
uint8_t Teensy3Touch::_pactive;
uint8_t Teensy3Touch::_cactive;
uint8_t Teensy3Touch::_scanning;
void (*Teensy3Touch::_callback)(uint16_t *);

#if defined(HAS_KINETIS_TSI) || defined(HAS_KINETIS_TSI_LITE)

#if defined(__MK20DX128__) || defined(__MK20DX256__)
// Teensy 3.0, 3.1, and 3.2
const uint8_t Teensy3Touch::_pin2tsi[34] = {
//0    1    2    3    4    5    6    7    8    9
  9,  10, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
255, 255,  14,  15, 255,  12, 255, 255, 255, 255,
255, 255,  11,   5
};

#elif defined(__MK66FX1M0__)
// Teensy 3.6
const uint8_t Teensy3Touch::_pin2tsi[40] = {
//0    1    2    3    4    5    6    7    8    9
  9,  10, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
255, 255,  14,  15, 255, 255, 255, 255, 255,  11,
 12, 255, 255, 255, 255, 255, 255, 255, 255, 255
};

#elif defined(__MKL26Z64__)
// Teensy LC
const uint8_t Teensy3Touch::_pin2tsi[27] = {
//0 1 2 3 4 5 6 7 8 9
  9,  10, 255,   2,   3, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255,  13,   0,   6,   8,   7,
255, 255,  14,  15, 255, 255, 255
};

#endif

// called at end of scan, ie scan of all enabled electrodes
extern "C" {

void tsi0_isr() { Teensy3Touch::touchISR(); }

}
#endif

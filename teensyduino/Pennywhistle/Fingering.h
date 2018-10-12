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
** Fingering translates a vector of normalized touches
** into a MIDI note.
**
** In the simplest case there are six holes on the front
** which implement the penny whistle fingering.
**
** A pair of thumbholes on the rear can be used to shift
** registers or modulate to different keys or scales.
** 
** We can also have a seventh hole on the front to implement
** recorder fingering.
**
** We pass the vector of normalized touch readings, an array
** of uint8_t, with 0 representing the minimum touch and 255
** the maximum touch.  We translate the touch vector into a
**
** Then all eight keys down is written as 0b11111111 in binary
** or as 0xFF in hexadecimal or as 255 in decimal notation
**
** I assume that the six keys will be fingered by the
** first three fingers of each hand.
**
** Translating fingering into notes assumes that the
** pennywhistle has been assigned a scale and a key
** and an octave to play in.  The scale determines the
** relationship between adjacent notes, the key determines
** which note the relationships begin from, and the octave
** determines how low or high the lowest note is.
**
** Tuning up entails, probably, scrolling through the possible
** scales, keys, and octaves to select the desired one.
**
** 0) Use the penny whistle fingering
** 0x3f 0b111111 -> do,
** 0x3e 0b111110 -> re,
** 0x3d 0b111100 -> mi,
** 0x3c 0b111000 -> fa,
** 0x3b 0b110000 -> sol,
** 0x3a 0b100000 -> la,
** 0x39 0b000000 -> ti,
** use the thumbholes to select the register.
**
** 1) Take the six keys, assign them to the bits in
** a byte starting from the least significant bit at
** the most distal key, let the four lowest keys
** control the scale as in:
** 0x3f 0b111111 -> do,
** 0x3e 0b111110 -> re,
** 0x3d 0b111101 -> mi,
** 0x3c 0b111100 -> fa,
** 0x3b 0b111011 -> sol,
** 0x3a 0b111010 -> la,
** 0x39 0b111001 -> ti,
** 0x38 0b111000 -> do',
** 0x37 0b110111 -> do',
** and so on repeating as for the lower octave.
** Then let the last two keys make accidental flats and sharps.
**
** 2) Move the bulk of the scale fingering to the index and middle
** fingers, so they do the four bit fingering that controls the main
** scale:
** 0x3f 0b111111 -> do,
** 0x3d 0b111101 -> re,
** 0x3b 0b111011 -> mi,
** 0x39 0b111001 -> fa,
** 0x2f 0b101111 -> sol,
** 0x2d 0b101101 -> la,
** 0x2b 0b101011 -> ti,
** 0x29 0b101001 -> do',
** 0x17 0b011111 -> do',
** and so on repeating as for the lower octave.
**
** 3) Use a Gray reflected binary code
** where only one bit changes between
** adjacent codes:
**      0b111111 -> do,
**      0b111101 -> re,
**      0b111001 -> mi,
**      0b111011 -> fa,
**      0b101011 -> sol,
**      0b101001 -> la,
**      0b101011 -> ti,
**      0b101111 -> do',
**      0b001111 -> re',
**      0b001101 -> mi',
**      0b001001 -> fa',
**      0b001011 -> sol',
**      0b011011 -> la',
**      0b011001 -> ti',
**      0b011101 -> do'',
**      0b011111 -> re'',
**
** 4) Thinking of using the thumbholes to indicate 
**     0b11?????? root_note,
**     0b10?????? root_note up a fifth,
**     0b01?????? root_note down a fifth,
**     0b00?????? and mute=0
*/
#ifndef Fingering_h
#define Fingering_h

#include "Config.h"
#include "Midi.h"

class Fingering {
 protected:
  Fingering() {}				// no instance

  static uint8_t scale[7];
  static uint8_t root_note;
  static uint8_t scale_type;

  static uint8_t last_note;

 public:
  static void begin() { begin(ROOTNOTE, SCALETYPE); }

  static void begin(uint8_t rootnote, uint8_t scaletype) {
    set_scale(rootnote, scaletype);
  }

  static void set_scale(uint8_t root, uint8_t type) {
	root_note = root;
	scale_type = type;
	Midi::scale(root, type, scale);
  }
  static uint8_t get_root_note() { return root_note; }
  static uint8_t get_scale_type() { return scale_type; }
  static uint8_t translate(uint16_t finger_up) { 
    if (USEBINARY)
      if (USEGRAYCODE)
	if (USESTRONGFINGERS)
	  return  last_note = translate_strong_finger_bits_are_gray_code(finger_up);
	else 
	  return  last_note = translate_low_bits_are_gray_code(finger_up);
      else
	if (USESTRONGFINGERS)
	  return  last_note = translate_strong_finger_bits_are_note(finger_up);
	else
	  return  last_note = translate_low_bits_are_note(finger_up);
    else
      return last_note = translate_natural(finger_up);
  }
  static uint8_t lastNote() { return last_note; }

protected:
  static uint8_t translate_low_bits_are_gray_code(uint8_t finger_up) {
	uint8_t note;
	uint8_t octave;
	bool sharp = (finger_up & 16) == 0;
	bool flat = (finger_up & 32) == 0;
	uint8_t midi_note;
	switch (finger_up & 0xF) {
	case 0x0: note = 0; break;
	case 0x1: note = 1; break;
	case 0x3: note = 2; break;
	case 0x2: note = 3; break;
	case 0x6: note = 4; break;
	case 0x7: note = 5; break;
	case 0x5: note = 6; break;
	case 0x4: note = 7; break;
	case 0xc: note = 8; break;
	case 0xd: note = 9; break;
	case 0xf: note = 10; break;
	case 0xe: note = 11; break;
	case 0xa: note = 12; break;
	case 0xb: note = 13; break;
	case 0x9: note = 14; break;
	case 0x8: note = 15; break;
	}
	for (octave = 0; note >= 7; octave += 12)
	  note -= 7;
	midi_note = scale[note]+octave;
	if (flat) midi_note -= 1;
	if (sharp) midi_note += 1;
	return midi_note;
  }

  // low bits are 0x4, 0x2, and 0x1, 0x8 indicates octave up,
  // 0x10 flats and 0x20 sharps
  static uint8_t translate_low_bits_are_note(uint8_t finger_up) {
	uint8_t note = finger_up & 7;
	uint8_t octave = (finger_up & 8) ? 12 : 0;
	bool sharp = (finger_up & 16) == 0;
	bool flat = (finger_up & 32) == 0;
	uint8_t midi_note;
	if (note == 7)
	  midi_note = scale[0]+octave+12;
	else
	  midi_note = scale[note]+octave;
	if (flat) midi_note -= 1;
	if (sharp) midi_note += 1;
	return midi_note;
  }

  // the strong fingers are 0x20, 0x10, 0x4, and 0x2
  // reassemble into the low bits format
  static uint8_t translate_strong_finger_bits_are_note(uint8_t finger_up) {
	return translate_low_bits_are_note(((finger_up&0x30)>>2)|((finger_up & 0x6)>>1)|((finger_up&0x1)<<4)|((finger_up&0x8)<<2));
  }

  static uint8_t translate_strong_finger_bits_are_gray_code(uint8_t finger_up) {
	return translate_low_bits_are_gray_code(((finger_up&0x30)>>2)|((finger_up & 0x6)>>1)|((finger_up&0x1)<<4)|((finger_up&0x8)<<2));
  }
  
  static uint8_t translate_natural(uint8_t finger_up) {
#if NPADS == 8 || NPADS == 6
    uint8_t note = finger_up & 0b00111111;
#elif NPADS == 7 || NPADS == 9    
    uint8_t note = finger_up & 0b001111111;
#else
#error "unimplemented NPADS"
#endif
#if NPADS == 8
    uint8_t octave = (finger_up & 0b11000000)>>6;
#elif NPADS == 9
    uint8_t octave = (finger_up & 0b110000000)>>7;
#else
    uint8_t octave = 3;
#endif
    switch (octave) {
    case 3: octave = 0; break;
    case 2: octave = +12; break;
    case 1: octave = -12; break;
    case 0: return 255; break;
    }
#if NPADS == 8 || NPADS == 6 || NPADS == 5
    // implement first open hole sets note
    // should check for half hole to flat
    switch (note) {
    case 0b111111:
      return scale[0]+octave;
    case 0b111110:
      return scale[1]+octave;
    case 0b111100: case 0b111101:
      return scale[2]+octave;
    case 0b111000: case 0b111001: case 0b111010: case 0b111011: 
      return scale[3]+octave;
    case 0b110000: case 0b110001: case 0b110010: case 0b110011: case 0b110100: case 0b110101: case 0b110110: case 0b110111:
      return scale[4]+octave;
    case 0b100000: case 0b100001: case 0b100010: case 0b100011: case 0b100100: case 0b100101: case 0b100110: case 0b100111:
    case 0b101000: case 0b101001: case 0b101010: case 0b101011: case 0b101100: case 0b101101: case 0b101110: case 0b101111:
      return scale[5]+octave;
    case 0b000000: case 0b000001: case 0b000010: case 0b000011: case 0b000100: case 0b000101: case 0b000110: case 0b000111:
    case 0b001000: case 0b001001: case 0b001010: case 0b001011: case 0b001100: case 0b001101: case 0b001110: case 0b001111:
    case 0b010000: case 0b010001: case 0b010010: case 0b010011: case 0b010100: case 0b010101: case 0b010110: case 0b010111:
    case 0b011000: case 0b011001: case 0b011010: case 0b011011: 
      return scale[6]+octave;
    case 0b011100: case 0b011101: case 0b011110: case 0b011111:
      return scale[0]+12+octave;
    default: 
      return 0xFF;
    }
#elif NPADS == 7 || NPADS == 9    
    switch (note) {
    case 0b1111111:
      return scale[0]+octave;
    case 0b1111110:
      return scale[1]+octave;
    case 0b1111100: case 0b1111101:
      return scale[2]+octave;
    case 0b1111000: case 0b1111001: case 0b1111010: case 0b1111011: 
      return scale[3]+octave;
    case 0b1110000: case 0b1110001: case 0b1110010: case 0b1110011: case 0b1110100: case 0b1110101: case 0b1110110: case 0b1110111:
      return scale[4]+octave;
    case 0b1100000: case 0b1100001: case 0b1100010: case 0b1100011: case 0b1100100: case 0b1100101: case 0b1100110: case 0b1100111:
    case 0b1101000: case 0b1101001: case 0b1101010: case 0b1101011: case 0b1101100: case 0b1101101: case 0b1101110: case 0b1101111:
      return scale[5]+octave;
    case 0b1000000: case 0b1000001: case 0b1000010: case 0b1000011: case 0b1000100: case 0b1000101: case 0b1000110: case 0b1000111:
    case 0b1001000: case 0b1001001: case 0b1001010: case 0b1001011: case 0b1001100: case 0b1001101: case 0b1001110: case 0b1001111:
    case 0b1010000: case 0b1010001: case 0b1010010: case 0b1010011: case 0b1010100: case 0b1010101: case 0b1010110: case 0b1010111:
    case 0b1011000: case 0b1011001: case 0b1011010: case 0b1011011: case 0b1011100: case 0b1011101: case 0b1011110: case 0b1011111: 
      return scale[6]+octave;
    case 0b0100000: case 0b0100001: case 0b0100010: case 0b0100011: case 0b0100100: case 0b0100101: case 0b0100110: case 0b0100111:
    case 0b0101000: case 0b0101001: case 0b0101010: case 0b0101011: case 0b0101100: case 0b0101101: case 0b0101110: case 0b0101111:
    case 0b0110000: case 0b0110001: case 0b0110010: case 0b0110011: case 0b0110100: case 0b0110101: case 0b0110110: case 0b0110111:
    case 0b0111000: case 0b0111001: case 0b0111010: case 0b0111011: case 0b0111100: case 0b0111101: case 0b0111110: case 0b0111111: 
      return scale[0]+12+octave;
    default:
      return 0xFF;
    }
#endif
  }
};

uint8_t Fingering::scale[7];
uint8_t Fingering::root_note;
uint8_t Fingering::scale_type;
uint8_t Fingering::last_note;
#endif	// Fingering_h

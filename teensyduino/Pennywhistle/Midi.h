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
#ifndef Midi_h
#define Midi_h

#include "Config.h"
#include <Audio.h>
#include <math.h>

class Midi
{
 protected:
  Midi() {}				/* no instance */
 public:
  /* Notes */
  static const uint8_t lowest = 0;
  static const uint8_t lowest_piano = 21;
  static const uint8_t lowest_bass_clef = 43;
  static const uint8_t highest_bass_clef = 57;
  static const uint8_t middle_C = 60;
  static const uint8_t lowest_treble_clef = 64;
  static const uint8_t A_440 = 69;
  static const uint8_t highest_treble_clef = 77;
  static const uint8_t highest_piano = 108;
  static const uint8_t highest = 127;

  static const uint8_t C = 60;
  static const uint8_t C_sharp = 61;
  static const uint8_t D_flat = 61;
  static const uint8_t D = 62;
  static const uint8_t D_sharp = 63;
  static const uint8_t E_flat = 63;
  static const uint8_t E = 64;
  static const uint8_t F = 65;
  static const uint8_t F_sharp = 66;
  static const uint8_t G_flat = 66;
  static const uint8_t G = 67;
  static const uint8_t G_sharp = 68;
  static const uint8_t A_flat = 68;
  static const uint8_t A = 69;
  static const uint8_t A_sharp = 70;
  static const uint8_t B_flat = 70;
  static const uint8_t B = 71;
  
  static const uint8_t MajorScale = 0;
  static const uint8_t NaturalMinor = 1;			/* flat 3rd, flat 6th, flat 7th */
  static const uint8_t HarmonicMinor = 2;			/* flat 3rd, flat 6th */
  static const uint8_t AscendingMelodicMinor = 3;		/* flat 3rd */
  static const uint8_t PhrygianDominant = 4;			/* flat 2nd, flat 3rd, flat 6th */
  static const uint8_t DoubleHarmonic = 5;			/* flat 2nd, flat 6th */
  static const uint8_t LydianMode = 6;				/* aug 4th */
  static const uint8_t MixolydianMode = 7;			/* flat 7th */
  static const uint8_t DorianMode = 8;				/* flat 3rd, flat 7th */
  static const uint8_t PhrygianMode = 9;			/* flat 2nd, flat 3rd, flat 6th, flat 7th */
  static const uint8_t LocrianMode = 10;			/* flat 2nd, flat 3rd, flat 5th, flat 6th, flat 7th */

  static const uint8_t DescendingMelodicMinor = 1;
  static const uint8_t MinorScale = 1;
  static const uint8_t IonianMode = 0;
  static const uint8_t AeolianMode = 1;

  static const uint8_t NoteOff = 0x80;
  static const uint8_t NoteOn = 0x90;
  static const uint8_t AfterTouch = 0xA0;
  static const uint8_t ControlChange = 0xB0;
  static const uint8_t ProgramChange = 0xC0;
  static const uint8_t ChannelPressure = 0xD0;
  static const uint8_t PitchWheel = 0xE0;

  /* control changes */
  static const uint8_t CC_DataEntry_MSB = 0x06;
  static const uint8_t CC_DataEntry_LSB = 0x26;

  static const uint8_t CC_DataIncr = 0x60;
  static const uint8_t CC_DataDecr = 0x61;
  static const uint8_t CC_NRPN_LSB = 0x62;
  static const uint8_t CC_NRPN_MSB = 0x63;
  static const uint8_t CC_RPN_LSB = 0x64;
  static const uint8_t CC_RPN_MSB = 0x65;

  static const uint8_t CC_AllSoundOff = 0x78;
  static const uint8_t CC_ResetAllControllers = 0x79;
  static const uint8_t CC_LocalControl = 0x7A; /* 0 off 127 on */

  static const char *note_name(uint8_t note, bool as_flat = false) {
    switch (note % 12) {
    case 0: return "C";
    case 1: return as_flat ? "D♭" : "C♯";
    case 2: return "D";
    case 3: return as_flat ? "E♭" : "D♯";
    case 4: return "E";
    case 5: return "F";
    case 6: return as_flat ? "G♭" : "F♯";
    case 7: return "G";
    case 8: return as_flat ? "A♭" : "G♯";
    case 9: return "A";
    case 10: return as_flat ? "B♭" : "A♯";
    case 11: return "B";
    default: return "?";
    }
  }

  static int8_t note_octave(uint8_t note) { return (note/12)-1; }

  static void scale(const uint8_t root, const uint8_t type, uint8_t *dest) {
    switch (type) {
    default: /* fall through */
    case MajorScale: transpose(root, c_major, dest); return;
    case NaturalMinor: transpose(root, c_natural_minor, dest); return;
    case HarmonicMinor: transpose(root, c_harmonic_minor, dest); return;
    case AscendingMelodicMinor: transpose(root, c_ascending_melodic_minor, dest); return;
    case PhrygianDominant: transpose(root, c_phrygian_dominant, dest); return;
    case DoubleHarmonic: transpose(root, c_double_harmonic, dest); return;
    case LydianMode: transpose(root, c_lydian_mode, dest); return;
    case MixolydianMode: transpose(root, c_mixolydian_mode, dest); return;
    case DorianMode: transpose(root, c_dorian_mode, dest); return;
    case PhrygianMode: transpose(root, c_phrygian_mode, dest); return;
    case LocrianMode: transpose(root, c_locrian_mode, dest); return;
    }
  }

 protected:
  /* Scales */
  static void transpose(const uint8_t new_root, const uint8_t old_root, const uint8_t *src, uint8_t *dest) {
    for (uint8_t i = 0; i < 7; i += 1)
      dest[i] = src[i] - old_root + new_root;
  }
  static void transpose(const uint8_t new_root, const uint8_t *src, uint8_t *dest) {
    transpose(new_root, src[0], src, dest);
  }

  /* C rooted scales and modes */
  static const uint8_t c_major[7];
  static const uint8_t c_natural_minor[7];
  static const uint8_t c_harmonic_minor[7];
  static const uint8_t c_ascending_melodic_minor[7];
  static const uint8_t c_phrygian_dominant[7];
  static const uint8_t c_double_harmonic[7];
  static const uint8_t c_lydian_mode[7];
  static const uint8_t c_mixolydian_mode[7];
  static const uint8_t c_dorian_mode[7];
  static const uint8_t c_phrygian_mode[];
  static const uint8_t c_locrian_mode[7];
  
 public:
  static const float _frequency[128];

  static float hertz_from_midi(uint8_t note) { return 440.0f * pow(2, (note-A_440)/12.0); }

  static float frequency(uint8_t note) { return _frequency[note&127]; }

};

/* scales */
const uint8_t Midi::c_major[] =			  { Midi::C, Midi::D,   Midi::E,   Midi::F,   Midi::G,   Midi::A,   Midi::B };
const uint8_t Midi::c_natural_minor[] =		  { Midi::C, Midi::D,   Midi::E-1, Midi::F,   Midi::G,   Midi::A-1, Midi::B-1 };
const uint8_t Midi::c_harmonic_minor[] =	  { Midi::C, Midi::D,   Midi::E-1, Midi::F,   Midi::G,   Midi::A-1, Midi::B };
const uint8_t Midi::c_ascending_melodic_minor[] = { Midi::C, Midi::D,   Midi::E-1, Midi::F,   Midi::G,   Midi::A,   Midi::B };
const uint8_t Midi::c_phrygian_dominant[] =	  { Midi::C, Midi::D-1, Midi::E,   Midi::F,   Midi::G,   Midi::A-1, Midi::B };
const uint8_t Midi::c_double_harmonic[] =	  { Midi::C, Midi::D-1, Midi::E,   Midi::F,   Midi::G,   Midi::A-1, Midi::B };
const uint8_t Midi::c_lydian_mode[] =		  { Midi::C, Midi::D,   Midi::E,   Midi::G-1, Midi::G,   Midi::A,   Midi::B };
const uint8_t Midi::c_mixolydian_mode[] =	  { Midi::C, Midi::D,   Midi::E,   Midi::F,   Midi::G,   Midi::A,   Midi::B-1 };
const uint8_t Midi::c_dorian_mode[] =		  { Midi::C, Midi::D,   Midi::E-1, Midi::F,   Midi::G,   Midi::A,   Midi::B-1 };
const uint8_t Midi::c_phrygian_mode[] =		  { Midi::C, Midi::D-1, Midi::E-1, Midi::F,   Midi::G,   Midi::A-1, Midi::B-1 };
const uint8_t Midi::c_locrian_mode[] =		  { Midi::C, Midi::D-1, Midi::E-1, Midi::F,   Midi::G-1, Midi::A-1, Midi::B-1 };

/* commands */

  
/* frequencies */
const float Midi::_frequency[128] = {
  8.175799, /* midi   0, C-1 */
  8.661957, /* midi   1, C#-1 */
  9.177024, /* midi   2, D-1 */
  9.722718, /* midi   3, D#-1 */
  10.300861, /* midi   4, E-1 */
  10.913382, /* midi   5, F-1 */
  11.562326, /* midi   6, F#-1 */
  12.249857, /* midi   7, G-1 */
  12.978272, /* midi   8, G#-1 */
  13.750000, /* midi   9, A-1 */
  14.567618, /* midi  10, A#-1 */
  15.433853, /* midi  11, B-1 */
  16.351598, /* midi  12, C0 */
  17.323914, /* midi  13, C#0 */
  18.354048, /* midi  14, D0 */
  19.445436, /* midi  15, D#0 */
  20.601722, /* midi  16, E0 */
  21.826764, /* midi  17, F0 */
  23.124651, /* midi  18, F#0 */
  24.499715, /* midi  19, G0 */
  25.956544, /* midi  20, G#0 */
  27.500000, /* midi  21, A0 */
  29.135235, /* midi  22, A#0 */
  30.867706, /* midi  23, B0 */
  32.703196, /* midi  24, C1 */
  34.647829, /* midi  25, C#1 */
  36.708096, /* midi  26, D1 */
  38.890873, /* midi  27, D#1 */
  41.203445, /* midi  28, E1 */
  43.653529, /* midi  29, F1 */
  46.249303, /* midi  30, F#1 */
  48.999429, /* midi  31, G1 */
  51.913087, /* midi  32, G#1 */
  55.000000, /* midi  33, A1 */
  58.270470, /* midi  34, A#1 */
  61.735413, /* midi  35, B1 */
  65.406391, /* midi  36, C2 */
  69.295658, /* midi  37, C#2 */
  73.416192, /* midi  38, D2 */
  77.781746, /* midi  39, D#2 */
  82.406889, /* midi  40, E2 */
  87.307058, /* midi  41, F2 */
  92.498606, /* midi  42, F#2 */
  97.998859, /* midi  43, G2 */
  103.826174, /* midi  44, G#2 */
  110.000000, /* midi  45, A2 */
  116.540940, /* midi  46, A#2 */
  123.470825, /* midi  47, B2 */
  130.812783, /* midi  48, C3 */
  138.591315, /* midi  49, C#3 */
  146.832384, /* midi  50, D3 */
  155.563492, /* midi  51, D#3 */
  164.813778, /* midi  52, E3 */
  174.614116, /* midi  53, F3 */
  184.997211, /* midi  54, F#3 */
  195.997718, /* midi  55, G3 */
  207.652349, /* midi  56, G#3 */
  220.000000, /* midi  57, A3 */
  233.081881, /* midi  58, A#3 */
  246.941651, /* midi  59, B3 */
  261.625565, /* midi  60, C4 */
  277.182631, /* midi  61, C#4 */
  293.664768, /* midi  62, D4 */
  311.126984, /* midi  63, D#4 */
  329.627557, /* midi  64, E4 */
  349.228231, /* midi  65, F4 */
  369.994423, /* midi  66, F#4 */
  391.995436, /* midi  67, G4 */
  415.304698, /* midi  68, G#4 */
  440.000000, /* midi  69, A4 */
  466.163762, /* midi  70, A#4 */
  493.883301, /* midi  71, B4 */
  523.251131, /* midi  72, C5 */
  554.365262, /* midi  73, C#5 */
  587.329536, /* midi  74, D5 */
  622.253967, /* midi  75, D#5 */
  659.255114, /* midi  76, E5 */
  698.456463, /* midi  77, F5 */
  739.988845, /* midi  78, F#5 */
  783.990872, /* midi  79, G5 */
  830.609395, /* midi  80, G#5 */
  880.000000, /* midi  81, A5 */
  932.327523, /* midi  82, A#5 */
  987.766603, /* midi  83, B5 */
  1046.502261, /* midi  84, C6 */
  1108.730524, /* midi  85, C#6 */
  1174.659072, /* midi  86, D6 */
  1244.507935, /* midi  87, D#6 */
  1318.510228, /* midi  88, E6 */
  1396.912926, /* midi  89, F6 */
  1479.977691, /* midi  90, F#6 */
  1567.981744, /* midi  91, G6 */
  1661.218790, /* midi  92, G#6 */
  1760.000000, /* midi  93, A6 */
  1864.655046, /* midi  94, A#6 */
  1975.533205, /* midi  95, B6 */
  2093.004522, /* midi  96, C7 */
  2217.461048, /* midi  97, C#7 */
  2349.318143, /* midi  98, D7 */
  2489.015870, /* midi  99, D#7 */
  2637.020455, /* midi 100, E7 */
  2793.825851, /* midi 101, F7 */
  2959.955382, /* midi 102, F#7 */
  3135.963488, /* midi 103, G7 */
  3322.437581, /* midi 104, G#7 */
  3520.000000, /* midi 105, A7 */
  3729.310092, /* midi 106, A#7 */
  3951.066410, /* midi 107, B7 */
  4186.009045, /* midi 108, C8 */
  4434.922096, /* midi 109, C#8 */
  4698.636287, /* midi 110, D8 */
  4978.031740, /* midi 111, D#8 */
  5274.040911, /* midi 112, E8 */
  5587.651703, /* midi 113, F8 */
  5919.910763, /* midi 114, F#8 */
  6271.926976, /* midi 115, G8 */
  6644.875161, /* midi 116, G#8 */
  7040.000000, /* midi 117, A8 */
  7458.620184, /* midi 118, A#8 */
  7902.132820, /* midi 119, B8 */
  8372.018090, /* midi 120, C9 */
  8869.844191, /* midi 121, C#9 */
  9397.272573, /* midi 122, D9 */
  9956.063479, /* midi 123, D#9 */
  10548.081821, /* midi 124, E9 */
  11175.303406, /* midi 125, F9 */
  11839.821527, /* midi 126, F#9 */
  12543.853951, /* midi 127, G9 */
};

#endif	// Midi_h

  

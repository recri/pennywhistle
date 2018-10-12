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
#ifndef Config_h
#define Config_h 1

/* fingering parameters */
/* the root note of the scale being played, ie the key */
#ifndef ROOTNOTE
#define ROOTNOTE Midi::C
#endif

/* the scale type or mode being played */
#ifndef SCALETYPE
#define SCALETYPE Midi::MajorScale
#endif

/* fingering alternatives, all false implies penny whistle or recorder fingering */
#ifndef USESTRONGFINGERS
#define USESTRONGFINGERS false
#endif
#ifndef USEGRAYCODE
#define USEGRAYCODE false
#endif
#ifndef USEBINARY
#define USEBINARY false
#endif
/*
  This define specifies the number of array
  elements in TouchPads and selects one of
  several pin array definitions in AudioUSB.ino
*/

#ifndef NPADS
#define NPADS 6
#endif

/*
  this define specifies the pin numbers used for
  touch pads
*/
#ifndef PADS
#if NPADS == 6
#define PADS 19, 1, 17, 29, 15, 30
#elif NPADS == 7
#define PADS 0, 19, 1, 17, 29, 15, 30
#elif NPADS == 8
#define PADS 19, 1, 17, 29, 15, 30, 16, 18
#elif NPADS == 9
#define PADS 0, 19, 1, 17, 29, 15, 30, 16, 18
#endif
#endif
/*
  this define specifies the number of
  averaging steps taken in hardware
  valid from 0 to 15
*/
#ifndef HARDWARE_AVERAGING
#define HARDWARE_AVERAGING 0
#endif

#ifndef SOFTWARE_AVERAGING
#define SOFTWARE_AVERAGING 0
#endif

/*
  this define specifies the 
  the normalized threshold
  used to distinguish touched
  and non-touched
*/
#ifndef TOUCH_THRESHOLD
#define TOUCH_THRESHOLD 0x40
#endif

/*
  this define specifies how many steps of consistent
  on/off value are required to actually accept a change
  in value, used in debouncer in TouchPads
*/
#ifndef DEBOUNCER_STEPS
#define DEBOUNCER_STEPS 31
#endif

// ** NRPN 4 -> reset

#define NPRN_NOTE	0		/* base note non-registered parameter number */
#define NPRN_SCALE	1		/* scale type non-registered parameter number */
#define NPRN_THRESHOLD	2		/* touch threshold non-registered parameter number */
#define NPRN_THRESHOLD2	3		/* touch half-hole threshold non-registered parameter number */
#define NPRN_STEPS	4		/* debouncer steps non-registered parameter number */
#define NPRN_NSCAN	5		/* TSI number of scans non-registered parameter number */
#define NPRN_REFCHRG	6		/* TSI reference charge non-registered parameter number */
#define NPRN_EXTCHRG	7		/* TSI external charge  non-registered parameter number */
#define NPRN_PRESCALE	8		/* TSI prescale non-registered parameter number */
#define NPRN_NPADS	9		/* number of pads non-registered parameter number */
#define NPRN_FINGER    10		/* fingering scheme non-registered parameter number */

/* disable parts looking for broken stuff */
#define TOUCHPADS_ENABLED 1
#define FINGERING_ENABLED 1
#define PRESSURE_ENABLED 1
#define MIDI_INPUT_ENABLED 1
#endif // config_h


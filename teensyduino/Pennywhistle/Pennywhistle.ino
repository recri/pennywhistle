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
** Pennywhistle - Teensy 3.6 audio widget
** Audio in from I2S ICS43434 microphone breakout from Tindie, 
** pressure from I2C BMP280 barometer from ebay,
** audio out to I2S MAX98537A breakout from Adafruit or Sparkfun
** and touches on touch pads on Teensy.
** Multiple program outcomes from there:
** send microphone audio and MIDI events to host,
** and take audio back from host for audio out;
** synthesize instruments locally, 
** and send the result to host and to DACs;
** swap different instruments into the Teensy Audio chain.
*/

static const char _file[] = __FILE__;
static const char _date[] = __DATE__;
static const char _time[] = __TIME__;

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// Create the Audio components.
AudioInputI2S            i2s1;           //xy=84,77
AudioInputUSB            usb2;           //xy=82,132
AudioAmplifier           amp1;           //xy=234,77
AudioAmplifier           amp2;           //xy=236,136
AudioOutputUSB           usb1;           //xy=389,74
AudioOutputI2S           i2s2;           //xy=393,133
AudioConnection          patchCord2(i2s1, 0, amp1, 0);
AudioConnection          patchCord3(amp1, 0, usb1, 0);
AudioConnection          patchCord1(usb2, 0, amp2, 0);
AudioConnection          patchCord4(amp2, 0, i2s2, 0);

#include "Config.h"
#if TOUCHPADS_ENABLED
#include "TouchPads.h"
#endif
#if FINGERING_ENABLED
#include "Fingering.h"
#include "Midi.h"
#endif
#if PRESSURE_ENABLED
#include "Pressure.h"
#endif
#include "AudioIn.h"
#include "AudioOut.h"
#include "Monitor.h"

uint8_t pads[NPADS] = { PADS };

#if MIDI_INPUT_ENABLED
static void OnNoteOff(byte channel, byte note, byte velocity) {
  Serial.print("rcvd note on "); Serial.println(note);
}

static void OnNoteOn(byte channel, byte note, byte velocity) {
  Serial.print("rcvd note off "); Serial.println(note);
}

/*
** Use NRPN for these rather than unassigned
** see assignments in Config.h
** 
** Then there will be all the controls for various
** parts of the synthesizers, 
**  breath noise gain,
**  vibrato gain,
**  breath pressure scaling
**  ...
**
** Then there are the standard messages that could be implemented
**  all notes off
**  all keys off
**  local/
**  coarse tuning, could retune the base note within the octave
**  fine tuning, could adjust to the Teensy clock
*/
static void OnControlChange(byte channel, byte control, byte value) {
  Serial.print("rcvd ctl chg "); Serial.print(control); Serial.print(" "); Serial.println(value);
  switch (control) {
#if FINGERING_ENABLED
  case 0x10: /* control change: set base note for fingering */
    Fingering::set_scale(value, Fingering::get_scale_type()); return;
  case 0x11: /* control change: set scale type for fingering */
    Fingering::set_scale(Fingering::get_root_note(), value); return;
#endif
#if TOUCHPADS_ENABLED
  case 0x12: /* control change: set threshold for normalized touch */
    TouchPads::set_threshold(value); return;
  case 0x13: /* control change: set steps for debouncing */
    TouchPads::set_steps(value); return;
  case 0x14: /* control change: reset ranges */
    TouchPads::reset(); return;
#endif
  case 0x15: /* control change: hardware averaging */
    return;
  case 0x16: /* control change: refchrg */
    return;
  case 0x17: /* control change: extchrg */
    return;
  case 0x18: /* control change: prescale */
    return;
  }
}

/*
** should try, I suppose, to match the general midi assignments
** Program Change has to do with the standard synth voices
** But could be overloaded to switch in different voices
*/
static void OnProgramChange(byte channel, byte program) {
  Serial.print("rcvd pgm chg "); Serial.println(program);
}
#endif // MIDI_INPUT_ENABLED

void setup() { 
  Monitor::begin();
  Monitor::message("initialize Audio memory\n");
  AudioMemory(12);
#if MIDI_INPUT_ENABLED
  Monitor::message("initialize USB handlers\n");
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandleProgramChange(OnProgramChange);
#endif
#if TOUCHPADS_ENABLED
  Monitor::message("initialize touch pads\n");
  TouchPads::begin(NPADS, pads);
#endif
#if FINGERING_ENABLED
  Monitor::message("initialize fingering\n");
  Fingering::begin();
#endif
#if PRESSURE_ENABLED
  Monitor::message("initialize pressure\n");
  Pressure::begin();
#endif
  Monitor::message("initialize audio input\n");
  AudioIn::begin();
  amp1.gain(1.00000);
  Monitor::message("initialize audio output\n");
  AudioOut::begin();
  amp2.gain(0.03125);
  Monitor::message("setup finished\n");
}

#if TOUCHPADS_ENABLED
static uint32_t last_touch_clock = 0;
#endif

#if PRESSURE_ENABLED
static uint16_t last_pressure = 0;
static uint16_t pressure = 0;
#endif

#if FINGERING_ENABLED
static uint8_t channel = 1;
static uint8_t last_note = 0xFF;
static uint8_t note = 0xFF;
#endif

void loop() {
  Monitor::update(); 

  if (TouchPads::clock() != last_touch_clock) {
    last_touch_clock = TouchPads::clock();
    Monitor::touch_stream();
  }
  uint32_t new_pressure = Pressure::readPressure();
  if (new_pressure != pressure) {
    last_pressure = pressure; pressure = new_pressure;
    Monitor::pressure_stream();
    // translate pressure into AfterTouch and send
  }
  if (TouchPads::available()) {
    uint8_t new_note = Fingering::translate(TouchPads::last_touch());
    if (new_note != note) {
      last_note = note; note = new_note;
      Monitor::note_stream();
      if (last_note != 0xFF) usbMIDI.sendNoteOff(last_note, 0, channel);
      if (note != 0xFF) usbMIDI.sendNoteOn(note, 127, channel);
      usbMIDI.send_now();
    }
  }
  usbMIDI.read(channel);
}

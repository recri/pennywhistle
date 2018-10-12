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
#ifndef Monitor_h
#define Monitor_h

#if defined(USB_MIDI_SERIAL) || defined(USB_MIDI_AUDIO_SERIAL)
#define MONITOR_ACTIVE
#endif

namespace Monitor {

  void begin() {
#ifdef MONITOR_ACTIVE
    Serial.begin(9600);		// usb serial
    while ( ! Serial) delay(1);
#endif
  }
  void message(const char *msg) {
#ifdef MONITOR_ACTIVE
    ::Serial.print(msg);
    // ::Serial.flush();
#endif
  }
#ifdef MONITOR_ACTIVE
  static uint8_t stream_note = 0;
  static uint8_t stream_musical = 0;
  static uint8_t stream_pressure = 0;
  static uint8_t stream_touch = 0;
  void note() { Serial.printf("%d\n", Fingering::lastNote()); }
  void musical() {
    uint8_t note = Fingering::lastNote();
    Serial.printf("%s%d ", Midi::note_name(note), Midi::note_octave(note));
  }  
  void pressure() { Serial.printf("%7d\n", Pressure::lastPressure()); }
  void touch() {
    for (int i = 0; i < NPADS; i += 1) {
      // Serial.printf("%d:%d:%d:%02x ", TouchPads::minTouch(i), TouchPads::touch(i), TouchPads::maxTouch(i), TouchPads::normTouch(i));
      Serial.printf("%d ", TouchPads::avgTouch(i));
    }
    Serial.println();
  }
#endif // MONITOR_ACTIVE

  void note_stream() {
#ifdef MONITOR_ACTIVE
    if (stream_note) note();
#endif // MONITOR_ACTIVE
  }
  void muscial_stream() {
#ifdef MONITOR_ACTIVE
    if (stream_musical) musical();
#endif // MONITOR_ACTIVE
  }
  void pressure_stream() {
#ifdef MONITOR_ACTIVE
    if (stream_pressure) pressure();
#endif // MONITOR_ACTIVE
  }
  void touch_stream() {
#ifdef MONITOR_ACTIVE
    if (stream_touch) touch();
#endif // MONITOR_ACTIVE
  }

  void update() {
#ifdef MONITOR_ACTIVE
    if (Serial.available()) {
      uint8_t c = Serial.read();
      switch (c) {
      case 'm': musical(); return;
      case 'M': stream_musical ^= 1; return;
      case 'n': note(); return;
      case 'N': stream_note ^= 1; return;
      case 't': touch(); return;
      case 'T': stream_touch ^= 1; return;
      case 'p': pressure(); return;
      case 'P': stream_pressure ^= 1; return;
      case 'v': AudioOut::set_enabled(AudioOut::is_enabled()^1); return;
	// case '+': AudioOut::set_gain(AudioOut::get_gain()+3); return;
	// case '-': AudioOut::set_gain(AudioOut::get_gain()-3); return;
      case '?':
	Serial.printf("file %s, date %s, time %s\n", _file, _date, _time);
	Serial.printf("AudioMemoryUsage = %d, AudioMemoryUsageMax = %d\n", AudioMemoryUsage(), AudioMemoryUsageMax());
	AudioMemoryUsageMaxReset();
	Serial.printf("AudioProcessorUsage = %f%%, AudioProcessorUsageMax = %f%%\n", AudioProcessorUsage(), AudioProcessorUsageMax());
	AudioProcessorUsageMaxReset();
	return;
      }
    }
#endif // MONITOR_ACTIVE
  }
};

#endif // Monitor_h

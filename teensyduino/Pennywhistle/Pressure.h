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
#ifndef Pressure_h
#define Pressure_h

#include <Wire.h>

namespace Pressure {
  /*=========================================================================
    I2C ADDRESS/BITS/SETTINGS
    -----------------------------------------------------------------------*/
#define BMP280_ADDRESS                (0x76)
#define BMP280_CHIPID                 (0x58)
  /*=========================================================================*/

  /*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
  enum {
    BMP280_REGISTER_DIG_T1              = 0x88,
    BMP280_REGISTER_DIG_T2              = 0x8A,
    BMP280_REGISTER_DIG_T3              = 0x8C,

    BMP280_REGISTER_DIG_P1              = 0x8E,
    BMP280_REGISTER_DIG_P2              = 0x90,
    BMP280_REGISTER_DIG_P3              = 0x92,
    BMP280_REGISTER_DIG_P4              = 0x94,
    BMP280_REGISTER_DIG_P5              = 0x96,
    BMP280_REGISTER_DIG_P6              = 0x98,
    BMP280_REGISTER_DIG_P7              = 0x9A,
    BMP280_REGISTER_DIG_P8              = 0x9C,
    BMP280_REGISTER_DIG_P9              = 0x9E,

    BMP280_REGISTER_CHIPID             = 0xD0,
    BMP280_REGISTER_VERSION            = 0xD1,
    BMP280_REGISTER_SOFTRESET          = 0xE0,

    BMP280_REGISTER_CAL26              = 0xE1,  // R calibration stored in 0xE1-0xF0

    BMP280_REGISTER_CONTROL            = 0xF4,
    BMP280_REGISTER_CONFIG             = 0xF5,
    BMP280_REGISTER_PRESSUREDATA       = 0xF7,
    BMP280_REGISTER_TEMPDATA           = 0xFA,
  };

  /*=========================================================================*/

  /*=========================================================================
    CALIBRATION DATA
    -----------------------------------------------------------------------*/
  typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;

    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;

    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    int8_t   dig_H6;
  } bmp280_calib_data;

  static bmp280_calib_data bmp280_calib;
  static int32_t t_fine; 
  static uint32_t last_t;
  static uint32_t last_p;
  
  static uint8_t read8(uint8_t reg) {
    Wire.beginTransmission((uint8_t)BMP280_ADDRESS);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)BMP280_ADDRESS, (byte)1);
    return Wire.read();
  }

  static void write8(byte reg, byte value) {
    Wire.beginTransmission((uint8_t)BMP280_ADDRESS);
    Wire.write((uint8_t)reg);
    Wire.write((uint8_t)value);
    Wire.endTransmission();
  }

  static uint16_t read16(uint8_t reg) {
    Wire.beginTransmission((uint8_t)BMP280_ADDRESS);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)BMP280_ADDRESS, (byte)2);
    return (Wire.read() << 8) | Wire.read();
  }

  static uint16_t read16_LE(byte reg) {
    uint16_t temp = read16(reg);
    return (temp >> 8) | (temp << 8);
  }
  int16_t readS16(byte reg) { return (int16_t)read16(reg); }

  int16_t readS16_LE(byte reg) { return (int16_t)read16_LE(reg); }

  uint32_t read24(byte reg) {
    uint32_t value;

    Wire.beginTransmission((uint8_t)BMP280_ADDRESS);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)BMP280_ADDRESS, (byte)3);
    
    value = Wire.read();
    value <<= 8;
    value |= Wire.read();
    value <<= 8;
    value |= Wire.read();

    return value;
  }

  static void readCoefficients(void) {
    bmp280_calib.dig_T1 = read16_LE(BMP280_REGISTER_DIG_T1);
    bmp280_calib.dig_T2 = readS16_LE(BMP280_REGISTER_DIG_T2);
    bmp280_calib.dig_T3 = readS16_LE(BMP280_REGISTER_DIG_T3);

    bmp280_calib.dig_P1 = read16_LE(BMP280_REGISTER_DIG_P1);
    bmp280_calib.dig_P2 = readS16_LE(BMP280_REGISTER_DIG_P2);
    bmp280_calib.dig_P3 = readS16_LE(BMP280_REGISTER_DIG_P3);
    bmp280_calib.dig_P4 = readS16_LE(BMP280_REGISTER_DIG_P4);
    bmp280_calib.dig_P5 = readS16_LE(BMP280_REGISTER_DIG_P5);
    bmp280_calib.dig_P6 = readS16_LE(BMP280_REGISTER_DIG_P6);
    bmp280_calib.dig_P7 = readS16_LE(BMP280_REGISTER_DIG_P7);
    bmp280_calib.dig_P8 = readS16_LE(BMP280_REGISTER_DIG_P8);
    bmp280_calib.dig_P9 = readS16_LE(BMP280_REGISTER_DIG_P9);
  }

  /**************************************************************************/
  uint16_t readTemperature(void) {
    int32_t var1, var2;

    int32_t adc_T = read24(BMP280_REGISTER_TEMPDATA);
    adc_T >>= 4;

    var1  = ((((adc_T>>3) - ((int32_t)bmp280_calib.dig_T1 <<1))) * ((int32_t)bmp280_calib.dig_T2)) >> 11;

    var2  = (((((adc_T>>4) - ((int32_t)bmp280_calib.dig_T1)) * ((adc_T>>4) - ((int32_t)bmp280_calib.dig_T1))) >> 12) *
	     ((int32_t)bmp280_calib.dig_T3)) >> 14;

    t_fine = var1 + var2;

    return last_t = (t_fine * 5 + 128) >> 8;

  }
  uint32_t lastTemperature(void) { return last_t; }
  /**************************************************************************/
  /*!

   */
  /**************************************************************************/
  uint32_t readPressure(void) {
    int64_t var1, var2, p;

    // Must be done first to get the t_fine variable set up
    readTemperature();

    int32_t adc_P = read24(BMP280_REGISTER_PRESSUREDATA);
    adc_P >>= 4;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)bmp280_calib.dig_P6;
    var2 = var2 + ((var1*(int64_t)bmp280_calib.dig_P5)<<17);
    var2 = var2 + (((int64_t)bmp280_calib.dig_P4)<<35);
    var1 = ((var1 * var1 * (int64_t)bmp280_calib.dig_P3)>>8) + ((var1 * (int64_t)bmp280_calib.dig_P2)<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)bmp280_calib.dig_P1)>>33;

    if (var1 == 0) {
      return 0;  // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p<<31) - var2)*3125) / var1;
    var1 = (((int64_t)bmp280_calib.dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((int64_t)bmp280_calib.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)bmp280_calib.dig_P7)<<4);
    return last_p = (p+128)>>8;
  }
  uint32_t lastPressure(void) { return last_p; }
  
  static int begin() {
    Serial.println("setting SDA to 34");
    Wire.setSDA(34);
    Serial.println("setting SCL to 33");
    Wire.setSCL(33);
    Serial.println("calling wire begin");
    Wire.begin();
    Serial.println("reading the BMP280 chip ID");
    uint8_t chipid = read8(BMP280_REGISTER_CHIPID);
    if (chipid != BMP280_CHIPID) {
      Serial.print("wrong chipid: "); Serial.println(chipid, HEX);
      return 0;
    }
    Serial.print("chipid: "); Serial.println(chipid, HEX);
    Serial.print("ctl_meas: "); Serial.println(read8(BMP280_REGISTER_CONTROL), HEX); /* 0xF4 */
    Serial.print("config:  "); Serial.println(read8(BMP280_REGISTER_CONFIG), HEX);   /* 0xF5 */
    Serial.println("readCoefficients");
    readCoefficients();
    Serial.println("writing control");
    write8(BMP280_REGISTER_CONTROL, 0x3F); /* 0xF4  */

    return 1;
  }

}
#endif // Pressure_h

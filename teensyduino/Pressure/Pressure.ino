/* -*-C++-*- */

#include <Wire.h>
#include <SPI.h>
#include "bmp280.h"

static struct bmp280_dev bmp;
static struct bmp280_config conf;
static struct bmp280_uncomp_data ucomp_data;
static uint8_t meas_dur;

static int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len) {
  Wire.beginTransmission(dev_id);
  Wire.write(reg_addr);
  Wire.endTransmission();
  Wire.requestFrom(dev_id, (uint8_t)len);
  for (int i = 0; i < len; i += 1) data[i] = Wire.read();
  // Serial.printf("read %02x, %02x, %d, %02x\n", dev_id, reg_addr, len, data[0]);
  return BMP280_OK;
}

static int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len) {
  Wire.beginTransmission(dev_id);
  Wire.write(reg_addr);
  for (int i = 0; i < len; i += 1) Wire.write(data[i]);
  Wire.endTransmission();
  return BMP280_OK;
}

static void user_delay_ms(uint32_t period) {
  delay(period);
}

void setup() {
  Serial.begin(9600);

  while (! Serial);
  
  Serial.println("BMP280 test");

  Wire.setSDA(34);
  Wire.setSCL(33);
  Wire.begin();
  Serial.println("Wire initialized");

  bmp.dev_id = BMP280_I2C_ADDR_PRIM;
  bmp.intf = BMP280_I2C_INTF;
  bmp.read = user_i2c_read;
  bmp.write = user_i2c_write;
  bmp.delay_ms = user_delay_ms;
  
  int8_t rslt = bmp280_init(&bmp);
  if (rslt != BMP280_OK) {  
    Serial.print("bmp280_init returned "); Serial.println(rslt); 
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }
  Serial.println("BMP280 initialized");
  
  /* Always read the current settings before writing, especially when
   * all the configuration is not modified 
   */
  rslt = bmp280_get_config(&conf, &bmp);
  /* Check if rslt == BMP280_OK, if not, then handle accordingly */
  if (rslt != BMP280_OK) {
    Serial.printf("failed to get config\n");
    while (1);
  }
  Serial.println("BMP280 config read");

  /* Overwrite the desired settings */
  conf.filter = BMP280_FILTER_OFF;
  conf.os_pres = BMP280_OS_16X;
  conf.os_temp = BMP280_OS_4X;
  conf.odr = BMP280_ODR_0_5_MS;

  rslt = bmp280_set_config(&conf, &bmp);
  /* Check if rslt == BMP280_OK, if not, then handle accordingly */
  if (rslt != BMP280_OK) {
    Serial.printf("failed to set config\n");
    while (1);
  }
  Serial.println("BMP280 config written");

  /* Always set the power mode after setting the configuration */
  rslt = bmp280_set_power_mode(BMP280_NORMAL_MODE, &bmp);
  /* Check if rslt == BMP280_OK, if not, then handle accordingly */
  if (rslt != BMP280_OK) {
    Serial.printf("failed to set power mode\n");
    while (1);
  }
  Serial.println("BMP280 mode set");

  meas_dur = bmp280_compute_meas_time(&bmp);
  Serial.print("Measurement duration: "); Serial.print(meas_dur); Serial.println("ms");
}

void process0() {
  int32_t ut = ucomp_data.uncomp_temp;
  int32_t up = ucomp_data.uncomp_press;
  int32_t t32 = bmp280_comp_temp_32bit(ucomp_data.uncomp_temp, &bmp);
  uint32_t p32 = bmp280_comp_pres_32bit(ucomp_data.uncomp_press, &bmp);
  uint32_t p64 = bmp280_comp_pres_64bit(ucomp_data.uncomp_press, &bmp);
  double t = bmp280_comp_temp_double(ucomp_data.uncomp_temp, &bmp);
  double p = bmp280_comp_pres_double(ucomp_data.uncomp_press, &bmp);
  Serial.printf("UT: %ld, UP: %ld, T32: %ld, P32: %ld, P64: %ld, P64N: %ld, T: %f, P: %f\r\n",
		ut, up, t32, p32, p64, p64 / 256, t, p);
}
void process1() {
  static double accum = 0;
  static int count = 0;
  static int baseline;
  uint32_t p64 = bmp280_comp_pres_64bit(ucomp_data.uncomp_press, &bmp);
#define N 1000
  if (count < N) {
    accum += p64>>8;
    count += 1;
  } else if (count == N) {
    baseline = accum / N;
    Serial.printf("baseline = %f\n", baseline);
    count += 1;
  } else {
    Serial.printf(" %d", (p64>>8) - baseline);
    if ((count++ % 20) == 0) Serial.println();
  }
}
void process2() {
  static int32_t count = 0;
  uint32_t p64 = bmp280_comp_pres_64bit(ucomp_data.uncomp_press, &bmp);
  Serial.printf("%9d", p64);
  if ((++count % 20) == 0) Serial.println();
}
void process3() {
  static int32_t count = 0;
  static uint32_t last_p = 0;
  uint32_t p64 = bmp280_comp_pres_64bit(ucomp_data.uncomp_press, &bmp);
  uint32_t p = (p64+128)>>8;
  if (p != last_p) {
    last_p = p;
    Serial.printf("%7d", p);
    if ((++count % 20) == 0) Serial.println();
  }
}

#if 0
void processz() {
  int32_t t32 = bmp280_comp_temp_32bit(ucomp_data.uncomp_temp, &bmp);
  uint32_t p32 = bmp280_comp_pres_32bit(ucomp_data.uncomp_press, &bmp);
  uint32_t p64 = bmp280_comp_pres_64bit(ucomp_data.uncomp_press, &bmp);
  double t = bmp280_comp_temp_double(ucomp_data.uncomp_temp, &bmp);
  double p = bmp280_comp_pres_double(ucomp_data.uncomp_press, &bmp);
}
#endif

#define process process3

void loop() {
  // bmp.delay_ms(meas_dur); /* Measurement time */

  uint8_t rslt = bmp280_get_uncomp_data(&ucomp_data, &bmp);
  /* Check if rslt == BMP280_OK, if not, then handle accordingly */
  if (rslt != BMP280_OK) {
    Serial.printf("failed to get uncompensated data\n");
  } else {
    process();
  }
}

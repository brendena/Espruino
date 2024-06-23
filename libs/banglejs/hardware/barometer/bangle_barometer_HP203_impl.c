#include "jswrap_bangle_barometer.h"

#ifdef PRESSURE_DEVICE_HP203

#ifndef PRESSURE_DEVICE_HP203_EN
#define PRESSURE_DEVICE_HP203_EN 1
#endif




bool banglejs_barometer_get_data_impl(double* altitude, double* pressure, double* temperature)
{
  if (PRESSURE_DEVICE_HP203_EN) {
    unsigned char buf[6];
    // ADC_CVT - 0b010 01 000  - pressure and temperature channel, OSR = 4096
    buf[0] = 0x48; jsi2cWrite(PRESSURE_I2C, PRESSURE_ADDR, 1, buf, true);
    // wait 100ms
    jshDelayMicroseconds(100*1000); // we should really have a callback
    // READ_PT
    buf[0] = 0x10; jsi2cWrite(PRESSURE_I2C, PRESSURE_ADDR, 1, buf, false);
    jsi2cRead(PRESSURE_I2C, PRESSURE_ADDR, 6, buf, true);
    int tmpTemperature = (buf[0]<<16)|(buf[1]<<8)|buf[2];
    if (tmpTemperature&0x800000) tmpTemperature-=0x1000000;
    int tmpPressure = (buf[3]<<16)|(buf[4]<<8)|buf[5];
    *temperature = tmpTemperature/100.0;
    *pressure = tmpPressure/100.0;

    buf[0] = 0x31; jsi2cWrite(PRESSURE_I2C, PRESSURE_ADDR, 1, buf, false); // READ_A
    jsi2cRead(PRESSURE_I2C, PRESSURE_ADDR, 3, buf, true);
    int tmpAltitude = (buf[0]<<16)|(buf[1]<<8)|buf[2];
    if (tmpAltitude&0x800000) tmpAltitude-=0x1000000;
    *altitude = tmpAltitude/100.0;
    return true;
  }
  return false;
};

#endif
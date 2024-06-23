#pragma once
#include "jsutils.h"
#include "jsi2c.h"
#include "jswrap_math.h"
#include "bangle_defines.h"
#include "../jswrap_bangle_barometer.h"
#include "SPL06_registers.h"


short barometer_c0, barometer_c1, barometer_c01, barometer_c11, barometer_c20, barometer_c21, barometer_c30;
int barometer_c00, barometer_c10;


bool SPL06_07_barometer_get_data_impl(double* altitude, double* pressure, double* temperature)
{
    static int oversample_scalefactor[] = {524288, 1572864, 3670016, 7864320, 253952, 516096, 1040384, 2088960};
    unsigned char buf[6];

    // status values
    buf[0] = SPL06_MEASCFG; jsi2cWrite(PRESSURE_I2C, PRESSURE_ADDR, 1, buf, false);
    jsi2cRead(PRESSURE_I2C, PRESSURE_ADDR, 1, buf, true);
    int status = buf[0];
    if ((status & 0b00110000) != 0b00110000) {
      // data hasn't arrived yet
      return false;
    }

    // raw values
    buf[0] = SPL06_PRSB2; jsi2cWrite(PRESSURE_I2C, PRESSURE_ADDR, 1, buf, false);
    jsi2cRead(PRESSURE_I2C, PRESSURE_ADDR, 6, buf, true);
    int praw = (buf[0]<<16)|(buf[1]<<8)|buf[2];
    praw = twosComplement(praw, 24);
    int traw = (buf[3]<<16)|(buf[4]<<8)|buf[5];
    traw = twosComplement(traw, 24);

    double traw_scaled = (double)traw / oversample_scalefactor[SPL06_8SAMPLES]; // temperature oversample by 8x
    double praw_scaled = (double)praw / oversample_scalefactor[SPL06_8SAMPLES]; // pressure oversample by 8x
    *temperature = (barometer_c0/2) + (barometer_c1*traw_scaled);
    double pressurePa = (barometer_c00 + praw_scaled * (barometer_c10 + praw_scaled * (barometer_c20 + praw_scaled * barometer_c30)) +
                        traw_scaled * barometer_c01 +
                        traw_scaled * praw_scaled * ( barometer_c11 + praw_scaled * barometer_c21));
    *pressure = pressurePa / 100; // convert Pa to hPa/millibar
    *altitude = 44330 * (1.0 - jswrap_math_pow(*pressure / barometerSeaLevelPressure, 0.1903));
    // TODO: temperature corrected altitude?
    return true;

}

void SPL06_07_barometer_on_impl()
{
    unsigned char buf[SPL06_COEF_NUM];
    jswrap_banglejs_barometerWr(SPL06_RESET, 0x89); // Perform soft reset
    // wait for reset complete (max 100ms) - usually takes 40ms
    int timeout = 10;
    do {
      // we can't poll every ms because initially the top bits are still set in MEAS_CFG
      jshDelayMicroseconds(10000);
      buf[0] = SPL06_MEASCFG; jsi2cWrite(PRESSURE_I2C, PRESSURE_ADDR, 1, buf, false);
      jsi2cRead(PRESSURE_I2C, PRESSURE_ADDR, 1, buf, true);
    } while (((buf[0]&0xC0) != 0xC0) && timeout--);
    // set up the sensor
    jswrap_banglejs_barometerWr(SPL06_CFGREG, 0); // No FIFO or IRQ (should be default but has been nonzero when read!
    jswrap_banglejs_barometerWr(SPL06_PRSCFG, 0x33); // pressure oversample by 8x, 8 measurement per second
    jswrap_banglejs_barometerWr(SPL06_TMPCFG, 0xB3); // temperature oversample by 8x, 8 measurements per second, external sensor
    jswrap_banglejs_barometerWr(SPL06_MEASCFG, 7); // continuous temperature and pressure measurement
    // read calibration data
    buf[0] = SPL06_COEF_START; jsi2cWrite(PRESSURE_I2C, PRESSURE_ADDR, 1, buf, false);
    jsi2cRead(PRESSURE_I2C, PRESSURE_ADDR, SPL06_COEF_NUM, buf, true);
    barometer_c0 = twosComplement(((unsigned short)buf[0] << 4) | (((unsigned short)buf[1] >> 4) & 0x0F), 12);
    barometer_c1 = twosComplement((((unsigned short)buf[1] & 0x0F) << 8) | buf[2], 12);
    barometer_c00 = twosComplement(((unsigned int)buf[3] << 12) | ((unsigned int)buf[4] << 4) | (((unsigned int)buf[5] >> 4) & 0x0F), 20);
    barometer_c10 = twosComplement((((unsigned int)buf[5] & 0x0F) << 16) | ((unsigned int)buf[6] << 8) | (unsigned int)buf[7], 20);
    barometer_c01 = twosComplement(((unsigned short)buf[8] << 8) | (unsigned short)buf[9], 16);
    barometer_c11 = twosComplement(((unsigned short)buf[10] << 8) | (unsigned short)buf[11], 16);
    barometer_c20 = twosComplement(((unsigned short)buf[12] << 8) | (unsigned short)buf[13], 16);
    barometer_c21 = twosComplement(((unsigned short)buf[14] << 8) | (unsigned short)buf[15], 16);
    barometer_c30 = twosComplement(((unsigned short)buf[16] << 8) | (unsigned short)buf[17], 16);
  
}

void SPL06_07_barometer_off_impl(){
    banglejs_barometer_i2c_Wr(SPL06_MEASCFG, 0); // Barometer off
}    

void SPL06_07_barometer_device_init_impl(){
    unsigned char buf[2] = {SPL06_RESET,0x89};
    jsi2cWrite(PRESSURE_I2C, PRESSURE_ADDR, 1, buf, true); // SOFT_RST
}



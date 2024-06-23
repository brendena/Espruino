#pragma once
#include "bangle_defines.h"
#include "jsi2c.h"
#include "jswrap_math.h"
#include "../jswrap_bangle_barometer.h"
#include "SPL06_registers.h"


int barometerDT[3]; // temp calibration
int barometerDP[9]; // pressure calibration

bool SPL06_01_barometer_get_data_impl(double* altitude, double* pressure, double* temperature)
{
    unsigned char buf[8];
    buf[0] = 0xF7; jsi2cWrite(PRESSURE_I2C, PRESSURE_ADDR, 1, buf, false); // READ_A
    jsi2cRead(PRESSURE_I2C, PRESSURE_ADDR, 6, buf, true);
    int uncomp_pres = (buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4);
    int uncomp_temp = (buf[3] << 12) | (buf[4] << 4) | (buf[5] >> 4);
    double var1, var2;
    // temperature
    var1 = (((double) uncomp_temp) / 16384.0 - ((double) barometerDT[0]) / 1024.0) *
        ((double) barometerDT[1]);
    var2 =
        ((((double) uncomp_temp) / 131072.0 - ((double) barometerDT[0]) / 8192.0) *
        (((double) uncomp_temp) / 131072.0 - ((double) barometerDT[0]) / 8192.0)) *
        ((double) barometerDT[2]);
    int32_t t_fine = (int32_t) (var1 + var2);
    *temperature = ((var1 + var2) / 5120.0);
    // pressure
    var1 = ((double) t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double) barometerDP[5]) / 32768.0;
    var2 = var2 + var1 * ((double) barometerDP[4]) * 2.0;
    var2 = (var2 / 4.0) + (((double) barometerDP[3]) * 65536.0);
    var1 = (((double)barometerDP[2]) * var1 * var1 / 524288.0 + ((double)barometerDP[1]) * var1) /
        524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double) barometerDP[0]);

    double tmpPressure = 1048576.0 - (double)uncomp_pres;
    if (var1 < 0 || var1 > 0) {
        tmpPressure = (tmpPressure - (var2 / 4096.0)) * 6250.0 / var1;
        var1 = ((double)barometerDP[8]) * (tmpPressure) * (tmpPressure) / 2147483648.0;
        var2 = (tmpPressure) * ((double)barometerDP[7]) / 32768.0;
        tmpPressure = tmpPressure + (var1 + var2 + ((double)barometerDP[6])) / 16.0;
        tmpPressure = tmpPressure/100.0;
    } else {
        tmpPressure = 0;
    }

    *altitude = 44330 * (1.0 - jswrap_math_pow(tmpPressure / barometerSeaLevelPressure, 0.1903));

    *pressure = tmpPressure;
        // TODO: temperature corrected altitude?
    return true;

}

void SPL06_01_barometer_on_impl()
{
    jswrap_banglejs_barometerWr(0xF4, 0x27); // ctrl_meas_reg - normal mode, no pressure/temp oversample
    jswrap_banglejs_barometerWr(0xF5, 0xA0); // config_reg - 1s standby, no filter, I2C
    // read calibration data
    unsigned char buf[24];
    buf[0] = 0x88; jsi2cWrite(PRESSURE_I2C, PRESSURE_ADDR, 1, buf, false);
    jsi2cRead(PRESSURE_I2C, PRESSURE_ADDR, 24, buf, true);
    int i;
    barometerDT[0] = ((int)buf[1] << 8) | (int)buf[0];  //first coeff is unsigned
    for (i=1;i<3;i++)
    {
        barometerDT[i] = twosComplement(((int)buf[(i*2)+1] << 8) | (int)buf[i*2], 16);
    }
    barometerDP[0] = ((int)buf[7] << 8) | (int)buf[6];  //first coeff is unsigned
    for (i=1;i<9;i++)
    {
        barometerDP[i] = twosComplement(((int)buf[(i*2)+7] << 8) | (int)buf[(i*2)+6], 16);
    }
}

void SPL06_01_barometer_off_impl(){
    banglejs_barometer_i2c_Wr(0xF4, 0); // Barometer off
}

void SPL06_01_barometer_device_init_impl(){
    unsigned char buf[2] = {0xE0,0xB6};
    jsi2cWrite(PRESSURE_I2C, PRESSURE_ADDR, 1, buf, true); // reset
}
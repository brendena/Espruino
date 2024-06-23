#pragma once

#define DEFAULT_LCD_POWER_TIMEOUT 0 // don't turn LCD off
#define DEFAULT_BACKLIGHT_TIMEOUT 3000
#define DEFAULT_LOCK_TIMEOUT 5000

#ifndef EMULATED
    extern JshI2CInfo i2cAccel;
    extern JshI2CInfo i2cMag;
    extern JshI2CInfo i2cPressure;
    extern JshI2CInfo i2cHRM;
    #define ACCEL_I2C &i2cAccel
    #define MAG_I2C &i2cMag
    #define PRESSURE_I2C &i2cPressure
    #define HRM_I2C &i2cHRM
    #define GPS_UART EV_SERIAL1
    #define HEARTRATE 1
#endif
#include "bangle_defines.h"

const ESPR_cmd_i2c bangle_accel_KX023_init[] = {
    {ESPR_CMD_I2C_WRITE,0x18,0x0a}, // CNTL1 Off (top bit)
    {ESPR_CMD_I2C_WRITE,0x19,0x80}, // CNTL2 Software reset
};


const ESPR_cmd_i2c bangle_accel_KX023_init_2[] ={
    {ESPR_CMD_I2C_SLEEP,0,2000},
    {ESPR_CMD_I2C_WRITE,0x1a,0b10011000},  // CNTL3 12.5Hz tilt, 400Hz tap, 0.781Hz motion detection
    {ESPR_CMD_I2C_WRITE,0x1b,0b00000000},  // ODCNTL - 12.5Hz acceleration output data rate, filtering low-pass ODR/9
    {ESPR_CMD_I2C_WRITE,0x1c,0}, // INC1 disabled
    {ESPR_CMD_I2C_WRITE,0x1d,0}, // INC2 disabled
    {ESPR_CMD_I2C_WRITE,0x1e,0x3F}, // INC3 enable tap detect in all 6 directions
    {ESPR_CMD_I2C_WRITE,0x1f,0}, // INC4 disabled
    {ESPR_CMD_I2C_WRITE,0x20,0}, // INC5 disabled
    {ESPR_CMD_I2C_WRITE,0x21,0}, // INC6 disabled
    {ESPR_CMD_I2C_WRITE,0x23,3}, // WUFC wakeupi detect counter
    {ESPR_CMD_I2C_WRITE,0x24,3}, // TDTRC Tap detect enable
    {ESPR_CMD_I2C_WRITE,0x25, 0x78}, // TDTC Tap detect double tap (0x78 default)
    {ESPR_CMD_I2C_WRITE,0x26, 0xCB}, // TTH Tap detect threshold high (0xCB default)
    {ESPR_CMD_I2C_WRITE,0x27, 0x25}, // TTL Tap detect threshold low (0x1A default)
// setting TTL=0x1A means that when the HRM is on, interference sometimes means a spurious tap is detected! https://forum.espruino.com/conversations/390041
    {ESPR_CMD_I2C_WRITE,0x30,1},
//jswrap_banglejs_accelWr(0x35,0 << 4); // LP_CNTL no averaging of samples
    {ESPR_CMD_I2C_WRITE,0x35,2 << 4}, // LP_CNTL 4x averaging of samples
    {ESPR_CMD_I2C_WRITE,0x3e,0}, // clear the buffer
    {ESPR_CMD_I2C_WRITE,0x18,0b00101100},  // CNTL1 Off, low power, DRDYE=1, 4g range, TDTE (tap enable)=1, Wakeup=0, Tilt=0
    {ESPR_CMD_I2C_WRITE,0x18,0b10101100}  // CNTL1 On, low power, DRDYE=1, 4g range, TDTE (tap enable)=1, Wakeup=0, Tilt=0
};    // high power vs low power uses an extra 150uA


void banglejs_accel_init_impl(bool firstRun){
    if(firstRun){
        banglejs_i2cCmd_list_process(ACCEL_I2C,ACCEL_ADDR, &bangle_accel_KX023_init, SIZE_OF_CMD_I2C(bangle_accel_KX023_init));
        unsigned char buf[2] = {0x19, 0x80}; // Second I2C address for software reset (issue #1972)
        jsi2cWrite(ACCEL_I2C, ACCEL_ADDR-2, 2, buf, true);
        banglejs_i2cCmd_list_process(ACCEL_I2C,ACCEL_ADDR, &bangle_accel_KX023_init_2, SIZE_OF_CMD_I2C(bangle_accel_KX023_init_2));
    }
}
void banglejs_accel_off_impl(){
    jswrap_banglejs_accelWr(0x18,0x0a);
}
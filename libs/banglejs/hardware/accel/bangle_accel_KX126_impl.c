#ifdef ACCEL_DEVICE_KX126
#include "lib/kx126_registers.h"
const ESPR_cmd_i2c bangle_accel_init[] = {
    {ESPR_CMD_I2C_WRITE,KX126_CNTL1,0x00}, // CNTL1 standby mode (top bit)
    {ESPR_CMD_I2C_WRITE,KX126_CNTL2,KX126_CNTL2_SRST}, // CNTL2 Software reset (top bit)
    {ESPR_CMD_I2C_SLEEP,0,2000},
    {ESPR_CMD_I2C_WRITE,KX126_CNTL3,KX126_CNTL3_OTP_12P5|KX126_CNTL3_OTDT_400|KX126_CNTL3_OWUF_0P781}, // CNTL3 12.5Hz tilt, 400Hz tap, 0.781Hz motion detection
    {ESPR_CMD_I2C_WRITE,KX126_ODCNTL,KX126_ODCNTL_OSA_12P5}, // ODCNTL - 12.5Hz output data rate (ODR), with low-pass filter set to ODR/9
    {ESPR_CMD_I2C_WRITE,KX126_INC1,0},      // INC1 - interrupt output pin INT1 disabled
    {ESPR_CMD_I2C_WRITE,KX126_INC2,0},      // INC2 - wake-up & back-to-sleep ignores all 3 axes
    {ESPR_CMD_I2C_WRITE,KX126_INC3,0x3F},   // INC3 - enable tap detection in all 6 directions
    {ESPR_CMD_I2C_WRITE,KX126_INC4,0},      // INC4 - no routing of interrupt reporting to pin INT1
    {ESPR_CMD_I2C_WRITE,KX126_INC5,0},      // INC5 - interrupt output pin INT2 disabled
    {ESPR_CMD_I2C_WRITE,KX126_INC6,0},      // INC6 - no routing of interrupt reporting to pin INT2
    {ESPR_CMD_I2C_WRITE,KX126_INC7,0},      // INC7 - no step counter interrupts reported on INT1 or INT2
    {ESPR_CMD_I2C_WRITE,KX126_TDTRC,3},     // TDTRC - enable interrupts on single and double taps
    {ESPR_CMD_I2C_WRITE,KX126_TDTC, 0x78},  // TDTC - tap detect double tap (0x78 default)
    {ESPR_CMD_I2C_WRITE,KX126_TTH, 0xCB},   // TTH - tap detect threshold high (0xCB default)
    {ESPR_CMD_I2C_WRITE,KX126_TTL, 0x22},   // TTL - tap detect threshold low (0x1A default)
    {ESPR_CMD_I2C_WRITE,KX126_BUF_CLEAR,0}, // clear the buffer
    {ESPR_CMD_I2C_WRITE,KX126_TDTC,KX126_CNTL1,KX126_CNTL1_DRDYE|KX126_CNTL1_GSEL_4G|KX126_CNTL1_TDTE}, // CNTL1 - standby mode, low power, enable "data ready" interrupt, 4g, enable tap, disable tilt & pedometer (for now)
    {ESPR_CMD_I2C_WRITE,KX126_CNTL1,KX126_CNTL1_DRDYE|KX126_CNTL1_GSEL_4G|KX126_CNTL1_TDTE|KX126_CNTL1_PC1} // CNTL1 - same as above but change from standby to operating mode
};


void banglejs_accel_init_impl(bool firstRun){
    if(firstRun){
        banglejs_i2cCmd_list_process(ACCEL_I2C,ACCEL_ADDR, &bangle_accel_init, SIZE_OF_CMD_I2C(bangle_accel_init));
    }
}

void banglejs_accel_off_impl(){
    jswrap_banglejs_accelWr(KX126_CNTL1,0);
}

#endif
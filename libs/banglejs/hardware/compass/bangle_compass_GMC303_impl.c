#include "bangle_compass_impl.h"
#ifdef MAG_DEVICE_GMC303
const ESPR_cmd_i2c bangle_compass_init[] = {
    {ESPR_CMD_I2C_WRITE,0x32,1}, // soft reset
    {ESPR_CMD_I2C_WRITE,0x31,0}, // power down mode
};


void banglejs_compass_device_init_impl(bool firstRun){
    banglejs_i2cCmd_list_process(ACCEL_I2C,ACCEL_ADDR, &bangle_compass_init, SIZE_OF_CMD_I2C(bangle_compass_init));    
}


bool banglejs_compass_get_pos_impl(Vector3 *mag)
{
    bool newReading = false;
    unsigned char buf[7];
    buf[0] = 0x10;
    jsi2cWrite(MAG_I2C, MAG_ADDR, 1, buf, false);
    jsi2cRead(MAG_I2C, MAG_ADDR, 7, buf, true);
    if (buf[0]&1) { // then we have data
      int16_t magRaw[3];
      magRaw[0] = buf[1] | (buf[2]<<8);
      magRaw[1] = buf[3] | (buf[4]<<8);
      magRaw[2] = buf[5] | (buf[6]<<8);
      // no sign extend because magRaw is 16 bit signed already
      // apply calibration (and we multiply by 4 to bring the values more in line with what we get for Bangle.js's magnetometer)
      mag->y = (magRaw[0] * (128+magCalib[0])) >> 5; // x/y are swapped
      mag->x = (magRaw[1] * (128+magCalib[1])) >> 5;
      mag->z = (magRaw[2] * (128+magCalib[2])) >> 5;
      newReading = true;


//not sure why this is just on this device
#ifdef LCD_ROTATION
  #if LCD_ROTATION == 180
      mag->y = -mag->y;
      mag->x = -mag->x;
  #elif LCD_ROTATION == 0
      // all ok
  #else
    #error "LCD rotation is only implemented for 180 and 0 degrees"
  #endif
#endif
    }
    return newReading;
}

void banglejs_compass_off_impl(){
    jswrap_banglejs_compassWr(0x31,0);
}
void banglejs_compass_on_impl(){
    jswrap_banglejs_compassWr(0x31,4); // continuous measurement mode, 20Hz
    // Get magnetometer calibration values
    magCalib[0] = 0x60;
    jsi2cWrite(MAG_I2C, MAG_ADDR, 1, magCalib, false);
    jsi2cRead(MAG_I2C, MAG_ADDR, 3, magCalib, true);
}
#endif
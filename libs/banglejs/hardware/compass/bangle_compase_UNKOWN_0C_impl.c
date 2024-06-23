#include "bangle_compass_impl.h"
#ifdef MAG_DEVICE_UNKNOWN_0C

bool banglejs_compass_get_pos_impl(Vector3 *mag)
{
    bool newReading = false;
    unsigned char buf[7];
    buf[0]=0x4E;
    jsi2cWrite(MAG_I2C, MAG_ADDR, 1, buf, false);
    jsi2cRead(MAG_I2C, MAG_ADDR, 7, buf, true);
    if (!(buf[0]&16)) { // then we have data that wasn't read before
      // &2 seems always set
      // &16 seems set if we read twice
      // &32 might be reading in progress
      mag->y = buf[2] | (buf[1]<<8);
      mag->x = buf[4] | (buf[3]<<8);
      mag->z = buf[6] | (buf[5]<<8);
      // Now read 0x3E which should kick off a new reading
      buf[0]=0x3E;
      jsi2cWrite(MAG_I2C, MAG_ADDR, 1, buf, false);
      jsi2cRead(MAG_I2C, MAG_ADDR, 1, buf, true);
      newReading = true;
    }
    return newReading;
}
void banglejs_compass_on_impl(){
  jsvUnLock(jswrap_banglejs_compassRd(0x3E,0));
}

#endif
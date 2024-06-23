#include "bangle_accel_impl.h"
#include "../cpu_hardware/bangle_cpu_hardware_impl.h"

void banglejs_accel_i2c_Wr(JsVarInt reg, JsVarInt data){
#ifdef ACCEL_I2C
  _jswrap_banglejs_i2cWr(ACCEL_I2C, ACCEL_ADDR, reg, data);
#endif
}
JsVar * banglejs_accel_i2c_Rd(JsVarInt reg, JsVarInt cnt)
{
#ifdef ACCEL_I2C
  return _jswrap_banglejs_i2cRd(ACCEL_I2C, ACCEL_ADDR, reg, cnt);
#else
  return 0;
#endif
}

ESPR_WEAK bool banglejs_accel_state_impl(unsigned char *tapped){return 0;}
ESPR_WEAK void banglejs_accel_get_pos_impl(short *x,short *y, short *z){};


ESPR_WEAK void banglejs_accel_init_impl(bool firstRun){}
ESPR_WEAK void  banglejs_accel_off_impl(){}
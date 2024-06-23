#include "jswrap_bangle_compass.h"
#include "../cpu_hardware/bangle_cpu_hardware_impl.h"
#include "bangle_state.h"

void    banglejs_compass_i2c_Wr(JsVarInt reg, JsVarInt data)
{
#ifdef MAG_I2C
  _jswrap_banglejs_i2cWr(MAG_I2C, MAG_ADDR, reg, data);
#endif
}


JsVar * banglejs_compass_i2c_Rd(JsVarInt reg, JsVarInt cnt)
{
#ifdef MAG_I2C
  return _jswrap_banglejs_i2cRd(MAG_I2C, MAG_ADDR, reg, cnt);
#else
  return 0;
#endif
}

void banglejs_compass_init_impl()
{
  banglejs_compass_device_init_impl();
  
  bangleFlags &= ~JSBF_COMPASS_ON;
  // ensure compass readings are reset to power-on state
  jswrap_banglejs_resetCompass();
}

ESPR_WEAK void banglejs_compass_device_init_impl(){};
ESPR_WEAK void banglejs_compass_off_impl(){};
ESPR_WEAK void banglejs_compass_on_impl(){};

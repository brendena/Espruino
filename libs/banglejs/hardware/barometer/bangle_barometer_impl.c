#include "jswrap_bangle_barometer.h"
#include "bangle_state.h"
#include "../cpu_hardware/bangle_cpu_hardware_impl.h"


void    banglejs_barometer_i2c_Wr(JsVarInt reg, JsVarInt data)
{
    #ifdef PRESSURE_I2C
    _jswrap_banglejs_i2cWr(PRESSURE_I2C, PRESSURE_ADDR, reg, data);
    #endif
}

JsVar * banglejs_barometer_i2c_Rd(JsVarInt reg, JsVarInt cnt)
{
#ifdef PRESSURE_I2C
  return _jswrap_banglejs_i2cRd(PRESSURE_I2C, PRESSURE_ADDR, reg, cnt);
#else
  return 0;
#endif
}

void banglejs_barometer_idle_impl(JsVar *bangle)
{
  if (bangleTasks & JSBT_PRESSURE_DATA) {
    JsVar *o = jswrap_banglejs_getBarometerObject();
    if (o) {
      jsiQueueObjectCallbacks(bangle, JS_EVENT_PREFIX"pressure", &o, 1);
      if (promisePressure) {
        // disable sensor now we have a result (if it was on for another reason, "getPressure" as ID ensures it'll still stay on)
        JsVar *id = jsvNewFromString("getPressure");
        jswrap_banglejs_setBarometerPower(0, id);
        jsvUnLock(id);
        // resolve the promise
        jspromise_resolve(promisePressure, o);
        jsvUnLock(promisePressure);
        promisePressure = 0;
      }
      jsvUnLock(o);
    }
  }
}



ESPR_WEAK void banglejs_barometer_on_impl(){};
ESPR_WEAK void banglejs_barometer_off_impl(){};
ESPR_WEAK bool banglejs_barometer_get_data_impl(double *altitude, double *pressure, double *temperature){};
ESPR_WEAK void banglejs_barometer_init_impl()
{
  banglejs_barometer_device_init_impl();
  bangleFlags &= ~JSBF_BAROMETER_ON;
};
ESPR_WEAK void banglejs_barometer_device_init_impl(){}; 
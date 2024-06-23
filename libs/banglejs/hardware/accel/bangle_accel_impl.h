#include "jswrap_bangle_accel.h"

void    banglejs_accel_i2c_Wr(JsVarInt reg, JsVarInt data);
JsVar * banglejs_accel_i2c_Rd(JsVarInt reg, JsVarInt cnt);


void banglejs_accel_init_impl(bool firstRun);
void banglejs_accel_off_impl();
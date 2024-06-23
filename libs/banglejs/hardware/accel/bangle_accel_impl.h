#include "jswrap_bangle_accel.h"

void    banglejs_accel_i2c_Wr(JsVarInt reg, JsVarInt data);
JsVar * banglejs_accel_i2c_Rd(JsVarInt reg, JsVarInt cnt);


bool banglejs_accel_state_impl(unsigned char *tapped);
void banglejs_accel_get_pos_impl(short *x,short *y, short *z);

void banglejs_accel_init_impl(bool firstRun);
void banglejs_accel_off_impl();
#pragma once
#include "bangle_defines.h"

void    banglejs_compass_i2c_Wr(JsVarInt reg, JsVarInt data);
JsVar * banglejs_compass_i2c_Rd(JsVarInt reg, JsVarInt cnt);




void banglejs_compass_init_impl();
void banglejs_compass_device_init_impl(); //overright for device specfic init
void banglejs_compass_off_impl();
void banglejs_compass_on_impl();
bool banglejs_compass_get_pos_impl(Vector3 *mag);
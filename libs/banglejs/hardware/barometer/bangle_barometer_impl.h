#pragma once


void    banglejs_barometer_i2c_Wr(JsVarInt reg, JsVarInt data);
JsVar * banglejs_barometer_i2c_Rd(JsVarInt reg, JsVarInt cnt);

bool banglejs_setBarometerPower(bool isOn, JsVar *appId);
void banglejs_barometer_on_impl();
void banglejs_barometer_off_impl();

void banglejs_barometer_init_impl();
void banglejs_barometer_device_init_impl(); //overright for device specfic init
void banglejs_barometer_idle_impl(JsVar *bangle);



bool banglejs_barometer_get_data_impl(double *altitude, double *pressure, double *temperature);
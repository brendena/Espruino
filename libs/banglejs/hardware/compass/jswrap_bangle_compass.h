#pragma once
#include "bangle_defines.h"
#include "bangle_compass_impl.h"

#define MAG_CHARGE_TIMEOUT 3000 // time after charging when magnetometer value gets automatically reset


void jswrap_banglejs_compassWr(JsVarInt reg, JsVarInt data);
JsVar * jswrap_banglejs_compassRd(JsVarInt reg, JsVarInt data);

int jswrap_banglejs_isCompassOn();
void jswrap_banglejs_resetCompass();
bool jswrap_banglejs_setCompassPower(bool isOn, JsVar *appId);
JsVar *jswrap_banglejs_getCompass();

// compass data
extern Vector3 mag, magmin, magmax;
extern bool magOnWhenCharging;

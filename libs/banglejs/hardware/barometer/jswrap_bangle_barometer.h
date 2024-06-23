#pragma once
#include "bangle_defines.h"
#include "bangle_barometer_impl.h"
#include "jswrap_promise.h"
#include "jsinteractive.h"
#include "jswrap_error.h"

void   jswrap_banglejs_barometerWr(JsVarInt reg, JsVarInt data);
JsVar *jswrap_banglejs_barometerRd(JsVarInt reg, JsVarInt cnt);

bool   jswrap_banglejs_setBarometerPower(bool isOn, JsVar *appId);
int    jswrap_banglejs_isBarometerOn();
bool   jswrap_banglejs_barometerPoll();

JsVar *jswrap_banglejs_getBarometerObject();

extern JsVar *promisePressure;
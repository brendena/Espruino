#pragma once
#include "bangle_defines.h"
#include "bangle_accel_impl.h"

void jswrap_banglejs_accelWr(JsVarInt reg, JsVarInt data);
JsVar * jswrap_banglejs_accelRd(JsVarInt reg, JsVarInt data);
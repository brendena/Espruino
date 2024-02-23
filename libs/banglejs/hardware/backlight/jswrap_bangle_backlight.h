#pragma once
#include "bangle_defines.h"
#include "bangle_backlight_impl.h"
#include "jswrap_bangle.h"
#include "jsparse.h"
#include "stdio.h"
#include "jsinteractive.h"
/*JS functions*/
void jswrap_banglejs_setLCDBrightness(JsVarFloat v);
void jswrap_banglejs_setLCDPowerBacklight(bool isOn);
int  jswrap_banglejs_isBacklightOn();



/*global state*/
extern uint8_t lcdBrightness;
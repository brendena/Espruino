#pragma once
#include "bangle_defines.h"
#include "jsparse.h"
#include "stdio.h"
#include "graphics.h"
#include "../backlight/bangle_backlight_impl.h"
#include "bangle_display_impl.h"
/*JS functions*/
void jswrap_banglejs_lcdWr(JsVarInt cmd, JsVar *data);
void jswrap_banglejs_setLCDPower(bool isOn);
void jswrap_banglejs_setLCDMode(JsVar *mode);
JsVar *jswrap_banglejs_getLCDMode();
void jswrap_banglejs_setLCDOffset(int y);
void jswrap_banglejs_setLCDOverlay(JsVar *imgVar, JsVar * xv, int y, JsVar *options);
int jswrap_banglejs_isLCDOn();


#include "bangle_defines.h"
#include "jsparse.h"
#include "bangle_time.h"
#include "stdio.h"
#include "jsinteractive.h"
#include ""

/*JS functions*/
void jswrap_banglejs_setLCDBrightness(JsVarFloat v);
void jswrap_banglejs_setLCDPowerBacklight(bool isOn);
int  jswrap_banglejs_isBacklightOn();
void jswrap_banglejs_pwrBacklight(bool on);

/*c funcs*/
void backlight_init();
void backlight_kill();
void backlight_idle();


/*global state*/
extern uint8_t lcdBrightness;
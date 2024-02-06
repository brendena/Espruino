#include "bangle_defines.h"
#include "jsparse.h"
#include "bangle_time.h"
#include "stdio.h"
#include "graphics.h"
#include "lcd_spi_generic.h"
#include "../backlight/bangle_backlight.h"
#include "../../jswrap_bangle.h"
/*JS functions*/
void jswrap_banglejs_lcdWr(JsVarInt cmd, JsVar *data);
void jswrap_banglejs_setLCDPower(bool isOn);
void jswrap_banglejs_setLCDMode(JsVar *mode);
JsVar *jswrap_banglejs_getLCDMode();
void jswrap_banglejs_setLCDOffset(int y);
void jswrap_banglejs_setLCDOverlay(JsVar *imgVar, int x, int y);
int jswrap_banglejs_isLCDOn();


/*c funcs*/
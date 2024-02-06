#ifdef BANGLEJS_P8

#include "bangle_backlight.h"
void jswrap_banglejs_pwrBacklight(bool on) {
  jshPinOutput(LCD_BL,!on);
}

#endif
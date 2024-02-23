#include "bangle_backlight_impl.h"


ESPR_WEAK void banglejs_setLCDPowerBacklight_impl(bool isOn) {
    banglejs_pwrBacklight_impl(isOn && (lcdBrightness>0));
    #ifdef LCD_BL
    if (isOn && lcdBrightness > 0 && lcdBrightness < 255) {
        jshPinAnalogOutput(LCD_BL, lcdBrightness/256.0, 200, JSAOF_NONE);
    }
    #endif
}


ESPR_WEAK void banglejs_pwrBacklight_impl(bool on){
#ifdef LCD_BL
    jshPinOutput(LCD_BL, on);
#endif
}

ESPR_WEAK void banglejs_backlight_init_impl(){};
ESPR_WEAK void banglejs_backlight_idle_impl(){};
ESPR_WEAK void banglejs_backlight_kill_impl(){};



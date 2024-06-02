#include "bangle_backlight_impl.h"
#ifndef EMULATED

ESPR_WEAK void banglejs_setLCDPowerBacklight_impl(bool isOn) {
    banglejs_pwrBacklight_impl(isOn && (lcdBrightness>0));

}


ESPR_WEAK void banglejs_pwrBacklight_impl(bool on){}


ESPR_WEAK void banglejs_backlight_init_impl(){};
ESPR_WEAK void banglejs_backlight_idle_impl(){};
ESPR_WEAK void banglejs_backlight_kill_impl(){};


#endif
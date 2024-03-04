#pragma once
#include "jswrap_bangle_backlight.h"
#include "bangle_state.h"

ESPR_EMULATE(void banglejs_setLCDPowerBacklight_impl(bool isOn));
ESPR_EMULATE(void banglejs_pwrBacklight_impl(bool on));

//state
ESPR_EMULATE(void banglejs_backlight_init_impl());
ESPR_EMULATE(void banglejs_backlight_idle_impl());
ESPR_EMULATE(void banglejs_backlight_kill_impl());
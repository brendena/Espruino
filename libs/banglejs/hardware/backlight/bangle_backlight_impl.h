#pragma once
#include "jswrap_bangle_backlight.h"
#include "bangle_state.h"

void banglejs_setLCDPowerBacklight_impl(bool isOn);
void banglejs_pwrBacklight_impl(bool on);

//state
void banglejs_backlight_init_impl();
void banglejs_backlight_idle_impl();
void banglejs_backlight_kill_impl();
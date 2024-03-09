#pragma once
#include "jswrap_bangle_display.h"
#include "bangle_state.h"

void banglejs_lcdWr_impl(int cmd, int dLen, const uint8_t *dPtr);
void banglejs_setLCDMode_impl(JsVar *mode);
char * banglejs_getLCDMode_impl();
void banglejs_setLCDOffset_impl(int y);
void banglejs_setLCDOverlay_impl(JsVar *imgVar, int x, int y);

void banglejs_setLCDPowerController_impl(bool isOn);

// This name needs to be this because a header library references it in a header
void graphicsInternalFlip();



void banglejs_display_init_impl(JsGraphics *gfx);
void banglejs_display_idle_impl();
void banglejs_display_kill_impl();

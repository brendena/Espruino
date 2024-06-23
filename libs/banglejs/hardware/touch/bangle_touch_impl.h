#pragma once
#include "jswrap_bangle_touch.h"
#include "jshardware.h"
#include "jsi2c.h"
#include "unistroke.h"

#ifdef EMULATED
extern void touchHandlerInternal(int tx, int ty, int pts, int gesture);
#endif

void banglejs_touch_hw_init_impl();
void banglejs_touch_init_impl();

#ifdef TOUCH_DEVICE
extern short touchMinX, touchMinY, touchMaxX, touchMaxY;
#endif

#ifdef TOUCH_I2C
void touchHandler(bool state, IOEventFlags flags);
#endif



void banglejs_touch_idle_impl(JsVar *bangle );
/*
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2019 Gordon Williams <gw@pur3.co.uk>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * Contains JavaScript interface for Bangle.js (http://www.espruino.com/Bangle.js)
 * ----------------------------------------------------------------------------
 */
#pragma once
#include "bangle_defines.h"
#include "jspin.h"
#include "hardware/backlight/jswrap_bangle_backlight.h"
#include "hardware/display/jswrap_bangle_display.h"
#include "hardware/touch/jswrap_bangle_touch.h"
#include "hardware/cpu_hardware/bangle_cpu_hardware_impl.h"
#include "hardware/accel/jswrap_bangle_accel.h"
#include "hardware/compass/jswrap_bangle_compass.h"
#include "hardware/barometer/jswrap_bangle_barometer.h"

void jswrap_banglejs_setLCDTimeout(JsVarFloat timeout);
void jswrap_banglejs_setLocked(bool isLocked);
int jswrap_banglejs_isLocked();

void jswrap_banglejs_setPollInterval(JsVarFloat interval);
void jswrap_banglejs_setOptions(JsVar *options);
JsVar *jswrap_banglejs_getOptions();


//charging
int jswrap_banglejs_isCharging();
JsVarInt jswrap_banglejs_getBattery();

//HRM
bool jswrap_banglejs_setHRMPower(bool isOn, JsVar *appId);
int jswrap_banglejs_isHRMOn();
void jswrap_banglejs_hrmWr(JsVarInt reg, JsVarInt data);
JsVar *jswrap_banglejs_hrmRd(JsVarInt reg, JsVarInt cnt);

//GPS
bool jswrap_banglejs_setGPSPower(bool isOn, JsVar *appId);
int jswrap_banglejs_isGPSOn();
JsVar *jswrap_banglejs_getGPSFix();

//Beep
void jswrap_banglejs_beep_callback(); // internal use only
JsVar *jswrap_banglejs_beep(int time, int freq);
void jswrap_banglejs_buzz_callback(); // internal use only
JsVar *jswrap_banglejs_buzz(int time, JsVarFloat amt);



int jswrap_banglejs_getStepCount();
void jswrap_banglejs_setStepCount(JsVarInt count);



JsVar *jswrap_banglejs_getHealthStatus();

JsVar *jswrap_banglejs_dbg();
void jswrap_banglejs_ioWr(JsVarInt mask, bool on);

JsVar *jswrap_banglejs_project(JsVar *latlong);


void jswrap_banglejs_off();
void jswrap_banglejs_softOff();
JsVar *jswrap_banglejs_getLogo();
void jswrap_banglejs_factoryReset(bool noReboot);

JsVar *jswrap_banglejs_appRect();

void jswrap_banglejs_hwinit();
void jswrap_banglejs_init();
void jswrap_banglejs_kill();
bool jswrap_banglejs_idle();
bool jswrap_banglejs_gps_character(char ch);

/* If we're busy and really don't want to be interrupted (eg clearing flash memory)
 then we should *NOT* allow the home button to set EXEC_INTERRUPTED (which happens
 if it was held, JSBT_RESET was set, and then 0.5s later it wasn't handled).
 */
void jswrap_banglejs_kickPollWatchdog();



/// Called from jsinteractive when an event is parsed from the event queue for Bangle.js (executed outside IRQ)
void jsbangle_exec_pending(IOEvent *event);
/// queue an event for Bangle.js (usually called from inside an IRQ)
void jsbangle_push_event(JsBangleEvent type, uint16_t value);


void jswrap_banglejs_powerusage(JsVar *devices);

//private bangle functions
void _jswrap_banglejs_setLocked(bool isLocked, const char *reason);

extern JsBangleTasks bangleTasks;


bool setDeviceRequested(const char *deviceName, JsVar *appID, bool powerOn);
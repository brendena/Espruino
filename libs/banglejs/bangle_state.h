#pragma once
/*
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2024 Gordon Williams <gw@pur3.co.uk>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * Contains all the universal state for the Bangle.js. This is state that 
 * is not owned by a specific driver but is needed for the whole system to
 * function.  These values are currently being constructed in jswrap_bangle.c . 
 * ----------------------------------------------------------------------------
 */
#include "bangle_defines.h"

extern volatile JsBangleFlags bangleFlags;
extern JsBangleTasks bangleTasks;
extern bool i2cBusy;

extern int backlightTimeout;
extern volatile uint16_t inactivityTimer; // in ms
extern int lockTimeout; // in ms
extern int lcdPowerTimeout; // in ms
extern volatile uint16_t pollInterval; // in ms
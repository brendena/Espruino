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
 * Contains all the defines for Bangle.js
 * ----------------------------------------------------------------------------
 */

#pragma once
#include "platform_config.h"
#include "hardware/devices/device_specific_headers.h"
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "jstypes.h"
#include "jshardware.h"
#include "jswrap_arraybuffer.h"
#ifndef EMULATED
  #include "app_timer.h"
#endif

typedef enum {
  JSBF_NONE,
  JSBF_WAKEON_FACEUP = 1<<0,
  JSBF_WAKEON_BTN1   = 1<<1,
  JSBF_WAKEON_BTN2   = 1<<2,
  JSBF_WAKEON_BTN3   = 1<<3,
  JSBF_WAKEON_TOUCH  = 1<<4,
  JSBF_WAKEON_DBLTAP = 1<<5,
  JSBF_WAKEON_TWIST  = 1<<6,
  JSBF_BEEP_VIBRATE  = 1<<7, // use vibration motor for beep
  JSBF_ENABLE_BEEP   = 1<<8,
  JSBF_ENABLE_BUZZ   = 1<<9,
  JSBF_ACCEL_LISTENER = 1<<10, ///< we have a listener for accelerometer data
  JSBF_POWER_SAVE    = 1<<11, ///< if no movement detected for a while, lower the accelerometer poll interval
  JSBF_HRM_ON        = 1<<12,
  JSBF_GPS_ON        = 1<<13,
  JSBF_COMPASS_ON    = 1<<14,
  JSBF_BAROMETER_ON  = 1<<15,
  JSBF_LCD_ON        = 1<<16,
  JSBF_LCD_BL_ON     = 1<<17,
  JSBF_LOCKED        = 1<<18,
  JSBF_HRM_INSTANT_LISTENER = 1<<19,
  JSBF_LCD_DBL_REFRESH = 1<<20, ///< On Bangle.js 2, toggle extcomin twice for each poll interval (avoids screen 'flashing' behaviour off axis)
#ifdef BANGLEJS_Q3
  /** On some Bangle.js 2, BTN1 (which is used for reloading apps) gets a low resistance across it
  (possibly due to water damage) and the internal resistor can no longer overcome that resistance
  so the button appears stuck on. With this fix we force the button pin low just before reading to try
  and overcome that resistance, and we also disable the button watch interrupt. */
  JSBF_BTN_LOW_RESISTANCE_FIX = 1<<21,
#endif
  JSBF_DEFAULT = ///< default at power-on
      JSBF_WAKEON_TWIST|
      JSBF_WAKEON_BTN1|JSBF_WAKEON_BTN2|JSBF_WAKEON_BTN3
} JsBangleFlags;

typedef enum {
  JSBT_NONE,
  JSBT_RESET = 1<<0, ///< reset the watch and reload code from flash
  JSBT_LCD_ON = 1<<1, ///< LCD controller (can turn this on without the backlight)
  JSBT_LCD_OFF = 1<<2,
  JSBT_LCD_BL_ON = 1<<3, ///< LCD backlight
  JSBT_LCD_BL_OFF = 1<<4,
  JSBT_LOCK = 1<<5, ///< watch is locked
  JSBT_UNLOCK = 1<<6, ///< watch is unlocked
  JSBT_ACCEL_DATA = 1<<7, ///< need to push xyz data to JS
  JSBT_ACCEL_TAPPED = 1<<8, ///< tap event detected
#ifdef GPS_PIN_RX
  JSBT_GPS_DATA = 1<<9, ///< we got a complete set of GPS data in 'gpsFix'
  JSBT_GPS_DATA_LINE = 1<<10, ///< we got a line of GPS data
  JSBT_GPS_DATA_PARTIAL = 1<<11, ///< we got some GPS data but it needs storing for later because it was too big to go in our buffer
  JSBT_GPS_DATA_OVERFLOW = 1<<12, ///< we got more GPS data than we could handle and had to drop some
#endif
#ifdef PRESSURE_DEVICE
  JSBT_PRESSURE_DATA = 1<<13,
#endif
  JSBT_MAG_DATA = 1<<14, ///< need to push magnetometer data to JS
  JSBT_GESTURE_DATA = 1<<15, ///< we have data from a gesture
  JSBT_HRM_DATA = 1<<16, ///< Heart rate data is ready for analysis
  JSBT_CHARGE_EVENT = 1<<17, ///< we need to fire a charging event
  JSBT_STEP_EVENT = 1<<18, ///< we've detected a step via the pedometer
  JSBT_SWIPE = 1<<19, ///< swiped over touchscreen, info in touchGesture
  JSBT_TOUCH_LEFT = 1<<20, ///< touch lhs of touchscreen
  JSBT_TOUCH_RIGHT = 1<<21, ///< touch rhs of touchscreen
  JSBT_TOUCH_MASK = JSBT_TOUCH_LEFT | JSBT_TOUCH_RIGHT,
#ifdef TOUCH_DEVICE
  JSBT_DRAG = 1<<22,
#endif
#if ESPR_BANGLE_UNISTROKE
  JSBT_STROKE = 1<<23, // a gesture has been made on the touchscreen
#endif
  JSBT_TWIST_EVENT = 1<<24, ///< Watch was twisted
  JSBT_FACE_UP = 1<<25, ///< Watch was turned face up/down (faceUp holds the actual state)
  JSBT_ACCEL_INTERVAL_DEFAULT = 1<<26, ///< reschedule accelerometer poll handler to default speed
  JSBT_ACCEL_INTERVAL_POWERSAVE = 1<<27, ///< reschedule accelerometer poll handler to powersave speed
  JSBT_HRM_INSTANT_DATA = 1<<28, ///< Instant heart rate data
  JSBT_HEALTH = 1<<29, ///< New 'health' event
  JSBT_MIDNIGHT = 1<<30, ///< Fired at midnight each day - for housekeeping tasks
} JsBangleTasks;


/// What state was the touchscreen last in
typedef enum {
  TS_NONE = 0,
  TS_LEFT = 1,
  TS_RIGHT = 2,
  TS_BOTH = 3,
  TS_SWIPED = 4
} TouchState;


/// Struct with currently tracked health info
typedef struct {
  uint8_t index; ///< time_in_ms / HEALTH_INTERVAL - we fire a new Health event when this changes
  uint32_t movement; ///< total accelerometer difference. Used for activity tracking.
  uint16_t movementSamples; ///< Number of samples added to movement
  uint16_t stepCount; ///< steps during current period
  uint16_t bpm10;  ///< beats per minute (x10)
  uint8_t bpmConfidence; ///< confidence of current BPM figure
} HealthState;

// Used when pushing events/retrieving events from the event queue
typedef enum {
  JSBE_HRM_ENV, // new HRM environment reading
} JsBangleEvent;


typedef enum {
  TG_SWIPE_NONE,
  TG_SWIPE_LEFT,
  TG_SWIPE_RIGHT,
  TG_SWIPE_UP,
  TG_SWIPE_DOWN,
} TouchGestureType;


typedef enum {
  ESPR_CMD_I2C_WRITE = 1,
  ESPR_CMD_I2C_MULTI_BYTES,
  ESPR_CMD_I2C_SLEEP,
} ESPR_CMD_I2C_TYPES;

typedef struct ESPR_CMD_I2C{
  ESPR_CMD_I2C_TYPES type;
  uint8_t reg;
  uint32_t data;
}ESPR_cmd_i2c;

#define SIZE_OF_CMD_I2C(arr) (sizeof(arr)/sizeof(ESPR_cmd_i2c))


#define ESPR_WEAK __attribute__((weak))

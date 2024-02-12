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
 * This file is designed to be parsed during the build process
 *
 * Contains JavaScript interface for Bangle.js (http://www.espruino.com/Bangle.js)
 * ----------------------------------------------------------------------------
 */

#include <jswrap_bangle.h>
#include "jsinteractive.h"
#include "jsdevices.h"
#include "jsnative.h"
#include "jshardware.h"
#include "jsdevices.h"
#include "jspin.h"
#include "jstimer.h"
#include "jswrap_promise.h"
#include "jswrap_date.h"
#include "jswrap_math.h"
#include "jswrap_storage.h"
#include "jswrap_array.h"
#include "jswrap_arraybuffer.h"
#include "jswrap_heatshrink.h"
#include "jswrap_espruino.h"
#include "jswrap_terminal.h"
#include "jsflash.h"
#include "graphics.h"
#include "bitmap_font_6x8.h"
#include "jswrap_bluetooth.h"
#include "app_timer.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_soc.h"
#include "nrf_saadc.h"
#include "nrf5x_utils.h"

#include "bluetooth.h" // for self-test
#include "jsi2c.h" // accelerometer/etc

#include "hardware/display/bangle_lcd.h"
#include "bangle_defines.h"
#include "bangle_time.h"


/*TYPESCRIPT
declare const BTN1: Pin;
declare const BTN2: Pin;
declare const BTN3: Pin;
declare const BTN4: Pin;
declare const BTN5: Pin;

declare const g: Graphics<false>;

type WidgetArea = "tl" | "tr" | "bl" | "br";
type Widget = {
  area: WidgetArea;
  width: number;
  sortorder?: number;
  draw: (this: Widget, w: Widget) => void;
  x?: number;
  y?: number;
};
declare const WIDGETS: { [key: string]: Widget };
*/

/*JSON{
  "type": "class",
  "class" : "Bangle",
  "ifdef" : "BANGLEJS"
}
Class containing utility functions for the [Bangle.js Smart
Watch](http://www.espruino.com/Bangle.js)
*/
/*TYPESCRIPT{
  "class" : "Bangle"
}
static CLOCK: ShortBoolean;
static strokes: undefined | { [key: string]: Unistroke };
*/
/*TYPESCRIPT
type ShortBoolean = boolean | 0 | 1;
*/


/*JSON{
  "type" : "variable",
  "name" : "VIBRATE",
  "generate_full" : "VIBRATE_PIN",
  "ifdef" : "BANGLEJS",
  "return" : ["pin",""]
}
The Bangle.js's vibration motor.
*/

/*TYPESCRIPT
type AccelData = {
  x: number;
  y: number;
  z: number;
  diff: number;
  mag: number;
};
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "accel",
  "params" : [["xyz","JsVar",""]],
  "ifdef" : "BANGLEJS",
  "typescript": "on(event: \"accel\", callback: (xyz: AccelData) => void): void;"
}
Accelerometer data available with `{x,y,z,diff,mag}` object as a parameter.

* `x` is X axis (left-right) in `g`
* `y` is Y axis (up-down) in `g`
* `z` is Z axis (in-out) in `g`
* `diff` is difference between this and the last reading in `g`
* `mag` is the magnitude of the acceleration in `g`

You can also retrieve the most recent reading with `Bangle.getAccel()`.
 */
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "step",
  "params" : [["up","int","The number of steps since Bangle.js was last reset"]],
  "ifdef" : "BANGLEJS"
}
Called whenever a step is detected by Bangle.js's pedometer.
 */
/*TYPESCRIPT
type HealthStatus = {
  movement: number;
  steps: number;
  bpm: number;
  bpmConfidence: number;
};
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "health",
  "params" : [["info","JsVar","An object containing the last 10 minutes health data"]],
  "ifdef" : "BANGLEJS",
  "typescript" : "on(event: \"health\", callback: (info: HealthStatus) => void): void;"
}
See `Bangle.getHealthStatus()` for more information. This is used for health
tracking to allow Bangle.js to record historical exercise data.
 */
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "faceUp",
  "params" : [["up","bool","`true` if face-up"]],
  "ifdef" : "BANGLEJS"
}
Has the watch been moved so that it is face-up, or not face up?
 */
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "twist",
  "ifdef" : "BANGLEJS"
}
This event happens when the watch has been twisted around it's axis - for
instance as if it was rotated so someone could look at the time.

To tweak when this happens, see the `twist*` options in `Bangle.setOptions()`
 */
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "charging",
  "params" : [["charging","bool","`true` if charging"]],
  "ifdef" : "BANGLEJS"
}
Is the battery charging or not?
 */
/*TYPESCRIPT
type CompassData = {
  x: number;
  y: number;
  z: number;
  dx: number;
  dy: number;
  dz: number;
  heading: number;
};
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "mag",
  "params" : [["xyz","JsVar",""]],
  "ifdef" : "BANGLEJS",
  "typescript" : "on(event: \"mag\", callback: (xyz: CompassData) => void): void;"
}
Magnetometer/Compass data available with `{x,y,z,dx,dy,dz,heading}` object as a
parameter

* `x/y/z` raw x,y,z magnetometer readings
* `dx/dy/dz` readings based on calibration since magnetometer turned on
* `heading` in degrees based on calibrated readings (will be NaN if magnetometer
  hasn't been rotated around 360 degrees).

**Note:** In 2v15 firmware and earlier the heading is inverted (360-heading). There's
a fix in the bootloader which will apply a fix for those headings, but old apps may
still expect an inverted value.

To get this event you must turn the compass on with `Bangle.setCompassPower(1)`.

You can also retrieve the most recent reading with `Bangle.getCompass()`.
 */
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "GPS-raw",
  "params" : [
     ["nmea","JsVar","A string containing the raw NMEA data from the GPS"],
     ["dataLoss","bool","This is set to true if some lines of GPS data have previously been lost (eg because system was too busy to queue up a GPS-raw event)"]
  ],
  "ifdef" : "BANGLEJS",
  "typescript" : "on(event: \"GPS-raw\", callback: (nmea: string, dataLoss: boolean) => void): void;"
}
Raw NMEA GPS / u-blox data messages received as a string

To get this event you must turn the GPS on with `Bangle.setGPSPower(1)`.
 */
/*TYPESCRIPT
type GPSFix = {
  lat: number;
  lon: number;
  alt: number;
  speed: number;
  course: number;
  time: Date;
  satellites: number;
  fix: number;
  hdop: number
};
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "GPS",
  "params" : [["fix","JsVar","An object with fix info (see below)"]],
  "ifdef" : "BANGLEJS",
  "typescript" : "on(event: \"GPS\", callback: (fix: GPSFix) => void): void;"
}
GPS data, as an object. Contains:

```
{ "lat": number,      // Latitude in degrees
  "lon": number,      // Longitude in degrees
  "alt": number,      // altitude in M
  "speed": number,    // Speed in kph
  "course": number,   // Course in degrees
  "time": Date,       // Current Time (or undefined if not known)
  "satellites": 7,    // Number of satellites
  "fix": 1            // NMEA Fix state - 0 is no fix
  "hdop": number,     // Horizontal Dilution of Precision
}
```

If a value such as `lat` is not known because there is no fix, it'll be `NaN`.

`hdop` is a value from the GPS receiver that gives a rough idea of accuracy of
lat/lon based on the geometry of the satellites in range. Multiply by 5 to get a
value in meters. This is just a ballpark estimation and should not be considered
remotely accurate.

To get this event you must turn the GPS on with `Bangle.setGPSPower(1)`.
 */
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "HRM",
  "params" : [["hrm","JsVar","An object with heart rate info (see below)"]],
  "ifdef" : "BANGLEJS",
  "typescript" : "on(event: \"HRM\", callback: (hrm: { bpm: number, confidence: number, raw: Uint8Array }) => void): void;"
}
Heat rate data, as an object. Contains:

```
{ "bpm": number,             // Beats per minute
  "confidence": number,      // 0-100 percentage confidence in the heart rate
  "raw": Uint8Array,         // raw samples from heart rate monitor
}
```

To get this event you must turn the heart rate monitor on with
`Bangle.setHRMPower(1)`.
 */
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "HRM-raw",
  "params" : [["hrm","JsVar","A object containing instant readings from the heart rate sensor"]],
  "ifdef" : "BANGLEJS",
  "typescript" : "on(event: \"HRM-raw\", callback: (hrm: { raw: number, filt: number, bpm: number, confidence: number }) => void): void;"
}
Called when heart rate sensor data is available - see `Bangle.setHRMPower(1)`.

`hrm` is of the form:

```
{ "raw": -1,       // raw value from sensor
  "filt": -1,      // bandpass-filtered raw value from sensor
  "bpm": 88.9,     // last BPM value measured
  "confidence": 0  // confidence in the BPM value
}
```
 */
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "HRM-env",
  "params" : [["env","JsVar","An integer containing current environment reading (light level)"]],
  "ifdef" : "BANGLEJS2"
}
Called when an environment sample heart rate sensor data is available (this is the amount of light received by the HRM sensor from the environment when its LED is off). On the newest VC31B based watches this is only 4 bit (0..15).

To get it you need to turn the HRM on with `Bangle.setHRMPower(1)` and also set `Bangle.setOptions({hrmPushEnv:true})`.

It is also possible to poke registers with `Bangle.hrmWr` to increase the poll rate if needed. See https://banglejs.com/apps/?id=flashcount for an example of this.

 */
/*TYPESCRIPT
type PressureData = {
  temperature: number;
  pressure: number;
  altitude: number;
}
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "pressure",
  "params" : [["e","JsVar","An object containing `{temperature,pressure,altitude}`"]],
  "ifdef" : "BANGLEJS",
  "typescript" : "on(event: \"pressure\", callback: (e: PressureData) => void): void;"
}
When `Bangle.setBarometerPower(true)` is called, this event is fired containing
barometer readings.

Same format as `Bangle.getPressure()`
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "lcdPower",
  "params" : [["on","bool","`true` if screen is on"]],
  "ifdef" : "BANGLEJS"
}
Has the screen been turned on or off? Can be used to stop tasks that are no
longer useful if nothing is displayed. Also see `Bangle.isLCDOn()`
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "backlight",
  "params" : [["on","bool","`true` if backlight is on"]],
  "ifdef" : "BANGLEJS"
}
Has the backlight been turned on or off? Can be used to stop tasks that are no
longer useful if want to see in sun screen only. Also see `Bangle.isBacklightOn()`
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "lock",
  "params" : [
    ["on","bool","`true` if screen is locked, `false` if it is unlocked and touchscreen/buttons will work"],
    ["reason","string","(2v20 onwards) If known, the reason for locking/unlocking - 'button','js','tap','doubleTap','faceUp','twist','timeout'"]
    ],
  "ifdef" : "BANGLEJS"
}
Has the screen been locked? Also see `Bangle.isLocked()`
*/
/*TYPESCRIPT
type TapAxis = -2 | -1 | 0 | 1 | 2;
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "tap",
  "params" : [["data","JsVar","`{dir, double, x, y, z}`"]],
  "ifdef" : "BANGLEJS",
  "typescript" : "on(event: \"tap\", callback: (data: { dir: \"left\" | \"right\" | \"top\" | \"bottom\" | \"front\" | \"back\", double: boolean, x: TapAxis, y: TapAxis, z: TapAxis }) => void): void;"
}
If the watch is tapped, this event contains information on the way it was
tapped.

`dir` reports the side of the watch that was tapped (not the direction it was
tapped in).

```
{
  dir : "left/right/top/bottom/front/back",
  double : true/false // was this a double-tap?
  x : -2 .. 2, // the axis of the tap
  y : -2 .. 2, // the axis of the tap
  z : -2 .. 2 // the axis of the tap
```
 */
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "gesture",
  "params" : [["xyz","JsVar","An Int8Array of XYZXYZXYZ data"]],
  "ifdef" : "BANGLEJS",
  "typescript" : "on(event: \"gesture\", callback: (xyz: Int8Array) => void): void;"
}
Emitted when a 'gesture' (fast movement) is detected
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "aiGesture",
  "params" : [["gesture","JsVar","The name of the gesture (if '.tfnames' exists, or the index. 'undefined' if not matching"],
              ["weights","JsVar","An array of floating point values output by the model"]],
  "ifdef" : "BANGLEJS",
  "typescript" : "on(event: \"aiGesture\", callback: (gesture: string | undefined, weights: number[]) => void): void;"
}
Emitted when a 'gesture' (fast movement) is detected, and a Tensorflow model is
in storage in the `".tfmodel"` file.

If a `".tfnames"` file is specified as a comma-separated list of names, it will
be used to decode `gesture` from a number into a string.
*/
/*TYPESCRIPT
type SwipeCallback = (directionLR: -1 | 0 | 1, directionUD?: -1 | 0 | 1) => void;
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "swipe",
  "params" : [["directionLR","int","`-1` for left, `1` for right, `0` for up/down"],
              ["directionUD","int","`-1` for up, `1` for down, `0` for left/right (Bangle.js 2 only)"]],
  "ifdef" : "BANGLEJS",
  "typescript" : "on(event: \"swipe\", callback: SwipeCallback): void;"
}
Emitted when a swipe on the touchscreen is detected (a movement from
left->right, right->left, down->up or up->down)

Bangle.js 1 is only capable of detecting left/right swipes as it only contains a
2 zone touchscreen.
*/
/*TYPESCRIPT
type TouchCallback = (button: number, xy?: { x: number, y: number }) => void;
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "touch",
  "params" : [
    ["button","int","`1` for left, `2` for right"],
    ["xy","JsVar","Object of form `{x,y,type}` containing touch coordinates (if the device supports full touch). Clipped to 0..175 (LCD pixel coordinates) on firmware 2v13 and later.`type` is only available on Bangle.js 2 and is an integer, either 0 for swift touches or 2 for longer ones."]
  ],
  "ifdef" : "BANGLEJS",
  "typescript" : "on(event: \"touch\", callback: TouchCallback): void;"
}
Emitted when the touchscreen is pressed
*/
/*TYPESCRIPT
type DragCallback = (event: {
  x: number;
  y: number;
  dx: number;
  dy: number;
  b: 1 | 0;
}) => void;
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "drag",
  "params" : [["event","JsVar","Object of form `{x,y,dx,dy,b}` containing touch coordinates, difference in touch coordinates, and an integer `b` containing number of touch points (currently 1 or 0)"]],
  "ifdef" : "BANGLEJS",
  "typescript" : "on(event: \"drag\", callback: DragCallback): void;"
}
Emitted when the touchscreen is dragged or released

The touchscreen extends past the edge of the screen and while `x` and `y`
coordinates are arranged such that they align with the LCD's pixels, if your
finger goes towards the edge of the screen, `x` and `y` could end up larger than
175 (the screen's maximum pixel coordinates) or smaller than 0. Coordinates from
the `touch` event are clipped.
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "stroke",
  "params" : [["event","JsVar","Object of form `{xy:Uint8Array([x1,y1,x2,y2...])}` containing touch coordinates"]],
  "ifdef" : "BANGLEJS_Q3",
  "typescript" : "on(event: \"stroke\", callback: (event: { xy: Uint8Array, stroke?: string }) => void): void;"
}
Emitted when the touchscreen is dragged for a large enough distance to count as
a gesture.

If Bangle.strokes is defined and populated with data from `Unistroke.new`, the
`event` argument will also contain a `stroke` field containing the most closely
matching stroke name.

For example:

```
Bangle.strokes = {
  up : Unistroke.new(new Uint8Array([57, 151, ... 158, 137])),
  alpha : Unistroke.new(new Uint8Array([161, 55, ... 159, 161])),
};
Bangle.on('stroke',o=>{
  print(o.stroke);
  g.clear(1).drawPoly(o.xy);
});
// Might print something like
{
  "xy": new Uint8Array([149, 50, ... 107, 136]),
  "stroke": "alpha"
}
```
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "midnight",
  "ifdef" : "BANGLEJS"
}
Emitted at midnight (at the point the `day` health info is reset to 0).

Can be used for housekeeping tasks that don't want to be run during the day.
*/

#define ACCEL_HISTORY_LEN 50 ///< Number of samples of accelerometer history



#if HOME_BTN==1
#define HOME_BTN_PININDEX    BTN1_PININDEX
#endif
#if HOME_BTN==2
#define HOME_BTN_PININDEX    BTN2_PININDEX
#endif
#if HOME_BTN==3
#define HOME_BTN_PININDEX    BTN3_PININDEX
#endif
#if HOME_BTN==4
#define HOME_BTN_PININDEX    BTN4_PININDEX
#endif
#if HOME_BTN==5
#define HOME_BTN_PININDEX    BTN5_PININDEX
#endif
// =========================================================================


volatile JsBangleFlags bangleFlags = JSBF_NONE;
JsBangleTasks bangleTasks;

/// Nordic app timer to handle call of peripheralPollHandler
APP_TIMER_DEF(m_peripheral_poll_timer_id);



const char *lockReason = 0; ///< If JSBT_LOCK/UNLOCK is set, this is the reason (if known) - should point to a constant string (not on stack!)

void jswrap_banglejs_pwrGPS(bool on) {

}

void jswrap_banglejs_pwrHRM(bool on) {

}



void graphicsInternalFlip() {
}

/// Flip buffer contents with the screen.
void lcd_flip(JsVar *parent, bool all) {
}



// Called when the Bangle should be woken up. Returns true if we did wake up (event was handled)
bool wakeUpBangle(const char *reason) {
  return 0;
}

/** This is called to set whether an app requests a device to be on or off.
 * The value returned is whether the device should be on.
 * Devices: GPS/Compass/HRM/Barom
 */
bool setDeviceRequested(const char *deviceName, JsVar *appID, bool powerOn) {
  return 0;
}
// Check whether a specific device has been requested to be on or not
bool getDeviceRequested(const char *deviceName) {
  return 0;
}

void jswrap_banglejs_setPollInterval_internal(uint16_t msec) {

}

/* If we're busy and really don't want to be interrupted (eg clearing flash memory)
 then we should *NOT* allow the home button to set EXEC_INTERRUPTED (which happens
 if it was held, JSBT_RESET was set, and then 0.5s later it wasn't handled).
 */
void jswrap_banglejs_kickPollWatchdog() {
}

/* Scan peripherals for any data that's needed
 * Also, holding down both buttons will reboot */
void peripheralPollHandler() {

}



void btnHandlerCommon(int button, bool state, IOEventFlags flags) {

}

// returns true if handled and shouldn't create a normal watch event
bool btnTouchHandler() {

  return false;
}

void btn1Handler(bool state, IOEventFlags flags) {
  btnHandlerCommon(1,state,flags);
}
#ifdef BTN2_PININDEX
void btn2Handler(bool state, IOEventFlags flags) {
  btnHandlerCommon(2,state,flags);
}
#endif
#ifdef BTN3_PININDEX
void btn3Handler(bool state, IOEventFlags flags) {
  btnHandlerCommon(3,state,flags);
}
#endif
#if defined(BANGLEJS_F18)
void btn4Handler(bool state, IOEventFlags flags) {
  if (btnTouchHandler()) return;
  btnHandlerCommon(4,state,flags);
}
void btn5Handler(bool state, IOEventFlags flags) {
  if (btnTouchHandler()) return;
  btnHandlerCommon(5,state,flags);
}
#else
void btn4Handler(bool state, IOEventFlags flags) {
  btnHandlerCommon(4,state,flags);
}
#endif



void touchHandlerInternal(int tx, int ty, int pts, int gesture) {

}


void touchHandler(bool state, IOEventFlags flags) {
 
}





/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "setLCDPower",
    "generate" : "jswrap_banglejs_setLCDPower",
    "params" : [
      ["isOn","bool","True if the LCD should be on, false if not"]
    ],
    "ifdef" : "BANGLEJS"
}
This function can be used to turn Bangle.js's LCD off or on.

This function resets the Bangle's 'activity timer' (like pressing a button or
the screen would) so after a time period of inactivity set by
`Bangle.setLCDTimeout` the screen will turn off.

If you want to keep the screen on permanently (until apps are changed) you can
do:

```
Bangle.setLCDTimeout(0); // turn off the timeout
Bangle.setLCDPower(1); // keep screen on
```


**When on full, the LCD draws roughly 40mA.** You can adjust When brightness
using `Bangle.setLCDBrightness`.
*/
void jswrap_banglejs_setLCDPower(bool isOn) {
}








/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "setLCDTimeout",
    "generate" : "jswrap_banglejs_setLCDTimeout",
    "params" : [
      ["isOn","float","The timeout of the display in seconds, or `0`/`undefined` to turn power saving off. Default is 10 seconds."]
    ],
    "ifdef" : "BANGLEJS"
}
This function can be used to turn Bangle.js's LCD power saving on or off.

With power saving off, the display will remain in the state you set it with
`Bangle.setLCDPower`.

With power saving on, the display will turn on if a button is pressed, the watch
is turned face up, or the screen is updated (see `Bangle.setOptions` for
configuration). It'll turn off automatically after the given timeout.

**Note:** This function also sets the Backlight and Lock timeout (the time at
which the touchscreen/buttons start being ignored). To set both separately, use
`Bangle.setOptions`
*/
void jswrap_banglejs_setLCDTimeout(JsVarFloat timeout) {
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "setPollInterval",
    "generate" : "jswrap_banglejs_setPollInterval",
    "params" : [
      ["interval","float","Polling interval in milliseconds (Default is 80ms - 12.5Hz to match accelerometer)"]
    ],
    "ifdef" : "BANGLEJS"
}
Set how often the watch should poll its sensors (accel/hr/mag) for new data and kick the
Watchdog timer. It isn't recommended that you make this interval much larger
than 1000ms, but values up to 4000ms are allowed.

Calling this will set `Bangle.setOptions({powerSave: false})` - disabling the
dynamic adjustment of poll interval to save battery power when Bangle.js is
stationary.
*/
void jswrap_banglejs_setPollInterval(JsVarFloat interval) {
}

/*TYPESCRIPT
type BangleOptions<Boolean = boolean> = {
  wakeOnBTN1: Boolean;
  wakeOnBTN2: Boolean;
  wakeOnBTN3: Boolean;
  wakeOnFaceUp: Boolean;
  wakeOnTouch: Boolean;
  wakeOnDoubleTap: Boolean;
  wakeOnTwist: Boolean;
  twistThreshold: number;
  twistMaxY: number;
  twistTimeout: number;
  gestureStartThresh: number;
  gestureEndThresh: number;
  gestureInactiveCount: number;
  gestureMinLength: number;
  powerSave: boolean;
  lockTimeout: number;
  lcdPowerTimeout: number;
  backlightTimeout: number;
  btnLoadTimeout: number;
};
*/
/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "setOptions",
    "generate" : "jswrap_banglejs_setOptions",
    "params" : [
      ["options","JsVar",""]
    ],
    "ifdef" : "BANGLEJS",
    "typescript" : "setOptions(options: { [key in keyof BangleOptions]?: BangleOptions<ShortBoolean>[key] }): void;"
}
Set internal options used for gestures, etc...

* `wakeOnBTN1` should the LCD turn on when BTN1 is pressed? default = `true`
* `wakeOnBTN2` (Bangle.js 1) should the LCD turn on when BTN2 is pressed?
  default = `true`
* `wakeOnBTN3` (Bangle.js 1) should the LCD turn on when BTN3 is pressed?
  default = `true`
* `wakeOnFaceUp` should the LCD turn on when the watch is turned face up?
  default = `false`
* `wakeOnTouch` should the LCD turn on when the touchscreen is pressed? On Bangle.js 1 this
is a physical press on the touchscreen, on Bangle.js 2 we have to use the accelerometer as
the touchscreen cannot be left powered without running the battery down. default = `false`
* `wakeOnDoubleTap` (2v20 onwards) should the LCD turn on when the watch is double-tapped on the screen?
This uses the accelerometer, not the touchscreen itself. default = `false`
* `wakeOnTwist` should the LCD turn on when the watch is twisted? default =
  `true`
* `twistThreshold` How much acceleration to register a twist of the watch strap?
  Can be negative for opposite direction. default = `800`
* `twistMaxY` Maximum acceleration in Y to trigger a twist (low Y means watch is
  facing the right way up). default = `-800`
* `twistTimeout` How little time (in ms) must a twist take from low->high
  acceleration? default = `1000`
* `gestureStartThresh` how big a difference before we consider a gesture
  started? default = `sqr(800)`
* `gestureEndThresh` how small a difference before we consider a gesture ended?
  default = `sqr(2000)`
* `gestureInactiveCount` how many samples do we keep after a gesture has ended?
  default = `4`
* `gestureMinLength` how many samples must a gesture have before we notify about
  it? default = `10`
* `powerSave` after a minute of not being moved, Bangle.js will change the
   accelerometer poll interval down to 800ms (10x accelerometer samples). On
   movement it'll be raised to the default 80ms. If `Bangle.setPollInterval` is
   used this is disabled, and for it to work the poll interval must be either
   80ms or 800ms. default = `true`. Setting `powerSave:false` will disable this
   automatic power saving, but will **not** change the poll interval from its
   current value. If you desire a specific interval (e.g. the default 80ms) you
   must set it manually with `Bangle.setPollInterval(80)` after setting
   `powerSave:false`.
* `lockTimeout` how many milliseconds before the screen locks
* `lcdPowerTimeout` how many milliseconds before the screen turns off
* `backlightTimeout` how many milliseconds before the screen's backlight turns
  off
* `btnLoadTimeout` how many milliseconds does the home button have to be pressed
for before the clock is reloaded? 1500ms default, or 0 means never.
* `hrmPollInterval` set the requested poll interval (in milliseconds) for the
  heart rate monitor. On Bangle.js 2 only 10,20,40,80,160,200 ms are supported,
  and polling rate may not be exact. The algorithm's filtering is tuned for
  20-40ms poll intervals, so higher/lower intervals may effect the reliability
  of the BPM reading. You must call this *before* `Bangle.setHRMPower` - calling
  when the HRM is already on will not affect the poll rate.
* `hrmSportMode` - on the newest Bangle.js 2 builds with with the proprietary
  heart rate algorithm, this is the sport mode passed to the algorithm. See `libs/misc/vc31_binary/algo.h`
  for more info. -1 = auto, 0 = normal (default), 1 = running, 2 = ...
* `hrmGreenAdjust` - (Bangle.js 2, 2v19+) if false (default is true) the green LED intensity won't be adjusted to get the HRM sensor 'exposure' correct. This is reset when the HRM is initialised with `Bangle.setHRMPower`.
* `hrmWearDetect` - (Bangle.js 2, 2v19+) if false (default is true) HRM readings won't be turned off if the watch isn't on your arm (based on HRM proximity sensor). This is reset when the HRM is initialised with `Bangle.setHRMPower`.
* `hrmPushEnv` - (Bangle.js 2, 2v19+) if true (default is false) HRM environment readings will be produced as `Bangle.on(`HRM-env`, ...)` events. This is reset when the HRM is initialised with `Bangle.setHRMPower`.
* `seaLevelPressure` (Bangle.js 2) Normally 1013.25 millibars - this is used for
  calculating altitude with the pressure sensor

Where accelerations are used they are in internal units, where `8192 = 1g`

*/
JsVar * _jswrap_banglejs_setOptions(JsVar *options, bool createObject) {
  return 0;
}
void jswrap_banglejs_setOptions(JsVar *options) {
}
/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "getOptions",
    "generate" : "jswrap_banglejs_getOptions",
    "return" : ["JsVar","The current state of all options"],
    "ifdef" : "BANGLEJS",
    "typescript" : "getOptions(): BangleOptions;"
}
Return the current state of options as set by `Bangle.setOptions`
*/
JsVar *jswrap_banglejs_getOptions() {
  return _jswrap_banglejs_setOptions(NULL, true);
}




/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "setLocked",
    "generate" : "jswrap_banglejs_setLocked",
    "params" : [
      ["isLocked","bool","`true` if the Bangle is locked (no user input allowed)"]
    ],
    "ifdef" : "BANGLEJS"
}
This function can be used to lock or unlock Bangle.js (e.g. whether buttons and
touchscreen work or not)
*/
void _jswrap_banglejs_setLocked(bool isLocked, const char *reason) {
}
void jswrap_banglejs_setLocked(bool isLocked) {
  _jswrap_banglejs_setLocked(isLocked, "js");
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "isLocked",
    "generate" : "jswrap_banglejs_isLocked",
    "return" : ["bool","Is the screen locked or not?"],
    "ifdef" : "BANGLEJS"
}
Also see the `Bangle.lock` event
*/
// emscripten bug means we can't use 'bool' as return value here!
int jswrap_banglejs_isLocked() {
  return 0;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "isCharging",
    "generate" : "jswrap_banglejs_isCharging",
    "return" : ["bool","Is the battery charging or not?"],
    "ifdef" : "BANGLEJS"
}
*/
// emscripten bug means we can't use 'bool' as return value here!
int jswrap_banglejs_isCharging() {
  return 0;
}

/// get battery percentage
JsVarInt jswrap_banglejs_getBattery() { 
  return 0;
}


/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "setHRMPower",
    "generate" : "jswrap_banglejs_setHRMPower",
    "params" : [
      ["isOn","bool","True if the heart rate monitor should be on, false if not"],
      ["appID","JsVar","A string with the app's name in, used to ensure one app can't turn off something another app is using"]
    ],
    "return" : ["bool","Is HRM on?"],
    "ifdef" : "BANGLEJS",
    "typescript" : "setHRMPower(isOn: ShortBoolean, appID: string): boolean;"
}
Set the power to the Heart rate monitor

When on, data is output via the `HRM` event on `Bangle`:

```
Bangle.setHRMPower(true, "myapp");
Bangle.on('HRM',print);
```

*When on, the Heart rate monitor draws roughly 5mA*
*/
bool jswrap_banglejs_setHRMPower(bool isOn, JsVar *appId) {
  return false;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "isHRMOn",
    "generate" : "jswrap_banglejs_isHRMOn",
    "return" : ["bool","Is HRM on?"],
    "ifdef" : "BANGLEJS"
}
Is the Heart rate monitor powered?

Set power with `Bangle.setHRMPower(...);`
*/
// emscripten bug means we can't use 'bool' as return value here!
int jswrap_banglejs_isHRMOn() {
  return 0;
}

/// Clear all data stored for the GPS input line
void gpsClearLine() {
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "setGPSPower",
    "generate" : "jswrap_banglejs_setGPSPower",
    "params" : [
      ["isOn","bool","True if the GPS should be on, false if not"],
      ["appID","JsVar","A string with the app's name in, used to ensure one app can't turn off something another app is using"]
    ],
    "return" : ["bool","Is the GPS on?"],
    "ifdef" : "BANGLEJS",
    "typescript" : "setGPSPower(isOn: ShortBoolean, appID: string): boolean;"
}
Set the power to the GPS.

When on, data is output via the `GPS` event on `Bangle`:

```
Bangle.setGPSPower(true, "myapp");
Bangle.on('GPS',print);
```

*When on, the GPS draws roughly 20mA*
*/
bool jswrap_banglejs_setGPSPower(bool isOn, JsVar *appId) {
  return false;
}
/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "isGPSOn",
    "generate" : "jswrap_banglejs_isGPSOn",
    "return" : ["bool","Is the GPS on?"],
    "ifdef" : "BANGLEJS"
}
Is the GPS powered?

Set power with `Bangle.setGPSPower(...);`
*/
// emscripten bug means we can't use 'bool' as return value here!
int jswrap_banglejs_isGPSOn() {
  return 0;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "getGPSFix",
    "generate" : "jswrap_banglejs_getGPSFix",
    "return" : ["JsVar","A GPS fix object with `{lat,lon,...}`"],
    "ifdef" : "BANGLEJS",
    "typescript" : "getGPSFix(): GPSFix;"
}
Get the last available GPS fix info (or `undefined` if GPS is off).

The fix info received is the same as you'd get from the `Bangle.GPS` event.
*/
JsVar *jswrap_banglejs_getGPSFix() {
  return 0;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "setCompassPower",
    "generate" : "jswrap_banglejs_setCompassPower",
    "params" : [
      ["isOn","bool","True if the Compass should be on, false if not"],
      ["appID","JsVar","A string with the app's name in, used to ensure one app can't turn off something another app is using"]
    ],
    "return" : ["bool","Is the Compass on?"],
    "ifdef" : "BANGLEJS",
    "typescript" : "setCompassPower(isOn: ShortBoolean, appID: string): boolean;"
}
Set the power to the Compass

When on, data is output via the `mag` event on `Bangle`:

```
Bangle.setCompassPower(true, "myapp");
Bangle.on('mag',print);
```

*When on, the compass draws roughly 2mA*
*/
bool jswrap_banglejs_setCompassPower(bool isOn, JsVar *appId) {
  return false;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "isCompassOn",
    "generate" : "jswrap_banglejs_isCompassOn",
    "return" : ["bool","Is the Compass on?"],
    "ifdef" : "BANGLEJS"
}
Is the compass powered?

Set power with `Bangle.setCompassPower(...);`
*/
// emscripten bug means we can't use 'bool' as return value here!
int jswrap_banglejs_isCompassOn() {
  return 0;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "resetCompass",
    "generate" : "jswrap_banglejs_resetCompass",
    "params" : [],
    "ifdef" : "BANGLEJS"
}
Resets the compass minimum/maximum values. Can be used if the compass isn't
providing a reliable heading any more.
*/
void jswrap_banglejs_resetCompass() {
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "setBarometerPower",
    "generate" : "jswrap_banglejs_setBarometerPower",
    "params" : [
      ["isOn","bool","True if the barometer IC should be on, false if not"],
      ["appID","JsVar","A string with the app's name in, used to ensure one app can't turn off something another app is using"]
    ],
    "return" : ["bool","Is the Barometer on?"],
    "#if" : "defined(DTNO1_F5) || defined(BANGLEJS_Q3) || defined(DICKENS)",
    "typescript" : "setBarometerPower(isOn: ShortBoolean, appID: string): boolean;"
}
Set the power to the barometer IC. Once enabled, `Bangle.pressure` events are
fired each time a new barometer reading is available.

When on, the barometer draws roughly 50uA
*/
bool jswrap_banglejs_setBarometerPower(bool isOn, JsVar *appId) {
  return false;
}


/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "isBarometerOn",
    "generate" : "jswrap_banglejs_isBarometerOn",
    "return" : ["bool","Is the Barometer on?"],
    "#if" : "defined(DTNO1_F5) || defined(BANGLEJS_Q3) || defined(DICKENS)"
}
Is the Barometer powered?

Set power with `Bangle.setBarometerPower(...);`
*/
// emscripten bug means we can't use 'bool' as return value here!
int jswrap_banglejs_isBarometerOn() {
  return 0;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "getStepCount",
    "generate" : "jswrap_banglejs_getStepCount",
    "return" : ["int","The number of steps recorded by the step counter"],
    "ifdef" : "BANGLEJS"
}
Returns the current amount of steps recorded by the step counter
*/
int jswrap_banglejs_getStepCount() {
  return 0;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "setStepCount",
    "generate" : "jswrap_banglejs_setStepCount",
    "params" : [
      ["count","int","The value with which to reload the step counter"]
    ],
    "ifdef" : "BANGLEJS"
}
Sets the current value of the step counter
*/
void jswrap_banglejs_setStepCount(JsVarInt count) {
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "getCompass",
    "generate" : "jswrap_banglejs_getCompass",
    "return" : ["JsVar","An object containing magnetometer readings (as below)"],
    "ifdef" : "BANGLEJS",
    "typescript" : "getCompass(): CompassData;"
}
Get the most recent Magnetometer/Compass reading. Data is in the same format as
the `Bangle.on('mag',` event.

Returns an `{x,y,z,dx,dy,dz,heading}` object

* `x/y/z` raw x,y,z magnetometer readings
* `dx/dy/dz` readings based on calibration since magnetometer turned on
* `heading` in degrees based on calibrated readings (will be NaN if magnetometer
  hasn't been rotated around 360 degrees).

**Note:** In 2v15 firmware and earlier the heading is inverted (360-heading). There's
a fix in the bootloader which will apply a fix for those headings, but old apps may
still expect an inverted value.

To get this event you must turn the compass on with `Bangle.setCompassPower(1)`.*/
JsVar *jswrap_banglejs_getCompass() {
  return 0;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "getAccel",
    "generate" : "jswrap_banglejs_getAccel",
    "return" : ["JsVar","An object containing accelerometer readings (as below)"],
    "ifdef" : "BANGLEJS",
    "typescript" : "getAccel(): AccelData & { td: number };"
}
Get the most recent accelerometer reading. Data is in the same format as the
`Bangle.on('accel',` event.

* `x` is X axis (left-right) in `g`
* `y` is Y axis (up-down) in `g`
* `z` is Z axis (in-out) in `g`
* `diff` is difference between this and the last reading in `g` (calculated by
  comparing vectors, not magnitudes)
* `td` is the elapsed
* `mag` is the magnitude of the acceleration in `g`
*/
JsVar *jswrap_banglejs_getAccel() {
  JsVar *o = jsvNewObject();
  return o;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "getHealthStatus",
    "generate" : "jswrap_banglejs_getHealthStatus",
    "return" : ["JsVar","Returns an object containing various health info"],
    "params" : [
      ["range","JsVar","What time period to return data for, see below:"]
    ],
    "ifdef" : "BANGLEJS",
    "typescript" : "getHealthStatus(range?: \"current\" | \"last\" | \"day\"): HealthStatus;"
}

`range` is one of:

* `undefined` or `'10min'` - health data so far in this 10 minute block (eg. 9:00.00 - 9:09.59)
* `'last'` - health data during the last 10 minute block
* `'day'` - the health data so far for the day


`getHealthStatus` returns an object containing:

* `movement` is the 32 bit sum of all `acc.diff` readings since power on (and
  rolls over). It is the difference in accelerometer values as `g*8192`
* `steps` is the number of steps during this period
* `bpm` the best BPM reading from HRM sensor during this period
* `bpmConfidence` best BPM confidence (0-100%) during this period

*/

JsVar *jswrap_banglejs_getHealthStatus(JsVar *range) {
  return 0;
}

/* After init is called (a second time, NOT first time), we execute any JS that is due to be executed,
 * then we call this afterwards to shut down anything that isn't required (compass/hrm/etc). */
void jswrap_banglejs_postInit() {
}

NO_INLINE void jswrap_banglejs_setTheme() {

}

/*JSON{
  "type" : "hwinit",
  "generate" : "jswrap_banglejs_hwinit"
}*/
//first function i think!


NO_INLINE void jswrap_banglejs_hwinit() {
  #ifdef NRF52832
    jswrap_ble_setTxPower(4);
  #endif

  
  graphicsStructInit(&graphicsInternal, LCD_WIDTH, LCD_HEIGHT, LCD_BPP);
  graphicsInternal.data.flags = 0;
  graphicsInternal.data.fontSize = JSGRAPHICS_FONTSIZE_6X8+1;
  
  graphicHwSetup(&graphicsInternal);
  
//setup graphics
  graphicsSetCallbacks(&graphicsInternal);
// set default graphics themes - before we even start to load settings.json
  jswrap_banglejs_setTheme();
//set screen to to default color

  //needs a delay for some reason?
  jshDelayMicroseconds(4000000);
  graphicsInternal.fillRect(&graphicsInternal,0,0,LCD_WIDTH,LCD_HEIGHT,graphicsTheme.bg);
}

/*JSON{
  "type" : "init",
  "generate" : "jswrap_banglejs_init"
}*/
NO_INLINE void jswrap_banglejs_init() {
  IOEventFlags channel;
  bool firstRun = jsiStatus & JSIS_FIRST_BOOT; 
  
#ifndef EMULATED
  // turn vibrate off every time Bangle is reset
  jshPinOutput(VIBRATE_PIN,0);
#endif



  jswrap_banglejs_setLCDOffset(0);
  graphicsStructResetState(&graphicsInternal);

    // Create backing graphics object for LCD
  JsVar *graphics = jspNewObject(0, "Graphics");
  // if there's nothing in the Graphics object, we assume it's for the built-in graphics
  if (!graphics) return; // low memory
  // add it as a global var
  jsvObjectSetChild(execInfo.root, "g", graphics);
  jsvObjectSetChild(execInfo.hiddenRoot, JS_GRAPHICS_VAR, graphics);
  graphicsInternal.graphicsVar = graphics;

  // Create 'flip' fn
  JsVar *fn = jsvNewNativeFunction((void (*)(void))lcd_flip, JSWAT_VOID|JSWAT_THIS_ARG|(JSWAT_BOOL << (JSWAT_BITS*1)));
  jsvObjectSetChildAndUnLock(graphics,"flip",fn);

  backlight_init();

  //splash screen
}

/*JSON{
  "type" : "idle",
  "generate" : "jswrap_banglejs_idle"
}*/
bool jswrap_banglejs_idle() {

  return false;
}

/*JSON{
  "type" : "EV_SERIAL1",
  "generate" : "jswrap_banglejs_gps_character",
  "#if" : "defined(BANGLEJS_F18) || defined(DTNO1_F5)  || defined(BANGLEJS_Q3)"
}*/
bool jswrap_banglejs_gps_character(char ch) {

  return true; // handled
}

// TODO Improve TypeScript declaration
/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "dbg",
    "generate" : "jswrap_banglejs_dbg",
    "return" : ["JsVar",""],
    "ifdef" : "BANGLEJS"
}
Reads debug info. Exposes the current values of `accHistoryIdx`, `accGestureCount`, `accIdleCount`, `pollInterval` and others.

Please see the declaration of this function for more information (click the `==>` link above [this description](http://www.espruino.com/Reference#l_Bangle_dbg))
*/
JsVar *jswrap_banglejs_dbg() {
  JsVar *o = jsvNewObject();
 
  return o;
}

void _jswrap_banglejs_i2cWr(JshI2CInfo *i2c, int i2cAddr, JsVarInt reg, JsVarInt data) {

}

JsVar *_jswrap_banglejs_i2cRd(JshI2CInfo *i2c, int i2cAddr, JsVarInt reg, JsVarInt cnt) {

  unsigned char buf[128] ={0};
  return jsvNewFromInteger(buf[0]);
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "accelWr",
    "generate" : "jswrap_banglejs_accelWr",
    "params" : [
      ["reg","int",""],
      ["data","int",""]
    ],
    "ifdef" : "BANGLEJS"
}
Writes a register on the accelerometer
*/
void jswrap_banglejs_accelWr(JsVarInt reg, JsVarInt data) {

}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "accelRd",
    "generate" : "jswrap_banglejs_accelRd",
    "params" : [
      ["reg","int",""],
      ["cnt","int","If specified, returns an array of the given length (max 128). If not (or 0) it returns a number"]
    ],
    "return" : ["JsVar",""],
    "ifdef" : "BANGLEJS",
    "typescript" : [
      "accelRd(reg: number, cnt?: 0): number;",
      "accelRd(reg: number, cnt: number): number[];"
    ]
}
Reads a register from the accelerometer

**Note:** On Espruino 2v06 and before this function only returns a number (`cnt`
is ignored).
*/


JsVar *jswrap_banglejs_accelRd(JsVarInt reg, JsVarInt cnt) {
  return 0;
}


/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "barometerWr",
    "generate" : "jswrap_banglejs_barometerWr",
    "params" : [
      ["reg","int",""],
      ["data","int",""]
    ],
    "#if" : "defined(DTNO1_F5) || defined(BANGLEJS_Q3) || defined(DICKENS)"
}
Writes a register on the barometer IC
*/
void jswrap_banglejs_barometerWr(JsVarInt reg, JsVarInt data) {

}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "barometerRd",
    "generate" : "jswrap_banglejs_barometerRd",
    "params" : [
      ["reg","int",""],
      ["cnt","int","If specified, returns an array of the given length (max 128). If not (or 0) it returns a number"]
    ],
    "return" : ["JsVar",""],
    "#if" : "defined(DTNO1_F5) || defined(BANGLEJS_Q3) || defined(DICKENS)",
    "typescript" : [
      "barometerRd(reg: number, cnt?: 0): number;",
      "barometerRd(reg: number, cnt: number): number[];"
    ]
}
Reads a register from the barometer IC
*/
JsVar *jswrap_banglejs_barometerRd(JsVarInt reg, JsVarInt cnt) {

  return 0;
}


/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "compassWr",
    "generate" : "jswrap_banglejs_compassWr",
    "params" : [
      ["reg","int",""],
      ["data","int",""]
    ],
    "ifdef" : "BANGLEJS"
}
Writes a register on the Magnetometer/Compass
*/
void jswrap_banglejs_compassWr(JsVarInt reg, JsVarInt data) {

}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "compassRd",
    "generate" : "jswrap_banglejs_compassRd",
    "params" : [
      ["reg","int",""],
      ["cnt","int","If specified, returns an array of the given length (max 128). If not (or 0) it returns a number"]
    ],
    "return" : ["JsVar",""],
    "ifdef" : "BANGLEJS",
    "typescript" : [
      "compassRd(reg: number, cnt?: 0): number;",
      "compassRd(reg: number, cnt: number): number[];"
    ]
}
Read a register on the Magnetometer/Compass
*/
JsVar *jswrap_banglejs_compassRd(JsVarInt reg, JsVarInt cnt) {
  return NULL;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "hrmWr",
    "generate" : "jswrap_banglejs_hrmWr",
    "params" : [
      ["reg","int",""],
      ["data","int",""]
    ],
    "ifdef" : "BANGLEJS_Q3"
}
Writes a register on the Heart rate monitor
*/
void jswrap_banglejs_hrmWr(JsVarInt reg, JsVarInt data) {

}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "hrmRd",
    "generate" : "jswrap_banglejs_hrmRd",
    "params" : [
      ["reg","int",""],
      ["cnt","int","If specified, returns an array of the given length (max 128). If not (or 0) it returns a number"]
    ],
    "return" : ["JsVar",""],
    "ifdef" : "BANGLEJS",
    "typescript" : [
      "hrmRd(reg: number, cnt?: 0): number;",
      "hrmRd(reg: number, cnt: number): number[];"
    ]
}
Read a register on the Heart rate monitor
*/
JsVar *jswrap_banglejs_hrmRd(JsVarInt reg, JsVarInt cnt) {

  return 0;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "ioWr",
    "generate" : "jswrap_banglejs_ioWr",
    "params" : [
      ["mask","int",""],
      ["isOn","int",""]
    ],
    "ifdef" : "BANGLEJS_F18"
}
Changes a pin state on the IO expander
*/
void jswrap_banglejs_ioWr(JsVarInt mask, bool on) {

}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "getPressure",
    "generate" : "jswrap_banglejs_getPressure",
    "return" : ["JsVar","A promise that will be resolved with `{temperature, pressure, altitude}`"],
    "#if" : "defined(DTNO1_F5) || defined(BANGLEJS_Q3) || defined(DICKENS)",
    "typescript" : "getPressure(): Promise<PressureData> | undefined;"
}
Read temperature, pressure and altitude data. A promise is returned which will
be resolved with `{temperature, pressure, altitude}`.

If the Barometer has been turned on with `Bangle.setBarometerPower` then this
will return almost immediately with the reading. If the Barometer is off,
conversions take between 500-750ms.

Altitude assumes a sea-level pressure of 1013.25 hPa

If there's no pressure device (for example, the emulator),
this returns undefined, rather than a promise.

```
Bangle.getPressure().then(d=>{
  console.log(d);
  // {temperature, pressure, altitude}
});
```
*/

bool jswrap_banglejs_barometerPoll() {

  return false;
}

JsVar *jswrap_banglejs_getBarometerObject() {
  JsVar *o = jsvNewObject();
  return o;
}

void jswrap_banglejs_getPressure_callback() {
 
}


JsVar *jswrap_banglejs_getPressure() {
  return 0;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "project",
    "generate" : "jswrap_banglejs_project",
    "params" : [
      ["latlong","JsVar","`{lat:..., lon:...}`"]
    ],
    "return" : ["JsVar","{x:..., y:...}"],
    "ifdef" : "BANGLEJS",
    "typescript" : "project(latlong: { lat: number, lon: number }): { x: number, y: number };"
}
Perform a Spherical [Web Mercator
projection](https://en.wikipedia.org/wiki/Web_Mercator_projection) of latitude
and longitude into `x` and `y` coordinates, which are roughly equivalent to
meters from `{lat:0,lon:0}`.

This is the formula used for most online mapping and is a good way to compare
GPS coordinates to work out the distance between them.
*/
JsVar *jswrap_banglejs_project(JsVar *latlong) {
  JsVar *o = jsvNewObject();
  return o;
}


static NO_INLINE void _jswrap_banglejs_setVibration() {
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "beep",
    "generate" : "jswrap_banglejs_beep",
    "params" : [
      ["time","int","[optional] Time in ms (default 200)"],
      ["freq","int","[optional] Frequency in hz (default 4000)"]
    ],
    "return" : ["JsVar","A promise, completed when beep is finished"],
    "return_object":"Promise",
    "ifdef" : "BANGLEJS"
}
Use the piezo speaker to Beep for a certain time period and frequency
*/
void jswrap_banglejs_beep_callback() {
}

JsVar *jswrap_banglejs_beep(int time, int freq) {
  return NULL;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "buzz",
    "generate" : "jswrap_banglejs_buzz",
    "params" : [
      ["time","int","[optional] Time in ms (default 200)"],
      ["strength","float","[optional] Power of vibration from 0 to 1 (Default 1)"]
    ],
    "return" : ["JsVar","A promise, completed when vibration is finished"],
    "return_object":"Promise",
    "ifdef" : "BANGLEJS"
}
Use the vibration motor to buzz for a certain time period
*/
void jswrap_banglejs_buzz_callback() {
}

JsVar *jswrap_banglejs_buzz(int time, JsVarFloat amt) {

  return NULL;
}

static void jswrap_banglejs_periph_off() {
}

// True if a button/charge input/etc should wake the Bangle from being off
static bool _jswrap_banglejs_shouldWake() {
  return false;
}


/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "off",
    "generate" : "jswrap_banglejs_off",
    "ifdef" : "BANGLEJS"
}
Turn Bangle.js off. It can only be woken by pressing BTN1.
*/
void jswrap_banglejs_off() {
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "softOff",
    "generate" : "jswrap_banglejs_softOff",
    "ifdef" : "BANGLEJS"
}
Turn Bangle.js (mostly) off, but keep the CPU in sleep mode until BTN1 is
pressed to preserve the RTC (current time).
*/
void jswrap_banglejs_softOff() {
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "getLogo",
    "generate" : "jswrap_banglejs_getLogo",
    "return" : ["JsVar", "An image to be used with `g.drawImage` (as a String)" ],
    "ifdef" : "BANGLEJS",
    "typescript" : "getLogo(): string;"
}

* On platforms with an LCD of >=8bpp this is 222 x 104 x 2 bits
* Otherwise it's 119 x 56 x 1 bits
*/
JsVar *jswrap_banglejs_getLogo() {
  return NULL;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "loadWidgets",
    "generate_js" : "libs/js/banglejs/Bangle_loadWidgets.min.js",
    "ifdef" : "BANGLEJS"
}
Load all widgets from flash Storage. Call this once at the beginning of your
application if you want any on-screen widgets to be loaded.

They will be loaded into a global `WIDGETS` array, and can be rendered with
`Bangle.drawWidgets`.
*/
/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "drawWidgets",
    "generate_js" : "libs/js/banglejs/Bangle_drawWidgets.min.js",
    "ifdef" : "BANGLEJS"
}
Draw any onscreen widgets that were loaded with `Bangle.loadWidgets()`.

Widgets should redraw themselves when something changes - you'll only need to
call drawWidgets if you decide to clear the entire screen with `g.clear()`.
*/
/*JSON{
    "type" : "staticmethod", "class" : "Bangle", "name" : "drawWidgets", "patch":true,
    "generate_js" : "libs/js/banglejs/Bangle_drawWidgets_Q3.min.js",
    "#if" : "defined(BANGLEJS) && defined(BANGLEJS_Q3)"
}
*/

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "showLauncher",
    "generate_js" : "libs/js/banglejs/Bangle_showLauncher.min.js",
    "ifdef" : "BANGLEJS"
}
Load the Bangle.js app launcher, which will allow the user to select an
application to launch.
*/

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "showClock",
    "generate_js" : "libs/js/banglejs/Bangle_showClock.min.js",
    "ifdef" : "BANGLEJS"
}
Load the Bangle.js clock - this has the same effect as calling `Bangle.load()`.
*/

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "showRecoveryMenu",
    "generate_js" : "libs/js/banglejs/Bangle_showRecoveryMenu.js",
    "ifdef" : "BANGLEJS"
}
Show a 'recovery' menu that allows you to perform certain tasks on your Bangle.

You can also enter this menu by restarting while holding down the `BTN1`
*/

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "load",
    "generate_js" : "libs/js/banglejs/Bangle_load.min.js",
    "params" : [
      ["file","JsVar","[optional] A string containing the file name for the app to be loaded"]
    ],
    "ifdef" : "BANGLEJS",
    "typescript": [
      "load(file: string): void;",
      "load(): void;"
    ]
}
This behaves the same as the global `load()` function, but if fast
loading is possible (`Bangle.setUI` was called with a `remove` handler)
then instead of a complete reload, the `remove` handler will be
called and the new app will be loaded straight after with `eval`.

**This should only be used if the app being loaded also uses widgets**
(eg it contains a `Bangle.loadWidgets()` call).

`load()` is slower, but safer. As such, care should be taken
when using `Bangle.load()` with `Bangle.setUI({..., remove:...})`
as if your remove handler doesn't completely clean up after your app,
memory leaks or other issues could occur - see `Bangle.setUI` for more
information.
*/

/*JSON{
    "type" : "staticmethod",
    "class" : "E",
    "name" : "showMenu",
    "generate_js" : "libs/js/banglejs/E_showMenu_F18.min.js",
    "params" : [
      ["menu","JsVar","An object containing name->function mappings to to be used in a menu"]
    ],
    "return" : ["JsVar", "A menu object with `draw`, `move` and `select` functions" ],
    "ifdef" : "BANGLEJS",
    "typescript": [
      "showMenu(menu: Menu): MenuInstance;",
      "showMenu(): void;"
    ]
}
Display a menu on the screen, and set up the buttons to navigate through it.

Supply an object containing menu items. When an item is selected, the function
it references will be executed. For example:

```
var boolean = false;
var number = 50;
// First menu
var mainmenu = {
  "" : { title : "-- Main Menu --" }, // options
  "LED On" : function() { LED1.set(); },
  "LED Off" : function() { LED1.reset(); },
  "Submenu" : function() { E.showMenu(submenu); },
  "A Boolean" : {
    value : boolean,
    format : v => v?"On":"Off",
    onchange : v => { boolean=v; }
  },
  "A Number" : {
    value : number,
    min:0,max:100,step:10,
    onchange : v => { number=v; }
  },
  "Exit" : function() { E.showMenu(); }, // remove the menu
};
// Submenu
var submenu = {
  "" : { title : "-- SubMenu --",
         back : function() { E.showMenu(mainmenu); } },
  "One" : undefined, // do nothing
  "Two" : undefined // do nothing
};
// Actually display the menu
E.showMenu(mainmenu);
```

The menu will stay onscreen and active until explicitly removed, which you can
do by calling `E.showMenu()` without arguments.

See http://www.espruino.com/graphical_menu for more detailed information.

On Bangle.js there are a few additions over the standard `graphical_menu`:

* The options object can contain:
  * `back : function() { }` - add a 'back' button, with the function called when
    it is pressed
  * `remove : function() { }` - add a handler function to be called when the
    menu is removed
  * (Bangle.js 2) `scroll : int` - an integer specifying how much the initial
    menu should be scrolled by
* The object returned by `E.showMenu` contains:
  * (Bangle.js 2) `scroller` - the object returned by `E.showScroller` -
    `scroller.scroll` returns the amount the menu is currently scrolled by
* In the object specified for editable numbers:
  * (Bangle.js 2) the `format` function is called with `format(value)` in the
    main menu, `format(value,1)` when in a scrollable list, or `format(value,2)`
    when in a popup window.

You can also specify menu items as an array (rather than an Object). This can be
useful if you have menu items with the same title, or you want to `push` menu
items onto an array:

```
var menu = [
  { title:"Something", onchange:function() { print("selected"); } },
  { title:"On or Off", value:false, onchange: v => print(v) },
  { title:"A Value", value:3, min:0, max:10, onchange: v => print(v) },
];
menu[""] = { title:"Hello" };
E.showMenu(menu);
```
*/

/*JSON{
    "type" : "staticmethod",
    "class" : "E",
    "name" : "showMessage",
    "generate_js" : "libs/js/banglejs/E_showMessage.min.js",
    "params" : [
      ["message","JsVar","A message to display. Can include newlines"],
      ["options","JsVar","[optional] a title for the message, or an object of options `{title:string, img:image_string}`"]
    ],
    "ifdef" : "BANGLEJS",
    "typescript" : "showMessage(message: string, title?: string | { title?: string, img?: string }): void;"
}

A utility function for displaying a full screen message on the screen.

Draws to the screen and returns immediately.

```
E.showMessage("These are\nLots of\nLines","My Title")
```

or to display an image as well as text:

```
E.showMessage("Lots of text will wrap automatically",{
  title:"Warning",
  img:atob("FBQBAfgAf+Af/4P//D+fx/n+f5/v+f//n//5//+f//n////3//5/n+P//D//wf/4B/4AH4A=")
})
```



*/
/*JSON{
    "type" : "staticmethod",
    "class" : "E",
    "name" : "showPrompt",
    "generate_js" : "libs/js/banglejs/E_showPrompt.min.js",
    "params" : [
      ["message","JsVar","A message to display. Can include newlines"],
      ["options","JsVar","[optional] an object of options (see below)"]
    ],
    "return" : ["JsVar","A promise that is resolved when 'Ok' is pressed"],
    "ifdef" : "BANGLEJS",
    "typescript" : [
      "showPrompt<T = boolean>(message: string, options?: { title?: string, buttons?: { [key: string]: T }, image?: string, remove?: () => void }): Promise<T>;",
      "showPrompt(): void;"
    ]
}

Displays a full screen prompt on the screen, with the buttons requested (or
`Yes` and `No` for defaults).

When the button is pressed the promise is resolved with the requested values
(for the `Yes` and `No` defaults, `true` and `false` are returned).

```
E.showPrompt("Do you like fish?").then(function(v) {
  if (v) print("'Yes' chosen");
  else print("'No' chosen");
});
// Or
E.showPrompt("How many fish\ndo you like?",{
  title:"Fish",
  buttons : {"One":1,"Two":2,"Three":3}
}).then(function(v) {
  print("You like "+v+" fish");
});
// Or
E.showPrompt("Continue?", {
  title:"Alert",
  img:atob("FBQBAfgAf+Af/4P//D+fx/n+f5/v+f//n//5//+f//n////3//5/n+P//D//wf/4B/4AH4A=")}).then(function(v) {
  if (v) print("'Yes' chosen");
  else print("'No' chosen");
});
```

To remove the prompt, call `E.showPrompt()` with no arguments.

The second `options` argument can contain:

```
{
  title: "Hello",                       // optional Title
  buttons : {"Ok":true,"Cancel":false}, // optional list of button text & return value
  img: "image_string"                   // optional image string to draw
  remove: function() { }                // Bangle.js: optional function to be called when the prompt is removed
}
```
*/

/*JSON{
    "type" : "staticmethod",
    "class" : "E",
    "name" : "showScroller",
    "generate_js" : "libs/js/banglejs/E_showScroller.min.js",
    "params" : [
      ["options","JsVar","An object containing `{ h, c, draw, select, back, remove }` (see below) "]
    ],
    "return" : ["JsVar", "A menu object with `draw()` and `drawItem(itemNo)` functions" ],
    "ifdef" : "BANGLEJS",
    "typescript" : [
      "showScroller(options?: { h: number, c: number, draw: (idx: number, rect: { x: number, y: number, w: number, h: number }) => void, select: (idx: number, touch?: {x: number, y: number}) => void, back?: () => void, remove?: () => void }): { draw: () => void, drawItem: (itemNo: number) => void };",
      "showScroller(): void;"
    ]
}
Display a scrollable menu on the screen, and set up the buttons/touchscreen to
navigate through it and select items.

Supply an object containing:

```
{
  h : 24, // height of each menu item in pixels
  c : 10, // number of menu items
  // a function to draw a menu item
  draw : function(idx, rect) { ... }
  // a function to call when the item is selected, touch parameter is only relevant
  // for Bangle.js 2 and contains the coordinates touched inside the selected item
  select : function(idx, touch) { ... }
  // optional function to be called when 'back' is tapped
  back : function() { ...}
  // Bangle.js: optional function to be called when the scroller should be removed
  remove : function() {}
}
```

For example to display a list of numbers:

```
E.showScroller({
  h : 40, c : 8,
  draw : (idx, r) => {
    g.setBgColor((idx&1)?"#666":"#999").clearRect(r.x,r.y,r.x+r.w-1,r.y+r.h-1);
    g.setFont("6x8:2").drawString("Item Number\n"+idx,r.x+10,r.y+4);
  },
  select : (idx) => console.log("You selected ", idx)
});
```

To remove the scroller, just call `E.showScroller()`
*/

/*JSON{
    "type" : "staticmethod", "class" : "E", "name" : "showMenu", "patch":true,
    "generate_js" : "libs/js/banglejs/E_showMenu_Q3.min.js",
    "#if" : "defined(BANGLEJS) && defined(BANGLEJS_Q3)"
}
*/
/*JSON{
    "type" : "staticmethod", "class" : "E", "name" : "showMenu", "patch":true,
    "generate_js" : "libs/js/banglejs/E_showMenu_F5.js",
    "#if" : "defined(BANGLEJS) && defined(DTNO1_F5)"
}
*/
/*JSON{
    "type" : "staticmethod", "class" : "E", "name" : "showPrompt", "patch":true,
    "generate_js" : "libs/js/banglejs/E_showPrompt_Q3.min.js",
    "#if" : "defined(BANGLEJS) && defined(BANGLEJS_Q3)"
}
*/
/*JSON{
    "type" : "staticmethod", "class" : "E", "name" : "showScroller", "patch":true,
    "generate_js" : "libs/js/banglejs/E_showScroller_Q3.min.js",
    "#if" : "defined(BANGLEJS) && defined(BANGLEJS_Q3)"
}
*/

/*JSON{
    "type" : "staticmethod",
    "class" : "E",
    "name" : "showAlert",
    "generate_js" : "libs/js/banglejs/E_showAlert.min.js",
    "params" : [
      ["message","JsVar","A message to display. Can include newlines"],
      ["options","JsVar","[optional] a title for the message or an object containing options"]
    ],
    "return" : ["JsVar","A promise that is resolved when 'Ok' is pressed"],
    "ifdef" : "BANGLEJS",
    "typescript" : [
      "showAlert(message?: string, options?: string): Promise<void>;",
      "showAlert(message?: string, options?: { title?: string, remove?: () => void }): Promise<void>;"
    ]
}

Displays a full screen prompt on the screen, with a single 'Ok' button.

When the button is pressed the promise is resolved.

```
E.showAlert("Hello").then(function() {
  print("Ok pressed");
});
// or
E.showAlert("These are\nLots of\nLines","My Title").then(function() {
  print("Ok pressed");
});
```

To remove the window, call `E.showAlert()` with no arguments.
*/

/*JSON{
    "type" : "variable",
    "name" : "LED",
    "generate" : "gen_jswrap_LED1",
    "return" : ["JsVar","A `Pin` object for a fake LED which appears on "],
    "ifdef" : "BANGLEJS", "no_docs":1
}

On most Espruino board there are LEDs, in which case `LED` will be an actual
Pin.

On Bangle.js there are no LEDs, so to remain compatible with example code that
might expect an LED, this is an object that behaves like a pin, but which just
displays a circle on the display
*/


/*TYPESCRIPT
type SetUIArg<Mode> = Mode | {
  mode: Mode,
  back?: () => void,
  remove?: () => void,
  redraw?: () => void,
};
*/
/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "setUI",
    "generate_js" : "libs/js/banglejs/Bangle_setUI_F18.min.js",
    "params" : [
      ["type","JsVar","The type of UI input: 'updown', 'leftright', 'clock', 'clockupdown' or undefined to cancel. Can also be an object (see below)"],
      ["callback","JsVar","A function with one argument which is the direction"]
    ],
    "ifdef" : "BANGLEJS",
    "typescript" : [
      "setUI(type?: undefined): void;",
      "setUI(type: SetUIArg<\"updown\" | \"leftright\">, callback: (direction?: -1 | 1) => void): void;",
      "setUI(type: SetUIArg<\"clock\">): void;",
      "setUI(type: SetUIArg<\"clockupdown\">, callback?: (direction: -1 | 1) => void): void;",
      "setUI(type: SetUIArg<\"custom\"> & { touch?: TouchCallback; swipe?: SwipeCallback; drag?: DragCallback; btn?: (n: 1 | 2 | 3) => void; clock?: boolean | 0 | 1 }): void;"
    ]
}
This puts Bangle.js into the specified UI input mode, and calls the callback
provided when there is user input.

Currently supported interface types are:

* 'updown' - UI input with upwards motion `cb(-1)`, downwards motion `cb(1)`,
  and select `cb()`
  * Bangle.js 1 uses BTN1/3 for up/down and BTN2 for select
  * Bangle.js 2 uses touchscreen swipe up/down and tap
* 'leftright' - UI input with left motion `cb(-1)`, right motion `cb(1)`, and
  select `cb()`
  * Bangle.js 1 uses BTN1/3 for left/right and BTN2 for select
  * Bangle.js 2 uses touchscreen swipe left/right and tap/BTN1 for select
* 'clock' - called for clocks. Sets `Bangle.CLOCK=1` and allows a button to
  start the launcher
  * Bangle.js 1 BTN2 starts the launcher
  * Bangle.js 2 BTN1 starts the launcher
* 'clockupdown' - called for clocks. Sets `Bangle.CLOCK=1`, allows a button to
  start the launcher, but also provides up/down functionality
  * Bangle.js 1 BTN2 starts the launcher, BTN1/BTN3 call `cb(-1)` and `cb(1)`
  * Bangle.js 2 BTN1 starts the launcher, touchscreen tap in top/bottom right
    hand side calls `cb(-1)` and `cb(1)`
* `{mode:"custom", ...}` allows you to specify custom handlers for different
  interactions. See below.
* `undefined` removes all user interaction code

While you could use setWatch/etc manually, the benefit here is that you don't
end up with multiple `setWatch` instances, and the actual input method (touch,
or buttons) is implemented dependent on the watch (Bangle.js 1 or 2)

**Note:** You can override this function in boot code to change the interaction
mode with the watch. For instance you could make all clocks start the launcher
with a swipe by using:

```
(function() {
  var sui = Bangle.setUI;
  Bangle.setUI = function(mode, cb) {
    var m = ("object"==typeof mode) ? mode.mode : mode;
    if (m!="clock") return sui(mode,cb);
    sui(); // clear
    Bangle.CLOCK=1;
    Bangle.swipeHandler = Bangle.showLauncher;
    Bangle.on("swipe", Bangle.swipeHandler);
  };
})();
```

The first argument can also be an object, in which case more options can be
specified:

```
Bangle.setUI({
  mode : "custom",
  back : function() {}, // optional - add a 'back' icon in top-left widget area and call this function when it is pressed , also call it when the hardware button is clicked (does not override btn if defined)
  remove : function() {}, // optional - add a handler for when the UI should be removed (eg stop any intervals/timers here)
  redraw : function() {}, // optional - add a handler to redraw the UI. Not needed but it can allow widgets/etc to provide other functionality that requires the screen to be redrawn
  touch : function(n,e) {}, // optional - (mode:custom only) handler for 'touch' events
  swipe : function(dir) {}, // optional - (mode:custom only) handler for 'swipe' events
  drag : function(e) {}, // optional - (mode:custom only) handler for 'drag' events (Bangle.js 2 only)
  btn : function(n) {}, // optional - (mode:custom only) handler for 'button' events (n==1 on Bangle.js 2, n==1/2/3 depending on button for Bangle.js 1)
  clock : 0 // optional - if set the behavior of 'clock' mode is added (does not override btn if defined)
});
```

If `remove` is specified, `Bangle.showLauncher`, `Bangle.showClock`, `Bangle.load` and some apps
may choose to just call the `remove` function and then load a new app without resetting Bangle.js.
As a result, **if you specify 'remove' you should make sure you test that after calling `Bangle.setUI()`
without arguments your app is completely unloaded**, otherwise you may end up with memory leaks or
other issues when switching apps. Please see http://www.espruino.com/Bangle.js+Fast+Load for more details on this.
*/
/*JSON{
    "type" : "staticmethod", "class" : "Bangle", "name" : "setUI", "patch":true,
    "generate_js" : "libs/js/banglejs/Bangle_setUI_Q3.min.js",
    "#if" : "defined(BANGLEJS) && defined(BANGLEJS_Q3)"
}
*/

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "factoryReset",
    "params" : [
      ["noReboot","bool","Do not reboot the watch when done (default false, so will reboot)"]
    ],
    "generate" : "jswrap_banglejs_factoryReset",
    "#if" : "defined(BANGLEJS_Q3) || defined(EMULATED) || defined(DICKENS)"
}

Erase all storage and reload it with the default contents.

This is only available on Bangle.js 2.0. On Bangle.js 1.0 you need to use
`Install Default Apps` under the `More...` tab of http://banglejs.com/apps
*/
extern void ble_app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name);
void jswrap_banglejs_factoryReset(bool noReboot) {
}

/*JSON{
    "type" : "staticproperty",
    "class" : "Bangle",
    "name" : "appRect",
    "generate" : "jswrap_banglejs_appRect",
    "return" : ["JsVar","An object of the form `{x,y,w,h,x2,y2}`"],
    "ifdef" : "BANGLEJS",
    "typescript" : "appRect: { x: number, y: number, w: number, h: number, x2: number, y2: number };"
}
Returns the rectangle on the screen that is currently reserved for the app.
*/
JsVar *jswrap_banglejs_appRect() {
  JsVar *o = jsvNewObject();  return o;
}


/// Called from jsinteractive when an event is parsed from the event queue for Bangle.js (executed outside IRQ)
void jsbangle_exec_pending(IOEvent *evt) {
}

/// Called from jsinteractive when an event is parsed from the event queue for Bangle.js
void jsbangle_push_event(JsBangleEvent type, uint16_t value) {
}

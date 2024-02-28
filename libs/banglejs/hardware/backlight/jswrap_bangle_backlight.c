#include "jswrap_bangle_backlight.h"
#include "bangle_backlight_impl.c"

/// LCD Brightness - 255=full
uint8_t lcdBrightness;

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "isBacklightOn",
    "generate" : "jswrap_banglejs_isBacklightOn",
    "return" : ["bool","Is the backlight on or not?"],
    "ifdef" : "BANGLEJS"
}
Also see the `Bangle.backlight` event
*/
// emscripten bug means we can't use 'bool' as return value here!
int jswrap_banglejs_isBacklightOn() {
  return (bangleFlags&JSBF_LCD_BL_ON)!=0;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "setBacklight",
    "generate" : "jswrap_banglejs_setLCDPowerBacklight",
    "params" : [
      ["isOn","bool","True if the LCD backlight should be on, false if not"]
    ],
    "ifdef" : "BANGLEJS"
}
This function can be used to turn Bangle.js's LCD backlight off or on.

This function resets the Bangle's 'activity timer' (like pressing a button or
the screen would) so after a time period of inactivity set by
`Bangle.setOptions({backlightTimeout: X});` the backlight will turn off.

If you want to keep the backlight on permanently (until apps are changed) you can
do:

```
Bangle.setOptions({backlightTimeout: 0}) // turn off the timeout
Bangle.setBacklight(1); // keep screen on
```

Of course, the backlight depends on `Bangle.setLCDPower` too, so any lcdPowerTimeout/setLCDTimeout will
also turn the backlight off. The use case is when you require the backlight timeout
to be shorter than the power timeout.
*/
/// Turn just the backlight on or off (or adjust brightness)
void jswrap_banglejs_setLCDPowerBacklight(bool isOn) {
  if (((bangleFlags&JSBF_LCD_BL_ON)!=0) != isOn) {
    JsVar *bangle =jsvObjectGetChildIfExists(execInfo.root, "Bangle");
    if (bangle) {
      JsVar *v = jsvNewFromBool(isOn);
      jsiQueueObjectCallbacks(bangle, JS_EVENT_PREFIX"backlight", &v, 1);
      jsvUnLock(v);
    }
    jsvUnLock(bangle);
  }

  if (isOn) {
    // programatically on counts as wake
    if (backlightTimeout > 0) inactivityTimer = 0;
    bangleFlags |= JSBF_LCD_BL_ON;
  }
  else {
    // ensure screen locks if programatically switch off
    if (lockTimeout > 0 && lockTimeout <= backlightTimeout) _jswrap_banglejs_setLocked(true, "backlight");
    bangleFlags &= ~JSBF_LCD_BL_ON;
  }
  banglejs_setLCDPowerBacklight_impl(isOn);
}



/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "setLCDBrightness",
    "generate" : "jswrap_banglejs_setLCDBrightness",
    "params" : [
      ["brightness","float","The brightness of Bangle.js's display - from 0(off) to 1(on full)"]
    ],
    "ifdef" : "BANGLEJS"
}
This function can be used to adjust the brightness of Bangle.js's display, and
hence prolong its battery life.

Due to hardware design constraints, software PWM has to be used which means that
the display may flicker slightly when Bluetooth is active and the display is not
at full power.

**Power consumption**

* 0 = 7mA
* 0.1 = 12mA
* 0.2 = 18mA
* 0.5 = 28mA
* 0.9 = 40mA (switching overhead)
* 1 = 40mA
*/
void jswrap_banglejs_setLCDBrightness(JsVarFloat v) {
  int b = (int)(v*256 + 0.5);
  if (b<0) b=0;
  if (b>255) b=255;
  lcdBrightness = b;
  if (bangleFlags&JSBF_LCD_ON)  // need to re-run to adjust brightness
    jswrap_banglejs_setLCDPowerBacklight(1);
}







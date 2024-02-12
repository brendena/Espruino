#include "bangle_lcd.h"

extern volatile JsBangleFlags bangleFlags;


__attribute__((weak)) void setDefaultPowerController(bool isOn){}

static void jswrap_banglejs_setLCDPowerController(bool isOn) {
  setDefaultPowerController(isOn);
#ifdef LCD_EN
  jshPinOutput(LCD_EN,isOn); // enable off
#endif
}


__attribute__((weak)) void setDefaultLcdWr(JsVarInt cmd,size_t dLen, const uint8_t * dPtr){}


/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "lcdWr",
    "generate" : "jswrap_banglejs_lcdWr",
    "params" : [
      ["cmd","int",""],
      ["data","JsVar",""]
    ],
    "ifdef" : "BANGLEJS"
}
Writes a command directly to the ST7735 LCD controller
*/
void jswrap_banglejs_lcdWr(JsVarInt cmd, JsVar *data) {
  JSV_GET_AS_CHAR_ARRAY(dPtr, dLen, data);
  setDefaultLcdWr(cmd, dLen, (const uint8_t *)dPtr);
}



__attribute__((weak)) void defaultPwrBacklight(unsigned char isOn, unsigned char lcdBrightness){
  jswrap_banglejs_pwrBacklight(isOn && (lcdBrightness>0));
  #ifdef LCD_BL
  if (isOn && lcdBrightness > 0 && lcdBrightness < 255) {
    jshPinAnalogOutput(LCD_BL, lcdBrightness/256.0, 200, JSAOF_NONE);
  }
  #endif // LCD_BL
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
  #ifndef EMULATED
  #if defined(ESPR_BACKLIGHT_FADE)
    if (!lcdFadeHandlerActive) {
      JsSysTime t = jshGetTimeFromMilliseconds(10);
      jstExecuteFn(backlightFadeHandler, NULL, t, t, NULL);
      lcdFadeHandlerActive = true;
      backlightFadeHandler();
    }
  #else // the default backlight mode (on Bangle.js 2/others)
    defaultPwrBacklight(isOn,lcdBrightness);
  #endif
  #endif // !EMULATED
}


/*This might be moved to the device page?*/
/*TYPESCRIPT
type LCDMode =
  | "direct"
  | "doublebuffered"
  | "120x120"
  | "80x80"
*/
/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "setLCDMode",
    "generate" : "jswrap_banglejs_setLCDMode",
    "params" : [
      ["mode","JsVar","The LCD mode (See below)"]
    ],
    "ifdef" : "BANGLEJS",
    "typescript" : "setLCDMode(mode?: LCDMode): void;"
}
This function can be used to change the way graphics is handled on Bangle.js.

Available options for `Bangle.setLCDMode` are:

* `Bangle.setLCDMode()` or `Bangle.setLCDMode("direct")` (the default) - The
  drawable area is 240x240 16 bit. Unbuffered, so draw calls take effect
  immediately. Terminal and vertical scrolling work (horizontal scrolling
  doesn't).
* `Bangle.setLCDMode("doublebuffered")` - The drawable area is 240x160 16 bit,
  terminal and scrolling will not work. `g.flip()` must be called for draw
  operations to take effect.
* `Bangle.setLCDMode("120x120")` - The drawable area is 120x120 8 bit,
  `g.getPixel`, terminal, and full scrolling work. Uses an offscreen buffer
  stored on Bangle.js, `g.flip()` must be called for draw operations to take
  effect.
* `Bangle.setLCDMode("80x80")` - The drawable area is 80x80 8 bit, `g.getPixel`,
  terminal, and full scrolling work. Uses an offscreen buffer stored on
  Bangle.js, `g.flip()` must be called for draw operations to take effect.

You can also call `Bangle.setLCDMode()` to return to normal, unbuffered
`"direct"` mode.
*/
__attribute__((weak)) void jswrap_banglejs_setLCDMode(JsVar *mode) {
  jsExceptionHere(JSET_ERROR, "setLCDMode is unsupported on this device");
}



/*Each display will need to support its own name's*/
__attribute__((weak)) void getLCDModeName(const char *name){}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "getLCDMode",
    "generate" : "jswrap_banglejs_getLCDMode",
    "return" : ["JsVar","The LCD mode as a String"],
    "ifdef" : "BANGLEJS",
    "typescript" : "getLCDMode(): LCDMode;"
}
The current LCD mode.

See `Bangle.setLCDMode` for examples.
*/
JsVar *jswrap_banglejs_getLCDMode() {
  const char *name=0;
  getLCDModeName(name);
  if (!name) return 0;
  return jsvNewFromString(name);
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "setLCDOffset",
    "generate" : "jswrap_banglejs_setLCDOffset",
    "params" : [
      ["y","int","The amount of pixels to shift the LCD up or down"]
    ],
    "ifdef" : "BANGLEJS"
}
This can be used to move the displayed memory area up or down temporarily. It's
used for displaying notifications while keeping the main display contents
intact.
*/
__attribute__((weak)) void jswrap_banglejs_setLCDOffset(int y) {}


__attribute__((weak)) void setDefaultSetLCDOverlay(JsVar *imgVar, int x, int y){}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "setLCDOverlay",
    "generate" : "jswrap_banglejs_setLCDOverlay",
    "params" : [
      ["img","JsVar","An image, or undefined to clear"],
      ["x","int","The X offset the graphics instance should be overlaid on the screen with"],
      ["y","int","The Y offset the graphics instance should be overlaid on the screen with"]
    ],
    "#if" : "defined(BANGLEJS_Q3) || defined(DICKENS)",
    "typescript" : [
      "setLCDOverlay(img: any, x: number, y: number): void;",
      "setLCDOverlay(): void;"
    ]
}
Overlay an image or graphics instance on top of the contents of the graphics buffer.

This only works on Bangle.js 2 because Bangle.js 1 doesn't have an offscreen buffer accessible from the CPU.

```
// display an alarm clock icon on the screen
var img = require("heatshrink").decompress(atob(`lss4UBvvv///ovBlMyqoADv/VAwlV//1qtfAQX/BINXDoPVq/9DAP
/AYIKDrWq0oREAYPW1QAB1IWCBQXaBQWq04WCAQP6BQeqA4P1AQPq1WggEK1WrBAIkBBQJsCBYO///fBQOoPAcqCwP3BQnwgECCwP9
GwIKCngWC14sB7QKCh4CBCwN/64KDgfACwWn6vWGwYsBCwOputWJgYsCgGqytVBQYsCLYOlqtqwAsFEINVrR4BFgghBBQosDEINWIQ
YsDEIQ3DFgYhCG4msSYeVFgnrFhMvOAgsEkE/FhEggYWCFgIhDkEACwQKBEIYKBCwSGFBQJxCQwYhBBQTKDqohCBQhCCEIJlDXwrKE
BQoWHBQdaCwuqJoI4CCwgKECwJ9CJgIKDq+qBYUq1WtBQf+BYIAC3/VBQX/tQKDz/9BQY5BAAVV/4WCBQJcBKwVf+oHBv4wCAAYhB`));
Bangle.setLCDOverlay(img,66,66);
```

Or use a `Graphics` instance:

```
var ovr = Graphics.createArrayBuffer(100,100,1,{msb:true}); // 1bpp
ovr.drawLine(0,0,100,100);
ovr.drawRect(0,0,99,99);
Bangle.setLCDOverlay(ovr,38,38);
```

Although `Graphics` can be specified directly, it can often make more sense to
create an Image from the `Graphics` instance, as this gives you access
to color palettes and transparent colors. For instance this will draw a colored
overlay with rounded corners:

```
var ovr = Graphics.createArrayBuffer(100,100,2,{msb:true});
ovr.setColor(1).fillRect({x:0,y:0,w:99,h:99,r:8});
ovr.setColor(3).fillRect({x:2,y:2,w:95,h:95,r:7});
ovr.setColor(2).setFont("Vector:30").setFontAlign(0,0).drawString("Hi",50,50);
Bangle.setLCDOverlay({
  width:ovr.getWidth(), height:ovr.getHeight(),
  bpp:2, transparent:0,
  palette:new Uint16Array([0,0,g.toColor("#F00"),g.toColor("#FFF")]),
  buffer:ovr.buffer
},38,38);
```
*/
void jswrap_banglejs_setLCDOverlay(JsVar *imgVar, int x, int y) {
  setDefaultSetLCDOverlay(imgVar, x, y);
  // set all as modified
  // TODO: Could look at old vs new overlay state and update only lines that had changed?
  graphicsInternal.data.modMinX = 0;
  graphicsInternal.data.modMinY = 0;
  graphicsInternal.data.modMaxX = LCD_WIDTH-1;
  graphicsInternal.data.modMaxY = LCD_HEIGHT-1;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "isLCDOn",
    "generate" : "jswrap_banglejs_isLCDOn",
    "return" : ["bool","Is the display on or not?"],
    "ifdef" : "BANGLEJS"
}
Also see the `Bangle.lcdPower` event
*/
// emscripten bug means we can't use 'bool' as return value here!
int jswrap_banglejs_isLCDOn() {
  return (bangleFlags&JSBF_LCD_ON)!=0;
}


__attribute__((weak)) void graphicHwSetup(JsGraphics * gInternal){}
#include "bangle_lcd.h"

extern volatile JsBangleFlags bangleFlags;

static void jswrap_banglejs_setLCDPowerController(bool isOn) {
#ifdef LCD_CONTROLLER_LPM013M126
  jshPinOutput(LCD_EXTCOMIN, 0);
  jshPinOutput(LCD_DISP, isOn); // enable
#endif
#ifdef LCD_CONTROLLER_ST7789_8BIT
  if (isOn) { // wake
    lcdST7789_cmd(0x11, 0, NULL); // SLPOUT
    jshDelayMicroseconds(20);
    lcdST7789_cmd(0x29, 0, NULL); // DISPON
  } else { // sleep
    lcdST7789_cmd(0x28, 0, NULL); // DISPOFF
    jshDelayMicroseconds(20);
    lcdST7789_cmd(0x10, 0, NULL); // SLPIN
  }
#endif
#if defined(LCD_CONTROLLER_ST7789V) || defined(LCD_CONTROLLER_ST7735) || defined(LCD_CONTROLLER_GC9A01)
  // TODO: LCD_CONTROLLER_GC9A01 - has an enable/power pin
  if (isOn) { // wake
    lcdCmd_SPILCD(0x11, 0, NULL); // SLPOUT
    jshDelayMicroseconds(5000);   // For GC9A01, we should wait 5ms after SLPOUT for power supply and clocks to stabilise before sending next command
    lcdCmd_SPILCD(0x29, 0, NULL); // DISPON
  } else { // sleep
    lcdCmd_SPILCD(0x28, 0, NULL); // DISPOFF
    jshDelayMicroseconds(20);
    lcdCmd_SPILCD(0x10, 0, NULL); // SLPIN - for GC9A01, it takeds 120ms to get into sleep mode after sending SPLIN
  }
#endif
#ifdef LCD_EN
  jshPinOutput(LCD_EN,isOn); // enable off
#endif
}


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
#ifdef LCD_CONTROLLER_ST7789_8BIT
  lcdST7789_cmd(cmd, dLen, (const uint8_t *)dPtr);
#endif
#if defined(LCD_CONTROLLER_ST7789V) || defined(LCD_CONTROLLER_ST7735) || defined(LCD_CONTROLLER_GC9A01)
  lcdCmd_SPILCD(cmd, dLen, (const uint8_t *)dPtr);
#endif
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
#ifdef BANGLEJS_F18
  app_timer_stop(m_backlight_on_timer_id);
  app_timer_stop(m_backlight_off_timer_id);
  if (isOn) { // wake
    if (lcdBrightness > 0) {
      if (lcdBrightness < 255) { //  only do PWM if brightness isn't full
        app_timer_start(m_backlight_on_timer_id, APP_TIMER_TICKS(BACKLIGHT_PWM_INTERVAL, APP_TIMER_PRESCALER), NULL);
      } else // full brightness
        jswrap_banglejs_pwrBacklight(true); // backlight on
    } else { // lcdBrightness == 0
      jswrap_banglejs_pwrBacklight(false); // backlight off
    }
  } else { // sleep
    jswrap_banglejs_pwrBacklight(false); // backlight off
  }
#elif defined(ESPR_BACKLIGHT_FADE)
  if (!lcdFadeHandlerActive) {
    JsSysTime t = jshGetTimeFromMilliseconds(10);
    jstExecuteFn(backlightFadeHandler, NULL, t, t, NULL);
    lcdFadeHandlerActive = true;
    backlightFadeHandler();
  }
#else // the default backlight mode (on Bangle.js 2/others)
  jswrap_banglejs_pwrBacklight(isOn && (lcdBrightness>0));
#ifdef LCD_BL
  if (isOn && lcdBrightness > 0 && lcdBrightness < 255) {
    jshPinAnalogOutput(LCD_BL, lcdBrightness/256.0, 200, JSAOF_NONE);
  }
#endif // LCD_BL
#endif
#endif // !EMULATED
}

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
void jswrap_banglejs_setLCDMode(JsVar *mode) {
#ifdef LCD_CONTROLLER_ST7789_8BIT
  LCDST7789Mode lcdMode = LCDST7789_MODE_UNBUFFERED;
  if (jsvIsUndefined(mode) || jsvIsStringEqual(mode,"direct"))
    lcdMode = LCDST7789_MODE_UNBUFFERED;
  else if (jsvIsStringEqual(mode,"null"))
    lcdMode = LCDST7789_MODE_NULL;
  else if (jsvIsStringEqual(mode,"doublebuffered"))
    lcdMode = LCDST7789_MODE_DOUBLEBUFFERED;
  else if (jsvIsStringEqual(mode,"120x120"))
    lcdMode = LCDST7789_MODE_BUFFER_120x120;
  else if (jsvIsStringEqual(mode,"80x80"))
    lcdMode = LCDST7789_MODE_BUFFER_80x80;
  else
    jsExceptionHere(JSET_ERROR,"Unknown LCD Mode %j",mode);

  JsVar *graphics = jsvObjectGetChildIfExists(execInfo.hiddenRoot, JS_GRAPHICS_VAR);
  if (!graphics) return;
  jswrap_graphics_setFont(graphics, NULL, 1); // reset fonts - this will free any memory associated with a custom font
  // remove the buffer if it was defined
  jsvObjectSetOrRemoveChild(graphics, "buffer", 0);
  unsigned int bufferSize = 0;
  switch (lcdMode) {
    case LCDST7789_MODE_NULL:
    case LCDST7789_MODE_UNBUFFERED:
      graphicsInternal.data.width = LCD_WIDTH;
      graphicsInternal.data.height = LCD_HEIGHT;
      graphicsInternal.data.bpp = 16;
      break;
    case LCDST7789_MODE_DOUBLEBUFFERED:
      graphicsInternal.data.width = LCD_WIDTH;
      graphicsInternal.data.height = 160;
      graphicsInternal.data.bpp = 16;
      break;
    case LCDST7789_MODE_BUFFER_120x120:
      graphicsInternal.data.width = 120;
      graphicsInternal.data.height = 120;
      graphicsInternal.data.bpp = 8;
      bufferSize = 120*120;
      break;
    case LCDST7789_MODE_BUFFER_80x80:
      graphicsInternal.data.width = 80;
      graphicsInternal.data.height = 80;
      graphicsInternal.data.bpp = 8;
      bufferSize = 80*80;
      break;
  }
  if (bufferSize) {
    jsvGarbageCollect();
    jsvDefragment();
    JsVar *arrData = jsvNewFlatStringOfLength(bufferSize);
    if (arrData) {
      jsvObjectSetChildAndUnLock(graphics, "buffer", jsvNewArrayBufferFromString(arrData, (unsigned int)bufferSize));
    } else {
      jsExceptionHere(JSET_ERROR, "Not enough memory to allocate offscreen buffer");
      jswrap_banglejs_setLCDMode(0); // go back to default mode
      return;
    }
    jsvUnLock(arrData);
  }
  graphicsStructResetState(&graphicsInternal); // reset colour, cliprect, etc
  jsvUnLock(graphics);
  lcdST7789_setMode( lcdMode );
  graphicsSetCallbacks(&graphicsInternal); // set the callbacks up after the mode change
#else
  jsExceptionHere(JSET_ERROR, "setLCDMode is unsupported on this device");
#endif
}

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
#ifdef LCD_CONTROLLER_ST7789_8BIT
  switch (lcdST7789_getMode()) {
    case LCDST7789_MODE_NULL:
      name = "null";
      break;
    case LCDST7789_MODE_UNBUFFERED:
      name = "direct";
      break;
    case LCDST7789_MODE_DOUBLEBUFFERED:
      name = "doublebuffered";
      break;
    case LCDST7789_MODE_BUFFER_120x120:
      name = "120x120";
      break;
    case LCDST7789_MODE_BUFFER_80x80:
      name = "80x80";
      break;
  }
#endif
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
void jswrap_banglejs_setLCDOffset(int y) {
#ifdef LCD_CONTROLLER_ST7789_8BIT
  lcdST7789_setYOffset(y);
#endif
}

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
#ifdef LCD_CONTROLLER_LPM013M126
  lcdMemLCD_setOverlay(imgVar, x, y);
#endif
#if defined(LCD_CONTROLLER_ST7789V) || defined(LCD_CONTROLLER_ST7735) || defined(LCD_CONTROLLER_GC9A01)
  //haven't got to this part yet
  //lcdSetOverlay_SPILCD(imgVar, x, y);
#endif
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

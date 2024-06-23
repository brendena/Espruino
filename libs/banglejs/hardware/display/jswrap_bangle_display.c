#include "bangle_display_impl.h"
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
  banglejs_lcdWr_impl(cmd, dLen, (const uint8_t *)dPtr);
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
#ifdef ESPR_BACKLIGHT_FADE
  if (isOn) banglejs_setLCDPowerController_impl(1);
  else jswrap_banglejs_setLCDPowerBacklight(0); // RB: don't turn on the backlight here if fading is enabled
#else
  banglejs_setLCDPowerController_impl(isOn);
  jswrap_banglejs_setLCDPowerBacklight(isOn);
#endif


  if (((bangleFlags&JSBF_LCD_ON)!=0) != isOn) {
    JsVar *bangle =jsvObjectGetChildIfExists(execInfo.root, "Bangle");
    if (bangle) {
      JsVar *v = jsvNewFromBool(isOn);
      jsiQueueObjectCallbacks(bangle, JS_EVENT_PREFIX"lcdPower", &v, 1);
      jsvUnLock(v);
    }
    jsvUnLock(bangle);
  }

  if (isOn) {
    // programatically on counts as wake
    if (lcdPowerTimeout > 0 || backlightTimeout > 0) inactivityTimer = 0;
    bangleFlags |= JSBF_LCD_ON;
  }
  else {
    bangleFlags &= ~JSBF_LCD_ON;
    // ensure screen locks if programatically switch off
    if (lockTimeout > 0 && lockTimeout <= lcdPowerTimeout) _jswrap_banglejs_setLocked(true, "lcd");
  }
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
  banglejs_setLCDMode_impl(mode);
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
  const char *name= banglejs_getLCDMode_impl();
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
  banglejs_setLCDOffset_impl(y);
}



/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "setLCDOverlay",
    "generate" : "jswrap_banglejs_setLCDOverlay",
    "params" : [
      ["img","JsVar","An image, or undefined to clear"],
      ["x","JsVar","The X offset the graphics instance should be overlaid on the screen with"],
      ["y","int","The Y offset the graphics instance should be overlaid on the screen with"],
      ["options","JsVar","[Optional] object `{remove:fn, id:\"str\"}`"]
    ],
    "#if" : "defined(BANGLEJS_Q3) || defined(DICKENS)",
    "typescript" : [
      "setLCDOverlay(img: any, x: number, y: number): void;",
      "setLCDOverlay(): void;",
      "setLCDOverlay(img: any, x: number, y: number, options: { id : string, remove: () => void }): void;",
      "setLCDOverlay(img: any, options: { id : string }}): void;"
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
Bangle.setLCDOverlay(img,66,66, {id: "myOverlay", remove: () => print("Removed")});
```

Or use a `Graphics` instance:

```
var ovr = Graphics.createArrayBuffer(100,100,2,{msb:true});
ovr.transparent = 0; // (optional) set a transparent color
ovr.palette = new Uint16Array([0,0,g.toColor("#F00"),g.toColor("#FFF")]); // (optional) set a color palette
ovr.setColor(1).fillRect({x:0,y:0,w:99,h:99,r:8});
ovr.setColor(3).fillRect({x:2,y:2,w:95,h:95,r:7});
ovr.setColor(2).setFont("Vector:30").setFontAlign(0,0).drawString("Hi",50,50);
Bangle.setLCDOverlay(ovr,38,38, {id: "myOverlay", remove: () => print("Removed")});
```

To remove an overlay, simply call:

```
Bangle.setLCDOverlay(undefined, {id: "myOverlay"});
```

Before 2v22 the `options` object isn't parsed, and as a result
the remove callback won't be called, and `Bangle.setLCDOverlay(undefined)` will
remove *any* active overlay.

The `remove` callback is called when the current overlay is removed or replaced with
another, but *not* if setLCDOverlay is called again with an image and the same ID.
*/
void jswrap_banglejs_setLCDOverlay(JsVar *imgVar, JsVar *xv, int y, JsVar *options) {
  bool removingOverlay = jsvIsUndefined(imgVar);
  int x = jsvGetInteger(xv);
  if (removingOverlay) // handle Bangle.setLCDOverlay(undefined, {id: "myOverlay"});
    options = xv;
  JsVar *id = 0;
  if (jsvIsObject(options)) {
    id = jsvObjectGetChildIfExists(options, "id");
  }
  JsVar *ovrId = jsvObjectGetChildIfExists(execInfo.hiddenRoot, "lcdOvrId");
  bool idIsDifferent = !jsvIsEqual(id, ovrId);
  jsvUnLock(ovrId);
  // if we are removing overlay, and supplied an ID, and it's different to the current one, don't do anything
  if (removingOverlay && id && idIsDifferent) {
    jsvUnLock(id);
    return;
  }
  // We're definitely changing overlay now... run the remove callback if it exists and the ID is different
  if (idIsDifferent) {
    JsVar *removeCb = jsvObjectGetChildIfExists(execInfo.hiddenRoot, "lcdOvrCb");
    if (removeCb) {
      jsiQueueEvents(0, removeCb, NULL, 0);
      jsvUnLock(removeCb);
    }
  }
  // update the fields in the Bangle object
  if (imgVar) {
    jsvObjectSetOrRemoveChild(execInfo.hiddenRoot, "lcdOvrId", id);
    JsVar *removeCb = jsvIsObject(options) ? jsvObjectGetChildIfExists(options, "remove") : NULL;
    jsvObjectSetOrRemoveChild(execInfo.hiddenRoot, "lcdOvrCb", removeCb);
    jsvUnLock(removeCb);
  } else { // we're removing the overlay, remove all callbacks
    jsvObjectRemoveChild(execInfo.hiddenRoot, "lcdOvrId");
    jsvObjectRemoveChild(execInfo.hiddenRoot, "lcdOvrCb");
  }
  jsvUnLock(id);
  banglejs_setLCDOverlay_impl(imgVar, x, y);
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

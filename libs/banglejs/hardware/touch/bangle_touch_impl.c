#include "bangle_touch_impl.h"

TouchGestureType touchGesture; /// is JSBT_SWIPE is set, what happened?
/// What state was the touchscreen last in
TouchState touchLastState; /// What happened in the last event?
TouchState touchLastState2; /// What happened in the event before last?
TouchState touchStatus; ///< What has happened *while the current touch is in progress

JshI2CInfo i2cTouch;
#define TOUCH_I2C &i2cTouch

#ifdef TOUCH_DEVICE

short touchX, touchY; ///< current touch event coordinates
short lastTouchX, lastTouchY; ///< last touch event coordinates - updated when JSBT_DRAG is fired
bool touchPts, lastTouchPts; ///< whether a fnger is currently touching or not
unsigned char touchType; ///< variable to differentiate press, long press, double press
short touchMinX = 0, touchMinY = 0, touchMaxX = 160, touchMaxY = 160; ///< touchscreen calibration values (what we expect from hardware, then we map this to LCD_WIDTH/HEIGHT)


// Convert Touchscreen gesture based on graphics orientation
TouchGestureType touchSwipeRotate(TouchGestureType g) {
  // gesture is the value that comes straight from the touchscreen
  if (graphicsInternal.data.flags & JSGRAPHICSFLAGS_INVERT_X) {
    if (g==TG_SWIPE_LEFT) g=TG_SWIPE_RIGHT;
    else if (g==TG_SWIPE_RIGHT) g=TG_SWIPE_LEFT;
  }
  if (graphicsInternal.data.flags & JSGRAPHICSFLAGS_INVERT_Y) {
    if (g==TG_SWIPE_UP) g=TG_SWIPE_DOWN;
    else if (g==TG_SWIPE_DOWN) g=TG_SWIPE_UP;
  }
  if (graphicsInternal.data.flags & JSGRAPHICSFLAGS_SWAP_XY) {
    if (g==TG_SWIPE_LEFT) g=TG_SWIPE_UP;
    else if (g==TG_SWIPE_RIGHT) g=TG_SWIPE_DOWN;
    else if (g==TG_SWIPE_UP) g=TG_SWIPE_LEFT;
    else if (g==TG_SWIPE_DOWN) g=TG_SWIPE_RIGHT;
  }
  return g;
}

void touchHandlerInternal(int tx, int ty, int pts, int gesture) {
  // ignore if locked
  if (bangleFlags & JSBF_LOCKED) return;
  // deal with the case where we rotated the Bangle.js screen
  deviceToGraphicsCoordinates(&graphicsInternal, &tx, &ty);

  int dx = tx-touchX;
  int dy = ty-touchY;

  touchX = tx;
  touchY = ty;
  touchPts = pts;
  JsBangleTasks lastBangleTasks = bangleTasks;
  static int lastGesture = 0;
  if (gesture!=lastGesture) {
    switch (gesture) { // gesture
    case 0:break; // no gesture
    case 1: // slide down
      touchGesture = touchSwipeRotate(TG_SWIPE_DOWN);
      bangleTasks |= JSBT_SWIPE;
      break;
    case 2: // slide up
      touchGesture = touchSwipeRotate(TG_SWIPE_UP);
      bangleTasks |= JSBT_SWIPE;
      break;
    case 3: // slide left
      touchGesture = touchSwipeRotate(TG_SWIPE_LEFT);
      bangleTasks |= JSBT_SWIPE;
      break;
    case 4: // slide right
      touchGesture = touchSwipeRotate(TG_SWIPE_RIGHT);
      bangleTasks |= JSBT_SWIPE;
      break;
    case 5: // single click
      if (touchX<80) bangleTasks |= JSBT_TOUCH_LEFT;
      else bangleTasks |= JSBT_TOUCH_RIGHT;
      touchType = 0;
      break;
    case 0x0B:     // double touch
      if (touchX<80) bangleTasks |= JSBT_TOUCH_LEFT;
      else bangleTasks |= JSBT_TOUCH_RIGHT;
      touchType = 1;
      break;
    case 0x0C:     // long touch
      if (touchX<80) bangleTasks |= JSBT_TOUCH_LEFT;
      else bangleTasks |= JSBT_TOUCH_RIGHT;
      touchType = 2;
      break;
    }
  }

  if (touchPts!=lastTouchPts || lastTouchX!=touchX || lastTouchY!=touchY) {
    bangleTasks |= JSBT_DRAG;
  #if ESPR_BANGLE_UNISTROKE
    if (unistroke_touch(touchX, touchY, dx, dy, touchPts)) {
      bangleTasks |= JSBT_STROKE;
    }
  #endif
  }
  // Ensure we process events if we modified bangleTasks
  if (lastBangleTasks != bangleTasks) {
    jshHadEvent();
    inactivityTimer = 0; // extend wake - only unlocked reaches here
  }
  lastGesture = gesture;
}

void banglejs_touch_hw_init_impl()
{
  jshI2CInitInfo(&i2cTouch);
  i2cTouch.bitrate = 0x7FFFFFFF; // make it as fast as we can go
  i2cTouch.pinSDA = TOUCH_PIN_SDA;
  i2cTouch.pinSCL = TOUCH_PIN_SCL;
  jsi2cSetup(&i2cTouch);
}




#ifdef TOUCH_I2C
void touchHandler(bool state, IOEventFlags flags) {
  if (state) return; // only interested in when low
  // Ok, now get touch info
  unsigned char buf[6];
  buf[0]=1;
  jsi2cWrite(TOUCH_I2C, TOUCH_ADDR, 1, buf, false);
  jsi2cRead(TOUCH_I2C, TOUCH_ADDR, 6, buf, true);

  // 0: Gesture type
  // 1: touch pts (0 or 1)
  // 2: Status / X hi (0x00 first, 0x80 pressed, 0x40 released)
  // 3: X lo (0..160)
  // 4: Y hi
  // 5: Y lo (0..160)

  int tx = buf[3]/* | ((buf[2] & 0x0F)<<8)*/; // top bits are never used on our touchscreen
  int ty = buf[5]/* | ((buf[4] & 0x0F)<<8)*/;
  if (tx>=250) tx=0; // on some devices, 251-255 gets reported for touches right at the top of the screen
  if (ty>=250) ty=0;
  touchHandlerInternal(
    (tx-touchMinX) * LCD_WIDTH / (touchMaxX-touchMinX), // touchX
    (ty-touchMinY) * LCD_HEIGHT / (touchMaxY-touchMinY), // touchY
    buf[1], // touchPts
    buf[0]); // gesture
}

#endif



#if defined(BANGLEJS_F18)
// returns true if handled and shouldn't create a normal watch event
bool btnTouchHandler() {
  if (bangleFlags&JSBF_WAKEON_TOUCH) {
    if (wakeUpBangle("touch"))
      return true; // eat the event
  }
  // if locked, ignore touch/swipe
  if (bangleFlags&JSBF_LOCKED) {
    touchLastState = touchLastState2 = touchStatus = TS_NONE;
    return false; // treat like a button
  }
  // unlocked
  JsBangleTasks lastBangleTasks = bangleTasks;
  // Detect touch/swipe
  TouchState state =
      (jshPinGetValue(BTN4_PININDEX)?TS_LEFT:0) |
      (jshPinGetValue(BTN5_PININDEX)?TS_RIGHT:0);
  touchStatus |= state;
  if ((touchLastState2==TS_RIGHT && touchLastState==TS_BOTH && state==TS_LEFT) ||
      (touchLastState==TS_RIGHT && state==1)) {
    touchStatus |= TS_SWIPED;
    touchGesture = TG_SWIPE_LEFT;
    bangleTasks |= JSBT_SWIPE;
  }
  if ((touchLastState2==TS_LEFT && touchLastState==TS_BOTH && state==TS_RIGHT) ||
      (touchLastState==TS_LEFT && state==TS_RIGHT)) {
    touchStatus |= TS_SWIPED;
    touchGesture = TG_SWIPE_RIGHT;
    bangleTasks |= JSBT_SWIPE;
  }
  if (!state) {
    if (touchLastState && !(touchStatus&TS_SWIPED)) {
      if (touchStatus&TS_LEFT) bangleTasks |= JSBT_TOUCH_LEFT;
      if (touchStatus&TS_RIGHT) bangleTasks |= JSBT_TOUCH_RIGHT;
    }
    touchStatus = TS_NONE;
  }
  if (lastBangleTasks != bangleTasks) inactivityTimer = 0;

  touchLastState2 = touchLastState;
  touchLastState = state;
  return false;
}
#endif


void banglejs_touch_idle_impl(JsVar *bangle )
{
    if (bangleTasks & JSBT_SWIPE) {
    JsVar *o[2] = {
        jsvNewFromInteger((touchGesture==TG_SWIPE_LEFT)?-1:((touchGesture==TG_SWIPE_RIGHT)?1:0)),
        jsvNewFromInteger((touchGesture==TG_SWIPE_UP)?-1:((touchGesture==TG_SWIPE_DOWN)?1:0)),
    };
    touchGesture = TG_SWIPE_NONE;
    jsiQueueObjectCallbacks(bangle, JS_EVENT_PREFIX"swipe", o, 2);
    jsvUnLockMany(2,o);
  }
  if (bangleTasks & JSBT_TOUCH_MASK) {
#ifdef TOUCH_DEVICE
    JsVar *o[2] = {
        jsvNewFromInteger(((bangleTasks & JSBT_TOUCH_LEFT)?1:0) |
                          ((bangleTasks & JSBT_TOUCH_RIGHT)?2:0)),
        jsvNewObject()
    };
    int x = touchX;
    int y = touchY;
    if (x<0) x=0;
    if (y<0) y=0;
    if (x>=LCD_WIDTH) x=LCD_WIDTH-1;
    if (y>=LCD_HEIGHT) y=LCD_HEIGHT-1;
    jsvObjectSetChildAndUnLock(o[1], "x", jsvNewFromInteger(x));
    jsvObjectSetChildAndUnLock(o[1], "y", jsvNewFromInteger(y));
    jsvObjectSetChildAndUnLock(o[1], "type", jsvNewFromInteger(touchType));
    jsiQueueObjectCallbacks(bangle, JS_EVENT_PREFIX"touch", o, 2);
    jsvUnLockMany(2,o);
#else
    JsVar *o = jsvNewFromInteger(((bangleTasks & JSBT_TOUCH_LEFT)?1:0) |
                                  ((bangleTasks & JSBT_TOUCH_RIGHT)?2:0));
    jsiQueueObjectCallbacks(bangle, JS_EVENT_PREFIX"touch", &o, 1);
    jsvUnLock(o);
#endif
  }


#ifdef TOUCH_DEVICE
  if (bangleTasks & JSBT_DRAG) {
    JsVar *o = jsvNewObject();
    jsvObjectSetChildAndUnLock(o, "x", jsvNewFromInteger(touchX));
    jsvObjectSetChildAndUnLock(o, "y", jsvNewFromInteger(touchY));
    jsvObjectSetChildAndUnLock(o, "b", jsvNewFromInteger(touchPts));
    jsvObjectSetChildAndUnLock(o, "dx", jsvNewFromInteger(lastTouchPts ? touchX-lastTouchX : 0));
    jsvObjectSetChildAndUnLock(o, "dy", jsvNewFromInteger(lastTouchPts ? touchY-lastTouchY : 0));
    jsiQueueObjectCallbacks(bangle, JS_EVENT_PREFIX"drag", &o, 1);
    jsvUnLock(o);
    lastTouchX = touchX;
    lastTouchY = touchY;
    lastTouchPts = touchPts;
  }
#endif
}

void banglejs_touch_init_impl()
{
  #ifdef BANGLEJS_Q3
  #ifndef EMULATED
    jshSetPinShouldStayWatched(TOUCH_PIN_IRQ,true);
    IOEventFlags channel = jshPinWatch(TOUCH_PIN_IRQ, true, JSPW_NONE);
    if (channel!=EV_NONE) jshSetEventCallback(channel, touchHandler);
  #endif
  #endif
  // touch
  touchStatus = TS_NONE;
  touchLastState = 0;
  touchLastState2 = 0;
}
#endif


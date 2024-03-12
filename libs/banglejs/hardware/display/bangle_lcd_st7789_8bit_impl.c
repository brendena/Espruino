#include "bangle_display_impl.h"

#ifdef LCD_CONTROLLER_ST7789_8BIT

void banglejs_lcdWr_impl(int cmd, int dLen, const uint8_t *dPtr){
    lcdST7789_cmd(cmd, dLen, (const uint8_t *)dPtr);
}

void banglejs_setLCDMode_impl(JsVar *mode)
{
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
}


char * banglejs_getLCDMode_impl()
{
  char *name =0;
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
  return name;
}

void banglejs_setLCDOffset_impl(int y)
{
  lcdST7789_setYOffset(y);
}

void banglejs_setLCDOverlay_impl(JsVar *imgVar, int x, int y)
{
  lcdSetOverlay_SPILCD(imgVar, x, y);
}

void banglejs_display_init_impl(JsGraphics *gfx){
  gfx->data.type = JSGRAPHICSTYPE_ST7789_8BIT;
  lcdST7789_init(gfx);
}
void graphicsInternalFlip()
{
  lcdST7789_flip(&graphicsInternal);
}

void banglejs_setLCDPowerController_impl(bool isOn) {
  if (isOn) { // wake
    lcdST7789_cmd(0x11, 0, NULL); // SLPOUT
    jshDelayMicroseconds(20);
    lcdST7789_cmd(0x29, 0, NULL); // DISPON
  } else { // sleep
    lcdST7789_cmd(0x28, 0, NULL); // DISPOFF
    jshDelayMicroseconds(20);
    lcdST7789_cmd(0x10, 0, NULL); // SLPIN
  }
}
//don't do anything in idle
void banglejs_display_idle_impl(){};
#endif
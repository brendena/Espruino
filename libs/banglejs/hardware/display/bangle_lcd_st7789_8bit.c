#ifdef LCD_CONTROLLER_ST7789_8BIT
void jswrap_banglejs_setLCDMode(JsVar *mode) {
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

void getLCDModeName(const char *name){
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
}

void setDefaultPowerController(bool isOn){
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

void setDefaultLcdWr(JsVarInt cmd,size_t dLen, const uint8_t * dPtr){
    lcdST7789_cmd(cmd, dLen, (const uint8_t *)dPtr);
}

void jswrap_banglejs_setLCDOffset(int y) {
  lcdST7789_setYOffset(y);
}

void graphicHwSetup(JsGraphics * gInternal){
    graphicsInternal.data.type = JSGRAPHICSTYPE_ST7789_8BIT;
    lcdST7789_init(&graphicsInternal);
}


#endif

#ifdef BANGLEJS_F18
void defaultPwrBacklight(unsigned char isOn, unsigned char lcdBrightness){
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
}


#endif
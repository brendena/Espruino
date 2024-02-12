#ifdef LCD_CONTROLLER_LPM013M126
void setDefaultPowerController(bool isOn){
  jshPinOutput(LCD_EXTCOMIN, 0);
  jshPinOutput(LCD_DISP, isOn); // enable
}

 void setDefaultSetLCDOverlay(JsVar *imgVar, int x, int y){
  lcdMemLCD_setOverlay(imgVar, x, y);
 }

 void graphicHwSetup(JsGraphics * gInternal){
    graphicsInternal.data.type = JSGRAPHICSTYPE_MEMLCD;
    lcdMemLCD_init(&graphicsInternal);
    jswrap_banglejs_pwrBacklight(true);
}

#endif
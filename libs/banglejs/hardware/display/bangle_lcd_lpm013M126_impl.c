#include "bangle_display_impl.h"

#ifdef LCD_CONTROLLER_LPM013M126

void banglejs_setLCDOverlay_impl(JsVar *imgVar, int x, int y)
{
  lcdMemLCD_setOverlay(imgVar, x, y);
}

void banglejs_display_init_impl(JsGraphics *gfx){
  gfx->data.type = JSGRAPHICSTYPE_MEMLCD;
  gfx->data.bpp = 16; // hack - so we can dither we pretend we're 16 bit
  lcdMemLCD_init(&graphicsInternal);
  banglejs_pwrBacklight_impl(true);
}

void banglejs_display_idle_impl(){
  lcdMemLCD_setOverlay(NULL, 0, 0);
}

void graphicsInternalFlip()
{
  lcdMemLCD_flip(&graphicsInternal);
}

void banglejs_setLCDPowerController_impl(bool isOn) {
  jshPinOutput(LCD_EXTCOMIN, 0);
  jshPinOutput(LCD_DISP, isOn); // enable
}

#endif
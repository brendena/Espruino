#include "bangle_display_impl.h"

ESPR_WEAK void banglejs_lcdWr_impl(int cmd, int dLen, const uint8_t *dPtr){}
ESPR_WEAK void banglejs_setLCDMode_impl(JsVar *mode)
{
  jsExceptionHere(JSET_ERROR, "setLCDMode is unsupported on this device");
}

ESPR_WEAK char * banglejs_getLCDMode_impl(){ return 0;}
ESPR_WEAK void banglejs_setLCDOffset_impl(int y){}
ESPR_WEAK void banglejs_setLCDOverlay_impl(JsVar *imgVar, int x, int y){}


ESPR_WEAK void banglejs_setLCDPowerController_impl(bool isOn) {
#ifdef LCD_EN
  jshPinOutput(LCD_EN,isOn); // enable off
#endif
}

ESPR_WEAK void graphicsInternalFlip(){}


ESPR_WEAK void banglejs_display_init_impl(JsGraphics *gfx){};
ESPR_WEAK void banglejs_display_idle_impl(){
  // Automatically flip!
  if (graphicsInternal.data.modMaxX >= graphicsInternal.data.modMinX) {
    graphicsInternalFlip();
  }
};
ESPR_WEAK void banglejs_display_kill_impl(){};

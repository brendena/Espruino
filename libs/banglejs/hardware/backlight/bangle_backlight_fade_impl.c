#include "bangle_backlight_impl.h"
#ifdef ESPR_BACKLIGHT_FADE

/// Actual LCD brightness (if we fade to a new brightness level)
uint8_t realLcdBrightness;
bool lcdFadeHandlerActive;

void banglejs_setLCDPowerBacklight_impl(bool isOn) {
  if (!lcdFadeHandlerActive) {
    JsSysTime t = jshGetTimeFromMilliseconds(10);
    jstExecuteFn(backlightFadeHandler, NULL, t, t, NULL);
    lcdFadeHandlerActive = true;
    backlightFadeHandler();
  }
}

void banglejs_backlight_init_impl(){
  realLcdBrightness = firstRun ? 0 : lcdBrightness;
  lcdFadeHandlerActive = false;
  jswrap_banglejs_setLCDPowerBacklight(bangleFlags & JSBF_LCD_BL_ON);
}
void banglejs_backlight_kill_impl(){
  if (lcdFadeHandlerActive) {
    jstStopExecuteFn(backlightFadeHandler, NULL);
    lcdFadeHandlerActive = false;
  }
}

void banglejs_backlight_idle_impl(){
  if (lcdFadeHandlerActive && realLcdBrightness == ((bangleFlags&JSBF_LCD_ON)?lcdBrightness:0)) 
  {
    jstStopExecuteFn(backlightFadeHandler, NULL);
    lcdFadeHandlerActive = false;
    if (!(bangleFlags&JSBF_LCD_ON))
    { 
      banglejs_setLCDPowerController_impl(0);
    }
  }
}


void backlightFadeHandler() {
  int target = (bangleFlags&JSBF_LCD_ON) ? lcdBrightness : 0;
  int brightness = realLcdBrightness;
  int step = brightness>>3; // to make this more linear
  if (step<4) step=4;
  if (target > brightness) {
    brightness += step;
    if (brightness > target)
      brightness = target;
  } else if (target < brightness) {
    brightness -= step;
    if (brightness < target)
      brightness = target;
  }
  realLcdBrightness = brightness;
  if (brightness==0) banglejs_pwrBacklight_impl(0);
  else if (realLcdBrightness==255) banglejs_pwrBacklight_impl(1);
  else {
    jshPinAnalogOutput(LCD_BL, realLcdBrightness/256.0, 200, JSAOF_NONE);
  }
}

static void backlightFadeHandler() {
  int target = (bangleFlags&JSBF_LCD_ON) ? lcdBrightness : 0;
  int brightness = realLcdBrightness;
  int step = brightness>>3; // to make this more linear
  if (step<4) step=4;
  if (target > brightness) {
    brightness += step;
    if (brightness > target)
      brightness = target;
  } else if (target < brightness) {
    brightness -= step;
    if (brightness < target)
      brightness = target;
  }
  realLcdBrightness = brightness;
  if (brightness==0) banglejs_pwrBacklight_impl(0);
  else if (realLcdBrightness==255) banglejs_pwrBacklight_impl(1);
  else {
    jshPinAnalogOutput(LCD_BL, realLcdBrightness/256.0, 200, JSAOF_NONE);
  }
}

#endif

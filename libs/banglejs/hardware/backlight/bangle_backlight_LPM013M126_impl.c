#include "bangle_backlight_impl.h"
#include "lcd_memlcd.h"
#ifdef LCD_CONTROLLER_LPM013M126

void banglejs_pwrBacklight_impl(bool on){
  #ifdef LCD_BL
    jshPinOutput(LCD_BL, on);
  #endif
  lcdMemLCD_extcominBacklight(on);
}

#endif
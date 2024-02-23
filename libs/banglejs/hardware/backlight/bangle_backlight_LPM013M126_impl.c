#include "bangle_backlight_impl.h"
#include "lcd_memlcd.h"
#ifdef LCD_CONTROLLER_LPM013M126

void banglejs_pwrBacklight_impl(bool on){
  lcdMemLCD_extcominBacklight(on);
}

#endif
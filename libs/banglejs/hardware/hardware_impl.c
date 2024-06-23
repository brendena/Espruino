//include all the backlight implementations
#ifndef EMULATED
//backlights
#include "backlight/bangle_backlight_f18_impl.c"
#include "backlight/bangle_backlight_fade_impl.c"
#include "backlight/bangle_backlight_LPM013M126_impl.c"
//display
#include "display/bangle_lcd_generic_spi_display_impl.c"
#include "display/bangle_lcd_lpm013M126_impl.c"
#include "display/bangle_lcd_st7789_8bit_impl.c"


#ifdef BANGLEJS_Q3
    #include "hardware/devices/device_bangle.c"
#endif

#endif
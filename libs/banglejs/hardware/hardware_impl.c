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

//accel
#include "accel/bangle_accel_KX023_impl.c"
#include "accel/bangle_accel_KXTJ3_1057_impl.c"
#include "accel/bangle_accel_KX126_impl.c"

//compass   
#include "compass/bangle_compase_UNKOWN_0C_impl.c"
#include "compass/bangle_compass_GMC303_impl.c"

#ifdef BANGLEJS_Q3
    #include "hardware/devices/device_bangle.c"
#endif

#endif
#include "jshardware.h"
#ifdef BANGLEJS_Q3
    #include "device_bangle2.h"
#endif

#define DEFAULT_ACCEL_POLL_INTERVAL 80 // in msec - 12.5 hz to match accelerometer
#define POWER_SAVE_ACCEL_POLL_INTERVAL 800 // in msec
#define POWER_SAVE_MIN_ACCEL 1638 // min acceleration before we exit power save... (8192*0.2)
#define POWER_SAVE_TIMEOUT 60000 // 60 seconds of inactivity
#define ACCEL_POLL_INTERVAL_MAX 4000 // in msec - DEFAULT_ACCEL_POLL_INTERVAL_MAX+TIMER_MAX must be <65535

#ifndef DEFAULT_BTN_LOAD_TIMEOUT
    #define DEFAULT_BTN_LOAD_TIMEOUT 1500 // in msec - how long does the button have to be pressed for before we restart
#endif
#define TIMER_MAX 60000 // 60 sec - enough to fit in uint16_t without overflow if we add ACCEL_POLL_INTERVAL
#ifndef DEFAULT_LCD_POWER_TIMEOUT
    #define DEFAULT_LCD_POWER_TIMEOUT 30000 // in msec - default for lcdPowerTimeout
#endif
#ifndef DEFAULT_BACKLIGHT_TIMEOUT
    #define DEFAULT_BACKLIGHT_TIMEOUT DEFAULT_LCD_POWER_TIMEOUT
#endif
#ifndef DEFAULT_LOCK_TIMEOUT
    #define DEFAULT_LOCK_TIMEOUT 30000 // in msec - default for lockTimeout
#endif
#ifndef DEFAULT_TWIST_THRESHOLD
    #define DEFAULT_TWIST_THRESHOLD 800
#endif
#ifndef DEFAULT_TWIST_MAXY
    #define DEFAULT_TWIST_MAXY -800
#endif
#ifndef WAKE_FROM_OFF_TIME
    #define WAKE_FROM_OFF_TIME 200
#endif

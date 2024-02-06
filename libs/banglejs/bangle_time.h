#include "jstimer.h"

/*
Things to remember that these need to be able to able to overight the defaults
*/

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

/// How often should we fire 'health' events?
#define HEALTH_INTERVAL 600000 // 10 minutes (600 seconds)



/// time since a button/touchscreen/etc was last pressed
extern volatile uint16_t inactivityTimer; // in ms
/// time since the Bangle's charge state was changed
extern volatile uint16_t chargeTimer; // in ms
/// How long has BTN1 been held down for (or TIMER_MAX is a reset has already happened)
extern volatile uint16_t homeBtnTimer; // in ms
/// How long has BTN1 been held down and watch hasn't reset (used to queue an interrupt)
extern volatile uint16_t homeBtnInterruptTimer; // in ms
/// How long does the home button have to be pressed before the default app is reloaded?
extern int btnLoadTimeout; // in ms
/// Is LCD power automatic? If true this is the number of ms for the timeout, if false it's 0
extern int lcdPowerTimeout; // in ms
/// Is LCD backlight automatic? If true this is the number of ms for the timeout, if false it's 0
extern int backlightTimeout; // in ms
/// Is locking automatic? If true this is the number of ms for the timeout, if false it's 0
extern int lockTimeout; // in ms
/// If a button was pressed to wake the LCD up, which one was it?
extern char lcdWakeButton;
/// If a button was pressed to wake the LCD up, when should we start accepting events for it?
extern JsSysTime lcdWakeButtonTime;
/// LCD Brightness - 255=full
extern uint8_t lcdBrightness;
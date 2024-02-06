#include "bangle_time.h"
#include "stdio.h"
volatile uint16_t inactivityTimer; // in ms
volatile uint16_t chargeTimer; // in ms
volatile uint16_t homeBtnTimer; // in ms
volatile uint16_t homeBtnInterruptTimer; // in ms
int btnLoadTimeout; // in ms
int lcdPowerTimeout; // in ms
int backlightTimeout; // in ms
int lockTimeout; // in ms
char lcdWakeButton;
JsSysTime lcdWakeButtonTime;
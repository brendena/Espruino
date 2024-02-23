#include "bangle_backlight_impl.h"

#if BANGLEJS_F18
APP_TIMER_DEF(m_backlight_on_timer_id);
APP_TIMER_DEF(m_backlight_off_timer_id);

void banglejs_setLCDPowerBacklight_impl(bool isOn) {
  app_timer_stop(m_backlight_on_timer_id);
  app_timer_stop(m_backlight_off_timer_id);
  if (isOn) { // wake
    if (lcdBrightness > 0) {
      if (lcdBrightness < 255) { //  only do PWM if brightness isn't full
        app_timer_start(m_backlight_on_timer_id, APP_TIMER_TICKS(BACKLIGHT_PWM_INTERVAL, APP_TIMER_PRESCALER), NULL);
      } else // full brightness
        banglejs_pwrBacklight_impl(true); // backlight on
    } else { // lcdBrightness == 0
      banglejs_pwrBacklight_impl(false); // backlight off
    }
  } else { // sleep
    banglejs_pwrBacklight_impl(false); // backlight off
  }
}

void banglejs_pwrBacklight_impl(bool on){
  jswrap_banglejs_ioWr(IOEXP_LCD_BACKLIGHT, !on);
}

void banglejs_backlight_init_impl(){
  uint32_t err_code;
  // Backlight PWM
  err_code = app_timer_create(&m_backlight_on_timer_id,
                        APP_TIMER_MODE_REPEATED,
                        backlightOnHandler);
  jsble_check_error(err_code);
  err_code = app_timer_create(&m_backlight_off_timer_id,
                      APP_TIMER_MODE_SINGLE_SHOT,
                      backlightOffHandler);
  jsble_check_error(err_code);
}
void banglejs_backlight_kill_impl(){
  app_timer_stop(m_backlight_on_timer_id);
  app_timer_stop(m_backlight_off_timer_id);
}

void backlightOnHandler() {
  if (i2cBusy) return;
  banglejs_pwrBacklight_impl(true); // backlight on
  app_timer_start(m_backlight_off_timer_id, APP_TIMER_TICKS(BACKLIGHT_PWM_INTERVAL, APP_TIMER_PRESCALER) * lcdBrightness >> 8, NULL);
}
void backlightOffHandler() {
  if (i2cBusy) return;
  banglejs_pwrBacklight_impl(false); // backlight off
}

#endif
#ifdef BANGLEJS_F18

void backlightOnHandler() {
  if (i2cBusy) return;
  jswrap_banglejs_pwrBacklight(true); // backlight on
  app_timer_start(m_backlight_off_timer_id, APP_TIMER_TICKS(BACKLIGHT_PWM_INTERVAL, APP_TIMER_PRESCALER) * lcdBrightness >> 8, NULL);
}
void backlightOffHandler() {
  if (i2cBusy) return;
  jswrap_banglejs_pwrBacklight(false); // backlight off
}


void backlight_init(){
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

void jswrap_banglejs_kill() {
  app_timer_stop(m_backlight_on_timer_id);
  app_timer_stop(m_backlight_off_timer_id);
}

#endif
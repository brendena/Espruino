#pragma once

#include "jsutils.h"
#include "jshardware.h"
#include "jswrap_graphics.h"
#include "lcd_spilcd_info.h"
#if defined(NRF52_SERIES)
  #include "nrf_gpio.h"
#endif


void lcdCmd_SPILCD(int cmd, int dataLen, const unsigned char *data);
void lcdSendInitCmd_SPILCD_buffer(unsigned char *cmd);
void lcdSendInitCmd_SPILCD();
void lcdSetPalette_SPILCD(const char *pal);
void lcdInit_SPILCD(JsGraphics *gfx);
#include "bangle_display_impl.h"
ESPR_WEAK void banglejs_lcdWr_impl(int cmd, int dLen, const uint8_t *dPtr){}

ESPR_WEAK void banglejs_setLCDMode_impl(JsVar *mode)
{
  jsExceptionHere(JSET_ERROR, "setLCDMode is unsupported on this device");
}

ESPR_WEAK char * banglejs_getLCDMode_impl()
{
  return 0;
}

ESPR_WEAK void banglejs_setLCDOffset_impl(int y){}

ESPR_WEAK void banglejs_setLCDOverlay_impl(JsVar *imgVar, int x, int y){}


ESPR_WEAK void banglejs_setLCDPowerController_impl(bool isOn) {
#ifdef LCD_CONTROLLER_LPM013M126
  jshPinOutput(LCD_EXTCOMIN, 0);
  jshPinOutput(LCD_DISP, isOn); // enable
#endif
#ifdef LCD_CONTROLLER_ST7789_8BIT
  if (isOn) { // wake
    lcdST7789_cmd(0x11, 0, NULL); // SLPOUT
    jshDelayMicroseconds(20);
    lcdST7789_cmd(0x29, 0, NULL); // DISPON
  } else { // sleep
    lcdST7789_cmd(0x28, 0, NULL); // DISPOFF
    jshDelayMicroseconds(20);
    lcdST7789_cmd(0x10, 0, NULL); // SLPIN
  }
#endif
#if defined(LCD_CONTROLLER_ST7789V) || defined(LCD_CONTROLLER_ST7735) || defined(LCD_CONTROLLER_GC9A01)
  // TODO: LCD_CONTROLLER_GC9A01 - has an enable/power pin
  if (isOn) { // wake
    lcdCmd_SPILCD(0x11, 0, NULL); // SLPOUT
    jshDelayMicroseconds(5000);   // For GC9A01, we should wait 5ms after SLPOUT for power supply and clocks to stabilise before sending next command
    lcdCmd_SPILCD(0x29, 0, NULL); // DISPON
  } else { // sleep
    lcdCmd_SPILCD(0x28, 0, NULL); // DISPOFF
    jshDelayMicroseconds(20);
    lcdCmd_SPILCD(0x10, 0, NULL); // SLPIN - for GC9A01, it takeds 120ms to get into sleep mode after sending SPLIN
  }
#endif
#ifdef LCD_EN
  jshPinOutput(LCD_EN,isOn); // enable off
#endif
}

void graphicsInternalFlip()
{
#ifdef LCD_CONTROLLER_LPM013M126
  lcdMemLCD_flip(&graphicsInternal);
#endif
#ifdef LCD_CONTROLLER_ST7789_8BIT
  lcdST7789_flip(&graphicsInternal);
#endif
#if defined(LCD_CONTROLLER_ST7789V) || defined(LCD_CONTROLLER_ST7735) || defined(LCD_CONTROLLER_GC9A01)
  lcdFlip_SPILCD(&graphicsInternal);
#endif
}


ESPR_WEAK void banglejs_display_init_impl(JsGraphics *gfx){};
ESPR_WEAK void banglejs_display_idle_impl(){};
ESPR_WEAK void banglejs_display_kill_impl(){};
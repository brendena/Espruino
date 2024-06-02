#include "bangle_display_impl.h"

#if defined(LCD_CONTROLLER_ST7789V) || defined(LCD_CONTROLLER_ST7735) || defined(LCD_CONTROLLER_GC9A01)
#include "lcd_spilcd.h"


void banglejs_lcdWr_impl(int cmd, int dLen, const uint8_t *dPtr){
  lcdCmd_SPILCD(cmd, dLen, dPtr);
}

void banglejs_display_init_impl(JsGraphics *gfx){
  gfx->data.type = JSGRAPHICSTYPE_SPILCD;
  lcdInit_SPILCD(&graphicsInternal);
}

void banglejs_display_kill_impl(){
  lcdSetOverlay_SPILCD(NULL, 0, 0);
}
void graphicsInternalFlip()
{
  lcdFlip_SPILCD(&graphicsInternal);
}

void banglejs_setLCDPowerController_impl(bool isOn) {
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
}
#endif
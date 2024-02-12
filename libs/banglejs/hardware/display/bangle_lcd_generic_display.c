#if defined(LCD_CONTROLLER_ST7789V) || defined(LCD_CONTROLLER_ST7735) || defined(LCD_CONTROLLER_GC9A01)
#include "bangle_lcd.h"

void setDefaultPowerController(bool isOn){
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

void setDefaultLcdWr(JsVarInt cmd,size_t dLen, const uint8_t * dPtr){
    lcdCmd_SPILCD(cmd, dLen, dPtr);
}

void setDefaultSetLCDOverlay(JsVar *imgVar, int x, int y){
    //lcdSetOverlay_SPILCD(imgVar, x, y);
}

void graphicHwSetup(JsGraphics * gInternal){
#if defined(BANGLEJS_P8) 
    graphicsInternal.data.type = JSGRAPHICSTYPE_SPILCD;
#else
    gInternal->data.type = JSGRAPHICSTYPE_LCD_SPI_UNBUF;
#endif
    lcdInit_SPILCD(gInternal);
}

#endif


#pragma once

#include "pineTimeDevice.h"
#include "lcd_spilcd.h"

void setupBangleHw(JsGraphics * gInternal){
    // turn power on
    // setup screen
    gInternal->data.type = JSGRAPHICSTYPE_LCD_SPI_UNBUF;
    gInternal->data.width = 240;
    gInternal->data.height = 240;
    lcdInit_SPILCD(gInternal);
    
}

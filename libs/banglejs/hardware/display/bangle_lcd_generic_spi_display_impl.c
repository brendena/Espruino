#include "bangle_display_impl.h"

#if defined(LCD_CONTROLLER_ST7789V) || defined(LCD_CONTROLLER_ST7735) || defined(LCD_CONTROLLER_GC9A01)
void banglejs_lcdWr_impl(int cmd, int dLen, const uint8_t *dPtr){
  lcdCmd_SPILCD(cmd, dLen, (const uint8_t *)dPtr);
}

void banglejs_display_init_impl(JsGraphics *gfx){
  gfx->data.type = JSGRAPHICSTYPE_SPILCD;
  lcdInit_SPILCD(&graphicsInternal);
}

#endif
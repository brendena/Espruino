#pragma once
#include "bangle_defines.h"

void _jswrap_banglejs_i2cWr(JshI2CInfo *i2c, int i2cAddr, JsVarInt reg, JsVarInt data);
JsVar *_jswrap_banglejs_i2cRd(JshI2CInfo *i2c, int i2cAddr, JsVarInt reg, JsVarInt cnt);

void banglejs_i2cCmd_list_process(JshI2CInfo *inf, unsigned char address, ESPR_cmd_i2c* cmd,unsigned int len);
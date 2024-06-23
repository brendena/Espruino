#include "bangle_cpu_hardware_impl.h"
#include "bangle_state.h"
#include "jsi2c.h"
#ifndef EMULATED
void _jswrap_banglejs_i2cWr(JshI2CInfo *i2c, int i2cAddr, JsVarInt reg, JsVarInt data) {
  unsigned char buf[2];
  buf[0] = (unsigned char)reg;
  buf[1] = (unsigned char)data;
  i2cBusy = true;
  jsi2cWrite(i2c, i2cAddr, 2, buf, true);
  i2cBusy = false;
}

JsVar *_jswrap_banglejs_i2cRd(JshI2CInfo *i2c, int i2cAddr, JsVarInt reg, JsVarInt cnt) {
  if (cnt<0) cnt=0;
  unsigned char buf[128];
  if (cnt>(int)sizeof(buf)) cnt=sizeof(buf);
  buf[0] = (unsigned char)reg;
  i2cBusy = true;
  jsi2cWrite(i2c, i2cAddr, 1, buf, false);
  jsi2cRead(i2c, i2cAddr, (cnt==0)?1:cnt, buf, true);
  i2cBusy = false;
  if (cnt) {
    JsVar *ab = jsvNewArrayBufferWithData(cnt, buf);
    JsVar *d = jswrap_typedarray_constructor(ARRAYBUFFERVIEW_UINT8, ab,0,0);
    jsvUnLock(ab);
    return d;
  } else return jsvNewFromInteger(buf[0]);
}
#endif


void banglejs_i2cCmd_list_process(JshI2CInfo *inf, unsigned char address, ESPR_cmd_i2c* cmd, unsigned int len)
{
  for(unsigned int i =0; i < len; i++){
    ESPR_cmd_i2c* c = &cmd[i];
    if(c->type == ESPR_CMD_I2C_WRITE){
      _jswrap_banglejs_i2cWr(inf, address, c->reg, c->data);
    }else if(c->type == ESPR_CMD_I2C_SLEEP){
      jshDelayMicroseconds(c->data);
    }
  }
}
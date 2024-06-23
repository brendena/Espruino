#include "jswrap_bangle_accel.h"



/// accelerometer data. 8192 = 1G
Vector3 acc;
/// squared accelerometer magnitude
int accMagSquared;
/// magnitude of difference in accelerometer vectors since last reading
unsigned int accDiff;

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "accelWr",
    "generate" : "jswrap_banglejs_accelWr",
    "params" : [
      ["reg","int",""],
      ["data","int",""]
    ],
    "ifdef" : "BANGLEJS"
}
Writes a register on the accelerometer
*/
void jswrap_banglejs_accelWr(JsVarInt reg, JsVarInt data) {
  banglejs_accel_i2c_Wr(reg, data);
}


/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "accelRd",
    "generate" : "jswrap_banglejs_accelRd",
    "params" : [
      ["reg","int",""],
      ["cnt","int","If specified, returns an array of the given length (max 128). If not (or 0) it returns a number"]
    ],
    "return" : ["JsVar",""],
    "ifdef" : "BANGLEJS",
    "typescript" : [
      "accelRd(reg: number, cnt?: 0): number;",
      "accelRd(reg: number, cnt: number): number[];"
    ]
}
Reads a register from the accelerometer

**Note:** On Espruino 2v06 and before this function only returns a number (`cnt`
is ignored).
*/


JsVar *jswrap_banglejs_accelRd(JsVarInt reg, JsVarInt cnt) {
  return banglejs_accel_i2c_Rd(reg, cnt);
}


/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "getAccel",
    "generate" : "jswrap_banglejs_getAccel",
    "return" : ["JsVar","An object containing accelerometer readings (as below)"],
    "ifdef" : "BANGLEJS",
    "typescript" : "getAccel(): AccelData & { td: number };"
}
Get the most recent accelerometer reading. Data is in the same format as the
`Bangle.on('accel',` event.

* `x` is X axis (left-right) in `g`
* `y` is Y axis (up-down) in `g`
* `z` is Z axis (in-out) in `g`
* `diff` is difference between this and the last reading in `g` (calculated by
  comparing vectors, not magnitudes)
* `td` is the elapsed
* `mag` is the magnitude of the acceleration in `g`
*/
JsVar *jswrap_banglejs_getAccel() {
  JsVar *o = jsvNewObject();
  if (o) {
    jsvObjectSetChildAndUnLock(o, "x", jsvNewFromFloat(acc.x/8192.0));
    jsvObjectSetChildAndUnLock(o, "y", jsvNewFromFloat(acc.y/8192.0));
    jsvObjectSetChildAndUnLock(o, "z", jsvNewFromFloat(acc.z/8192.0));
    jsvObjectSetChildAndUnLock(o, "mag", jsvNewFromFloat(sqrt(accMagSquared)/8192.0));
    jsvObjectSetChildAndUnLock(o, "diff", jsvNewFromFloat(accDiff/8192.0));
  }
  return o;
}



/*TYPESCRIPT
type AccelData = {
  x: number;
  y: number;
  z: number;
  diff: number;
  mag: number;
};
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "accel",
  "params" : [["xyz","JsVar",""]],
  "ifdef" : "BANGLEJS",
  "typescript": "on(event: \"accel\", callback: (xyz: AccelData) => void): void;"
}
Accelerometer data available with `{x,y,z,diff,mag}` object as a parameter.

* `x` is X axis (left-right) in `g`
* `y` is Y axis (up-down) in `g`
* `z` is Z axis (in-out) in `g`
* `diff` is difference between this and the last reading in `g`
* `mag` is the magnitude of the acceleration in `g`

You can also retrieve the most recent reading with `Bangle.getAccel()`.
 */
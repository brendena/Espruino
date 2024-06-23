#include "jswrap_bangle_compass.h"
#include "bangle_state.h"
#include "jswrap_math.h"
#include "jswrap_bangle.h"

#ifdef MAG_I2C
// compass data
Vector3 mag, magmin, magmax;
bool magOnWhenCharging;

#endif // MAG_I2C
#ifdef MAG_DEVICE_GMC303
uint8_t magCalib[3]; // Magnetic Coefficient Registers - used to rescale the magnetometer values
#endif


/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "compassWr",
    "generate" : "jswrap_banglejs_compassWr",
    "params" : [
      ["reg","int",""],
      ["data","int",""]
    ],
    "ifdef" : "BANGLEJS"
}
Writes a register on the Magnetometer/Compass
*/
void jswrap_banglejs_compassWr(JsVarInt reg, JsVarInt data) {
    banglejs_compass_i2c_Wr(reg, data);
}



/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "compassRd",
    "generate" : "jswrap_banglejs_compassRd",
    "params" : [
      ["reg","int",""],
      ["cnt","int","If specified, returns an array of the given length (max 128). If not (or 0) it returns a number"]
    ],
    "return" : ["JsVar",""],
    "ifdef" : "BANGLEJS",
    "typescript" : [
      "compassRd(reg: number, cnt?: 0): number;",
      "compassRd(reg: number, cnt: number): number[];"
    ]
}
Read a register on the Magnetometer/Compass
*/
JsVar *jswrap_banglejs_compassRd(JsVarInt reg, JsVarInt cnt) {
  return banglejs_compass_i2c_Rd(reg, cnt);
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "resetCompass",
    "generate" : "jswrap_banglejs_resetCompass",
    "params" : [],
    "ifdef" : "BANGLEJS"
}
Resets the compass minimum/maximum values. Can be used if the compass isn't
providing a reliable heading any more.
*/
void jswrap_banglejs_resetCompass() {
#ifdef MAG_I2C
  mag.x = 0;
  mag.y = 0;
  mag.z = 0;
  magmin.x = 32767;
  magmin.y = 32767;
  magmin.z = 32767;
  magmax.x = -32768;
  magmax.y = -32768;
  magmax.z = -32768;
#endif
}


/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "isCompassOn",
    "generate" : "jswrap_banglejs_isCompassOn",
    "return" : ["bool","Is the Compass on?"],
    "ifdef" : "BANGLEJS"
}
Is the compass powered?

Set power with `Bangle.setCompassPower(...);`
*/
// emscripten bug means we can't use 'bool' as return value here!
int jswrap_banglejs_isCompassOn() {
  return (bangleFlags & JSBF_COMPASS_ON)!=0;
}



/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "getCompass",
    "generate" : "jswrap_banglejs_getCompass",
    "return" : ["JsVar","An object containing magnetometer readings (as below)"],
    "ifdef" : "BANGLEJS",
    "typescript" : "getCompass(): CompassData;"
}
Get the most recent Magnetometer/Compass reading. Data is in the same format as
the `Bangle.on('mag',` event.

Returns an `{x,y,z,dx,dy,dz,heading}` object

* `x/y/z` raw x,y,z magnetometer readings
* `dx/dy/dz` readings based on calibration since magnetometer turned on
* `heading` in degrees based on calibrated readings (will be NaN if magnetometer
  hasn't been rotated around 360 degrees).

**Note:** In 2v15 firmware and earlier the heading is inverted (360-heading). There's
a fix in the bootloader which will apply a fix for those headings, but old apps may
still expect an inverted value.

To get this event you must turn the compass on with `Bangle.setCompassPower(1)`.*/
JsVar *jswrap_banglejs_getCompass() {
#ifdef MAG_I2C
  JsVar *o = jsvNewObject();
  if (o) {
    jsvObjectSetChildAndUnLock(o, "x", jsvNewFromInteger(mag.x));
    jsvObjectSetChildAndUnLock(o, "y", jsvNewFromInteger(mag.y));
    jsvObjectSetChildAndUnLock(o, "z", jsvNewFromInteger(mag.z));
    int dx = mag.x - ((magmin.x+magmax.x)/2);
    int dy = mag.y - ((magmin.y+magmax.y)/2);
    int dz = mag.z - ((magmin.z+magmax.z)/2);
    jsvObjectSetChildAndUnLock(o, "dx", jsvNewFromInteger(dx));
    jsvObjectSetChildAndUnLock(o, "dy", jsvNewFromInteger(dy));
    jsvObjectSetChildAndUnLock(o, "dz", jsvNewFromInteger(dz));
    int cx = magmax.x-magmin.x;
    int cy = magmax.y-magmin.y;
    int c = cx*cx+cy*cy;
    double h = NAN;
    if (c>3000) { // only give a heading if we think we have valid data (eg enough magnetic field difference in min/max
      h = jswrap_math_atan2(dx,dy)*180/PI;
      if (h<0) h+=360;
      h = 360-h; // ensure heading matches with what we'd expect from a compass
    }
    jsvObjectSetChildAndUnLock(o, "heading", jsvNewFromFloat(h));
  }
  return o;
#else
  return 0;
#endif
}


/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "setCompassPower",
    "generate" : "jswrap_banglejs_setCompassPower",
    "params" : [
      ["isOn","bool","True if the Compass should be on, false if not"],
      ["appID","JsVar","A string with the app's name in, used to ensure one app can't turn off something another app is using"]
    ],
    "return" : ["bool","Is the Compass on?"],
    "ifdef" : "BANGLEJS",
    "typescript" : "setCompassPower(isOn: ShortBoolean, appID: string): boolean;"
}
Set the power to the Compass

When on, data is output via the `mag` event on `Bangle`:

```
Bangle.setCompassPower(true, "myapp");
Bangle.on('mag',print);
```

*When on, the compass draws roughly 2mA*
*/
bool jswrap_banglejs_setCompassPower(bool isOn, JsVar *appId) {
#ifdef MAG_I2C
  bool wasOn = bangleFlags & JSBF_COMPASS_ON;
  isOn = setDeviceRequested("Compass", appId, isOn);
  //jsiConsolePrintf("setCompassPower %d %d\n",wasOn,isOn);

  if (isOn) bangleFlags |= JSBF_COMPASS_ON;
  else bangleFlags &= ~JSBF_COMPASS_ON;

  if (isOn) {
    if (!wasOn) { // If it wasn't on before, reset
      banglejs_compass_on_impl();
    }
  } else { // !isOn -> turn off
    banglejs_compass_off_impl();
  }
  return isOn;
#else
  return false;
#endif
}



/*TYPESCRIPT
type CompassData = {
  x: number;
  y: number;
  z: number;
  dx: number;
  dy: number;
  dz: number;
  heading: number;
};
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "mag",
  "params" : [["xyz","JsVar",""]],
  "ifdef" : "BANGLEJS",
  "typescript" : "on(event: \"mag\", callback: (xyz: CompassData) => void): void;"
}
Magnetometer/Compass data available with `{x,y,z,dx,dy,dz,heading}` object as a
parameter

* `x/y/z` raw x,y,z magnetometer readings
* `dx/dy/dz` readings based on calibration since magnetometer turned on
* `heading` in degrees based on calibrated readings (will be NaN if magnetometer
  hasn't been rotated around 360 degrees).

**Note:** In 2v15 firmware and earlier the heading is inverted (360-heading). There's
a fix in the bootloader which will apply a fix for those headings, but old apps may
still expect an inverted value.

To get this event you must turn the compass on with `Bangle.setCompassPower(1)`.

You can also retrieve the most recent reading with `Bangle.getCompass()`.
 */
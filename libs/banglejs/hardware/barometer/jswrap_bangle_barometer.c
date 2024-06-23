#include "jswrap_bangle_barometer.h"
#include "bangle_state.h"
#include "jsparse.h"
#include "jswrap_bangle.h"
/// Promise when pressure is requested
JsVar *promisePressure; // return promise of getPressure() - when set, the next pressure reading will complete this promise
volatile uint16_t barometerOnTimer; // how long has the barometer been on?
double barometerPressure;
double barometerTemperature;
double barometerAltitude;


/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "barometerWr",
    "generate" : "jswrap_banglejs_barometerWr",
    "params" : [
      ["reg","int",""],
      ["data","int",""]
    ],
    "#if" : "defined(DTNO1_F5) || defined(BANGLEJS_Q3) || defined(DICKENS)"
}
Writes a register on the barometer IC
*/
void jswrap_banglejs_barometerWr(JsVarInt reg, JsVarInt data) {
  banglejs_barometer_i2c_Wr(reg, data);
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "barometerRd",
    "generate" : "jswrap_banglejs_barometerRd",
    "params" : [
      ["reg","int",""],
      ["cnt","int","If specified, returns an array of the given length (max 128). If not (or 0) it returns a number"]
    ],
    "return" : ["JsVar",""],
    "#if" : "defined(DTNO1_F5) || defined(BANGLEJS_Q3) || defined(DICKENS)",
    "typescript" : [
      "barometerRd(reg: number, cnt?: 0): number;",
      "barometerRd(reg: number, cnt: number): number[];"
    ]
}
Reads a register from the barometer IC
*/
JsVar *jswrap_banglejs_barometerRd(JsVarInt reg, JsVarInt cnt) {
  return banglejs_barometer_i2c_Rd(reg, cnt);
}


/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "isBarometerOn",
    "generate" : "jswrap_banglejs_isBarometerOn",
    "return" : ["bool","Is the Barometer on?"],
    "#if" : "defined(DTNO1_F5) || defined(BANGLEJS_Q3) || defined(DICKENS)"
}
Is the Barometer powered?

Set power with `Bangle.setBarometerPower(...);`
*/
// emscripten bug means we can't use 'bool' as return value here!
int jswrap_banglejs_isBarometerOn() {
  return (bangleFlags & JSBF_BAROMETER_ON)!=0;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "setBarometerPower",
    "generate" : "jswrap_banglejs_setBarometerPower",
    "params" : [
      ["isOn","bool","True if the barometer IC should be on, false if not"],
      ["appID","JsVar","A string with the app's name in, used to ensure one app can't turn off something another app is using"]
    ],
    "return" : ["bool","Is the Barometer on?"],
    "#if" : "defined(DTNO1_F5) || defined(BANGLEJS_Q3) || defined(DICKENS)",
    "typescript" : "setBarometerPower(isOn: ShortBoolean, appID: string): boolean;"
}
Set the power to the barometer IC. Once enabled, `Bangle.pressure` events are
fired each time a new barometer reading is available.

When on, the barometer draws roughly 50uA
*/
bool jswrap_banglejs_setBarometerPower(bool isOn, JsVar *appId) {
#ifdef PRESSURE_DEVICE
  bool wasOn = bangleFlags & JSBF_BAROMETER_ON;
  isOn = setDeviceRequested("Barom", appId, isOn);
  if (!isOn) bangleFlags &= ~JSBF_BAROMETER_ON; // if not on, change flag immediately to stop peripheralPollHandler
  int tries = 3;
  while (tries-- > 0) {
    if (isOn) {
      if (!wasOn) {
        barometerOnTimer = 0;
        banglejs_barometer_on_impl();
      } // wasOn
    } else { // !isOn -> turn off
      banglejs_barometer_off_impl();
    }
    // ensure we change the flag here so that peripheralPollHandler polls it now
    if (isOn) bangleFlags |= JSBF_BAROMETER_ON;
    if (!tries || !jspHasError()) return isOn; // return -  all is going correctly (or we tried a few times and failed)
    // we had an error (almost certainly I2C) - clear the error and try again hopefully
    jsvUnLock(jspGetException());
  }
  // only turn on
  if (isOn) bangleFlags |= JSBF_BAROMETER_ON;
  return isOn;
#else // PRESSURE_DEVICE
  return false;
#endif // PRESSURE_DEVICE
}


/*TYPESCRIPT
type PressureData = {
  temperature: number;
  pressure: number;
  altitude: number;
}
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "pressure",
  "params" : [["e","JsVar","An object containing `{temperature,pressure,altitude}`"]],
  "ifdef" : "BANGLEJS",
  "typescript" : "on(event: \"pressure\", callback: (e: PressureData) => void): void;"
}
When `Bangle.setBarometerPower(true)` is called, this event is fired containing
barometer readings.

Same format as `Bangle.getPressure()`
*/



/*JSON{
    "type" : "staticmethod",
    "class" : "Bangle",
    "name" : "getPressure",
    "generate" : "jswrap_banglejs_getPressure",
    "return" : ["JsVar","A promise that will be resolved with `{temperature, pressure, altitude}`"],
    "#if" : "defined(DTNO1_F5) || defined(BANGLEJS_Q3) || defined(DICKENS)",
    "typescript" : "getPressure(): Promise<PressureData> | undefined;"
}
Read temperature, pressure and altitude data. A promise is returned which will
be resolved with `{temperature, pressure, altitude}`.

If the Barometer has been turned on with `Bangle.setBarometerPower` then this
will return with the *next* reading as of 2v21 (or the existing reading on 2v20 or earlier). If the Barometer is off,
conversions take between 500-750ms.

Altitude assumes a sea-level pressure of 1013.25 hPa

If there's no pressure device (for example, the emulator),
this returns `undefined`, rather than a Promise.

```
Bangle.getPressure().then(d=>{
  console.log(d);
  // {temperature, pressure, altitude}
});
```
*/

JsVar *jswrap_banglejs_getPressure() {
#ifdef PRESSURE_DEVICE
  if (promisePressure) {
    jsExceptionHere(JSET_ERROR, "Conversion in progress");
    return 0;
  }
  promisePressure = jspromise_create();
  if (!promisePressure) return 0;
  // If barometer is already on, our promise is enough. jswrap_banglejs_idle (after peripheralPollHandler)
  // will see promisePressure and will resolve it when new data is available
  if (bangleFlags & JSBF_BAROMETER_ON)
    return jsvLockAgain(promisePressure);
  // Turning barometer on, will turn off in jswrap_banglejs_idle (after peripheralPollHandler)
  JsVar *id = jsvNewFromString("getPressure");
  jswrap_banglejs_setBarometerPower(1, id);
  jsvUnLock(id);
  /* Occasionally on some devices (https://github.com/espruino/Espruino/issues/2137)
  you can get an I2C error. This stops the error from being fired when getPressure
  is called and instead rejects the promise. */
  JsVar *exception = jspGetException();
  if (exception) {
    jspromise_reject(promisePressure, exception);
    jsvUnLock2(promisePressure, exception);
    JsVar *r = promisePressure;
    promisePressure = 0;
    return r;
  }

  bool hadError = jspHasError();
  if (hadError) {
    JsVar *msg = jsvNewFromString("I2C barometer error");
    JsVar *exception = jswrap_internalerror_constructor(msg);
    jspromise_reject(promisePressure, exception);
    jsvUnLock3(promisePressure, exception, msg);
    JsVar *r = promisePressure;
    promisePressure = 0;
    return r;
  }
  return jsvLockAgain(promisePressure);
#endif
  return NULL;
}



#ifdef PRESSURE_DEVICE
bool jswrap_banglejs_barometerPoll() {
  // if the Barometer hadn't been on long enough, don't try and get data
  int powerOnTimeout = 500;
#ifdef PRESSURE_DEVICE_BMP280_EN
  if (PRESSURE_DEVICE_BMP280_EN)
    powerOnTimeout = 750; // some devices seem to need this long to boot reliably
#endif
#ifdef PRESSURE_DEVICE_SPL06_007_EN
if (PRESSURE_DEVICE_SPL06_007_EN)
  powerOnTimeout = 400; // on SPL06 we may actually be leaving it *too long* before requesting data, and it starts to do another reading
#endif
  if (barometerOnTimer < TIMER_MAX)
    barometerOnTimer += pollInterval;
  if (barometerOnTimer < powerOnTimeout)
    return false;
  // Otherwise, poll it
  return banglejs_barometer_get_data_impl(&barometerAltitude,&barometerPressure,&barometerTemperature);
}

JsVar *jswrap_banglejs_getBarometerObject() {
  JsVar *o = jsvNewObject();
  if (o) {
    jsvObjectSetChildAndUnLock(o,"temperature", jsvNewFromFloat(barometerTemperature));
    jsvObjectSetChildAndUnLock(o,"pressure", jsvNewFromFloat(barometerPressure));
    jsvObjectSetChildAndUnLock(o,"altitude", jsvNewFromFloat(barometerAltitude));
//    jsvObjectSetChildAndUnLock(o,"SPL06", jsvNewFromBool(pressureSPL06Enabled));
//    jsvObjectSetChildAndUnLock(o,"BMP280", jsvNewFromBool(pressureBMP280Enabled));
  }
  return o;
}
#endif // PRESSURE_DEVICE

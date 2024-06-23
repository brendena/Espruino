#include "jswrap_bangle_barometer.h"

#ifdef PRESSURE_DEVICE_BMP280 

#include "lib/SPL06_1.h"
#ifdef PRESSURE_DEVICE_SPL06_007_EN
    #include "lib/SPL06_7.h"
#endif


void banglejs_barometer_on_impl()
{
#ifdef PRESSURE_DEVICE_SPL06_007_EN
    if (PRESSURE_DEVICE_SPL06_007_EN) 
    {
        SPL06_07_barometer_on_impl();
    }
#endif
    if (PRESSURE_DEVICE_BMP280_EN) 
    {
        SPL06_01_barometer_on_impl();
    }
};

void banglejs_barometer_off_impl()
{
#ifdef PRESSURE_DEVICE_SPL06_007_EN
    if (PRESSURE_DEVICE_SPL06_007_EN) 
    {
        SPL06_07_barometer_off_impl();
    }
#endif
    if (PRESSURE_DEVICE_BMP280_EN) 
    {
        SPL06_01_barometer_off_impl();
    }
};

bool banglejs_barometer_get_data_impl(double *altitude, double *pressure, double *temperature)
{

#ifdef PRESSURE_DEVICE_SPL06_007_EN
    if (PRESSURE_DEVICE_SPL06_007_EN) 
    {
        return SPL06_07_barometer_get_data_impl(altitude,pressure,temperature);
    }
#endif
    if (PRESSURE_DEVICE_BMP280_EN) 
    {
        return SPL06_01_barometer_get_data_impl(altitude,pressure,temperature);
    }
    return false;
};

void banglejs_barometer_device_init_impl()
{
#ifdef PRESSURE_DEVICE_SPL06_007_EN
    if (PRESSURE_DEVICE_SPL06_007_EN) 
    {
        SPL06_07_barometer_device_init_impl();
    }
#endif
    if (PRESSURE_DEVICE_BMP280_EN) 
    {
        SPL06_01_barometer_device_init_impl();
    }
}

#endif

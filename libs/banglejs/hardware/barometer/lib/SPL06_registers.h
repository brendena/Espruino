#pragma once

#define SPL06_PRSB2 0x00       ///< Pressure/temp data start
#define SPL06_PRSCFG 0x06      ///< Pressure config
#define SPL06_TMPCFG 0x07      ///< Temperature config
#define SPL06_MEASCFG 0x08     ///< Sensor status and config
#define SPL06_CFGREG 0x09      ///< FIFO config
#define SPL06_RESET 0x0C       ///< reset
#define SPL06_COEF_START 0x10  ///< Start of calibration coefficients
#define SPL06_COEF_NUM 18	     ///< Number of calibration coefficient registers
#define SPL06_8SAMPLES 3

/*
* Author: Hiroyuki Mino <omronsupportupm@omron.com>
* Copyright (c) 2019 Omron Electronic Components - Americas
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#pragma once

#include <iostream> //Required for c++ standard IO
#include "mraa/i2c.hpp" //Required for MRAA I2c operations
#include <interfaces/iTemperature.hpp> //Required to Temperature Unit functionality
#include <interfaces/iPressure.hpp> //Required to Pressure Unit functionality

/**
 * MACROS and enum
 */
#define OM2SMPB02E_DEFAULT_I2C_BUS_NUM                                 6
#define OM2SMPB02E_DEFAULT_I2C_ADDR                                    0x56
#define OM2SMPB02E_COEFFI_MAX_VALUE                             (UINT8)25
//#define DEBUG /* Debug print */

typedef     unsigned char        UINT8;      /* 8 bit Unsigned Integer */
typedef     int                  INT32;      /* 32 bit Signed Integer  */
typedef     unsigned int         UINT32;     /* 32 bit Signed Integer  */
typedef     char                 INT8;       /* 8 bit Signed Integer   */
typedef     double               DOUBLE;     /* 64 bit Floating-Point  */
typedef     unsigned short int   UINT16;     /* 16 bit Unsigned Integer */
typedef     short int            INT16;      /* 16 bit Signed Integer */
typedef     float                FLOAT;      /* 32 bit Floating-Point */
typedef     std::string          STRING;     /* string                */

/* Registers */
#define OM2SMPB02E_REGI2C_PRES_TXD2               0xF7
#define OM2SMPB02E_REGI2C_IO_SETUP                0xF5
#define OM2SMPB02E_REGI2C_CTRL_MEAS               0xF4
#define OM2SMPB02E_REGI2C_IIR                     0xF1
#define OM2SMPB02E_REGI2C_CHIP_ID                 0xD1
#define OM2SMPB02E_REGI2C_COEFS                   0xA0

/**
 * OM2SMPB02E_VAL_IOSETUP_STANDBY_VALUES enum.
 * An Enum contains io setup stand by values.
 */
typedef enum OM2SMPB02E_VAL_IOSETUP_STANDBY_VALUES{
    OM2SMPB02E_VAL_IOSETUP_STANDBY_0001MS = 0x00,
    OM2SMPB02E_VAL_IOSETUP_STANDBY_0005MS = 0x20,
    OM2SMPB02E_VAL_IOSETUP_STANDBY_0050MS = 0x40,
    OM2SMPB02E_VAL_IOSETUP_STANDBY_0250MS = 0x60,
    OM2SMPB02E_VAL_IOSETUP_STANDBY_0500MS = 0x80,
    OM2SMPB02E_VAL_IOSETUP_STANDBY_1000MS = 0xA0,
    OM2SMPB02E_VAL_IOSETUP_STANDBY_2000MS = 0xC0,
    OM2SMPB02E_VAL_IOSETUP_STANDBY_4000MS = 0xE0,
}OM2SMPB02E_VAL_IOSETUP_STANDBY_VALUES_T;

#define OM2SMPB02E_MASK_TEMPAVERAGE      ((UINT8)0xE0)
#define OM2SMPB02E_VAL_TEMPAVERAGE_00    ((UINT8)0x00)
#define OM2SMPB02E_VAL_TEMPAVERAGE_01    ((UINT8)0x20)
#define OM2SMPB02E_VAL_TEMPAVERAGE_02    ((UINT8)0x40)
#define OM2SMPB02E_VAL_TEMPAVERAGE_04    ((UINT8)0x60)
#define OM2SMPB02E_VAL_TEMPAVERAGE_08    ((UINT8)0x80)
#define OM2SMPB02E_VAL_TEMPAVERAGE_16    ((UINT8)0xA0)
#define OM2SMPB02E_VAL_TEMPAVERAGE_32    ((UINT8)0xC0)
#define OM2SMPB02E_VAL_TEMPAVERAGE_64    ((UINT8)0xE0)

#define OM2SMPB02E_MASK_PRESAVERAGE      ((UINT8)0x1C)
#define OM2SMPB02E_VAL_PRESAVERAGE_00    ((UINT8)0x00)
#define OM2SMPB02E_VAL_PRESAVERAGE_01    ((UINT8)0x04)
#define OM2SMPB02E_VAL_PRESAVERAGE_02    ((UINT8)0x08)
#define OM2SMPB02E_VAL_PRESAVERAGE_04    ((UINT8)0x0C)
#define OM2SMPB02E_VAL_PRESAVERAGE_08    ((UINT8)0x10)
#define OM2SMPB02E_VAL_PRESAVERAGE_16    ((UINT8)0x14)
#define OM2SMPB02E_VAL_PRESAVERAGE_32    ((UINT8)0x18)
#define OM2SMPB02E_VAL_PRESAVERAGE_64    ((UINT8)0x1C)

/**
 * OM2SMPB02E_VAL_MEASMODE_VALUES enum.
 * An Enum contains measurement mode values.
 */
typedef enum OM2SMPB02E_VAL_MEASMODE_VALUES{
    OM2SMPB02E_VAL_MEASMODE_HIGHSPEED      = (OM2SMPB02E_VAL_PRESAVERAGE_02 | OM2SMPB02E_VAL_TEMPAVERAGE_01),
    OM2SMPB02E_VAL_MEASMODE_LOWPOWER       = (OM2SMPB02E_VAL_PRESAVERAGE_04 | OM2SMPB02E_VAL_TEMPAVERAGE_01),
    OM2SMPB02E_VAL_MEASMODE_STANDARD       = (OM2SMPB02E_VAL_PRESAVERAGE_08 | OM2SMPB02E_VAL_TEMPAVERAGE_01),
    OM2SMPB02E_VAL_MEASMODE_HIGHACCURACY   = (OM2SMPB02E_VAL_PRESAVERAGE_16 | OM2SMPB02E_VAL_TEMPAVERAGE_02),
    OM2SMPB02E_VAL_MEASMODE_ULTRAHIGH      = (OM2SMPB02E_VAL_PRESAVERAGE_32 | OM2SMPB02E_VAL_TEMPAVERAGE_04),
}OM2SMPB02E_VAL_MEASMODE_VALUES_T;


#define OM2SMPB02E_MASK_POWERMODE_VALUE   ((UINT8)0x03)

/**
 * OM2SMPB02E_VAL_POWERMODE_VALUES enum.
 * An Enum contains power mode values.
 */
typedef enum OM2SMPB02E_VAL_POWERMODE_VALUES{
    OM2SMPB02E_VAL_POWERMODE_SLEEP  = 0x00,
    OM2SMPB02E_VAL_POWERMODE_FORCED = 0x01,
    OM2SMPB02E_VAL_POWERMODE_NORMAL = 0x03,
}OM2SMPB02E_VAL_POWERMODE_VALUES_T;

/**
 * OM2SMPB02E_VAL_IIR_VALUES enum.
 * An Enum contains IIR values.
 */
typedef enum OM2SMPB02E_VAL_IIR_VALUES{
    OM2SMPB02E_VAL_IIR_OFF     = 0x00,
    OM2SMPB02E_VAL_IIR_02TIMES = 0x01,
    OM2SMPB02E_VAL_IIR_04TIMES = 0x02,
    OM2SMPB02E_VAL_IIR_08TIMES = 0x03,
    OM2SMPB02E_VAL_IIR_16TIMES = 0x04,
    OM2SMPB02E_VAL_IIR_32TIMES = 0x05,
}OM2SMPB02E_VAL_IIR_VALUES_T;

/* Coeff */
#define OM2SMPB02E_COEFF_S_A1   ((DOUBLE)( 4.3E-04))
#define OM2SMPB02E_COEFF_A_A1   ((DOUBLE)(-6.3E-03))
#define OM2SMPB02E_COEFF_S_A2   ((DOUBLE)( 1.2E-10))
#define OM2SMPB02E_COEFF_A_A2   ((DOUBLE)(-1.9E-11))
#define OM2SMPB02E_COEFF_S_BT1  ((DOUBLE)( 9.1E-02))
#define OM2SMPB02E_COEFF_A_BT1  ((DOUBLE)( 1.0E-01))
#define OM2SMPB02E_COEFF_S_BT2  ((DOUBLE)( 1.2E-06))
#define OM2SMPB02E_COEFF_A_BT2  ((DOUBLE)( 1.2E-08))
#define OM2SMPB02E_COEFF_S_BP1  ((DOUBLE)( 1.9E-02))
#define OM2SMPB02E_COEFF_A_BP1  ((DOUBLE)( 3.3E-02))
#define OM2SMPB02E_COEFF_S_B11  ((DOUBLE)( 1.4E-07))
#define OM2SMPB02E_COEFF_A_B11  ((DOUBLE)( 2.1E-07))
#define OM2SMPB02E_COEFF_S_BP2  ((DOUBLE)( 3.5E-10))
#define OM2SMPB02E_COEFF_A_BP2  ((DOUBLE)(-6.3E-10))
#define OM2SMPB02E_COEFF_S_B12  ((DOUBLE)( 7.6E-13))
#define OM2SMPB02E_COEFF_A_B12  ((DOUBLE)( 2.9E-13))
#define OM2SMPB02E_COEFF_S_B21  ((DOUBLE)( 1.2E-14))
#define OM2SMPB02E_COEFF_A_B21  ((DOUBLE)( 2.1E-15))
#define OM2SMPB02E_COEFF_S_BP3  ((DOUBLE)( 7.9E-17))
#define OM2SMPB02E_COEFF_A_BP3  ((DOUBLE)( 1.3E-16))

/* CALIBRATION DATA */
typedef struct OM2SMPB02E_SETTING{
    /* Compensation Factor */
    UINT8  ex;
    DOUBLE _A0, _A1, _A2;
    DOUBLE _B00, _BT1, _BP1;
    DOUBLE _B11, _BT2, _BP2;
    DOUBLE _B12, _B21, _BP3;
}OM2SMPB02E_SETTING_T;

#define OM2SMPB02E_CONV8S_S24_BE(a, b, c) \
    (INT32)((((UINT32)a << 16) & 0x00FF0000) | \
            (((UINT32)b << 8) & 0x0000FF00) | \
            ((UINT32)c & 0x000000FF))


/**
 * OM2SMPB02E_STATUS enum
 * An Enum contains status code of operations
 */
typedef enum OM2SMPB02E_STATUS{
OM2SMPB02E_STATUS_FAILURE = 0,
OM2SMPB02E_STATUS_SUCCESS = 1,
OM2SMPB02E_STATUS_I2C_NOT_INITED,
OM2SMPB02E_STATUS_I2C_WRITE_FAILED,
OM2SMPB02E_STATUS_I2C_READ_FAILED,
OM2SMPB02E_STATUS_INVALID_MEASURMENT_INTERVAL,
OM2SMPB02E_STATUS_INVALID_INPUT,
OM2SMPB02E_STATUS_PRES_MEASUREMENT_SKIP,
OM2SMPB02E_STATUS_TEMP_MEASUREMENT_SKIP,
OM2SMPB02E_STATUS_TEMP_PRES_MEASUREMENT_SKIP,
OM2SMPB02E_STATUS_SLEEP_MODE,
}OM2SMPB02E_STATUS_T;

/*=========================================================================*/

namespace upm {
    /**
      * @brief Digital Barometric Perssure Sensor
      * @defgroup 2smpb02e libupm-2smpb02e
      * @ingroup omron i2c barometric pressure sensor
      */
    /**
     * @library 2smpb02e
     * @sensor 2smpb-02e
     * @comname Omron Barometric Pressure sensors
     * @type barometric pressure
     * @man omron
     * @con I2C
     * @web https://www.components.omron.com/product-detail?partId=35065
     *
     * @brief API for the Omron Mems barometric pressure sensor interface
     *
     * It is connected via a I2C Interface.
     *
     * @snippet 2smpb02e.cxx Interesting
     */

    class OM2SMPB02E  : virtual private iTemperature, virtual private iPressure{
        public :
            /**
             * OM2SMPB02E constructor
             * @param bus I2c bus the sensor is attached to. Default is 6.
             * @param address I2c address Optional device address. Default is 0x56.
             */
            OM2SMPB02E(INT32 bus = OM2SMPB02E_DEFAULT_I2C_BUS_NUM,UINT8 address = OM2SMPB02E_DEFAULT_I2C_ADDR);

            /**
             * OM2SMPB02E destructor
             */
            ~OM2SMPB02E();

            /**
             * Get Barometric Pressure sensor get pressure and temperature data.
             *
             * @param outPres Pressure value. Value is based on current pressure scale unit setting.
             * @param outTemp Temperature data. Value is based on current temperature scale unit setting.
             * @return One of the OM2SMPB02E_STATUS_T value.
             */
            OM2SMPB02E_STATUS_T getPresAndTempValues(FLOAT &outPres, FLOAT &outTemp);

            /**
             * Get Barometric Pressure sensor get pressure data.
             *
             * @param outPres Pressure value. Value is based on current pressure scale unit setting.
             * @return One of the OM2SMPB02E_STATUS_T value.
             */
            OM2SMPB02E_STATUS_T getPressure(FLOAT &outPres);

            /**
             * Get Barometric Pressure sensor get temperature data.
             *
             * @param outTemp Temperature data. Value is based on current temperature scale unit setting.
             * @return One of the OM2SMPB02E_STATUS_T value.
             */
            OM2SMPB02E_STATUS_T getTemperature(FLOAT &outTemp);

            /**
             * Get power mode.
             *
             * @param powerMode power mode.
             * @return One of the OM2SMPB02E_STATUS_T value.
             */
            OM2SMPB02E_STATUS_T getPowerMode(OM2SMPB02E_VAL_POWERMODE_VALUES_T &powerMode);

            /**
             * Set power mode.
             *
             * @param powerMode power mode.
             * @return One of the OM2SMPB02E_STATUS_T value.
             */
            OM2SMPB02E_STATUS_T setPowerMode(OM2SMPB02E_VAL_POWERMODE_VALUES_T powerMode);

            /**
             * Get measurement mode.
             *
             * @param measurementMode measurement mode.
             * @return One of the OM2SMPB02E_STATUS_T value.
             */
            OM2SMPB02E_STATUS_T getMeasurementMode(OM2SMPB02E_VAL_MEASMODE_VALUES_T &measurementMode);

            /**
             * Set measurement mode.
             *
             * @param measurementMode measurement mode.
             * @return One of the OM2SMPB02E_STATUS_T value.
             */
            OM2SMPB02E_STATUS_T setMeasurementMode(OM2SMPB02E_VAL_MEASMODE_VALUES_T measurementMode);

            /**
             * Get Standby time value
             *
             * @param standbyTimeValue standby time.
             * @return One of the OM2SMPB02E_STATUS_T value.
             */
            OM2SMPB02E_STATUS_T getStandbyTimeValue(OM2SMPB02E_VAL_IOSETUP_STANDBY_VALUES_T &standbyTimeValue);

            /**
             * Set Standby time value
             *
             * @param standbyTimeValue standby time.
             * @return One of the OM2SMPB02E_STATUS_T value.
             */
            OM2SMPB02E_STATUS_T setStandbyTimeValue(OM2SMPB02E_VAL_IOSETUP_STANDBY_VALUES_T standbyTimeValue);

            /**
             * Get IIR filter value
             *
             * @param iirValue iif filter value.
             * @return One of the OM2SMPB02E_STATUS_T value.
             */
            OM2SMPB02E_STATUS_T getIIRFilterValue(OM2SMPB02E_VAL_IIR_VALUES_T &iirValue);

            /**
             * Set IIR filter value
             *
             * @param iirValue iif filter value.
             * @return One of the OM2SMPB02E_STATUS_T value.
             */
            OM2SMPB02E_STATUS_T setIIRFilterValue(OM2SMPB02E_VAL_IIR_VALUES_T iirValue);

            /**
             * Get the temperature unit.
             *
             * @param unit   Temperature scale unit
             * @return One of the OM2SMPB02E_STATUS_T values.
             */
            OM2SMPB02E_STATUS_T getTemperatureScale(upm::TemperatureUnit &unit);

            /**
             * Set the temperature unit.
             *
             * @param unit   Temperature scale unit
             * @return One of the OM2SMPB02E_STATUS_T values.
             */
            OM2SMPB02E_STATUS_T setTemperatureScale(upm::TemperatureUnit unit);

            /**
             * Get the Pressure unit.
             *
             * @param unit  Pressure scale unit
             * @return One of the OM2SMPB02E_STATUS_T values.
             */
            OM2SMPB02E_STATUS_T getPressureScale(upm::PressureUnit &unit);

            /**
             * Set the Pressure unit.
             *
             * @param unit   Pressure scale unit
             * @return One of the OM2SMPB02E_STATUS_T values.
             */
            OM2SMPB02E_STATUS_T setPressureScale(upm::PressureUnit unit);

            /**
             * Set the I2C Frequency.
             *
             * @param I2cMode  frequency
             * @return One of the OM2SMPB02E_STATUS_T values.
             */
            OM2SMPB02E_STATUS_T setI2cFrequency(mraa::I2cMode frequency);

            /**
             * Get the device Id.
             *
             * @param deviceId Device ID
             * @return One of the OM2SMPB02E_STATUS_T values.
             */
            OM2SMPB02E_STATUS_T getDeviceId(UINT8 &deviceId);

        private:
            INT32 mBus;
            INT32 mAddress;
            upm::TemperatureUnit mTempScale;
            upm::PressureUnit mPressureScale;
            mraa::I2c* mI2c;
            UINT8 mBuf[OM2SMPB02E_COEFFI_MAX_VALUE] = {0};
            OM2SMPB02E_SETTING_T mCompFactor;
            UINT8 mTempMeasValue = OM2SMPB02E_VAL_TEMPAVERAGE_00;
            UINT8 mPresMeasValue = OM2SMPB02E_VAL_PRESAVERAGE_00;
            OM2SMPB02E_VAL_POWERMODE_VALUES_T mPowerMode = OM2SMPB02E_VAL_POWERMODE_SLEEP;

            /**
             * Returns the temperature. This method is not publicly accessible.
             *
             * @return Dummy value
             */
            virtual FLOAT getTemperature(){return 0.0;}

            /**
             * Returns the Pressure. This method is not publicly accessible.
             *
             * @return Dummy value
             */
            virtual FLOAT getPressure(){return 0.0;}

            /**
             * Get the co-effi. values for Temperature and Pressure.
             *
             * @param settings Settings.
             * @return One of the OM2SMPB02E_STATUS_T values.
             */
            OM2SMPB02E_STATUS_T getCOEFFValues(OM2SMPB02E_SETTING_T &Setting);

            /**
             * Convert byte buffer to double. Byte buffer format is a signed-16bit Big-Endian.
             *
             * @param a
             * @param s
             * @param inBuf
             * @param outBuf
             * @param offSet
             * @return One of the OM2SMPB02E_STATUS_T values.
             */
            OM2SMPB02E_STATUS_T conv16ToDouble(DOUBLE a, DOUBLE s, UINT8* inValue, DOUBLE &outValue, INT32 offSet);

            /**
             * Convert byte buffer to double. Byte buffer format is a signed-20Q4,from -32768.0 to 32767.9375.
             *
             * @param inValue
             * @param outValue
             * @param ex
             * @param offSet
             * @return One of the OM2SMPB02E_STATUS_T values.
             */
            OM2SMPB02E_STATUS_T conv20Q4ToDouble(UINT8* inValue, DOUBLE &outValue, UINT8 ex, INT32 offSet);

            /*
             * Copy Constructor
             */
            OM2SMPB02E(const OM2SMPB02E&);

            /**
             * Operator Overloading
            */
            OM2SMPB02E& operator=(const OM2SMPB02E&);
    };
}

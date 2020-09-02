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

/*=========================================================================*/

#pragma once


#include <iostream> //Required for c++ standard IO
#include "mraa/i2c.hpp" //Required for MRAA I2c operations
#include <interfaces/iTemperature.hpp> //Required to Temperature Unit functionality
#include <vector>

/**
 * MACROS and enum
 */
#define D6T_DEFAULT_I2C_BUS_NUM 			6	
#define D6T_DEFAULT_I2C_ADDR					0x0a
#define D6T_SENSOR_DATA_READ_CMD 				0x4C
#define D6T_1AND8_CHANNEL_VALUE_BUFFER_ROW_LENGTH 			28
#define D6T_1AND8_CHANNEL_VALUE_BUFFER_COLUMN_LENGTH 			3
#define D6T_16_CHANNEL_VALUE_BUFFER_ROW_LENGTH				32
#define D6T_16_CHANNEL_VALUE_BUFFER_COLUMN_LENGTH 			2
#define D6T_1AND8_CHANNEL_REG_PATTERN_ROW_LENGTH 			5
#define D6T_1AND8_CHANNEL_REG_PATTERN_COLUMN_LENGTH 			4
#define D6T_16_CHANNEL_REG_PATTERN_ROW_LENGTH 				3
#define D6T_16_CHANNEL_REG_PATTERN_COLUMN_LENGTH 			9
#define D6T_16_CHANNEL_WRITE_ADDR_READ_REG_VALUE			0x62

/**
 * D6T_STATUS enum
 * An Enum contains status code of operations
 */
typedef enum D6T_STATUS{
D6T_STATUS_FAILURE = 0,
D6T_STATUS_SUCCESS =1,
D6T_STATUS_I2C_NOT_INITED,
D6T_STATUS_I2C_WRITE_FAILED,
D6T_STATUS_I2C_READ_FAILED,
D6T_STATUS_INVALID_SENSOR_TYPE,
D6T_STATUS_MEMORY_ALLOCATION_FAIL,
D6T_STATUS_PEC_FAILURE,
D6T_STATUS_INVALID_MEASURMENT_INTERVAL,
D6T_STATUS_INVALID_INPUT,
}D6T_STATUS_T;

/**
 * D6T_SENSOR_TYPES enum
 * An Enum contains TYPES of D6T sensors
 */
typedef enum D6T_SENSOR_TYPES{
D6T_1X1_CHANNEL = 1,
D6T_1X8_CHANNEL = 8,
D6T_4X4_CHANNEL = 16,
}D6T_SENSOR_TYPES_T;

/**
 * D6T_SENSOR_BUFFER_LENGTH enum
 * An enum contains Length of BUFFER
 */
typedef enum D6T_SENSOR_BUFFER_LENGTH{
D6T_1X1_CHANNEL_BUFFER_LENGTH = 5,
D6T_1X8_CHANNEL_BUFFER_LENGTH = 19,
D6T_4X4_CHANNEL_BUFFER_LENGTH = 35,
}D6T_SENSOR_BUFFER_LENGTH_T;

/*=========================================================================*/

namespace upm {
    /**
      * @brief Mems Thermal Sensors
      * @defgroup d6t libupm-d6t
      * @ingroup omron i2c thermal sensor
      */
    /**
     * @library d6t
     * @sensor d6t
     * @comname Omron Mems thermal sensors
     * @type thermal
     * @man omron
     * @con I2C
     * @web https://www.components.omron.com/parametric-search?nodeId=404010&nodeParentId=4040
     *
     * @brief API for the Omron Mems thermal sensors interface
     *
     * It is connected via a I2C Interface.
     *
     * @snippet d6t.cxx Interesting
     */
    class D6T : virtual private iTemperature{
        public :
            /**
             * D6T constructor
             *
             * @param type D6T sensor type
             * @param bus I2c bus the sensor is attached to. Default is 6.
             * @param address I2c address Optional device address. Default is 0x0a.
             */
            D6T(D6T_SENSOR_TYPES_T type, int bus = D6T_DEFAULT_I2C_BUS_NUM,uint8_t address = D6T_DEFAULT_I2C_ADDR);

            /**
             * D6T destructor
             */
            ~D6T();

            /**
             * Get thermal sensor temperature data.
	     * User can call this API at interval greater than or equal to sensor measurement interval.
             *
             * @param ptat   Reference temperature data stored in the sensor
             * @param temperatureData   Temperature data for each pixel. Sufficient memory should be allocated on app side.
             * @param length   Length of temperatureData. This values is used to fillup temperatureData.
             * @param pec    Packet error check code
             * @return One of the D6T_STATUS_T values
             */
            D6T_STATUS_T getTemperature(float *ptat, float* temperatureData, uint8_t length, uint8_t *pec);

            /**
             * Get thermal sensor temperature data.
	     * User can call this API at interval greater than or equal to sensor measurement interval.
             *
             * @param ptat   Reference temperature data stored in the sensor
             * @param temperatureData   Temperature data for each pixel.
             * @param pec    Packet error check code
             * @return One of the D6T_STATUS_T values
             */
            D6T_STATUS_T getTemperature(float &ptat, std::vector<float> &temperatureData, uint8_t &pec);

            /**
             * Get the thermal sensor temperature unit.
             *
             * @param unit   Temperature scale unit
	     *
             * @return One of the D6T_STATUS_T values
             */
            D6T_STATUS_T getTemperatureScale(upm::TemperatureUnit &unit);

            /**
             * Set the thermal sensor temperature unit.
             *
             * @param unit   Temperature scale unit
	     *
             * @return One of the D6T_STATUS_T values
             */
            D6T_STATUS_T setTemperatureScale(upm::TemperatureUnit unit);

            /**
             * Display thermal sensor data with PTAT, P0 to P15 and PEC values.
	     *
	     * User can call this API at interval greater than or equal to sensor measurement interval.
             *
             * @return One of the D6T_STATUS_T values
             */
            D6T_STATUS_T displayTemperature();

	    /**
	     * Set the thermal sensor type.
	     *
	     * @param type   Sensor type
	     *
	     * @return One of the D6T_STATUS_T values
	     */
	    D6T_STATUS_T setSensorType(D6T_SENSOR_TYPES_T type);

	    /**
	     * Get the thermal sensor type.
	     *
	     * @param type   Sensor type
	     *
	     * @return One of the D6T_STATUS_T values
	     */
	    D6T_STATUS_T getSensorType(D6T_SENSOR_TYPES_T &type);

	    /**
	     * Get Measurement Interval time.
	     *
	     * @param value   Interval value
	     *
	     * @return One of the D6T_STATUS_T values
	     */
	    D6T_STATUS_T getMeasurementInterval(uint8_t &value);

	    /**
	     * Set Measurement Interval time.
	     *
	     * @param value   Interval value
	     *
	     * @return One of the D6T_STATUS_T values
	     */
	    D6T_STATUS_T setMeasurementInterval(uint8_t value);


        private:
            int m_bus;
            int m_address;
	    upm::TemperatureUnit m_tempScale;
	    D6T_SENSOR_TYPES_T d6t_sensor_type;
	    D6T_SENSOR_BUFFER_LENGTH_T d6t_sensor_data_length;
	    uint8_t *m_buf = NULL;
            mraa::I2c* m_i2c;
	    static uint8_t intervalValues1Channel[D6T_1AND8_CHANNEL_VALUE_BUFFER_ROW_LENGTH][D6T_1AND8_CHANNEL_VALUE_BUFFER_COLUMN_LENGTH];
            static uint8_t intervalValues8Channel[D6T_1AND8_CHANNEL_VALUE_BUFFER_ROW_LENGTH][D6T_1AND8_CHANNEL_VALUE_BUFFER_COLUMN_LENGTH];
            static uint8_t intervalValues16Channel[D6T_16_CHANNEL_VALUE_BUFFER_ROW_LENGTH][D6T_16_CHANNEL_VALUE_BUFFER_COLUMN_LENGTH];
	    static uint8_t intervalBytePattern1And8Channel[D6T_1AND8_CHANNEL_REG_PATTERN_ROW_LENGTH][D6T_1AND8_CHANNEL_REG_PATTERN_COLUMN_LENGTH];
	    static uint8_t intervalBytePattern16Channel[D6T_16_CHANNEL_REG_PATTERN_ROW_LENGTH][D6T_16_CHANNEL_REG_PATTERN_COLUMN_LENGTH];

            /**
            * Returns the temperature. This method is not publicly accessible.
            *
            * @return Dummy value
            */
            virtual float getTemperature(){return 0.0;}

            /**
            * Returns the updated temperature. This method is not publicly accessible.
            *
            * @return One of the D6T_STATUS_T values
            */
            virtual D6T_STATUS_T getUpdatedTemperature();

	    /**
	     * Calculate CRC for given data.
	     *
	     * @param data   Sensor PEC data
	     *
	     * @return CRC value
	     */
            uint8_t calculateCrc(uint8_t data);

	    /**
	     * Get the thermal sensor type.
	     *
	     * @param buf   Sensor data buffer
	     * @param len Sensor data buffer length
	     *
	     * @return Return success or Failure
	     */
            bool checkPEC(uint8_t *buf , uint8_t count);

	    /**
	     * Get register value of measurement interval.
	     *
	     * @param r_addr  Address of register
	     * @param data  Interval setting register values
	     *
	     * @return One of the D6T_STATUS_T values
	     */
	    D6T_STATUS_T getMeasurementSettingRegisterValue(uint8_t r_addr, uint8_t* data);

	    /*
	     * Copy Constructor
	     */
	    D6T(const D6T&);

	    /**
	     * Operator Overloading
	     */
	    D6T& operator=(const D6T&);

     };
}

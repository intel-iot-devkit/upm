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

#include <stdexcept>
#include <stdio.h>
#include <iomanip>
#include <string.h>
#include "d6t.hpp"

using namespace upm;


/**
 * Measurement interval values for 1 Channel Sensor
 */
uint8_t D6T::intervalValues1Channel[D6T_1AND8_CHANNEL_VALUE_BUFFER_ROW_LENGTH][D6T_1AND8_CHANNEL_VALUE_BUFFER_COLUMN_LENGTH]={
	                                     {0x80, 0xD2, 0x79},{0x80, 0xD3, 0x7E},{0x80, 0xD4, 0x6B},{0x80, 0xD5, 0x6C},
	                                     {0x80, 0xD6, 0x65},{0x80, 0xD7, 0x62},{0x80, 0xD8, 0x4F},{0x80, 0xD9, 0x48},
	                                     {0x80, 0xDA, 0x41},{0x80, 0xDB, 0x46},{0x80, 0xDC, 0x53},{0x80, 0xDD, 0x54},
	                                     {0x80, 0xDE, 0x5D},{0x80, 0xDF, 0x5A},{0x90, 0xD2, 0x2E},{0x90, 0xD3, 0x29},
	                                     {0x90, 0xD4, 0x3C},{0x90, 0xD5, 0x3B},{0x90, 0xD6, 0x32},{0x90, 0xD7, 0x35},
	                                     {0x90, 0xD8, 0x18},{0x90, 0xD9, 0x1F},{0x90, 0xDA, 0x16},{0x90, 0xDB, 0x11},
	                                     {0x90, 0xDC, 0x04},{0x90, 0xDD, 0x03},{0x90, 0xDE, 0x0A},{0x90, 0xDF, 0x0D}};

/**
 * Measurement interval values for 8 Channel Sensor
 */
uint8_t D6T::intervalValues8Channel[D6T_1AND8_CHANNEL_VALUE_BUFFER_ROW_LENGTH][D6T_1AND8_CHANNEL_VALUE_BUFFER_COLUMN_LENGTH]={
	                                     {0x80, 0x32, 0xD7},{0x80, 0x33, 0xD0},{0x80, 0x34, 0xC5},{0x80, 0x35, 0xC2},
	                                     {0x80, 0x36, 0xCB},{0x80, 0x37, 0xCC},{0x80, 0x38, 0xE1},{0x80, 0x39, 0xE6},
	                                     {0x80, 0x3A, 0xEF},{0x80, 0x3B, 0xE8},{0x80, 0x3C, 0xFD},{0x80, 0x3D, 0xFA},
	                                     {0x80, 0x3E, 0xF3},{0x80, 0x3F, 0xF4},{0x90, 0x32, 0x80},{0x90, 0x33, 0x87},
	                                     {0x90, 0x34, 0x92},{0x90, 0x35, 0x95},{0x90, 0x36, 0x9C},{0x90, 0x37, 0x9B},
	                                     {0x90, 0x38, 0xB6},{0x90, 0x39, 0xB1},{0x90, 0x3A, 0xB8},{0x90, 0x3B, 0xBF},
	                                     {0x90, 0x3C, 0xAA},{0x90, 0x3D, 0xAD},{0x90, 0x3E, 0xA4},{0x90, 0x3F, 0xA3}};

/**
 * Measurement interval values for 16 Channel Sensor
 */
uint8_t D6T::intervalValues16Channel[D6T_16_CHANNEL_VALUE_BUFFER_ROW_LENGTH][D6T_16_CHANNEL_VALUE_BUFFER_COLUMN_LENGTH]={
	                                     {0x01,0x00},{0x02,0x00},{0x03,0x00},{0x04,0x00},{0x05,0x00},{0x06,0x00},
			                     {0x07,0x00},{0x08,0x00},{0x09,0x00},{0x0A,0x00},{0x0B,0x00},{0x0C,0x00},
			                     {0x0D,0x00},{0x0E,0x00},{0x0F,0x00},{0x10,0x00},{0x11,0x00},{0x12,0x00},
			                     {0x13,0x00},{0x14,0x00},{0x15,0x00},{0x16,0x00},{0x17,0x00},{0x18,0x00},
			                     {0x19,0x00},{0x1A,0x00},{0x1B,0x00},{0x1C,0x00},{0x1D,0x00},{0x1E,0x00},
			                     {0x1F,0x00},{0x20,0x00}};

/**
 * Measurement interval bytes write pattern
 * for 1 and 8 channel sensor
 */
uint8_t D6T::intervalBytePattern1And8Channel[D6T_1AND8_CHANNEL_REG_PATTERN_ROW_LENGTH][D6T_1AND8_CHANNEL_REG_PATTERN_COLUMN_LENGTH]={
	                                            {0x02, 0x00, 0x01, 0xEE},
                                                    {0x05, 0x90, 0x3F, 0xA3},
                                                    {0x03, 0x00, 0x03, 0x8B},
                                                    {0x03, 0x00, 0x07, 0x97},
                                                    {0x02, 0x00, 0x00, 0xE9}};
/**
 * Measurement interval bytes write pattern
 * for 16 channel sensor
 */
uint8_t D6T::intervalBytePattern16Channel[D6T_16_CHANNEL_REG_PATTERN_ROW_LENGTH][D6T_16_CHANNEL_REG_PATTERN_COLUMN_LENGTH]={
	                                         {0x50, 0x52, 0x45, 0x4C, 0x45, 0x41, 0x53, 0x45, 0x00},
                                                 {0x42, 0x14, 0x00},
						 {0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};



D6T::D6T(D6T_SENSOR_TYPES_T type, int bus,uint8_t address): m_bus(bus), m_address(address) {

    //Create I2c object
    m_i2c = new mraa::I2c(m_bus);

    //Check for I2c object created well or not
    if(m_i2c->address(m_address) != mraa::SUCCESS){
        throw std::invalid_argument(std::string(__FUNCTION__) + ": I2c.address() failed");
    }

    //Set default Temperature scale to celsius
    m_tempScale = upm::TemperatureUnit::CELSIUS;

    //Set sensor type
    d6t_sensor_type = type;
    setSensorType(type);

    //Set default floating point precision
    std::cout << std::fixed << std::setprecision(2);

}

D6T::~D6T() {

    //Delete I2c instance
    delete m_i2c;

    //Delete buffer
    delete m_buf;
}

D6T_STATUS_T D6T::getTemperature(float *ptat, float* temperatureData, uint8_t length, uint8_t *pec) {

    D6T_STATUS_T ret;
    uint8_t counter = 0;

    //Input arguments validation
    if(NULL == ptat || NULL == temperatureData || NULL == pec){
        return D6T_STATUS_INVALID_INPUT;
    }

    //Get updated temperature
    try{
        ret = getUpdatedTemperature();

	//Check return value
	if(ret != D6T_STATUS_SUCCESS){
		std::cerr << "Get Temperature data failed " << std::endl;
		return ret;
	}
    }catch(const std::runtime_error& e) {
        std::cerr << "Error while handling: " << e.what() << std::endl;
	return ret;
    }

    //Copy temperature data
    for(counter = 0; counter < length; counter ++) {
        temperatureData[counter] = (((m_buf[(counter*2) + 3] * 256) + m_buf[(counter *2) + 2]) / 10.0);
	if(m_tempScale != upm::TemperatureUnit::CELSIUS){
            temperatureData[counter] = convertCelsiusTo(temperatureData[counter],m_tempScale);
	}
    }

    *ptat = (((m_buf[1] * 256) + m_buf[0]) / 10.0);
    *pec = m_buf[d6t_sensor_data_length -1];

    return D6T_STATUS_SUCCESS;
}

D6T_STATUS_T D6T::getTemperature(float &ptat, std::vector<float> &temperatureData, uint8_t &pec)
{
    D6T_STATUS_T ret;
    uint8_t counter = 0;

    //Get updated temperature
    try{
        ret = getUpdatedTemperature();

	//Check return value
	if(ret != D6T_STATUS_SUCCESS){
		std::cerr << "Error while handling: " << std::endl;
		return ret;
        }
    }catch(const std::runtime_error& e) {
        std::cerr << "Error while handling: " << e.what() << std::endl;
        return ret;
    }

    //temporary variable
    float tmpData = 0;

    //Copy temperature data
    for(counter = 0; counter < d6t_sensor_type ; counter ++) {
        tmpData = ((((m_buf[(counter*2) + 3] * 256) + m_buf[(counter *2) + 2]) / 10.0));
	if(m_tempScale != upm::TemperatureUnit::CELSIUS){
            tmpData = convertCelsiusTo(tmpData,m_tempScale);
	}
	temperatureData.push_back(tmpData);
    }

    ptat = (((m_buf[1] * 256) + m_buf[0]) / 10.0);
    pec = m_buf[d6t_sensor_data_length - 1];

    return D6T_STATUS_SUCCESS;
}

D6T_STATUS_T D6T::getTemperatureScale(upm::TemperatureUnit &unit)
{
    unit = m_tempScale;
    return D6T_STATUS_SUCCESS;
}

D6T_STATUS_T D6T::setTemperatureScale(upm::TemperatureUnit unit)
{
    m_tempScale = unit;
    return D6T_STATUS_SUCCESS;
}

D6T_STATUS_T D6T::getSensorType(D6T_SENSOR_TYPES_T &type)
{
    type = d6t_sensor_type;
    return D6T_STATUS_SUCCESS;
}

D6T_STATUS_T D6T::setSensorType(D6T_SENSOR_TYPES_T type)
{
   D6T_STATUS_T returnValue = D6T_STATUS_SUCCESS;

   // Set sensor type
   d6t_sensor_type = type;
   switch(type)
   {
        case D6T_1X1_CHANNEL:
		d6t_sensor_data_length = D6T_1X1_CHANNEL_BUFFER_LENGTH;
            break;
	case D6T_1X8_CHANNEL:
		d6t_sensor_data_length = D6T_1X8_CHANNEL_BUFFER_LENGTH;
    	    break;
	case D6T_4X4_CHANNEL:
		d6t_sensor_data_length = D6T_4X4_CHANNEL_BUFFER_LENGTH;
	    break;
	default:
	        returnValue = D6T_STATUS_INVALID_SENSOR_TYPE;
		return returnValue;
   }

   // Memory Allocation
   m_buf = new uint8_t[d6t_sensor_data_length];

   // Check Memory Allocation
   if(m_buf == NULL){
	throw std::runtime_error(std::string(__FUNCTION__) +
                       ": Allocation failed" );
   }

   return returnValue;
}

D6T_STATUS_T D6T::displayTemperature()
{
    float ptat;
    float temperatureData[d6t_sensor_type];
    uint8_t pec;
    D6T_STATUS_T returnValue;
    uint8_t counter;

    //Get current thermal sensor temperature values
    returnValue = getTemperature(&ptat, temperatureData, d6t_sensor_type, &pec);

    //Check return value
    if(D6T_STATUS_SUCCESS != returnValue){
	    std::cout <<  "displayTemperature failed with error code: " << returnValue << std::endl;
	    return returnValue;
    }

    //Print values
    std::cout << "TEMP_DATA: ";
    for(counter = 0; counter < d6t_sensor_type; counter++) {
        std::cout << temperatureData[counter] << " ";
    }
    std::cout << "PTAT: " << ptat;
    std::cout << " PEC: " << unsigned(pec) << std::endl;

    return D6T_STATUS_SUCCESS;
}

D6T_STATUS_T D6T::getUpdatedTemperature()
{
    mraa::Result returnValue;

    //Check memory allocated for data
    if(m_buf == NULL){
	throw std::runtime_error(std::string(__FUNCTION__) +
                       ": Sensor channel type not set" );
    }

    //Make sensor ready to read data
    if((returnValue = m_i2c->writeByte(D6T_SENSOR_DATA_READ_CMD)) != mraa::SUCCESS){
        throw std::runtime_error(std::string(__FUNCTION__) +
                               ": I2c.writeByte() failed" );
    }

    //Read sensor data
    if (!m_i2c->read(m_buf, d6t_sensor_data_length)){
        throw std::runtime_error(std::string(__FUNCTION__) +
                                 ": I2c.read() failed");
    }

    //Validate PEC value
    if(checkPEC(m_buf, d6t_sensor_data_length - 1)){
        return D6T_STATUS_PEC_FAILURE;
    }

    return D6T_STATUS_SUCCESS;
}

uint8_t D6T::calculateCrc(uint8_t data)
{
    uint8_t index;
    uint8_t temp;

    //CRC method
    for(index=0;index<8;index++){
       temp = data;
       data <<= 1;
       if(temp & 0x80) data ^= 0x07;
    }

    return data;
}

bool D6T::checkPEC(uint8_t *buf , uint8_t count)
{
    uint8_t crc;
    uint8_t i;
    uint8_t defaulut_value = 0x14;

    //Calculate CRC
    crc = calculateCrc(defaulut_value);
    crc = calculateCrc(0x4C ^ crc );
    crc = calculateCrc(0x15 ^ crc );

    for(i=0;i<count;i++){
        crc = calculateCrc( buf[i] ^ crc );
    }

    return (crc == buf[count]);
}

D6T_STATUS_T D6T::getMeasurementInterval(uint8_t &value)
{
    D6T_STATUS_T returnValue = D6T_STATUS_SUCCESS;
    uint8_t count;
    uint8_t tempByte[2];

    switch(d6t_sensor_type)
    {
	 case D6T_1X1_CHANNEL:

		 try{
			 //Get register values
		         returnValue = getMeasurementSettingRegisterValue(intervalBytePattern1And8Channel[1][0],tempByte);

			 //Check return value
			 if(returnValue != D6T_STATUS_SUCCESS){
				 std::cerr << "Error while handling: " << std::endl;
				 return returnValue;
			 }
		 }catch(const std::runtime_error& e){
			 std::cerr << "Error while handling: " << e.what() << std::endl;
			 return returnValue;
		 }

		 //Find register value from buffer
		 for(count = 0; count < D6T_1AND8_CHANNEL_VALUE_BUFFER_ROW_LENGTH; count++){
			 if(memcmp(intervalValues1Channel[count],tempByte,sizeof(tempByte)) == 0){
				 value = count + 1;
				 break;
			 }
		 }

		 //Return invalid measurement interval
		 if(count == D6T_1AND8_CHANNEL_VALUE_BUFFER_ROW_LENGTH){
			 return D6T_STATUS_INVALID_MEASURMENT_INTERVAL;
		 }

		 break;

	 case D6T_1X8_CHANNEL:

		 try{
			 //Get register values
		         returnValue = getMeasurementSettingRegisterValue(intervalBytePattern1And8Channel[1][0],tempByte);

			 //Check return value
			 if(returnValue != D6T_STATUS_SUCCESS){
				 std::cerr << "Error while handling: " << std::endl;
				 return returnValue;
			 }
		 }catch(const std::runtime_error& e){
			 std::cerr << "Error while handling: " << e.what() << std::endl;
			 return returnValue;
		 }

		 //Find register value from buffer
		 for(count = 0; count < D6T_1AND8_CHANNEL_VALUE_BUFFER_ROW_LENGTH; count++){
			 if(memcmp(intervalValues8Channel[count],tempByte,sizeof(tempByte)) == 0){
				 value = count + 1;
				 break;
			 }
		 }

		 //Return invalid measurement interval
		 if(count == D6T_1AND8_CHANNEL_VALUE_BUFFER_ROW_LENGTH){
			 return D6T_STATUS_INVALID_MEASURMENT_INTERVAL;
		 }

		 break;
	 case D6T_4X4_CHANNEL:

		 try{

			 //Unlock register
                         if((m_i2c->write(intervalBytePattern16Channel[0],D6T_16_CHANNEL_REG_PATTERN_COLUMN_LENGTH)) != mraa::SUCCESS)
                         {
                                 throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.write failed");
                         }

                         //Get register values
                         returnValue = getMeasurementSettingRegisterValue(D6T_16_CHANNEL_WRITE_ADDR_READ_REG_VALUE,tempByte);

                         //Check return value
                         if(returnValue != D6T_STATUS_SUCCESS){
                                 std::cerr << "Fail to get measurement interval register value" << std::endl;
                                 return returnValue;
                         }

                         //Lock register
                         if((m_i2c->write(intervalBytePattern16Channel[2],D6T_16_CHANNEL_REG_PATTERN_COLUMN_LENGTH)) != mraa::SUCCESS)
                         {
                                 throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.write failed");
                         }

		 }catch(const std::runtime_error& e){
			 std::cerr << "Error while handling: " << e.what() << std::endl;
			 return returnValue;
		 }

		 //Find register value from buffer
		 for(count = 0; count < D6T_16_CHANNEL_VALUE_BUFFER_ROW_LENGTH; count++){
			 if(memcmp(intervalValues16Channel[count],tempByte,1) == 0){
				 value = count + 1;
				 break;
			 }
		 }

		 //Return invalid measurement interval
		 if(count == D6T_16_CHANNEL_VALUE_BUFFER_ROW_LENGTH){
			 return D6T_STATUS_INVALID_MEASURMENT_INTERVAL;
		 }

		 break;
	 default:
                returnValue = D6T_STATUS_INVALID_SENSOR_TYPE;
		return returnValue;

    }

    return D6T_STATUS_SUCCESS;
}

D6T_STATUS_T D6T::setMeasurementInterval(uint8_t value)
{
   D6T_STATUS_T returnValue = D6T_STATUS_SUCCESS;

   switch(d6t_sensor_type)
   {
        case D6T_1X1_CHANNEL:

		//Copy interval values
                intervalBytePattern1And8Channel[1][1] = intervalValues1Channel[value - 1][0];
                intervalBytePattern1And8Channel[1][2] = intervalValues1Channel[value - 1][1];
                intervalBytePattern1And8Channel[1][3] = intervalValues1Channel[value - 1][2];

		//Write interval setting
                for(int i = 0;i < D6T_1AND8_CHANNEL_REG_PATTERN_ROW_LENGTH; i++)
                {
                    if((m_i2c->write(intervalBytePattern1And8Channel[i],D6T_1AND8_CHANNEL_REG_PATTERN_COLUMN_LENGTH)) != mraa::SUCCESS){
			    throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.write failed");
		    }
                }

                break;
        case D6T_1X8_CHANNEL:

		//Copy interval values
                intervalBytePattern1And8Channel[1][1] = intervalValues8Channel[value - 1][0];
                intervalBytePattern1And8Channel[1][2] = intervalValues8Channel[value - 1][1];
                intervalBytePattern1And8Channel[1][3] = intervalValues8Channel[value - 1][2];

		//Write inteval setting
                for(int i = 0;i < D6T_1AND8_CHANNEL_REG_PATTERN_ROW_LENGTH; i++)
                {
                    if((m_i2c->write(intervalBytePattern1And8Channel[i],D6T_1AND8_CHANNEL_REG_PATTERN_COLUMN_LENGTH)) != mraa::SUCCESS){
			    throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.write failed");
		    }
                }
		break;
	case D6T_4X4_CHANNEL:

                //Copy inteval values
                intervalBytePattern16Channel[1][1] = intervalValues16Channel[value - 1][0];
                intervalBytePattern16Channel[1][2] = intervalValues16Channel[value - 1][1];

		//Write inteval setting
                for(int i = 0;i < D6T_16_CHANNEL_REG_PATTERN_ROW_LENGTH; i++)
                {
                    if((m_i2c->write(intervalBytePattern16Channel[i],i==1?D6T_16_CHANNEL_REG_PATTERN_ROW_LENGTH:D6T_16_CHANNEL_REG_PATTERN_COLUMN_LENGTH)) != mraa::SUCCESS){
                            throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.write failed");
		    }
                }
                break;
	default:
	        returnValue = D6T_STATUS_INVALID_SENSOR_TYPE;
		return returnValue;
   }

   return returnValue;
}

D6T_STATUS_T D6T::getMeasurementSettingRegisterValue(uint8_t r_addr, uint8_t* data)
{
   D6T_STATUS_T returnValue = D6T_STATUS_SUCCESS;

   if(data == NULL)
	   return D6T_STATUS_INVALID_INPUT;

   //Write register
   if((m_i2c->writeByte(r_addr)) != mraa::SUCCESS){
            throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.writeByte failed");
   }

   //Read values
   if (!m_i2c->read(data, 2)){
             throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.read() failed");
   }

   return returnValue;
}

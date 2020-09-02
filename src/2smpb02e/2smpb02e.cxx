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

#include "2smpb02e.hpp"

using namespace upm;

OM2SMPB02E::OM2SMPB02E(INT32 bus,UINT8 address): mBus(bus), mAddress(address){

    OM2SMPB02E_VAL_MEASMODE_VALUES_T tempByte;

    // Create I2c object
    mI2c = new mraa::I2c(mBus);

    // Check for I2c object created well or not
    if(mI2c->address(mAddress) != mraa::SUCCESS){
        throw std::invalid_argument(std::string(__FUNCTION__) + ": I2c.address() failed");
    }

    // Set default Temperature scale to celsius
    mTempScale = upm::TemperatureUnit::CELSIUS;

    // Set default Pressure scale to pascal
    mPressureScale = upm::PressureUnit::PA;

    // Write register
    if((mI2c->writeByte(OM2SMPB02E_REGI2C_COEFS)) != mraa::SUCCESS){

        #ifdef DEBUG
        std::cout << "write failed to get co-effi values" << std::endl;
        #endif

        throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.writeByte failed");
    }

    // Read values
    if(!mI2c->read(mBuf, OM2SMPB02E_COEFFI_MAX_VALUE)){

        #ifdef DEBUG
        std::cout << "read failed to get co-effi values" << std::endl;
        #endif

        throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.read() failed");
    }

    // Get co-effi values
    getCOEFFValues(mCompFactor);

    try{// Get Measurement Mode
        getMeasurementMode(tempByte);
    }catch(const std::runtime_error& e){
        throw std::runtime_error(std::string(__FUNCTION__) + ": getMeasurementMode() failed");
    }

    try{// Get power mode
        getPowerMode(mPowerMode);
    }catch(const std::runtime_error& e){
        throw std::runtime_error(std::string(__FUNCTION__) + ": getPowerMode() failed");
    }

}

OM2SMPB02E::~OM2SMPB02E(){

    // Delete I2c instance
    delete mI2c;
}

OM2SMPB02E_STATUS_T OM2SMPB02E::getPresAndTempValues(FLOAT &outPres, FLOAT &outTemp){

    UINT8 buf[6] = {0};
    UINT32 rawpres,rawtemp;
    DOUBLE Pr, Po;
    DOUBLE Tr, Dt;

    outPres = 0;
    outTemp = 0;

    // Check power mode and return if sleep mode
    if(mPowerMode == OM2SMPB02E_VAL_POWERMODE_SLEEP){
        std::cerr << "Error: The sensor is in sleep mode. Please change the power mode to either forced or normal." << std::endl;
        return OM2SMPB02E_STATUS_SLEEP_MODE;
    }

    // Check measurement mode average value
    if((mPresMeasValue == OM2SMPB02E_VAL_PRESAVERAGE_00) && (mTempMeasValue == OM2SMPB02E_VAL_TEMPAVERAGE_00)){
        std::cerr << "Error: Get pressure and temperature value requires both measurement mode enabled." << std::endl;
        return OM2SMPB02E_STATUS_TEMP_PRES_MEASUREMENT_SKIP;
    }

    // Write register
    if((mI2c->writeByte(OM2SMPB02E_REGI2C_PRES_TXD2)) != mraa::SUCCESS){

        #ifdef DEBUG
        std::cout << "write failed to get pres register" << std::endl;
        #endif

        throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.writeByte failed");
    }

    // Read values
    if(!mI2c->read(buf, sizeof(buf))){

        #ifdef DEBUG
        std::cout << "read failed to get pres value" << std::endl;
        #endif

        throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.read() failed");
    }

    rawpres = OM2SMPB02E_CONV8S_S24_BE(buf[0], buf[1], buf[2]);
    rawtemp = OM2SMPB02E_CONV8S_S24_BE(buf[3], buf[4], buf[5]);

    // Check power mode
    if(mPowerMode == OM2SMPB02E_VAL_POWERMODE_FORCED){
        try{// Set power mode
            setPowerMode(mPowerMode);
        }catch(const std::runtime_error& e){
            throw std::runtime_error(std::string(__FUNCTION__) + ": setPowerMode() failed");
        }
    }

    #ifdef DEBUG
    std::cout << "Raw pres data : " << rawpres << std::endl;
    std::cout << "Raw temp data : " << rawtemp << std::endl;
    #endif

    // temperature compensation
    if(rawtemp){
        Dt = (INT32)rawtemp - 0x800000;
        Tr = mCompFactor._A0 + (mCompFactor._A1 * Dt) + (mCompFactor._A2 * (Dt * Dt));

        // Check Temperature average value
        if(mTempMeasValue != OM2SMPB02E_VAL_TEMPAVERAGE_00){
            outTemp = Tr/256.0;

            // Check temperature unit scale and convert
            if(mTempScale != upm::TemperatureUnit::CELSIUS){
                outTemp = convertCelsiusTo(outTemp,mTempScale);
            }
        }
    }

    // barometer compensation
    if(rawtemp && rawpres){
        Pr = (INT32)rawpres - 0x800000;
        Po = mCompFactor._B00 + (mCompFactor._BT1 * Tr) + (mCompFactor._BP1 * Pr) +
        (mCompFactor._B11 * Tr * Pr) + mCompFactor._BT2 * (Tr * Tr) +
        (mCompFactor._BP2 * (Pr * Pr)) + (mCompFactor._B12 * Pr * (Tr * Tr)) +
        (mCompFactor._B21 * (Pr * Pr) * Tr) + (mCompFactor._BP3 * (Pr * Pr * Pr));

        // Check Pressure average value
        if((mPresMeasValue != OM2SMPB02E_VAL_PRESAVERAGE_00) && (mTempMeasValue != OM2SMPB02E_VAL_TEMPAVERAGE_00)){
            outPres = Po;

            // Check pressure unit scale and convert
            if(mPressureScale != upm::PressureUnit::PA){
                outPres = convertPaTo(outPres,mPressureScale);
            }
        }
    }

    #ifdef DEBUG
    std::cout << "outTemp data : " << outTemp  << std::endl;
    std::cout << "outPres data : " << outPres  << std::endl;
    #endif

    return OM2SMPB02E_STATUS_SUCCESS;
}

OM2SMPB02E_STATUS_T OM2SMPB02E::getPressure(FLOAT &outPres){

    FLOAT temTemp;

    // Check power mode and return if sleep mode
    if(mPowerMode == OM2SMPB02E_VAL_POWERMODE_SLEEP){
        std::cerr << "Error: The sensor is in sleep mode. Please change the power mode to either forced or normal." << std::endl;
        return OM2SMPB02E_STATUS_SLEEP_MODE;
    }

    // Check Pressure average value
    if(mPresMeasValue == OM2SMPB02E_VAL_PRESAVERAGE_00 || mTempMeasValue == OM2SMPB02E_VAL_TEMPAVERAGE_00){
        std::cerr << "Error: Get pressure value requires temperature and pressure both measurement mode enabled." << std::endl;
        return OM2SMPB02E_STATUS_TEMP_PRES_MEASUREMENT_SKIP;
    }

    // Get Preesure value
    return getPresAndTempValues(outPres,temTemp);
}

OM2SMPB02E_STATUS_T OM2SMPB02E::getTemperature(FLOAT &outTemp){

    FLOAT temPres;

    // Check power mode and return if sleep mode
    if(mPowerMode == OM2SMPB02E_VAL_POWERMODE_SLEEP){
        std::cerr << "Error: The sensor is in sleep mode. Please change the power mode to either forced or normal." << std::endl;
        return OM2SMPB02E_STATUS_SLEEP_MODE;
    }

    // Check Temperature average value
    if(mTempMeasValue == OM2SMPB02E_VAL_TEMPAVERAGE_00){
        std::cerr << "Error: Get temperature value requires temperature measurement mode enabled." << std::endl;
        return OM2SMPB02E_STATUS_TEMP_MEASUREMENT_SKIP;
    }

    // Get Temperature value
    return getPresAndTempValues(temPres,outTemp);
}

OM2SMPB02E_STATUS_T OM2SMPB02E::getPowerMode(OM2SMPB02E_VAL_POWERMODE_VALUES_T &powerMode){

    UINT8 tempByte;

    // Write register
    if((mI2c->writeByte(OM2SMPB02E_REGI2C_CTRL_MEAS)) != mraa::SUCCESS){

        #ifdef DEBUG
        std::cout << "write failed getPowerMode" << std::endl;
        #endif

        throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.writeByte failed");
    }

    // Read values
    if(!mI2c->read(&tempByte, 1)){

        #ifdef DEBUG
        std::cout << "read failed getPowerMode" << std::endl;
        #endif

        throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.read() failed");
    }

    #ifdef DEBUG
    std::cout << "Power Mode Register value : " << (unsigned)tempByte << std::endl;
    #endif

    // Parse power mode bits
    powerMode = (OM2SMPB02E_VAL_POWERMODE_VALUES_T)(tempByte & OM2SMPB02E_MASK_POWERMODE_VALUE);

    return OM2SMPB02E_STATUS_SUCCESS;
}

OM2SMPB02E_STATUS_T OM2SMPB02E::setPowerMode(OM2SMPB02E_VAL_POWERMODE_VALUES_T powerMode){

    OM2SMPB02E_VAL_MEASMODE_VALUES_T tempByte;
    UINT8 tempBuf[2];

    try{// Get measurement mode
        getMeasurementMode(tempByte);
    }catch(const std::runtime_error& e){
        std::cerr << "Error while handling: " << e.what() << std::endl;
        return OM2SMPB02E_STATUS_FAILURE;
    }

    tempBuf[0] = OM2SMPB02E_REGI2C_CTRL_MEAS;
    tempBuf[1] = (powerMode | tempByte);

    // Write register
    if((mI2c->write(tempBuf,sizeof(tempBuf))) != mraa::SUCCESS){

        #ifdef DEBUG
        std::cout << "write failed setPowerMode" << std::endl;
        #endif

        throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.write failed");
    }

    mPowerMode = powerMode;

    return OM2SMPB02E_STATUS_SUCCESS;
}

OM2SMPB02E_STATUS_T OM2SMPB02E::getMeasurementMode(OM2SMPB02E_VAL_MEASMODE_VALUES_T &measurementMode){

    UINT8 tempByte;

    // Write register
    if((mI2c->writeByte(OM2SMPB02E_REGI2C_CTRL_MEAS)) != mraa::SUCCESS){

        #ifdef DEBUG
        std::cout << "write failed getMeasurementMode" << std::endl;
        #endif

        throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.writeByte failed");
    }

    // Read values
    if(!mI2c->read(&tempByte, 1)){

        #ifdef DEBUG
        std::cout << "Read failed getMeasurementMode" << std::endl;
        #endif

        throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.read() failed");
    }

    // Parse measurement mode bits
    measurementMode = (OM2SMPB02E_VAL_MEASMODE_VALUES_T)(tempByte & (OM2SMPB02E_MASK_TEMPAVERAGE|OM2SMPB02E_MASK_PRESAVERAGE));

    // Mask the pressure and temperature value
    mPresMeasValue = (measurementMode & OM2SMPB02E_MASK_PRESAVERAGE);
    mTempMeasValue = (measurementMode & OM2SMPB02E_MASK_TEMPAVERAGE);

    return OM2SMPB02E_STATUS_SUCCESS;
}

OM2SMPB02E_STATUS_T OM2SMPB02E::setMeasurementMode(OM2SMPB02E_VAL_MEASMODE_VALUES_T measurementMode){

    UINT8 tempBuf[2];

    // Check measurement mode average value
    if(((measurementMode & OM2SMPB02E_MASK_PRESAVERAGE) == OM2SMPB02E_VAL_PRESAVERAGE_00) ||
            ((measurementMode & OM2SMPB02E_MASK_TEMPAVERAGE) == OM2SMPB02E_VAL_TEMPAVERAGE_00)){
        return OM2SMPB02E_STATUS_TEMP_PRES_MEASUREMENT_SKIP;
    }

    try{// Get power mode
        getPowerMode(mPowerMode);
    }catch(const std::runtime_error& e){
        std::cerr << "Error while handling: " << e.what() << std::endl;
        return OM2SMPB02E_STATUS_FAILURE;
    }

    tempBuf[0] = OM2SMPB02E_REGI2C_CTRL_MEAS;
    tempBuf[1] = (measurementMode | mPowerMode);

    // Write register
    if((mI2c->write(tempBuf,sizeof(tempBuf))) != mraa::SUCCESS){

        #ifdef DEBUG
        std::cout << "write failed setMeasurementMode" << std::endl;
        #endif

        throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.write failed");
    }

    // Mask the pressure and temperature value
    mPresMeasValue = (measurementMode & OM2SMPB02E_MASK_PRESAVERAGE);
    mTempMeasValue = (measurementMode & OM2SMPB02E_MASK_TEMPAVERAGE);

    return OM2SMPB02E_STATUS_SUCCESS;
}

OM2SMPB02E_STATUS_T OM2SMPB02E::getStandbyTimeValue(OM2SMPB02E_VAL_IOSETUP_STANDBY_VALUES_T &standbyTimeValue){

    UINT8 tempByte;

    // Write register
    if((mI2c->writeByte(OM2SMPB02E_REGI2C_IO_SETUP)) != mraa::SUCCESS){

        #ifdef DEBUG
        std::cout << "Write failed getStandbyTimeValue" << std::endl;
        #endif

        throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.writeByte failed");
    }

    // Read values
    if(!mI2c->read(&tempByte, 1)){

        #ifdef DEBUG
        std::cout << "Read failed getStandbyTimeValue" << std::endl;
        #endif

        throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.read() failed");
    }

    // Parse standby value bits
    standbyTimeValue = (OM2SMPB02E_VAL_IOSETUP_STANDBY_VALUES_T)(tempByte & OM2SMPB02E_MASK_TEMPAVERAGE);

    return OM2SMPB02E_STATUS_SUCCESS;
}

OM2SMPB02E_STATUS_T OM2SMPB02E::setStandbyTimeValue(OM2SMPB02E_VAL_IOSETUP_STANDBY_VALUES_T standbyTimeValue){

    UINT8 tempBuf[2];

    tempBuf[0] = OM2SMPB02E_REGI2C_IO_SETUP;
    tempBuf[1] = standbyTimeValue;

    // Write register
    if((mI2c->write(tempBuf,sizeof(tempBuf))) != mraa::SUCCESS){

        #ifdef DEBUG
        std::cout << "write failed setStandbyTimeValue" << std::endl;
        #endif

        throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.write failed");
    }

    return OM2SMPB02E_STATUS_SUCCESS;
}

OM2SMPB02E_STATUS_T OM2SMPB02E::getIIRFilterValue(OM2SMPB02E_VAL_IIR_VALUES_T &iirValue){

    UINT8 tempByte;

    // Write register
    if((mI2c->writeByte(OM2SMPB02E_REGI2C_IIR)) != mraa::SUCCESS){

        #ifdef DEBUG
        std::cout << "Write failed getIIRFilterValue" << std::endl;
        #endif

        throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.writeByte failed");
    }

    // Read values
    if(!mI2c->read(&tempByte, 1)){

        #ifdef DEBUG
        std::cout << "Read failed getIIRFilterValue" << std::endl;
        #endif

        throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.read() failed");
    }

    // Assign IIT Filter value
    iirValue = (OM2SMPB02E_VAL_IIR_VALUES_T)(tempByte);

    return OM2SMPB02E_STATUS_SUCCESS;
}

OM2SMPB02E_STATUS_T OM2SMPB02E::setIIRFilterValue(OM2SMPB02E_VAL_IIR_VALUES_T iirValue){

    UINT8 tempBuf[2];

    tempBuf[0] = OM2SMPB02E_REGI2C_IIR;
    tempBuf[1] = iirValue;

    // Write register
    if((mI2c->write(tempBuf,sizeof(tempBuf))) != mraa::SUCCESS){

        #ifdef DEBUG
        std::cout << "write failed setIIRFilterValue" << std::endl;
        #endif

        throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.write failed");
    }

    return OM2SMPB02E_STATUS_SUCCESS;
}

OM2SMPB02E_STATUS_T OM2SMPB02E::getTemperatureScale(upm::TemperatureUnit &unit){

    // Get temperature scale
    unit = mTempScale;
    return OM2SMPB02E_STATUS_SUCCESS;
}

OM2SMPB02E_STATUS_T OM2SMPB02E::setTemperatureScale(upm::TemperatureUnit unit){

    // Set temperature scale
    mTempScale = unit;
    return OM2SMPB02E_STATUS_SUCCESS;
}

OM2SMPB02E_STATUS_T OM2SMPB02E::getPressureScale(upm::PressureUnit &unit){

    // Get pressure scale
    unit = mPressureScale;
    return OM2SMPB02E_STATUS_SUCCESS;
}

OM2SMPB02E_STATUS_T OM2SMPB02E::setPressureScale(upm::PressureUnit unit){

    // Set pressure scale
    mPressureScale = unit;
    return OM2SMPB02E_STATUS_SUCCESS;
}

OM2SMPB02E_STATUS_T OM2SMPB02E::getDeviceId(UINT8 &deviceId){

    // Write register
    if((mI2c->writeByte(OM2SMPB02E_REGI2C_CHIP_ID)) != mraa::SUCCESS){

        #ifdef DEBUG
        std::cout << "write failed getDeviceId" << std::endl;
        #endif

        throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.writeByte failed");
    }

    // Read values
    if(!mI2c->read(&deviceId, 1)){

        #ifdef DEBUG
        std::cout << "read failed getDeviceId" << std::endl;
        #endif

        throw std::runtime_error(std::string(__FUNCTION__) + ": I2c.read() failed");
    }

    return OM2SMPB02E_STATUS_SUCCESS;
}

OM2SMPB02E_STATUS_T OM2SMPB02E::setI2cFrequency(mraa::I2cMode frequency){

    // Set I2C frequency
    if(mI2c->frequency(frequency) != mraa::SUCCESS){

        #ifdef DEBUG
        std::cout << "set I2C frequency failed" << std::endl;
        #endif

        throw std::invalid_argument(std::string(__FUNCTION__) + ": I2c.frequency() failed");
    }
    return OM2SMPB02E_STATUS_SUCCESS;
}

OM2SMPB02E_STATUS_T OM2SMPB02E::getCOEFFValues(OM2SMPB02E_SETTING_T &setting){

    // pressure parameters
    setting.ex = (mBuf[OM2SMPB02E_COEFFI_MAX_VALUE - 1] & 0xF0) >> 4;

    conv20Q4ToDouble(mBuf,setting._B00,setting.ex,0);
    conv16ToDouble(OM2SMPB02E_COEFF_A_BT1,OM2SMPB02E_COEFF_S_BT1,mBuf,setting._BT1,2);
    conv16ToDouble(OM2SMPB02E_COEFF_A_BT2,OM2SMPB02E_COEFF_S_BT2,mBuf,setting._BT2,4);
    conv16ToDouble(OM2SMPB02E_COEFF_A_BP1,OM2SMPB02E_COEFF_S_BP1,mBuf,setting._BP1,6);
    conv16ToDouble(OM2SMPB02E_COEFF_A_B11,OM2SMPB02E_COEFF_S_B11,mBuf,setting._B11,8);
    conv16ToDouble(OM2SMPB02E_COEFF_A_BP2,OM2SMPB02E_COEFF_S_BP2,mBuf,setting._BP2,10);
    conv16ToDouble(OM2SMPB02E_COEFF_A_B12,OM2SMPB02E_COEFF_S_B12,mBuf,setting._B12,12);
    conv16ToDouble(OM2SMPB02E_COEFF_A_B21,OM2SMPB02E_COEFF_S_B21,mBuf,setting._B21,14);
    conv16ToDouble(OM2SMPB02E_COEFF_A_BP3,OM2SMPB02E_COEFF_S_BP3,mBuf,setting._BP3,16);

    // temperature parameters
    setting.ex = (mBuf[OM2SMPB02E_COEFFI_MAX_VALUE - 1] & 0x0F);

    conv20Q4ToDouble(mBuf,setting._A0,setting.ex,18);
    conv16ToDouble(OM2SMPB02E_COEFF_A_A1,OM2SMPB02E_COEFF_S_A1,mBuf,setting._A1,20);
    conv16ToDouble(OM2SMPB02E_COEFF_A_A2,OM2SMPB02E_COEFF_S_A2,mBuf,setting._A2,22);

    return OM2SMPB02E_STATUS_SUCCESS;
}

OM2SMPB02E_STATUS_T OM2SMPB02E::conv16ToDouble(DOUBLE a, DOUBLE s, UINT8* inValue, DOUBLE &outValue, INT32 offSet){

    UINT16 val;
    INT16  ret;

    // Integer values shift
    val = (UINT16)((UINT16)(inValue[offSet] << 8) | (UINT16)inValue[offSet + 1]);

    // Check value
    if((val & 0x8000) != 0){
        ret = (INT16)((INT32)val - 0x10000);
    }else{
        ret = val;
    }

    // Convert value
    outValue = a + (DOUBLE)ret * s /32767.0;

    return OM2SMPB02E_STATUS_SUCCESS;
}

OM2SMPB02E_STATUS_T OM2SMPB02E::conv20Q4ToDouble(UINT8* inValue, DOUBLE &outValue, UINT8 ex, INT32 offSet){

    INT32 ret;
    UINT32 val;

    // Integet values shift
    val = (UINT32)((inValue[offSet] << 12) | (inValue[offSet + 1] << 4) | ex);

    // Check value
    if((val & 0x80000) != 0){
        ret = (INT32)val - 0x100000;
    }else{
        ret = val;
    }

    // Convert value
    outValue =  (DOUBLE)ret / 16.0;

    return OM2SMPB02E_STATUS_SUCCESS;
}


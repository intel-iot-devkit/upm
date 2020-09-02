/*
* Author: Takashi Kakiuchi <omronsupportupm@omron.com>
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

#include "b5t007001.hpp"
#include <stdexcept>
#include <unistd.h>

using namespace upm;

/**
 * Command number
 */
#define HVC_COM_GET_VERSION             (UINT8)0x00
#define HVC_COM_SET_CAMERA_ANGLE        (UINT8)0x01
#define HVC_COM_GET_CAMERA_ANGLE        (UINT8)0x02
#define HVC_COM_EXECUTE                 (UINT8)0x03
#define HVC_COM_EXECUTEEX               (UINT8)0x04
#define HVC_COM_SET_THRESHOLD           (UINT8)0x05
#define HVC_COM_GET_THRESHOLD           (UINT8)0x06
#define HVC_COM_SET_SIZE_RANGE          (UINT8)0x07
#define HVC_COM_GET_SIZE_RANGE          (UINT8)0x08
#define HVC_COM_SET_DETECTION_ANGLE     (UINT8)0x09
#define HVC_COM_GET_DETECTION_ANGLE     (UINT8)0x0A
#define HVC_COM_SET_BAUDRATE            (UINT8)0x0E
#define HVC_COM_REGISTRATION            (UINT8)0x10
#define HVC_COM_DELETE_DATA             (UINT8)0x11
#define HVC_COM_DELETE_USER             (UINT8)0x12
#define HVC_COM_DELETE_ALL              (UINT8)0x13
#define HVC_COM_GET_PERSON_DATA         (UINT8)0x15
#define HVC_COM_SAVE_ALBUM              (UINT8)0x20
#define HVC_COM_LOAD_ALBUM              (UINT8)0x21
#define HVC_COM_WRITE_ALBUM             (UINT8)0x22

/**
 * HVC frame buffer
 */
#define HVC_UART_FRAME_SIZE               32

/**
 * RX ignore temp buffer size
 */
#define RX_IGNORE_TEMP_BUF_SIZE 10000

/**
 * Debug print macro
 */
#define DEBUG_PRINT 0

/**
 *  Header for send signal data
 */
typedef enum {
    SEND_HEAD_SYNCBYTE = 0,
    SEND_HEAD_COMMANDNO,
    SEND_HEAD_DATALENGTHLSB,
    SEND_HEAD_DATALENGTHMSB,
    SEND_HEAD_NUM
}SEND_HEADER;

/**
 * Header for receive signal data
 */
typedef enum {
    RECEIVE_HEAD_SYNCBYTE = 0,
    RECEIVE_HEAD_STATUS,
    RECEIVE_HEAD_DATALENLL,
    RECEIVE_HEAD_DATALENLM,
    RECEIVE_HEAD_DATALENML,
    RECEIVE_HEAD_DATALENMM,
    RECEIVE_HEAD_NUM
}RECEIVE_HEADER;



B5T007001::B5T007001(UINT8 uart,HVC_UART_BAUD_RATE_T inRate){
    //Create UART object
    mUart = new mraa::Uart(uart);
    mraa::Result returnValue;

    //Check for UART object created well or not
    if(mUart == NULL){
        throw std::invalid_argument(std::string(__FUNCTION__) + ": Failed to initialize UART");
    }
    //Update baudrate for connected UART
    returnValue = mUart->setBaudRate(inRate);
    if(returnValue != mraa::SUCCESS){
        throw std::runtime_error(std::string(__FUNCTION__) +
                               ": failed to set baudrate" );
    }
    //STB default disabled
    mStbStatus = 0;

    // Init STB Handle
    mSTBHandle = NULL;

    //Set default timeout for UART
    returnValue = mUart->setTimeout(UART_SETTING_TIMEOUT, UART_SETTING_TIMEOUT, UART_SETTING_TIMEOUT);
    if(returnValue != mraa::SUCCESS){
        throw std::runtime_error(std::string(__FUNCTION__) +
                               ": failed to set timeout" );
    }
}

B5T007001::~B5T007001() {
    //Delete UART instance
    delete mUart;
    //Deinit SBI library
    stbDeInit();
}

B5T_STATUS_T B5T007001::UARTSendData(INT32 inDataSize, const UINT8 *inData){
    INT32 outWriteCount;

    if(NULL == inData){
        return B5T_STATUS_INVALID_INPUT;
    }
    if(inData[0] == 0xFE){
        //Ignore all character on rx line
        UARTRxIgnoreAllData();
    }

    //UART write
    outWriteCount = mUart->write((const INT8*)inData, inDataSize);

    //Handle error cases
    if(-1 == outWriteCount){
        std::cerr << std::string(__FUNCTION__) << ": Write failed with error code =" << outWriteCount << std::endl;
        return B5T_STATUS_WRITE_FAILED;
    }
    else if(inDataSize != outWriteCount){
        std::cerr << std::string(__FUNCTION__) << ": Write failed with un-sufficient data write. Exepected count= " << inDataSize << "And Actual count " << outWriteCount << std::endl;
        return B5T_STATUS_WRITE_FAILED;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::UARTReceiveData(INT32 inTimeOutTime, INT32 inDataSize, UINT8 *outResult, UINT8 isSyncRequired){
    INT32 tmpReadCount = 0;
    INT32 totalReadCount = 0;
    INT32 syncIndex=0, count=0;
    totalReadCount = 0;
    while(1){
        // Wait for data available on UART
#if DEBUG_PRINT
        std::cout << "DEBUG: Before dataAvail" << std::endl;
#endif
        if (!mUart->dataAvailable(inTimeOutTime))
        {
            std::cerr << __FUNCTION__ << ": Execution interrupted or timeout occurs for response" << std::endl;
            return B5T_STATUS_READ_TIMEOUT;
        }
        // Read data
#if DEBUG_PRINT
        std::cout << "DEBUG: Befor read inDataSize =" << inDataSize << "totalReadCount =" << totalReadCount << std::endl;
#endif
        tmpReadCount = mUart->read((INT8*)(outResult + totalReadCount), inDataSize - totalReadCount);
        if(-1 == tmpReadCount){
            std::cerr << __FUNCTION__ << ": Read failed" << std::endl;
	    return B5T_STATUS_READ_FAILED;
        }
        totalReadCount = totalReadCount + tmpReadCount;
	if((isSyncRequired == 1) && (outResult[0] != (UINT8)0xFE)){
            syncIndex = 0;
            //Remove all bytes before syncbyte
#if DEBUG_PRINT
	    std::cout << "DEBUG: inDataSize = " << inDataSize << "tmpReadCount = " << tmpReadCount << "totalReadCount = " << totalReadCount << std::endl;
            std::cerr << "DEBUG: re-read data as sync byte not received" << std::endl;
#endif
	    for(count = 0;count < totalReadCount; count++){
		if(outResult[count] == (UINT8)0xFE){
                    syncIndex = count;
                    break;
		}
	    }
	    if(syncIndex == 0){
#if DEBUG_PRINT
	        std::cout << "DEBUG: Ignorring all read count = " <<  totalReadCount <<std::endl;
#endif
                totalReadCount = 0;
		continue;
	    }
	    INT32 j=0;
	    for(count = syncIndex;count < totalReadCount; count++){
                outResult[j++] = outResult[count];
#if DEBUG_PRINT
		std::cout << "DEBUG: Moved index = " << count << "And j = " << j << " value =" << unsigned(outResult[count]) << std::endl;
#endif
	    }
	    totalReadCount -= syncIndex;
#if DEBUG_PRINT
	    std::cout << "DEBUG:  totalReadCount = " <<  totalReadCount << "syncIndex = " << syncIndex <<std::endl;
#endif
	    continue;
	}
        if(totalReadCount == inDataSize){
#if DEBUG_PRINT
	    std::cout << "DEBUG:  totalReadCount = " <<  totalReadCount << "inDataSize = " << inDataSize <<std::endl;
#endif
            break;
        }
	continue;
    }
    if(inDataSize != totalReadCount){
        std::cerr << std::string(__FUNCTION__) << ": Expected read count ="
		<< inDataSize << " Actual read count =" << totalReadCount << std::endl;
	return B5T_STATUS_READ_FAILED;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::UARTRxIgnoreAllData(void){
    UINT8 tmpBuf[RX_IGNORE_TEMP_BUF_SIZE];
    UINT32 maxTry = 0;
    INT32 tmpReadCount = 0;

    while(maxTry < 40){ //Maximum 40sec delay for executeDetection
        // Wait for data available on UART
        if (!mUart->dataAvailable(100)){ //100milisecond delay
#if DEBUG_PRINT
            std::cout << "DEBUG: No data here on RX Line" << std::endl;
#endif
            //Remove debug
            return B5T_STATUS_SUCCESS;
        }
	// Read data
	tmpReadCount = mUart->read((INT8*)tmpBuf, RX_IGNORE_TEMP_BUF_SIZE);
        if(-1 == tmpReadCount){
            break;
        }
#if DEBUG_PRINT
        std::cout << "DEBUG: Deleted data count = " << tmpReadCount << std::endl;
#endif
	sleep(1); //Sleep for 1sec as need to receive all data
	maxTry++;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::HVCSendCommand(UINT8 inCommandNo, INT32 inDataSize, UINT8 *inData){
    INT32 count;
    B5T_STATUS_T ret;
    UINT8 sendData[HVC_UART_FRAME_SIZE];

    //Create header
    sendData[SEND_HEAD_SYNCBYTE]        = (UINT8)0xFE;
    sendData[SEND_HEAD_COMMANDNO]       = (UINT8)inCommandNo;
    sendData[SEND_HEAD_DATALENGTHLSB]   = (UINT8)(inDataSize&0xff);
    sendData[SEND_HEAD_DATALENGTHMSB]   = (UINT8)((inDataSize>>8)&0xff);

    for(count = 0; count < inDataSize; count++){
        sendData[SEND_HEAD_NUM + count] = inData[count];
    }

    //Send command signal
    ret = UARTSendData(SEND_HEAD_NUM+inDataSize, sendData);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code =" << ret << std::endl;
        return ret;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::HVCReceiveData(INT32 inTimeOutTime, INT32 inDataSize, UINT8 *outResult){
    B5T_STATUS_T ret;
    //Check for invalid read size
    if(inDataSize <= 0){
        std::cerr << std::string(__FUNCTION__) << ": Invalid input" << std::endl;
        return B5T_STATUS_INVALID_INPUT;
    }
    //Receive data
    ret = UARTReceiveData(inTimeOutTime, inDataSize, outResult);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code =" << ret << std::endl;
        return ret;
    }
    return ret;
}

B5T_STATUS_T B5T007001::HVCReceiveHeader(INT32 inTimeOutTime, INT32 &outDataSize){
    B5T_STATUS_T ret;
    UINT8 headerData[HVC_UART_FRAME_SIZE];
    UINT8 outStatus= 0;

    // Get header part
    ret = UARTReceiveData(inTimeOutTime, RECEIVE_HEAD_NUM, headerData, 1);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }
    if((UINT8)0xFE != headerData[RECEIVE_HEAD_SYNCBYTE]){
        std::cerr << std::string(__FUNCTION__) << ": Header invalid data" << std::endl;
        return B5T_STATUS_HEADER_INVALID;
    }

    // Get data length
    outDataSize = headerData[RECEIVE_HEAD_DATALENLL] +
                    (headerData[RECEIVE_HEAD_DATALENLM]<<8) +
                    (headerData[RECEIVE_HEAD_DATALENML]<<16) +
                    (headerData[RECEIVE_HEAD_DATALENMM]<<24);

    //Get command execution result
    outStatus  = headerData[RECEIVE_HEAD_STATUS];

    //Check status code
    if(0 != outStatus){
        std::cerr << std::string(__FUNCTION__) << ": failed with firmware response code =" << unsigned(outStatus) <<std::endl;
	return B5T_STATUS_FW_INTERNAL_ERROR;
    }
    return ret;
}

B5T_STATUS_T B5T007001::HVCRegistration(INT32 inTimeOutTime, INT32 inUserID, INT32 inDataID, HVC_IMAGE &outImage){
    B5T_STATUS_T ret;
    INT32 size = 0;
    UINT8 sendData[HVC_UART_FRAME_SIZE];
    UINT8 recvData[HVC_UART_FRAME_SIZE];

    //Send Registration signal command
    sendData[0] = (UINT8)(inUserID&0xff);
    sendData[1] = (UINT8)((inUserID>>8)&0xff);
    sendData[2] = (UINT8)(inDataID&0xff);
    ret = HVCSendCommand(HVC_COM_REGISTRATION, sizeof(UINT8)*3, sendData);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    // Receive header
    ret = HVCReceiveHeader(inTimeOutTime, size);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    // Receive data
    if ( size >= (INT32)sizeof(UINT8)*4 ) {
        ret = HVCReceiveData(inTimeOutTime, sizeof(UINT8)*4, recvData);
        outImage.width = recvData[0] + (recvData[1]<<8);
        outImage.height = recvData[2] + (recvData[3]<<8);
        if(ret != B5T_STATUS_SUCCESS){
            std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
            return ret;
        }
        size -= sizeof(UINT8)*4;
    }

    // Image data
    if ( size >= (INT32)sizeof(UINT8)*64*64 ) {
        ret = HVCReceiveData(inTimeOutTime, sizeof(UINT8)*64*64, outImage.image);
        if(ret != B5T_STATUS_SUCCESS){
            std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
            return ret;
        }
        size -= sizeof(UINT8)*64*64;
    }
    return ret;
}

B5T_STATUS_T B5T007001::getVersion(INT32 inTimeOutTime, HVC_VERSION &outVersion){
    B5T_STATUS_T ret;
    INT32 size = 0;
    if(inTimeOutTime < 0){
        inTimeOutTime = UART_SETTING_TIMEOUT;
    }
    //Send GetVersion command signal
    ret = HVCSendCommand(HVC_COM_GET_VERSION, 0, NULL);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    //Receive header
    ret = HVCReceiveHeader(inTimeOutTime, size);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    if(size > (INT32)sizeof(HVC_VERSION) ) {
        size = sizeof(HVC_VERSION);
    }

    // Receive data
    ret = HVCReceiveData(inTimeOutTime, size, (UINT8*)&outVersion);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    return ret;
}

B5T_STATUS_T B5T007001::setCameraAngle(INT32 inTimeOutTime, UINT8 inAngleNo){
    B5T_STATUS_T ret;
    INT32 size = 0;
    UINT8 sendData[HVC_UART_FRAME_SIZE];
    if(inTimeOutTime < 0){
        inTimeOutTime = UART_SETTING_TIMEOUT;
    }

    sendData[0] = (UINT8)(inAngleNo&0xff);
    //Send SetCameraAngle command signal
    ret = HVCSendCommand(HVC_COM_SET_CAMERA_ANGLE, sizeof(UINT8), sendData);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    //Receive header
    ret = HVCReceiveHeader(inTimeOutTime, size);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    return ret;
}

B5T_STATUS_T B5T007001::getCameraAngle(INT32 inTimeOutTime, UINT8 &outAngleNo){
    B5T_STATUS_T ret;
    INT32 size = 0;
    UINT8 recvData[HVC_UART_FRAME_SIZE];
    if(inTimeOutTime < 0){
        inTimeOutTime = UART_SETTING_TIMEOUT;
    }

    //Send GetCameraAngle command signal
    ret = HVCSendCommand(HVC_COM_GET_CAMERA_ANGLE, 0, NULL);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    //Receive header
    ret = HVCReceiveHeader(inTimeOutTime, size);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    if ( size > (INT32)sizeof(UINT8) ) {
        size = sizeof(UINT8);
    }

    //Receive data
    ret = HVCReceiveData(inTimeOutTime, size, recvData);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }
    outAngleNo = recvData[0];

    return ret;
}

B5T_STATUS_T B5T007001::executeDetection(INT32 inTimeOutTime, INT32 inExec, INT32 inImageInfo, HVC_RESULT &outHVCResult,INT32 *outSTBFaceCount, STB_FACE **outSTBFaceResult,INT32 *outSTBBodyCount,STB_BODY **outSTBBodyResult){
    B5T_STATUS_T ret=B5T_STATUS_SUCCESS;
#if DEBUG_PRINT
    std::cout << "DEBUG: mStbStatus = " << mStbStatus << std::endl;
#endif
    if(inTimeOutTime < 0){
        inTimeOutTime = UART_EXECUTE_TIMEOUT;
    }
    ret = executeDetection(inTimeOutTime, inExec, inImageInfo, outHVCResult);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }
    if(mStbStatus == 0){
        return ret;
    }
#if DEBUG_PRINT
    std::cout << "DEBUG: calling STB " << std::endl;
#endif
    ret = stbExec(inExec, outHVCResult, outSTBFaceCount, outSTBFaceResult, outSTBBodyCount, outSTBBodyResult);
#if DEBUG_PRINT
    std::cout << "DEBUG: calling STB retStatus" << ret << std::endl;
#endif
    return ret;
}

B5T_STATUS_T B5T007001::executeDetection(INT32 inTimeOutTime, INT32 inExec, INT32 inImageInfo, HVC_RESULT &outHVCResult){
    B5T_STATUS_T ret=B5T_STATUS_SUCCESS;
    INT32 i, j;
    INT32 size = 0;
    UINT8 sendData[HVC_UART_FRAME_SIZE];
    UINT8 recvData[HVC_UART_FRAME_SIZE];
    if(inTimeOutTime < 0){
        inTimeOutTime = UART_EXECUTE_TIMEOUT;
    }

    // Send Execute command signal
    sendData[0] = (UINT8)(inExec&0xff);
    sendData[1] = (UINT8)((inExec>>8)&0xff);
    sendData[2] = (UINT8)(inImageInfo&0xff);
    ret = HVCSendCommand(HVC_COM_EXECUTEEX, sizeof(UINT8)*3, sendData);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    // Receive header
    ret = HVCReceiveHeader(inTimeOutTime, size);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    // Receive result data
    if ( size >= (INT32)sizeof(UINT8)*4 ) {
        outHVCResult.executedFunc = inExec;
        ret = HVCReceiveData(inTimeOutTime, sizeof(UINT8)*4, recvData);
        outHVCResult.bdResult.num = recvData[0];
        outHVCResult.hdResult.num = recvData[1];
        outHVCResult.fdResult.num = recvData[2];
        if(ret != B5T_STATUS_SUCCESS){
            std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
            return ret;
        }
        size -= sizeof(UINT8)*4;
    }

    // Get Human Body Detection result
    for(i = 0; i < outHVCResult.bdResult.num; i++){
        if ( size >= (INT32)sizeof(UINT8)*8 ) {
            ret = HVCReceiveData(inTimeOutTime, sizeof(UINT8)*8, recvData);
            outHVCResult.bdResult.bdResult[i].posX = (short)(recvData[0] + (recvData[1]<<8));
            outHVCResult.bdResult.bdResult[i].posY = (short)(recvData[2] + (recvData[3]<<8));
            outHVCResult.bdResult.bdResult[i].size = (short)(recvData[4] + (recvData[5]<<8));
            outHVCResult.bdResult.bdResult[i].confidence = (short)(recvData[6] + (recvData[7]<<8));
            if(ret != B5T_STATUS_SUCCESS){
                std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
                return ret;
            }
            size -= sizeof(UINT8)*8;
        }
    }

    // Get Hand Detection result
    for(i = 0; i < outHVCResult.hdResult.num; i++){
        if ( size >= (INT32)sizeof(UINT8)*8 ) {
            ret = HVCReceiveData(inTimeOutTime, sizeof(UINT8)*8, recvData);
            outHVCResult.hdResult.hdResult[i].posX = (short)(recvData[0] + (recvData[1]<<8));
            outHVCResult.hdResult.hdResult[i].posY = (short)(recvData[2] + (recvData[3]<<8));
            outHVCResult.hdResult.hdResult[i].size = (short)(recvData[4] + (recvData[5]<<8));
            outHVCResult.hdResult.hdResult[i].confidence = (short)(recvData[6] + (recvData[7]<<8));
            if(ret != B5T_STATUS_SUCCESS){
                std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
                return ret;
            }
            size -= sizeof(UINT8)*8;
        }
    }

    // Face-related results
    for(i = 0; i < outHVCResult.fdResult.num; i++){
        // Face Detection result
        if(0 != (outHVCResult.executedFunc & HVC_ACTIV_FACE_DETECTION)){
            if ( size >= (INT32)sizeof(UINT8)*8 ) {
                ret = HVCReceiveData(inTimeOutTime, sizeof(UINT8)*8, recvData);
                outHVCResult.fdResult.fcResult[i].dtResult.posX = (short)(recvData[0] + (recvData[1]<<8));
                outHVCResult.fdResult.fcResult[i].dtResult.posY = (short)(recvData[2] + (recvData[3]<<8));
                outHVCResult.fdResult.fcResult[i].dtResult.size = (short)(recvData[4] + (recvData[5]<<8));
                outHVCResult.fdResult.fcResult[i].dtResult.confidence = (short)(recvData[6] + (recvData[7]<<8));
                if(ret != B5T_STATUS_SUCCESS){
                    std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
                    return ret;
                }
                size -= sizeof(UINT8)*8;
            }
        }

        // Face direction
        if(0 != (outHVCResult.executedFunc & HVC_ACTIV_FACE_DIRECTION)){
            if ( size >= (INT32)sizeof(UINT8)*8 ) {
                ret = HVCReceiveData(inTimeOutTime, sizeof(UINT8)*8, recvData);
                outHVCResult.fdResult.fcResult[i].dirResult.yaw = (short)(recvData[0] + (recvData[1]<<8));
                outHVCResult.fdResult.fcResult[i].dirResult.pitch = (short)(recvData[2] + (recvData[3]<<8));
                outHVCResult.fdResult.fcResult[i].dirResult.roll = (short)(recvData[4] + (recvData[5]<<8));
                outHVCResult.fdResult.fcResult[i].dirResult.confidence = (short)(recvData[6] + (recvData[7]<<8));
                if(ret != B5T_STATUS_SUCCESS){
                    std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
                    return ret;
                }
                size -= sizeof(UINT8)*8;
            }
        }

        // Age
        if(0 != (outHVCResult.executedFunc & HVC_ACTIV_AGE_ESTIMATION)){
            if ( size >= (INT32)sizeof(UINT8)*3 ) {
                ret = HVCReceiveData(inTimeOutTime, sizeof(UINT8)*3, recvData);
                outHVCResult.fdResult.fcResult[i].ageResult.age = (char)(recvData[0]);
                outHVCResult.fdResult.fcResult[i].ageResult.confidence = (short)(recvData[1] + (recvData[2]<<8));
                if(ret != B5T_STATUS_SUCCESS){
                    std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
                    return ret;
                }
                size -= sizeof(UINT8)*3;
            }
        }

        // Gender
        if(0 != (outHVCResult.executedFunc & HVC_ACTIV_GENDER_ESTIMATION)){
            if ( size >= (INT32)sizeof(UINT8)*3 ) {
                ret = HVCReceiveData(inTimeOutTime, sizeof(UINT8)*3, recvData);
                outHVCResult.fdResult.fcResult[i].genderResult.gender = (char)(recvData[0]);
                outHVCResult.fdResult.fcResult[i].genderResult.confidence = (short)(recvData[1] + (recvData[2]<<8));
                if(ret != B5T_STATUS_SUCCESS){
                    std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
                    return ret;
                }
                size -= sizeof(UINT8)*3;
            }
        }

        // Gaze
        if(0 != (outHVCResult.executedFunc & HVC_ACTIV_GAZE_ESTIMATION)){
            if ( size >= (INT32)sizeof(UINT8)*2 ) {
                ret = HVCReceiveData(inTimeOutTime, sizeof(UINT8)*2, recvData);
                outHVCResult.fdResult.fcResult[i].gazeResult.gazeLR = (char)(recvData[0]);
                outHVCResult.fdResult.fcResult[i].gazeResult.gazeUD = (char)(recvData[1]);
                if(ret != B5T_STATUS_SUCCESS){
                    std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
                    return ret;
                }
                size -= sizeof(UINT8)*2;
            }
        }

        // Blink
        if(0 != (outHVCResult.executedFunc & HVC_ACTIV_BLINK_ESTIMATION)){
            if ( size >= (INT32)sizeof(UINT8)*4 ) {
                ret = HVCReceiveData(inTimeOutTime, sizeof(UINT8)*4, recvData);
                outHVCResult.fdResult.fcResult[i].blinkResult.ratioL = (short)(recvData[0] + (recvData[1]<<8));
                outHVCResult.fdResult.fcResult[i].blinkResult.ratioR = (short)(recvData[2] + (recvData[3]<<8));
                if(ret != B5T_STATUS_SUCCESS){
                    std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
                    return ret;
                }
                size -= sizeof(UINT8)*4;
            }
        }

        // Expression
        if(0 != (outHVCResult.executedFunc & HVC_ACTIV_EXPRESSION_ESTIMATION)){
            if ( size >= (INT32)sizeof(UINT8)*6 ) {
                ret = HVCReceiveData(inTimeOutTime, sizeof(UINT8)*6, recvData);
                outHVCResult.fdResult.fcResult[i].expressionResult.topExpression = -128;
                outHVCResult.fdResult.fcResult[i].expressionResult.topScore = -128;
                for(j = 0; j < 5; j++){
                    outHVCResult.fdResult.fcResult[i].expressionResult.score[j] = (char)(recvData[j]);
                    if(outHVCResult.fdResult.fcResult[i].expressionResult.topScore < outHVCResult.fdResult.fcResult[i].expressionResult.score[j]){
                        outHVCResult.fdResult.fcResult[i].expressionResult.topScore = outHVCResult.fdResult.fcResult[i].expressionResult.score[j];
                        outHVCResult.fdResult.fcResult[i].expressionResult.topExpression = j + 1;
                    }
                }
                outHVCResult.fdResult.fcResult[i].expressionResult.degree = (char)(recvData[5]);
                if(ret != B5T_STATUS_SUCCESS){
                    std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
                    return ret;
                }
                size -= sizeof(UINT8)*6;
            }
        }

        // Face Recognition
        if(0 != (outHVCResult.executedFunc & HVC_ACTIV_FACE_RECOGNITION)){
            if ( size >= (INT32)sizeof(UINT8)*4 ) {
                ret = HVCReceiveData(inTimeOutTime, sizeof(UINT8)*4, recvData);
                outHVCResult.fdResult.fcResult[i].recognitionResult.uid = (short)(recvData[0] + (recvData[1]<<8));
                outHVCResult.fdResult.fcResult[i].recognitionResult.confidence = (short)(recvData[2] + (recvData[3]<<8));
                if(ret != B5T_STATUS_SUCCESS){
                    std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
                    return ret;
                }
                size -= sizeof(UINT8)*4;
            }
        }
    }

    if(HVC_EXECUTE_IMAGE_NONE != inImageInfo){
        // Image data
        if ( size >= (INT32)sizeof(UINT8)*4 ) {
            ret = HVCReceiveData(inTimeOutTime, sizeof(UINT8)*4, recvData);
            outHVCResult.image.width = (short)(recvData[0] + (recvData[1]<<8));
            outHVCResult.image.height = (short)(recvData[2] + (recvData[3]<<8));
            if(ret != B5T_STATUS_SUCCESS){
                std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
                return ret;
            }
            size -= sizeof(UINT8)*4;
        }

        if ( size >= (INT32)sizeof(UINT8)*outHVCResult.image.width*outHVCResult.image.height ) {
            ret = HVCReceiveData(inTimeOutTime, sizeof(UINT8)*outHVCResult.image.width*outHVCResult.image.height, outHVCResult.image.image);
            if(ret != B5T_STATUS_SUCCESS){
                std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
                return ret;
            }
            size -= sizeof(UINT8)*outHVCResult.image.width*outHVCResult.image.height;
        }
    }
    return ret;
}

B5T_STATUS_T B5T007001::setThreshold(INT32 inTimeOutTime, HVC_THRESHOLD inThreshold){
    B5T_STATUS_T ret;
    INT32 size = 0;
    UINT8 sendData[HVC_UART_FRAME_SIZE];
    if(inTimeOutTime < 0){
        inTimeOutTime = UART_SETTING_TIMEOUT;
    }

    sendData[0] = (UINT8)(inThreshold.bdThreshold&0xff);
    sendData[1] = (UINT8)((inThreshold.bdThreshold>>8)&0xff);
    sendData[2] = (UINT8)(inThreshold.hdThreshold&0xff);
    sendData[3] = (UINT8)((inThreshold.hdThreshold>>8)&0xff);
    sendData[4] = (UINT8)(inThreshold.dtThreshold&0xff);
    sendData[5] = (UINT8)((inThreshold.dtThreshold>>8)&0xff);
    sendData[6] = (UINT8)(inThreshold.rsThreshold&0xff);
    sendData[7] = (UINT8)((inThreshold.rsThreshold>>8)&0xff);

    //Send SetThreshold command signal
    ret = HVCSendCommand(HVC_COM_SET_THRESHOLD, sizeof(UINT8)*8, sendData);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    //Receive header
    ret = HVCReceiveHeader(inTimeOutTime, size);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    return ret;
}

B5T_STATUS_T B5T007001::getThreshold(INT32 inTimeOutTime, HVC_THRESHOLD &outThreshold){
    B5T_STATUS_T ret;
    INT32 size = 0;
    UINT8 recvData[HVC_UART_FRAME_SIZE];
    if(inTimeOutTime < 0){
        inTimeOutTime = UART_SETTING_TIMEOUT;
    }

    //Send GetThreshold command signal
    ret = HVCSendCommand(HVC_COM_GET_THRESHOLD, 0, NULL);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    //Receive header
    ret = HVCReceiveHeader(inTimeOutTime, size);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    if ( size > (INT32)sizeof(UINT8)*8 ) {
        size = sizeof(UINT8)*8;
    }

    //Receive data
    ret = HVCReceiveData(inTimeOutTime, size, recvData);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }
    outThreshold.bdThreshold = recvData[0] + (recvData[1]<<8);
    outThreshold.hdThreshold = recvData[2] + (recvData[3]<<8);
    outThreshold.dtThreshold = recvData[4] + (recvData[5]<<8);
    outThreshold.rsThreshold = recvData[6] + (recvData[7]<<8);

    return ret;
}

B5T_STATUS_T B5T007001::setSizeRange(INT32 inTimeOutTime, HVC_SIZERANGE inSizeRange){
    B5T_STATUS_T ret;
    INT32 size = 0;
    UINT8 sendData[HVC_UART_FRAME_SIZE];
    if(inTimeOutTime < 0){
        inTimeOutTime = UART_SETTING_TIMEOUT;
    }

    sendData[0] = (UINT8)(inSizeRange.bdMinSize&0xff);
    sendData[1] = (UINT8)((inSizeRange.bdMinSize>>8)&0xff);
    sendData[2] = (UINT8)(inSizeRange.bdMaxSize&0xff);
    sendData[3] = (UINT8)((inSizeRange.bdMaxSize>>8)&0xff);
    sendData[4] = (UINT8)(inSizeRange.hdMinSize&0xff);
    sendData[5] = (UINT8)((inSizeRange.hdMinSize>>8)&0xff);
    sendData[6] = (UINT8)(inSizeRange.hdMaxSize&0xff);
    sendData[7] = (UINT8)((inSizeRange.hdMaxSize>>8)&0xff);
    sendData[8] = (UINT8)(inSizeRange.dtMinSize&0xff);
    sendData[9] = (UINT8)((inSizeRange.dtMinSize>>8)&0xff);
    sendData[10] = (UINT8)(inSizeRange.dtMaxSize&0xff);
    sendData[11] = (UINT8)((inSizeRange.dtMaxSize>>8)&0xff);
    //Send SetSizeRange command signal
    ret = HVCSendCommand(HVC_COM_SET_SIZE_RANGE, sizeof(UINT8)*12, sendData);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    //Receive header
    ret = HVCReceiveHeader(inTimeOutTime, size);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    return ret;
}

B5T_STATUS_T B5T007001::getSizeRange(INT32 inTimeOutTime, HVC_SIZERANGE &outSizeRange){
    B5T_STATUS_T ret;
    INT32 size = 0;
    UINT8 recvData[HVC_UART_FRAME_SIZE];
    if(inTimeOutTime < 0){
        inTimeOutTime = UART_SETTING_TIMEOUT;
    }

    //Send GetSizeRange command signal
    ret = HVCSendCommand(HVC_COM_GET_SIZE_RANGE, 0, NULL);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    //Receive header
    ret = HVCReceiveHeader(inTimeOutTime, size);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    if ( size > (INT32)sizeof(UINT8)*12 ) {
        size = sizeof(UINT8)*12;
    }

    //Receive data
    ret = HVCReceiveData(inTimeOutTime, size, recvData);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }
    outSizeRange.bdMinSize = recvData[0] + (recvData[1]<<8);
    outSizeRange.bdMaxSize = recvData[2] + (recvData[3]<<8);
    outSizeRange.hdMinSize = recvData[4] + (recvData[5]<<8);
    outSizeRange.hdMaxSize = recvData[6] + (recvData[7]<<8);
    outSizeRange.dtMinSize = recvData[8] + (recvData[9]<<8);
    outSizeRange.dtMaxSize = recvData[10] + (recvData[11]<<8);

    return ret;
}

B5T_STATUS_T B5T007001::setFaceDetectionAngle(INT32 inTimeOutTime, INT32 inPose, INT32 inAngle){
    B5T_STATUS_T ret;
    INT32 size = 0;
    UINT8 sendData[HVC_UART_FRAME_SIZE];

    if(inTimeOutTime < 0){
        inTimeOutTime = UART_SETTING_TIMEOUT;
    }

    sendData[0] = (UINT8)(inPose&0xff);
    sendData[1] = (UINT8)(inAngle&0xff);
    // Send SetFaceDetectionAngle command signal
    ret = HVCSendCommand(HVC_COM_SET_DETECTION_ANGLE, sizeof(UINT8)*2, sendData);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    // Receive header
    ret = HVCReceiveHeader(inTimeOutTime, size);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    return ret;
}

B5T_STATUS_T B5T007001::getFaceDetectionAngle(INT32 inTimeOutTime, INT32 &outPose, INT32 &outAngle){
    B5T_STATUS_T ret;
    INT32 size = 0;
    UINT8 recvData[HVC_UART_FRAME_SIZE];
    if(inTimeOutTime < 0){
        inTimeOutTime = UART_SETTING_TIMEOUT;
    }

    // Send GetFaceDetectionAngle signal command
    ret = HVCSendCommand(HVC_COM_GET_DETECTION_ANGLE, 0, NULL);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    // Receive header
    ret = HVCReceiveHeader(inTimeOutTime, size);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    if ( size > (INT32)sizeof(UINT8)*2 ) {
        size = sizeof(UINT8)*2;
    }

    // Receive data
    ret = HVCReceiveData(inTimeOutTime, size, recvData);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }
    outPose = recvData[0];
    outAngle = recvData[1];

    return ret;
}

B5T_STATUS_T B5T007001::setBaudRate(INT32 inTimeOutTime, HVC_UART_BAUD_RATE_T inRate){
    B5T_STATUS_T ret;
    INT32 size = 0;
    UINT8 sendData[HVC_UART_FRAME_SIZE];
    mraa::Result returnValue;
    if(inTimeOutTime < 0){
        inTimeOutTime = UART_SETTING_TIMEOUT;
    }

    switch(inRate){
        case HVC_UART_BAUD_RATE_9600:
            sendData[0] = (UINT8)(0x00);
            break;
        case HVC_UART_BAUD_RATE_38400:
            sendData[0] = (UINT8)(0x01);
            break;
        case HVC_UART_BAUD_RATE_115200:
            sendData[0] = (UINT8)(0x02);
            break;
        case HVC_UART_BAUD_RATE_230400:
            sendData[0] = (UINT8)(0x03);
            break;
        case HVC_UART_BAUD_RATE_460800:
            sendData[0] = (UINT8)(0x04);
            break;
        case HVC_UART_BAUD_RATE_921600:
            sendData[0] = (UINT8)(0x05);
            break;
        default:
	    return B5T_STATUS_INVALID_INPUT;
    }

    //Send SetBaudRate command signal
    ret = HVCSendCommand(HVC_COM_SET_BAUDRATE, sizeof(UINT8), sendData);
    if(ret != B5T_STATUS_SUCCESS){
        throw std::runtime_error(std::string(__FUNCTION__) +
                               ": failed to set baudrate" );
    }

    //Receive header
    ret = HVCReceiveHeader(inTimeOutTime, size);
    if(ret != B5T_STATUS_SUCCESS){
        throw std::runtime_error(std::string(__FUNCTION__) +
                               ": failed to set baudrate" );
    }
    //Update baudrate for connected UART
    returnValue = mUart->setBaudRate(inRate);
    if(returnValue != mraa::SUCCESS){
        throw std::runtime_error(std::string(__FUNCTION__) +
                               ": failed to set baudrate" );
    }
    return ret;
}

B5T_STATUS_T B5T007001::getRegisteredUsersInfo(INT32 inTimeOutTime, std::vector<UINT8> &outUserList, std::vector<UINT8> &outDataList){
    B5T_STATUS_T ret;
    UINT8 userID = 0;
    UINT8 dataID = 0;
    if(inTimeOutTime < 0){
        inTimeOutTime = UART_SETTING_TIMEOUT;
    }

    //Clear output data if exist
    outUserList.clear();
    outDataList.clear();

    for(userID=0; userID < MAX_USER_NUM; userID++){
        ret = getUserData(inTimeOutTime, userID, dataID);
        //printf("DEBUG: HVCGetUserData userID=(%d) dataID=(%d) status = (%d)\n", userID, dataID);
        if(ret != B5T_STATUS_SUCCESS){
            std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
            return ret;
        }
        if(dataID != 0){
            //printf("Adding userId = (%d) dataId = (%d)\n", userID, dataID);
            outUserList.push_back(userID);
            outDataList.push_back(dataID);
        }
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::addUser(INT32 inTimeOutTime, UINT8 inUserID, UINT8 inDataID, HVC_IMAGE &outImage){
    B5T_STATUS_T ret;
    UINT8 tmpDataID;

    if(inTimeOutTime < 0){
        inTimeOutTime = UART_SETTING_TIMEOUT;
    }
    //Add or Modify user
    ret = HVCRegistration(inTimeOutTime, inUserID, inDataID, outImage);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }
    //Check for successful user add
    ret = getUserData(inTimeOutTime, inUserID, tmpDataID);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    if(!(tmpDataID & (1 << inDataID))){
        std::cerr << std::string(__FUNCTION__) << ": failed for userID = " << inUserID << "And dataID = " << inDataID << std::endl;
	return B5T_STATUS_USER_ADD_FAILED;
    }
    return ret;
}

B5T_STATUS_T B5T007001::getUserData(INT32 inTimeOutTime, UINT8 inUserID, UINT8 &outDataNo){
    B5T_STATUS_T ret;
    INT32 size = 0;
    UINT8 sendData[8];
    UINT8 recvData[8];
    if(inTimeOutTime < 0){
        inTimeOutTime = UART_SETTING_TIMEOUT;
    }

    //Send Get Registration Info signal command
    sendData[0] = (UINT8)(inUserID&0xff);
    sendData[1] = (UINT8)((inUserID>>8)&0xff);
    ret = HVCSendCommand(HVC_COM_GET_PERSON_DATA, sizeof(UINT8)*2, sendData);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }
    //Receive header
    ret = HVCReceiveHeader(inTimeOutTime, size);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }
    if (size > (INT32)sizeof(UINT8)*2 ) {
        size = sizeof(UINT8)*2;
    }

    // Receive data
    ret = HVCReceiveData(inTimeOutTime, size, recvData);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }
    outDataNo = recvData[0] + (recvData[1]<<8);
    return ret;

}
B5T_STATUS_T B5T007001::saveAlbumToHost(INT32 inTimeOutTime, UINT8 *outAlbumData, INT32 &outAlbumDataSize){
    B5T_STATUS_T ret;
    INT32 size = 0;

    UINT8 *tmpAlbumData = NULL;;
    if(NULL == outAlbumData){
        std::cerr << std::string(__FUNCTION__) << ": Invalid input" << std::endl;
        return B5T_STATUS_INVALID_INPUT;
    }
    if(inTimeOutTime < 0){
        inTimeOutTime = UART_SAVE_ALBUM_TIMEOUT;
    }
    //Send Save Album signal command
    ret = HVCSendCommand(HVC_COM_SAVE_ALBUM, 0, NULL);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    ret = HVCReceiveHeader(inTimeOutTime, size);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    if ( size >= (INT32)sizeof(UINT8)*8 + HVC_ALBUM_SIZE_MIN ) {
        outAlbumDataSize = size;
        tmpAlbumData = outAlbumData;

        do{
            ret = HVCReceiveData(inTimeOutTime, sizeof(UINT8)*4, tmpAlbumData);
            if(ret != B5T_STATUS_SUCCESS){
                std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
                return ret;
            }
            tmpAlbumData += sizeof(UINT8)*4;

            ret = HVCReceiveData(inTimeOutTime, sizeof(UINT8)*4, tmpAlbumData);
            if(ret != B5T_STATUS_SUCCESS){
                std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
                return ret;
            }
            tmpAlbumData += sizeof(UINT8)*4;

            ret = HVCReceiveData(inTimeOutTime, size - sizeof(UINT8)*8, tmpAlbumData);
            if(ret != B5T_STATUS_SUCCESS){
                std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
                return ret;
            }
        }while(0);
    }
    return ret;
}

B5T_STATUS_T B5T007001::loadAlbumFromHost(INT32 inTimeOutTime, UINT8 *inAlbumData, INT32 inAlbumDataSize){
    B5T_STATUS_T ret;
    INT32 i;
    INT32 size = 0;

    if((NULL == inAlbumData) || (0 == inAlbumDataSize)){
        std::cerr << std::string(__FUNCTION__) << ": Invalid input" << std::endl;
        return B5T_STATUS_INVALID_INPUT;
    }
    if(inTimeOutTime < 0){
        inTimeOutTime = UART_LOAD_ALBUM_TIMEOUT;
    }
    UINT8 *pSendData = NULL;
    pSendData = (UINT8*)malloc(SEND_HEAD_NUM + 4 + inAlbumDataSize);

    // Send Save Album signal command
    // Create header
    pSendData[SEND_HEAD_SYNCBYTE]       = (UINT8)0xFE;
    pSendData[SEND_HEAD_COMMANDNO]      = (UINT8)HVC_COM_LOAD_ALBUM;
    pSendData[SEND_HEAD_DATALENGTHLSB]  = (UINT8)4;
    pSendData[SEND_HEAD_DATALENGTHMSB]  = (UINT8)0;

    pSendData[SEND_HEAD_NUM + 0]        = (UINT8)(inAlbumDataSize & 0x000000ff);
    pSendData[SEND_HEAD_NUM + 1]        = (UINT8)((inAlbumDataSize >> 8) & 0x000000ff);
    pSendData[SEND_HEAD_NUM + 2]        = (UINT8)((inAlbumDataSize >> 16) & 0x000000ff);
    pSendData[SEND_HEAD_NUM + 3]        = (UINT8)((inAlbumDataSize >> 24) & 0x000000ff);

     for(i = 0; i < inAlbumDataSize; i++){
        pSendData[SEND_HEAD_NUM + 4 + i] = inAlbumData[i];
    }
    // Send command signal
    ret = UARTSendData(SEND_HEAD_NUM+4+inAlbumDataSize, pSendData);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code =" << ret << std::endl;
        free(pSendData);
        return ret;
    }
    free(pSendData);
    // Receive header
    ret = HVCReceiveHeader(inTimeOutTime, size);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    return ret;
}

B5T_STATUS_T B5T007001::saveAlbumToROM(INT32 inTimeOutTime){
    B5T_STATUS_T ret;
    INT32 size = 0;
    if(inTimeOutTime < 0){
        inTimeOutTime = UART_SAVE_ALBUM_ROM_TIMEOUT;
    }

    //Send Write Album signal command
    ret = HVCSendCommand(HVC_COM_WRITE_ALBUM, 0, NULL);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    //Receive header
    ret = HVCReceiveHeader(inTimeOutTime, size);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    return ret;
}

B5T_STATUS_T B5T007001::deleteUserData(INT32 inTimeOutTime, UINT8 inUserID, UINT8 inDataID){
    B5T_STATUS_T ret;
    INT32 size = 0;
    UINT8 sendData[HVC_UART_FRAME_SIZE];
    if(inTimeOutTime < 0){
        inTimeOutTime = UART_SETTING_TIMEOUT;
    }

    // Send Delete Data signal command
    sendData[0] = (UINT8)(inUserID&0xff);
    sendData[1] = (UINT8)((inUserID>>8)&0xff);
    sendData[2] = (UINT8)(inDataID&0xff);
    ret = HVCSendCommand(HVC_COM_DELETE_DATA, sizeof(UINT8)*3, sendData);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    //Receive header
    ret = HVCReceiveHeader(inTimeOutTime, size);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }
    return ret;
}

B5T_STATUS_T B5T007001::deleteUser(INT32 inTimeOutTime, UINT8 inUserID){
    B5T_STATUS_T ret;
    INT32 size = 0;
    UINT8 sendData[HVC_UART_FRAME_SIZE];
    if(inTimeOutTime < 0){
        inTimeOutTime = UART_SETTING_TIMEOUT;
    }

    // Send Delete User signal command
    sendData[0] = (UINT8)(inUserID&0xff);
    sendData[1] = (UINT8)((inUserID>>8)&0xff);
    ret = HVCSendCommand(HVC_COM_DELETE_USER, sizeof(UINT8)*2, sendData);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    // Receive header
    ret = HVCReceiveHeader(inTimeOutTime, size);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }
    return ret;
}

B5T_STATUS_T B5T007001::deleteUserAll(INT32 inTimeOutTime){
    B5T_STATUS_T ret;
    INT32 size = 0;
    if(inTimeOutTime < 0){
        inTimeOutTime = UART_SETTING_TIMEOUT;
    }

    // Send Delete All signal command
    ret = HVCSendCommand(HVC_COM_DELETE_ALL, 0, NULL);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    // Receive header
    ret = HVCReceiveHeader(inTimeOutTime, size);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::setSTBStatus(STB_UINT32 inFlag){
    B5T_STATUS_T ret= B5T_STATUS_SUCCESS;

    //Flag 0 means disable STB
    if(inFlag == 0){
        stbDeInit();
        mStbStatus = 0;
	return ret;
    }
#if DEBUG_PRINT
        std::cout << "DEBUG: inFlag" << unsigned(inFlag) << std::endl;
#endif
    ret = stbInit(inFlag);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }
    ret = stbSetTrRetryCount(STB_RETRYCOUNT_DEFAULT);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }
    ret = stbSetTrSteadinessParam(STB_POSSTEADINESS_DEFAULT, STB_SIZESTEADINESS_DEFAULT);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }
    ret = stbSetPeThresholdUse(STB_PE_THRESHOLD_DEFAULT);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }
    ret = stbSetPeAngleUse(STB_PE_ANGLEUDMIN_DEFAULT, STB_PE_ANGLEUDMAX_DEFAULT, STB_PE_ANGLELRMIN_DEFAULT, STB_PE_ANGLELRMAX_DEFAULT);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }
    ret = stbSetPeCompleteFrameCount(STB_PE_FRAME_DEFAULT);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    ret = stbSetFrThresholdUse(STB_FR_THRESHOLD_DEFAULT);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    ret = stbSetFrAngleUse(STB_FR_ANGLEUDMIN_DEFAULT, STB_FR_ANGLEUDMAX_DEFAULT, STB_FR_ANGLELRMIN_DEFAULT, STB_FR_ANGLELRMAX_DEFAULT);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    ret = stbSetFrCompleteFrameCount(STB_FR_FRAME_DEFAULT);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    ret = stbSetFrMinRatio(STB_FR_RATIO_DEFAULT);
    if(ret != B5T_STATUS_SUCCESS){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return ret;
    }

    mStbStatus = inFlag;
    return ret;
}

B5T_STATUS_T B5T007001::getSTBStatus(STB_INT32 &outStatus){
    B5T_STATUS_T ret= B5T_STATUS_SUCCESS;
    outStatus = mStbStatus;
    return ret;
}

B5T_STATUS_T B5T007001::stbInit(STB_UINT32 inFuncFlag)
{
    if(NULL != mSTBHandle){
        STB_DeleteHandle(mSTBHandle);
        mSTBHandle = NULL;
    }

    mSTBHandle = STB_CreateHandle(inFuncFlag);
    if(NULL == mSTBHandle){
        return B5T_STATUS_STB_ERR_INITIALIZE;
    }
    return B5T_STATUS_SUCCESS;
}

void B5T007001::stbDeInit(void)
{
    if(NULL != mSTBHandle){
        STB_DeleteHandle(mSTBHandle);
        mSTBHandle = NULL;
    }
}

B5T_STATUS_T B5T007001::stbExec(STB_INT32 inActiveFunc, HVC_RESULT inResult, STB_INT32 *outSTBFaceCount, STB_FACE **outSTBFaceResult, STB_INT32 *outSTBBodyCount, STB_BODY **outSTBBodyResult)
{
    STB_INT32 ret;
    B5T_STATUS_T retStatus=B5T_STATUS_SUCCESS;
    STB_FRAME_RESULT frameRes;

    mFaceCount = 0;
    mBodyCount = 0;
    stbGetFrameResult(inActiveFunc, inResult, &frameRes);
    do{
        // Set frame information (Detection Result)
        ret = STB_SetFrameResult(mSTBHandle, &frameRes);
        if(STB_NORMAL != ret){
            std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
	    retStatus = B5T_STATUS_STB_FAILURE;
            break;
        }

        // STB Execution
        ret = STB_Execute(mSTBHandle);
        if(STB_NORMAL != ret){
            std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
	    retStatus = B5T_STATUS_STB_FAILURE;
            break;
        }

        // Get STB Result
        ret = STB_GetFaces(mSTBHandle, (STB_UINT32 *)&mFaceCount, mFace);
        if(STB_NORMAL != ret){
            std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
	    retStatus = B5T_STATUS_STB_FAILURE;
            break;
        }

        ret = STB_GetBodies(mSTBHandle, (STB_UINT32 *)&mBodyCount, mBody);
        if(STB_NORMAL != ret){
            std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
	    retStatus = B5T_STATUS_STB_FAILURE;
            break;
        }
    }while(0);
    if(retStatus == B5T_STATUS_SUCCESS){
        *outSTBFaceCount = mFaceCount;
        *outSTBFaceResult = mFace;
        *outSTBBodyCount = mBodyCount;
        *outSTBBodyResult = mBody;
    }
    return retStatus;
}

B5T_STATUS_T B5T007001::stbClear(void)
{
    STB_INT32 ret;
    ret = STB_ClearFrameResults(mSTBHandle);
    if(ret != STB_NORMAL){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return B5T_STATUS_STB_FAILURE;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::stbSetTrRetryCount(STB_INT32 inMaxRetryCount){
    STB_INT32 ret;
    ret = STB_SetTrRetryCount(mSTBHandle, inMaxRetryCount);
    if(STB_NORMAL != ret){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return B5T_STATUS_STB_FAILURE;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::stbGetTrRetryCount(STB_INT32 *outMaxRetryCount){
    STB_INT32 ret;
    ret = STB_GetTrRetryCount(mSTBHandle, outMaxRetryCount);
    if(STB_NORMAL != ret){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return B5T_STATUS_STB_FAILURE;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::stbSetTrSteadinessParam(STB_INT32 inPosSteadinessParam, STB_INT32 inSizeSteadinessParam){
    STB_INT32 ret;
    ret = STB_SetTrSteadinessParam(mSTBHandle, inPosSteadinessParam, inSizeSteadinessParam);
    if(STB_NORMAL != ret){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return B5T_STATUS_STB_FAILURE;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::stbGetTrSteadinessParam(STB_INT32 *outPosSteadinessParam, STB_INT32 *outSizeSteadinessParam){
    STB_INT32 ret;
    ret = STB_GetTrSteadinessParam(mSTBHandle, outPosSteadinessParam, outSizeSteadinessParam);
    if(STB_NORMAL != ret){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return B5T_STATUS_STB_FAILURE;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::stbSetPeThresholdUse(STB_INT32 inThreshold){
    STB_INT32 ret;
    ret = STB_SetPeThresholdUse(mSTBHandle, inThreshold);
    if(STB_NORMAL != ret){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return B5T_STATUS_STB_FAILURE;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::stbGetPeThresholdUse(STB_INT32 *outThreshold){
    STB_INT32 ret;
    ret = STB_GetPeThresholdUse(mSTBHandle, outThreshold);
    if(STB_NORMAL != ret){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return B5T_STATUS_STB_FAILURE;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::stbSetPeAngleUse(STB_INT32 inMinUDAngle, STB_INT32 inMaxUDAngle, STB_INT32 inMinLRAngle, STB_INT32 inMaxLRAngle){
    STB_INT32 ret;
    ret = STB_SetPeAngleUse(mSTBHandle, inMinUDAngle, inMaxUDAngle, inMinLRAngle, inMaxLRAngle);
    if(STB_NORMAL != ret){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return B5T_STATUS_STB_FAILURE;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::stbGetPeAngleUse(STB_INT32 *outMinUDAngle, STB_INT32 *outMaxUDAngle, STB_INT32 *outMinLRAngle, STB_INT32 *outMaxLRAngle){
    STB_INT32 ret;
    ret = STB_GetPeAngleUse(mSTBHandle, outMinUDAngle, outMaxUDAngle, outMinLRAngle, outMaxLRAngle);
    if(STB_NORMAL != ret){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return B5T_STATUS_STB_FAILURE;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::stbSetPeCompleteFrameCount(STB_INT32 inCompCount){
    STB_INT32 ret;
    ret = STB_SetPeCompleteFrameCount(mSTBHandle, inCompCount);
    if(STB_NORMAL != ret){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return B5T_STATUS_STB_FAILURE;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::stbGetPeCompleteFrameCount(STB_INT32 *outCompCount){
    STB_INT32 ret;
    ret = STB_GetPeCompleteFrameCount(mSTBHandle, outCompCount);
    if(STB_NORMAL != ret){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return B5T_STATUS_STB_FAILURE;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::stbSetFrThresholdUse(STB_INT32 inThreshold){
    STB_INT32 ret;
    ret = STB_SetFrThresholdUse(mSTBHandle, inThreshold);
    if(STB_NORMAL != ret){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return B5T_STATUS_STB_FAILURE;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::stbGetFrThresholdUse(STB_INT32 *outThreshold){
    STB_INT32 ret;
    ret = STB_GetFrThresholdUse(mSTBHandle, outThreshold);
    if(STB_NORMAL != ret){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return B5T_STATUS_STB_FAILURE;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::stbSetFrAngleUse(STB_INT32 inMinUDAngle, STB_INT32 inMaxUDAngle, STB_INT32 inMinLRAngle, STB_INT32 inMaxLRAngle){
    STB_INT32 ret;
    ret = STB_SetFrAngleUse(mSTBHandle, inMinUDAngle, inMaxUDAngle, inMinLRAngle, inMaxLRAngle);
    if(STB_NORMAL != ret){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return B5T_STATUS_STB_FAILURE;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::stbGetFrAngleUse(STB_INT32 *outMinUDAngle, STB_INT32 *outMaxUDAngle, STB_INT32 *outMinLRAngle, STB_INT32 *outMaxLRAngle){
    STB_INT32 ret;
    ret = STB_GetFrAngleUse(mSTBHandle, outMinUDAngle, outMaxUDAngle, outMinLRAngle, outMaxLRAngle);
    if(STB_NORMAL != ret){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return B5T_STATUS_STB_FAILURE;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::stbSetFrCompleteFrameCount(STB_INT32 inFrameCount){
    STB_INT32 ret;
    ret = STB_SetFrCompleteFrameCount(mSTBHandle, inFrameCount);
    if(STB_NORMAL != ret){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return B5T_STATUS_STB_FAILURE;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::stbGetFrCompleteFrameCount(STB_INT32 *outFrameCount){
    STB_INT32 ret;
    ret = STB_GetFrCompleteFrameCount(mSTBHandle, outFrameCount);
    if(STB_NORMAL != ret){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return B5T_STATUS_STB_FAILURE;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::stbSetFrMinRatio(STB_INT32 inMinRatio){
    STB_INT32 ret;
    ret = STB_SetFrMinRatio(mSTBHandle, inMinRatio);
    if(STB_NORMAL != ret){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return B5T_STATUS_STB_FAILURE;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::stbGetFrMinRatio(STB_INT32 *outMinRatio){
    STB_INT32 ret;
    ret = STB_GetFrMinRatio(mSTBHandle, outMinRatio);
    if(STB_NORMAL != ret){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return B5T_STATUS_STB_FAILURE;
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::stbGetFrameResult(STB_INT32 inActiveFunc, const HVC_RESULT inResult, STB_FRAME_RESULT *outFrameResult){
    UINT8 i;

    // Body Detection
    outFrameResult->bodys.nCount = inResult.bdResult.num;
    if(inActiveFunc & HVC_ACTIV_BODY_DETECTION){
        for(i = 0; i <  inResult.bdResult.num; i++){
            DETECT_RESULT dtRes = inResult.bdResult.bdResult[i];
            outFrameResult->bodys.body[i].center.nX     = dtRes.posX;
            outFrameResult->bodys.body[i].center.nY     = dtRes.posY;
            outFrameResult->bodys.body[i].nSize         = dtRes.size;
            outFrameResult->bodys.body[i].nConfidence   = dtRes.confidence;
        }
    }

    outFrameResult->faces.nCount = inResult.fdResult.num;
    for(i = 0; i < inResult.fdResult.num; i++){
        // Face Detection
        if(inActiveFunc & HVC_ACTIV_FACE_DETECTION){
            DETECT_RESULT dtRes = inResult.fdResult.fcResult[i].dtResult;
            outFrameResult->faces.face[i].center.nX     = dtRes.posX;
            outFrameResult->faces.face[i].center.nY     = dtRes.posY;
            outFrameResult->faces.face[i].nSize         = dtRes.size;
            outFrameResult->faces.face[i].nConfidence   = dtRes.confidence;
        }
        // Face Direction
        if(inActiveFunc & HVC_ACTIV_FACE_DIRECTION){
            DIR_RESULT dirRes = inResult.fdResult.fcResult[i].dirResult;
            outFrameResult->faces.face[i].direction.nUD         = dirRes.pitch;
            outFrameResult->faces.face[i].direction.nLR         = dirRes.yaw;
            outFrameResult->faces.face[i].direction.nRoll       = dirRes.roll;
            outFrameResult->faces.face[i].direction.nConfidence = dirRes.confidence;
        }
        else {
            outFrameResult->faces.face[i].direction.nUD         = 0;
            outFrameResult->faces.face[i].direction.nLR         = 0;
            outFrameResult->faces.face[i].direction.nRoll       = 0;
            outFrameResult->faces.face[i].direction.nConfidence = 0;
        }

        // Age
        if(inActiveFunc & HVC_ACTIV_AGE_ESTIMATION){
            AGE_RESULT ageRes = inResult.fdResult.fcResult[i].ageResult;
            outFrameResult->faces.face[i].age.nAge              = ageRes.age;
            outFrameResult->faces.face[i].age.nConfidence       = ageRes.confidence;
        }
               else {
            outFrameResult->faces.face[i].age.nAge              = -128;
            outFrameResult->faces.face[i].age.nConfidence       = -128;
        }

        // Gender
        if(inActiveFunc & HVC_ACTIV_GENDER_ESTIMATION){
            GENDER_RESULT genderRes = inResult.fdResult.fcResult[i].genderResult;
            outFrameResult->faces.face[i].gender.nGender        = genderRes.gender;
            outFrameResult->faces.face[i].gender.nConfidence    = genderRes.confidence;
        }
               else {
            outFrameResult->faces.face[i].gender.nGender        = -128;
            outFrameResult->faces.face[i].gender.nConfidence    = -128;
        }

        // Face recognition
        if(inActiveFunc & HVC_ACTIV_FACE_RECOGNITION){
            RECOGNITION_RESULT recogRes = inResult.fdResult.fcResult[i].recognitionResult;
            outFrameResult->faces.face[i].recognition.nUID      = recogRes.uid;
            outFrameResult->faces.face[i].recognition.nScore    = recogRes.confidence;
        }
               else {
            outFrameResult->faces.face[i].recognition.nUID      = -128;
            outFrameResult->faces.face[i].recognition.nScore    = -128;
        }
        // Gaze
        if(inActiveFunc & HVC_ACTIV_GAZE_ESTIMATION){
            GAZE_RESULT gazeRes = inResult.fdResult.fcResult[i].gazeResult;
            outFrameResult->faces.face[i].gaze.nLR = gazeRes.gazeLR;
            outFrameResult->faces.face[i].gaze.nUD = gazeRes.gazeUD;
        }
               else{
            outFrameResult->faces.face[i].gaze.nLR = -128;
            outFrameResult->faces.face[i].gaze.nUD = -128;
        }
        // Blink
        if(inActiveFunc & HVC_ACTIV_BLINK_ESTIMATION){
            BLINK_RESULT blinkRes = inResult.fdResult.fcResult[i].blinkResult;
            outFrameResult->faces.face[i].blink.nLeftEye = blinkRes.ratioL;
            outFrameResult->faces.face[i].blink.nRightEye = blinkRes.ratioR;
        }
               else{
            outFrameResult->faces.face[i].blink.nLeftEye = -128;
            outFrameResult->faces.face[i].blink.nRightEye = -128;
        }
        // Expression
        if(inActiveFunc & HVC_ACTIV_EXPRESSION_ESTIMATION){
            UINT8 count=0;
            EXPRESSION_RESULT exprRes = inResult.fdResult.fcResult[i].expressionResult;
            for(count=0; count < 5;count++){
               outFrameResult->faces.face[i].expression.anScore[count]= exprRes.score[count];
            }
            outFrameResult->faces.face[i].expression.nDegree = exprRes.degree;
        }
               else{
            outFrameResult->faces.face[i].expression.anScore[0]= -128;
            outFrameResult->faces.face[i].expression.nDegree = -128;
        }
    }
    return B5T_STATUS_SUCCESS;
}

B5T_STATUS_T B5T007001::stbGetVersion(STB_INT8* outMajorVersion, STB_INT8* outMinorVersion){
    STB_INT32 ret;
    ret = STB_GetVersion(outMajorVersion, outMinorVersion);
    if(STB_NORMAL != ret){
        std::cerr << std::string(__FUNCTION__) << ": failed with error code:" << ret << std::endl;
        return B5T_STATUS_STB_FAILURE;
    }
    return B5T_STATUS_SUCCESS;
}

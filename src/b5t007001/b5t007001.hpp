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

#pragma once


#include <iostream> //Required for c++ standard IO
#include <string> // Required for string
#include "mraa/uart.hpp" //Required for UART
#include <vector> //Required for vector
#include "STBAPI.h" //Required for STB
#include "STBCommonDef.h" //Required for STB

/**
 * MACROS and enum
 */
#define HVC_ALBUM_SIZE_MIN              32
#define HVC_ALBUM_SIZE_MAX              816032

/**
 * Execute detection flags
 */
#define HVC_ACTIV_BODY_DETECTION        0x00000001
#define HVC_ACTIV_HAND_DETECTION        0x00000002
#define HVC_ACTIV_FACE_DETECTION        0x00000004
#define HVC_ACTIV_FACE_DIRECTION        0x00000008
#define HVC_ACTIV_AGE_ESTIMATION        0x00000010
#define HVC_ACTIV_GENDER_ESTIMATION     0x00000020
#define HVC_ACTIV_GAZE_ESTIMATION       0x00000040
#define HVC_ACTIV_BLINK_ESTIMATION      0x00000080
#define HVC_ACTIV_EXPRESSION_ESTIMATION 0x00000100
#define HVC_ACTIV_FACE_RECOGNITION      0x00000200

#define HVC_EXECUTE_IMAGE_NONE          0x00000000
#define HVC_EXECUTE_IMAGE_QVGA          0x00000001
#define HVC_EXECUTE_IMAGE_QVGA_HALF     0x00000002

#define DEFAULT_UART_NUM                0         //This can be overwritten by application for custom number in constructure.
#define MAX_USER_NUM                    100       //Maximum user number
#define UART_SETTING_TIMEOUT            1000      //HVC setting command signal timeout period
#define UART_EXECUTE_TIMEOUT              ((10+10+6+3+15+15+1+1+15+10)*1000)
#define UART_SAVE_ALBUM_TIMEOUT           860000          // HVC save album command signal timeout period
#define UART_LOAD_ALBUM_TIMEOUT           860000          // HVC load album command signal timeout period
#define UART_SAVE_ALBUM_ROM_TIMEOUT       150000          // HVC save album to ROM command timeout period
#define STB_MAX_NUM 35

typedef     unsigned char        UINT8;      // 8 bit Unsigned Integer
typedef     int                  INT32;      //32 bit Signed   Integer
typedef     unsigned int         UINT32;     //32 bit Signed   Integer
typedef     char                 INT8;      //32 bit Signed   Integer
typedef     std::string          STRING;    //String

#define STB_RETRYCOUNT_DEFAULT               2            //Retry Count for STB
#define STB_POSSTEADINESS_DEFAULT           30            //Position Steadiness for STB
#define STB_SIZESTEADINESS_DEFAULT          30            //Size Steadiness for STB
#define STB_PE_FRAME_DEFAULT                10            //Complete Frame Count for property estimation in STB
#define STB_PE_ANGLEUDMIN_DEFAULT          -15            //Up/Down face angle minimum value for property estimation in STB
#define STB_PE_ANGLEUDMAX_DEFAULT           20            //Up/Down face angle maximum value for property estimation in STB
#define STB_PE_ANGLELRMIN_DEFAULT          -20            //Left/Right face angle minimum value for property estimation in STB
#define STB_PE_ANGLELRMAX_DEFAULT           20            //Left/Right face angle maximum value for property estimation in STB
#define STB_PE_THRESHOLD_DEFAULT             1            //Threshold for property estimation in STB
#define STB_FR_FRAME_DEFAULT                 5            //Complete Frame Count for recognition in STB
#define STB_FR_RATIO_DEFAULT                60            //Account Ratio for recognition in STB
#define STB_FR_ANGLEUDMIN_DEFAULT          -15            //Up/Down face angle minimum value for recognition in STB
#define STB_FR_ANGLEUDMAX_DEFAULT           20            //Up/Down face angle maximum value for recognition in STB
#define STB_FR_ANGLELRMIN_DEFAULT          -20            //Left/Right face angle maximum value for recognition in STB
#define STB_FR_ANGLELRMAX_DEFAULT           20            //Left/Right face angle minimum value for recognition in STB
#define STB_FR_THRESHOLD_DEFAULT             1            //Threshold for recognition in STB



/**
 * B5T_STATUS enum
 * An Enum contains status code of operations
 */
typedef enum B5T_STATUS{
B5T_STATUS_SUCCESS =0,  // Success
B5T_STATUS_WRITE_FAILED, // MRAA library UART write failed
B5T_STATUS_READ_FAILED,  // MRAA library UART read failed
B5T_STATUS_READ_TIMEOUT, // MRAA library UART read timeour occurs
B5T_STATUS_HEADER_INVALID, // Sensor FW returned invalid header
B5T_STATUS_INVALID_INPUT, // Invalid input
B5T_STATUS_FW_INTERNAL_ERROR, // HVC firmware error
B5T_STATUS_USER_ADD_FAILED, // User add failed
B5T_STATUS_STB_FAILURE, // STB failure
B5T_STATUS_STB_ERR_INITIALIZE, // STB library init failed
}B5T_STATUS_T;


/**
 * HVC_UART_BAUD_RATE_T enum
 * An enum contains values of supported UART baud rate
 */
typedef enum{
HVC_UART_BAUD_RATE_9600 = 9600,
HVC_UART_BAUD_RATE_38400 = 38400,
HVC_UART_BAUD_RATE_115200 = 115200,
HVC_UART_BAUD_RATE_230400 = 230400,
HVC_UART_BAUD_RATE_460800 = 460800,
HVC_UART_BAUD_RATE_921600 = 921600,
}HVC_UART_BAUD_RATE_T;


/**
 * Expression
 */
typedef enum {
    EX_NEUTRAL = 1,
    EX_HAPPINESS,
    EX_SURPRISE,
    EX_ANGER,
    EX_SADNESS
}EXPRESSION;

/**
 *Struct Device model and version info
 */
typedef struct {
    UINT8   string[12];
    UINT8   major;
    UINT8   minor;
    UINT8   relese;
    UINT8   revision[4];
}HVC_VERSION;

/**
 *Detection result
 */
typedef struct{
    INT32   posX;        //Center x-coordinate
    INT32   posY;       //Center y-coordinate
    INT32   size;       //Size
    INT32   confidence; //Degree of confidence
}DETECT_RESULT;

/**
 *Face direction
 */
typedef struct{
    INT32   yaw;        //Yaw angle
    INT32   pitch;      //Pitch angle
    INT32   roll;       //Roll angle
    INT32   confidence; //Degree of confidence
}DIR_RESULT;

/**
 *Age
 */
typedef struct{
    INT32   age;        //Age
    INT32   confidence; //Degree of confidence
}AGE_RESULT;

/**
 * Gender
 */
typedef struct{
    INT32   gender;     //Gender
    INT32   confidence; //Degree of confidence
}GENDER_RESULT;

/**
 *Gaze
 */
typedef struct{
    INT32   gazeLR;     //Yaw angle
    INT32   gazeUD;     //Pitch angle
}GAZE_RESULT;

/**
 *Blink
 */
typedef struct{
    INT32   ratioL;     //Left eye blink result
    INT32   ratioR;     //Right eye blink result
}BLINK_RESULT;

/**
 *Expression
 */
typedef struct{
    INT32   topExpression;  //Top expression
    INT32   topScore;       //Top score
    INT32   score[5];       //Score of 5 expression
    INT32   degree;         //Negative-positive degree
}EXPRESSION_RESULT;

/**
 *Face Recognition
 */
typedef struct{
    INT32   uid;        //User ID
    INT32   confidence; //Degree of confidence
}RECOGNITION_RESULT;

/**
 *Face Detection & Estimations result
 */
typedef struct{
    DETECT_RESULT       dtResult;           //Face detection result
    DIR_RESULT          dirResult;          //Face direction estimation result
    AGE_RESULT          ageResult;          //Age Estimation result
    GENDER_RESULT       genderResult;       //Gender Estimation result
    GAZE_RESULT         gazeResult;         //Gaze Estimation result
    BLINK_RESULT        blinkResult;        //Blink Estimation result
    EXPRESSION_RESULT   expressionResult;   //Expression Estimation result
    RECOGNITION_RESULT  recognitionResult;  //Face Recognition result
}FACE_RESULT;

/**
 *Human Body Detection results
 */
typedef struct{
    UINT8           num;            //Number of Detection
    DETECT_RESULT   bdResult[35];   //Detection result
}BD_RESULT;

/**
 *Hand Detection results
 */
typedef struct{
    UINT8           num;            //Number of Detection
    DETECT_RESULT   hdResult[35];   //Detection result
}HD_RESULT;

/**
 *Face Detection & Estimations results
 */
typedef struct{
    UINT8           num;            //Number of Detection
    FACE_RESULT     fcResult[35];   //Detection & Estimations result
}FD_RESULT;

/**
 *Image data
 */
typedef struct{
    INT32   width;
    INT32   height;
    UINT8   image[320*240];
}HVC_IMAGE;

/**
 * Result data of Execute command
 */
typedef struct{
    INT32       executedFunc;   //Execution flag
    BD_RESULT   bdResult;       //Human Body Detection results
    HD_RESULT   hdResult;       //Hand Detection results
    FD_RESULT   fdResult;       //Face Detection & Estimations results
    HVC_IMAGE   image;          //Image data
}HVC_RESULT;

/**
 *Threshold of confidence
 */
typedef struct{
    INT32   bdThreshold;        //Threshold of confidence of Human Body Detection
    INT32   hdThreshold;        //Threshold of confidence of Hand Detection
    INT32   dtThreshold;        //Threshold of confidence of Face Detection
    INT32   rsThreshold;        //Threshold of confidence of Face Recognition
}HVC_THRESHOLD;

/**
 *Detection size
 */
typedef struct{
    INT32   bdMinSize;          //Minimum detection size of Human Body Detection
    INT32   bdMaxSize;          //Maximum detection size of Human Body Detection
    INT32   hdMinSize;          //Minimum detection size of Hand Detection
    INT32   hdMaxSize;          //Maximum detection size of Hand Detection
    INT32   dtMinSize;          //Minimum detection size of Face Detection
    INT32   dtMaxSize;          //Maximum detection size of Face Detection
}HVC_SIZERANGE;


/*=========================================================================*/

namespace upm {
    /**
      * @brief Mems HVC Sensors
      * @defgroup b5t007001 libupm-b5t007001
      * @ingroup --add group
      */
    /**
     * @library b5t007001
     * @sensor b5t007001
     * @comname Omron HVC sensors
     * @type --add type
     * @man omron
     * @con UART
     * @web --add weblink
     *
     * @brief API for the Omron B5T007001 HVC sensors interface
     *
     * It is connected via a UART Interface.
     *
     * @snippet b5t007001.cxx Interesting
     */
    class B5T007001{
        public :
	   /**
            * B5T007001 object constructor
            *
            * @param uart   UART to use. Default value is DEFAULT_UART_NUM whenver not specified manually.
            * @param inRate UART baudrate. Default value is HVC_UART_BAUD_RATE_9600 whenever not specified manually.
            */
            B5T007001(UINT8 uart=DEFAULT_UART_NUM, HVC_UART_BAUD_RATE_T inRate=HVC_UART_BAUD_RATE_9600);

	   /**
            * B5T007001 object destructure
            *
            */
	    ~B5T007001();

            /**
             * Get HVC version information
             *
             * @param inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @param outVersion      Version data
             * @return One of the D6T_STATUS_T values
             */
           B5T_STATUS_T getVersion(INT32 inTimeOutTime, HVC_VERSION &outVersion);

            /**
             * Set HVC camera angle
             *
             * @param inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @param inAngleNo       Camera angle number
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T setCameraAngle(INT32 inTimeOutTime, UINT8 inAngleNo);

            /**
             * Get HVC camera angle
             *
             * @param inTimeOutTime    Timeout time (ms). If negative, recommended default timeout is used.
             * @param outAngleNo       Camera angle number
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T getCameraAngle(INT32 inTimeOutTime, UINT8 &outAngleNo);

            /**
             * Execute Detection for given parameters
             *
             * @param inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @param inExec          Execution parameter for face detection or hand detection etc
             * @param inImageInfo     Output image designation
             * @param outHVCResult    Result data
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T executeDetection(INT32 inTimeOutTime, INT32 inExec, INT32 inImageInfo, HVC_RESULT &outHVCResult);

            /**
             * Execute Detection for given parameters and return information with STB results
             *
             * @param inTimeOutTime    Timeout time (ms). If negative, recommended default timeout is used.
             * @param inExec           Execution parameter for face detection or hand detection etc
             * @param inImageInfo      Output image designation
             * @param outHVCResult     Result data
             * @param outSTBFaceCount  STB Face count
             * @param outSTBFaceResult STB Face result
             * @param outSTBBodyCount  STB Body count
             * @param outSTBBodyResult STB Body result
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T executeDetection(INT32 inTimeOutTime, INT32 inExec, INT32 inImageInfo, HVC_RESULT &outHVCResult,INT32 *outSTBFaceCount, STB_FACE **outSTBFaceResult,INT32 *outSTBBodyCount,STB_BODY **outSTBBodyResult);

            /**
             * Set the threshold value for Human Body Detection, Hand Detection, Face Detection and/or Face Recognition
             *
             * @param inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @param inThreshold     Threshold value
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T setThreshold(INT32 inTimeOutTime, HVC_THRESHOLD inThreshold);

            /**
             * Get the threshold value for Human Body Detection, Hand Detection, Face Detection and/or Face Recognition
             *
             * @param inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @param outThreshold   Threshold value
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T getThreshold(INT32 inTimeOutTime, HVC_THRESHOLD &outThreshold);

            /**
             * Set the detection size of Human Body Detection, Hand Detection, Face Detection
             *
             * @param inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @param inSizeRange     SizeRange value
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T setSizeRange(INT32 inTimeOutTime, HVC_SIZERANGE inSizeRange);

            /**
             * Get the detection size of Human Body Detection, Hand Detection, Face Detection
             *
             * @param inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @param outSizeRange   SizeRange value
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T getSizeRange(INT32 inTimeOutTime, HVC_SIZERANGE &outSizeRange);

            /**
             * Set the face angle for Face Detection, i.e. the yaw and roll angle range to be detected
             *
             * @param inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @param inPose          Yaw angle range
             * @param inAngle         Roll angle range
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T setFaceDetectionAngle(INT32 inTimeOutTime, INT32 inPose, INT32 inAngle);

            /**
             * Get the face angle for Face Detection
             *
             * @param inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @param outPose         Yaw angle range
             * @param outAngle        Roll angle range
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T getFaceDetectionAngle(INT32 inTimeOutTime, INT32 &outPose, INT32 &outAngle);

            /**
             * Set the UART baudrate
             *
             * @param inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @param inRate          UART baud rate
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T setBaudRate(INT32 inTimeOutTime, HVC_UART_BAUD_RATE_T inRate);

	    /**
             * Get registered userIDs and dataIDs
             *
             * @param  inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @param  outUserList     User ID list
             * @param  outDataList     Data ID list
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T getRegisteredUsersInfo(INT32 inTimeOutTime, std::vector<UINT8> &outUserList, std::vector<UINT8> &outDataList);
            /**
             * Add/Register new user
             *
             * @param  inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @param  inUserID        User ID (0-99)
             * @param  inDataID        Data ID (0-9)
             * @param  outImage        Image info
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T addUser(INT32 inTimeOutTime, UINT8 inUserID, UINT8 inDataID, HVC_IMAGE &outImage);

            /**
             * Delete specific user data for specific user id
             *
             * @param inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @param inUserID        User ID (0-99)
             * @param inDataID        Data ID (0-9)
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T deleteUserData(INT32 inTimeOutTime, UINT8 inUserID, UINT8 inDataID);

            /**
             * Delete specific user
             *
             * @param inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @param inUserID        User ID (0-99)
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T deleteUser(INT32 inTimeOutTime, UINT8 inUserID);

            /**
             * Delete all users
             *
             * @param inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T deleteUserAll(INT32 inTimeOutTime);

            /**
             * Get Registration Info (Face Recognition)
             *
             * @param inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @param inUserID        User ID (0-99)
             * @param outDataNo      Registration Info
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T getUserData(INT32 inTimeOutTime, UINT8 inUserID, UINT8 &outDataNo);

            /**
             * Save Album from HVC device to Host
             *
             * @param inTimeOutTime      Timeout time (ms). If negative, recommended default timeout is used.
             * @param outAlbumData       Album data
             * @param outAlbumDataSize   Album data size
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T saveAlbumToHost(INT32 inTimeOutTime, UINT8 *outAlbumData, INT32 &outAlbumDataSize);

            /**
             *  Load Album from Host to HVC device
             *
             * @param inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @param inAlbumData     Album data
             * @param inAlbumDataSize Album data size
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T loadAlbumFromHost(INT32 inTimeOutTime, UINT8 *inAlbumData, INT32 inAlbumDataSize);

            /**
             *  Save Album from HVC RAM to ROM memory
             *
             * @param inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T saveAlbumToROM(INT32 inTimeOutTime);

            /**
             * Set Stabilization status
             *
             * @param inFlag STB init flags
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T setSTBStatus(STB_UINT32 inFlag);

            /**
             * Get stabilization status
             *
             * @param outStatus       Enable/Disable status
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T getSTBStatus(STB_INT32 &outStatus);

            /**
             * Clear stabilization frame results
             *
             * @return One of the B5T_STATUS_T values
             */
	    B5T_STATUS_T stbClear(void);

            /**
             * Set maximum retry count for STB results
             *
             * @param inMaxRetryCount    Maximum retry count
             * @return One of the B5T_STATUS_T values
             */
	    B5T_STATUS_T stbSetTrRetryCount(STB_INT32 inMaxRetryCount);

            /**
             * Get maximum retry count for STB results
             *
             * @param outMaxRetryCount    Maximum retry count
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T stbGetTrRetryCount(STB_INT32 *outMaxRetryCount);

            /**
             * Set rectangle steadiness tracking parameter
             *
             * @param inPosSteadinessParam Rectangle position steadiness parameter
             * @param inSizeSteadinessParam Rectangle size steadiness parameter
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T stbSetTrSteadinessParam(STB_INT32 inPosSteadinessParam, STB_INT32 inSizeSteadinessParam);

            /**
             * Get rectangle steadiness tracking parameter
             *
             * @param outPosSteadinessParam Rectangle position steadiness parameter
             * @param outSizeSteadinessParam Rectangle size steadiness parameter
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T stbGetTrSteadinessParam(STB_INT32 *outPosSteadinessParam, STB_INT32 *outSizeSteadinessParam);

            /**
             * Set estimation result stabilizing threshold value
             *
             * @param inThreshold Face direction confidence threshold value
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T stbSetPeThresholdUse(STB_INT32 inThreshold);

            /**
             * Get estimation result stabilizing threshold value
             *
             * @param inThreshold Face direction confidence threshold value
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T stbGetPeThresholdUse(STB_INT32 *outThreshold);

            /**
             * Set estimation result stablizing angle
             *
             * @param inMinUDAngle Minimum up-down angle of face
             * @param inMaxUDAngle Maximum up-down angle of face
             * @param inMinLRAngle Minimum left-right angle of face
             * @param inMaxLRAngle Maximum left-right angle of face
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T stbSetPeAngleUse(STB_INT32 inMinUDAngle, STB_INT32 inMaxUDAngle, STB_INT32 inMinLRAngle, STB_INT32 inMaxLRAngle);

            /**
             * Get estimation result stablizing angle
             *
             * @param outMinUDAngle Minimum up-down angle of face
             * @param outMaxUDAngle Maximum up-down angle of face
             * @param outMinLRAngle Minimum left-right angle of face
             * @param outMaxLRAngle Maximum left-right angle of face
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T stbGetPeAngleUse(STB_INT32 *outMinUDAngle, STB_INT32 *outMaxUDAngle, STB_INT32 *outMinLRAngle, STB_INT32 *outMaxLRAngle);

            /**
             * Get age/gender esitmation complete frame count
             *
             * @param inCompCount The number of previous frames applying to fix the result
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T stbSetPeCompleteFrameCount(STB_INT32 inCompCount);

            /**
             * Set age/gender esitmation complete frame count
             *
             * @param outCompCount The number of previous frames applying to fix the result
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T stbGetPeCompleteFrameCount(STB_INT32 *outCompCount);

            /**
             * Set recognition stabilizing threshold value
             *
             * @param inThreshold Face direction confidence threshold value
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T stbSetFrThresholdUse(STB_INT32 inThreshold);

            /**
             * Get recognition stabilizing threshold value
             *
             * @param outThreshold Face direction confidence threshold value
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T stbGetFrThresholdUse(STB_INT32 *outThreshold);

            /**
             * Get recognition stabilizing angle
             *
             * @param inMinUDAngle Minimum up-down angle of face
             * @param inMaxUDAngle Maximum up-down angle of face
             * @param inMinLRAngle Minimum left-right angle of face
             * @param inMaxLRAngle Maximum left-right angle of face
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T stbSetFrAngleUse(STB_INT32 inMinUDAngle, STB_INT32 inMaxUDAngle, STB_INT32 inMinLRAngle, STB_INT32 inMaxLRAngle);

            /**
             * Set recognition stabilizing angle
             *
             * @param outMinUDAngle Minimum up-down angle of face
             * @param outMaxUDAngle Maximum up-down angle of face
             * @param outMinLRAngle Minimum left-right angle of face
             * @param outMaxLRAngle Maximum left-right angle of face
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T stbGetFrAngleUse(STB_INT32 *outMinUDAngle, STB_INT32 *outMaxUDAngle, STB_INT32 *outMinLRAngle, STB_INT32 *outMaxLRAngle);

            /**
             * Get recognition stablizing complete frame count
             *
             * @param inFrameCount The number of previous frames applying to fix the result
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T stbSetFrCompleteFrameCount(STB_INT32 inFrameCount);

            /**
             * Set recognition stablizing complete frame count
             *
             * @param outFrameCount The number of previous frames applying to fix the result
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T stbGetFrCompleteFrameCount(STB_INT32 *outFrameCount);

            /**
             * Get recognition minimum account ratio
             *
             * @param inMinRatio Recognition minimum account ratio
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T stbSetFrMinRatio(STB_INT32 inMinRatio);

            /**
             * Set recognition minimum account ratio
             *
             * @param outMinRatio Recognition minimum account ratio
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T stbGetFrMinRatio(STB_INT32 *outMinRatio);

            /**
             * Get Stb version
             *
             * @param outMajorVersion Major version number
	     * @param outMinorVersion Minor version number
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T stbGetVersion(STB_INT8* outMajorVersion, STB_INT8* outMinorVersion);

	private:
	    mraa::Uart* mUart;
	    INT32 mStbStatus;
            HSTB mSTBHandle;
            int mFaceCount;
            STB_FACE mFace[STB_MAX_NUM];
            int mBodyCount;
            STB_BODY mBody[STB_MAX_NUM];

	    /**
             * UART send command
             *
             * @param inDataSize   Data Size
             * @param inData       Data
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T UARTSendData(INT32 inDataSize, const UINT8 *inData);

	    /**
             * UART Receive Data
             *
             * @param inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @param inDataSize      Data Size
             * @param outResult       Received Data
             * @param isSyncRequired  Check for sync byte
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T UARTReceiveData(INT32 inTimeOutTime, INT32 inDataSize, UINT8 *outResult, UINT8 isSyncRequired=0);

            /**
             * HVC send command
             *
             * @param inCommandNo  Command Number
             * @param inDataSize   Data Size
	     * @param inData       Data
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T HVCSendCommand(UINT8 inCommandNo, INT32 inDataSize, UINT8 *inData);

	    /**
             * HVC Receive Data
             *
             * @param inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @param inDataSize   Data Size
	     * @param outResult    Received Data
             * @param inCommandNo  Command Number
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T HVCReceiveData(INT32 inTimeOutTime, INT32 inDataSize, UINT8 *outResult);

	    /**
             * HVC Receive Header
             *
             * @param inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @param outDataSize     Data Size
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T HVCReceiveHeader(INT32 inTimeOutTime, INT32 &outDataSize);

	    /**
             * HVC registration
             *
             * @param inTimeOutTime   Timeout time (ms). If negative, recommended default timeout is used.
             * @param inUserID        User ID
             * @param inDataID        Data ID
             * @param outImage        Out image data
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T HVCRegistration(INT32 inTimeOutTime, INT32 inUserID, INT32 inDataID, HVC_IMAGE &outImage);

	    /**
             * Clear data on RX line of UART
             *
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T UARTRxIgnoreAllData(void);

	    /**
             * Init stabilization library
             *
             * @param inFuncFlag        init flag
             * @return One of the B5T_STATUS_T values
             */
	    B5T_STATUS_T stbInit(STB_UINT32 inFuncFlag);

	    /**
             * Deinit stabilization library
             *
             * @return One of the B5T_STATUS_T values
             */
            void stbDeInit(void);

	    /**
             * Execute stabilization
             *
             * @param inActiveFunc       Active functions
             * @param inResult           HVC_RESULT info for stabilization
             * @param outSTBFaceCount    STB face count
             * @param outSTBFaceResult   STB face results
             * @param outSTBBodyCount    STB body count
             * @param outSTBBodyResult   STB body results
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T stbExec(STB_INT32 inActiveFunc, HVC_RESULT inResult, STB_INT32 *outSTBFaceCount, STB_FACE **outSTBFaceResult, STB_INT32 *outSTBBodyCount, STB_BODY **outSTBBodyResult);
	    /**
             * Get stabilization frame result
             *
             * @param inActiveFunc   Active functions
             * @param inResult       HVC_RESULT info for stabilization
             * @param outFrameResult Stabilization frame results
             * @return One of the B5T_STATUS_T values
             */
            B5T_STATUS_T stbGetFrameResult(STB_INT32 inActiveFunc, HVC_RESULT inResult, STB_FRAME_RESULT *outFrameResult);

	    /*
	     * Copy Constructor
	     */
	    B5T007001(const B5T007001&);

	    /**
	     * Operator Overloading
	     */
	    B5T007001& operator=(const B5T007001&);
    };
}

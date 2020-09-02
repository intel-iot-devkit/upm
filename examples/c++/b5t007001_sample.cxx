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

/* headers */
#include <iostream>
#include <signal.h>
#include <sys/time.h>
#include <iomanip>
#include <limits>
#include <fstream>
#include <string>
#include <unistd.h>

/* omron sensor header */
#include "b5t007001.hpp"

/* Define */
#define CLEAR_SCREEN "\033[2J\033[1;1H"

#define HUMAN_BODY_DETECT               1
#define HAND_DETECT                     2
#define FACE_DETECT                     3
#define AGE_ESTIMATE                    4
#define FACE_DIRECTION                  5
#define GENDER_ESTIMATE                 6
#define GAZE_ESTIMATE                   7
#define BLINK_ESTIMATE                  8
#define EXPRESSION_ESTIMATE             9
#define FACE_RECOGNITION                10
#define ALL                             11
#define SETTINGS                        12

#define DEBUG_PRINT 0
using namespace upm;
using namespace std;

volatile sig_atomic_t exitFlag = 0;
UINT8 isCinWaitState = 0; //State for cin is waiting for input or not.

void
sig_handler(int signum)
{
    // Check for Interrupt signal
    if (signum == SIGINT || signum == SIGTERM){
       exitFlag++;
    }
    if(exitFlag == 2){
        if(isCinWaitState == 1){
            std::cin.setstate(std::ios_base::badbit);
        }
    }
    cout << "\r\nPress enter to continue or press ctrl + c again to exit from application." << endl;
}

bool isNumberWithSpaces(string s)
{
    if(s.size() == 0)
    {
        return false;
    }
    for (UINT32 i = 0; i < s.length(); i++){
        if (s[i] != ' ' && isdigit(s[i]) == false){
            return false;
        }
    }
    return true;
}

bool isNumber(string s)
{
    if(s.size() == 0)
    {
        return false;
    }
    for (UINT32 i = 0; i < s.length(); i++){
        if (s[i] != '-'  && isdigit(s[i]) == false){
            return false;
        }
    }
    return true;
}

void getNumbersFromString(string tmpStr, vector<INT32> &outNumbers){
    UINT32 found=0, foundNext = 0;
    outNumbers.clear();
    found = tmpStr.find(' ', found);
    outNumbers.push_back(stoi(tmpStr.substr(0, found)));
    found = 0;
    for(UINT32 i=0;i< tmpStr.size();i++){
        found = tmpStr.find(' ', found+1);
        if ((found != string::npos) && (found < tmpStr.size()-1)){
	    foundNext = tmpStr.find(' ', found+1);
#if DEBUG_PRINT
            cout << "DEBUG: foundNext =" << foundNext << endl;
#endif
	    if(foundNext == string::npos) {
                foundNext = tmpStr.size() - found;
	    }
	    else
	    {
                foundNext = foundNext - found;
	    }
#if DEBUG_PRINT
            cout << "DEBUG: foundNext =" << foundNext << endl;
            cout << "DEBUG: First occurrence is " << stoi(tmpStr.substr(found+1, foundNext)) << endl;
#endif
            outNumbers.push_back(stoi(tmpStr.substr(found+1, foundNext)));
        }
        else
        {
            break;
        }
    }
}
UINT8 takeDigitInput(string printMsg, STB_INT32 &inputValue){
    string tmpStr;
    isCinWaitState = 1;
    try{
        cin.exceptions(std::ios_base::badbit);
        do{
           exitFlag = 0;
           cout << printMsg;
           getline(cin, tmpStr);
           if(isNumber(tmpStr)){
               inputValue = stoi(tmpStr);
               break;
           }
           else
           {
              cout << "Invalid input. Please try again." << endl;
           }

        }while(1);
    }catch (std::ios_base::failure& fail){
        isCinWaitState = 0;
        return false;
    }
    isCinWaitState = 0;
    return true;
}
void takeEnterInput(){
    cout << "Press enter to continue...";
    isCinWaitState = 1;
    try{
        exitFlag = 0;
        cin.clear(); // reset badbit
        cin.exceptions(std::ios_base::badbit);
        while(1){
           if(cin.get() == '\n'){
               break;
           }
        }
    }catch (std::ios_base::failure& fail){
    }
    isCinWaitState = 0;
}

UINT8
takeSelectedMultiChoiceInput(STRING str, std::vector<INT32> &value, INT32 startIndex, INT32 endIndex){// Take input

    isCinWaitState = 1;
    std::string tmpStr;
    UINT8 takeInput = 1;
    try{
        cin.exceptions(std::ios_base::badbit);

        do{
            cout << str;
            getline(cin,tmpStr);

            exitFlag = 0;
            if(!isNumberWithSpaces(tmpStr)){
                cout << "*************************************************************" << endl;
                cout << "Invalid choice. Please select from " << std::dec << startIndex << " to " << endIndex << "." << endl;
                cout << "*************************************************************" << endl;
                continue;
            }

            getNumbersFromString(tmpStr, value);

            takeInput = 0;
            // Check start index and end index with input value
            for(UINT8 i=0;i < value.size();i++){
                #if DEBUG_PRINT
                std::cout << "DEBUG: Processing values " << value[i] << std::endl;
                #endif

                if(!(value[i] <= endIndex && value[i] >= startIndex)){
                    cout << "*************************************************************" << endl;
                    cout << "Invalid choice. Please select from " << std::dec << startIndex << " to " << endIndex << "." << endl;
                    cout << "*************************************************************" << endl;
                    takeInput = 1;
                    break;
                }
            }
        }while(takeInput);

    }catch (std::ios_base::failure& fail){
        isCinWaitState = 0;
        return false;
    }
    isCinWaitState = 0;
    return true;
}


UINT8
takeSelectedInput(STRING str, INT32 &value, INT32 startIndex, INT32 endIndex){// Take input

    isCinWaitState = 1;

    try{
        cin.exceptions(std::ios_base::badbit);

        do{
            cout << str;
            cin >> value;

            // Clear and ignor bad input
            if(isdigit(value) != true){
                cin.clear(); // reset badbit
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skip bad input
            }

            exitFlag = 0;
            // Check start index and end index with input value
            if(value < startIndex || value > endIndex){
                cout << "*************************************************************" << endl;
                cout << "Invalid choice. Please select from " << std::dec << startIndex << " to " << endIndex << "." << endl;
                cout << "*************************************************************" << endl;
            }else{
                isCinWaitState = 0;
                return true;
            }

        }while((value < startIndex || value > endIndex) && exitFlag < 2);

    }catch (std::ios_base::failure& fail){
        isCinWaitState = 0;
        return false;
    }
    isCinWaitState = 0;
    return false;
}
string printSTBStatus(STB_STATUS status){

    if(status == STB_STATUS_NO_DATA){
       return "No data";
    }else if(status == STB_STATUS_CALCULATING){
        return "Calculating";
    }else if(status == STB_STATUS_COMPLETE){
        return "Complete";
    }else if(status == STB_STATUS_FIXED){
        return "Fixed";
    }
    return "Invalid status";
}

UINT8 getSTBFaceIndex(STB_INT32 nDetectID, STB_FACE *outSTBFaceResult, INT32 outSTBFaceCount, INT32 &nIndex){
    INT32 count = 0;

    for(count = 0; count < outSTBFaceCount; count++){
        if(outSTBFaceResult[count].nDetectID == nDetectID){
            nIndex = count;
            return true;
        }
    }

    return false;
}

int
main(int argc, char** argv)
{
    signal(SIGINT, sig_handler);

    int sub_choice,input;
    int loop;
    INT32 timeout = UART_EXECUTE_TIMEOUT;
    HVC_VERSION version;
    B5T_STATUS_T returnStatus;

    HVC_UART_BAUD_RATE_T baudRate;
    UINT8 cameraAngle;
    HVC_THRESHOLD threshold;
    HVC_SIZERANGE sizeRange;
    INT32 pose, angle;

    HVC_RESULT hvcResult;
    INT32 execFlag;
    INT32 imageNum = HVC_EXECUTE_IMAGE_QVGA_HALF;

    EXPRESSION exEnum;
    INT32 stbStatus = 0;

    INT32 outSTBFaceCount;
    STB_FACE *outSTBFaceResult;
    INT32 outSTBBodyCount;
    STB_BODY *outSTBBodyResult;
    string tmpStr;
    UINT32 i;
    std::vector<INT32> digitValues;
    uint32_t sleep_time = 1000;
    STB_INT8 majorVersion = 0, minorVersion = 0;
    STB_INT32 maxRetryCount=0;
    STB_INT32 posSteadinessParam, sizeSteadinessParam;
    STB_INT32 thresholdValue;
    STB_INT32 udAngleMin, udAngleMax, lrAngleMin,lrAngleMax;
    STB_INT32 frameCount;
    STB_INT32 minRatio;

    // Data and Time
    INT8 dateAndTime[30];
    struct timeval value;
    time_t curtime;

    UINT8 displaySamples = 1;
    UINT8 ret= true;
    INT32 nIndex;

    cout << "*************************************************************" << endl;
    cout << "*************************************************************" << endl;
    cout << "                        BaudRates                            " << endl;
    cout << "*************************************************************" << endl;
    cout << "*************************************************************" << endl;
    cout << "1. 9600                                                      " << endl;
    cout << "2. 38400                                                     " << endl;
    cout << "3. 115200                                                    " << endl;
    cout << "4. 230400                                                    " << endl;
    cout << "5. 460800                                                    " << endl;
    cout << "6. 921600                                                    " << endl;
    cout << "*************************************************************" << endl;

    ret = takeSelectedInput("Select from the list of BaudRates : ",input,1,6);

    if(ret != true){
        return 0;
    }

    switch(input){

        case 1: // 9600
            baudRate = HVC_UART_BAUD_RATE_9600;
            break;
        case 2: // 38400
            baudRate = HVC_UART_BAUD_RATE_38400;
            break;
        case 3: // 115200
            baudRate = HVC_UART_BAUD_RATE_115200;
            break;
        case 4: // 230400
            baudRate = HVC_UART_BAUD_RATE_230400;
            break;
        case 5: // 460800
            baudRate = HVC_UART_BAUD_RATE_460800;
            break;
        case 6: // 921600
            baudRate = HVC_UART_BAUD_RATE_921600;
            break;
        default:
            cout << "Invalid choice. Please select from 1 to 6." << endl;
            break;

    }//end input switch

    // Instantiate a B5T007001 sensor on UART.
    B5T007001 sensor(DEFAULT_UART_NUM, baudRate);

    // Get the sensor version
    returnStatus =  sensor.getVersion(timeout,version);

    if(returnStatus != B5T_STATUS_SUCCESS){
        cout << "Failed to get Version with error code : " << returnStatus << endl;
        return 0;
    }

    // String re-size
    string tmpstring((char*)version.string, sizeof(version.string));
    do{
        cout << CLEAR_SCREEN;
        cout << "*************************************************************" << endl;
        cout << " HVC Version : "
             << tmpstring
             << (unsigned) version.major   << "."
             << (unsigned) version.minor   << "."
             << (unsigned) version.relese  << "."
             << (unsigned) (version.revision[0]        +
                           (version.revision[1] << 8)  +
                           (version.revision[2] << 16) +
                           (version.revision[3] << 24))                         << endl;
        cout << "*************************************************************" << endl;
        cout << "*************************************************************" << endl;
        cout << "                        MAIN MENU                            " << endl;
        cout << "*************************************************************" << endl;
        cout << "*************************************************************" << endl;
        cout << "1. Human body detection                                      " << endl;
        cout << "2. Hand detection                                            " << endl;
        cout << "3. Face detection                                            " << endl;
        cout << "4. Age estimation                                            " << endl;
        cout << "5. Face direction estimation                                 " << endl;
        cout << "6. Gender estimation                                         " << endl;
        cout << "7. Gaze estimation                                           " << endl;
        cout << "8. Blink estimation                                          " << endl;
        cout << "9. Expression estimation                                     " << endl;
        cout << "10. Face recognition                                         " << endl;
        cout << "11. All (1 to 10)                                            " << endl;
        cout << "12. Settings                                                 " << endl;
        cout << "*************************************************************" << endl;
        ret = takeSelectedMultiChoiceInput("select values separated by spaces if multi choice for 1-12: ",digitValues,1,12);

        if(ret != true){
            break;
        }

	    execFlag = 0;
        displaySamples = 1;
        for(i=0;i < digitValues.size();i++){
            switch(digitValues[i]){
                case HUMAN_BODY_DETECT:
                    execFlag |=  HVC_ACTIV_BODY_DETECTION;
                    break;
                case HAND_DETECT:
                    execFlag |=  HVC_ACTIV_HAND_DETECTION;
                    break;
                case FACE_DETECT:
                    execFlag |=  HVC_ACTIV_FACE_DETECTION;
                    break;
                case AGE_ESTIMATE:
                    execFlag |=  HVC_ACTIV_AGE_ESTIMATION;
                    break;
                case FACE_DIRECTION:
                    execFlag |=  HVC_ACTIV_FACE_DIRECTION;
                    break;
                case GENDER_ESTIMATE:
                    execFlag |=  HVC_ACTIV_GENDER_ESTIMATION;
                    break;
                case GAZE_ESTIMATE:
                    execFlag |=  HVC_ACTIV_GAZE_ESTIMATION;
                    break;
                case BLINK_ESTIMATE:
                    execFlag |=  HVC_ACTIV_BLINK_ESTIMATION;
                    break;
                case EXPRESSION_ESTIMATE:
                    execFlag |=  HVC_ACTIV_EXPRESSION_ESTIMATION;
                    break;
                case FACE_RECOGNITION:
                    execFlag |=  HVC_ACTIV_FACE_RECOGNITION;
                    break;
                case ALL:
                    // Update all flags
                    execFlag |= HVC_ACTIV_BODY_DETECTION | HVC_ACTIV_HAND_DETECTION | HVC_ACTIV_FACE_DETECTION | HVC_ACTIV_FACE_DIRECTION |
                               HVC_ACTIV_AGE_ESTIMATION | HVC_ACTIV_GENDER_ESTIMATION | HVC_ACTIV_GAZE_ESTIMATION | HVC_ACTIV_BLINK_ESTIMATION |
                               HVC_ACTIV_EXPRESSION_ESTIMATION | HVC_ACTIV_FACE_RECOGNITION;
                    break;
                case SETTINGS:
                    cout << "*************************************************************" << endl;
                    cout << "*************************************************************" << endl;
                    cout << "                        Settings                             " << endl;
                    cout << "*************************************************************" << endl;
                    cout << "*************************************************************" << endl;
                    cout << "1. Get camera angle                                          " << endl;
                    cout << "2. Set camera angle                                          " << endl;
                    cout << "3. Get threshold value                                       " << endl;
                    cout << "4. Set threshold value                                       " << endl;
                    cout << "5. Get detection size                                        " << endl;
                    cout << "6. Set detection size                                        " << endl;
                    cout << "7. Get face angle                                            " << endl;
                    cout << "8. Set face angle                                            " << endl;
                    cout << "9. Set uart forwarding rate                                  " << endl;
                    cout << "10. Get stb library status                                   " << endl;
                    cout << "11. Set stb library status                                   " << endl;
                    cout << "12. Get stb version                                          " << endl;
                    cout << "13. Set stb maximum retry count(TR)                          " << endl;
                    cout << "14. Get stb maximum retry count(TR)                          " << endl;
                    cout << "15. Set stb rectangle steadiness parameter(TR)               " << endl;
                    cout << "16. Get stb rectangle steadiness parameter(TR)               " << endl;
                    cout << "17. Set stb estimation result stabilizing threshold value(PE)" << endl;
                    cout << "18. Get stb estimation result stabilizing threshold value(PE)" << endl;
                    cout << "19. Set stb estimation result stabilizing angle(PE)          " << endl;
                    cout << "20. Get stb estimation result stabilizing angle(PE)          " << endl;
                    cout << "21. Set stb age/gender estimation complete frame count(PE)   " << endl;
                    cout << "22. Get stb age/gender estimation complete frame count(PE)   " << endl;
                    cout << "23. Set stb recognition stabilizing threshold value(FR)      " << endl;
                    cout << "24. Get stb recognition stabilizing threshold value(FR)      " << endl;
                    cout << "25. Set stb recognition stabilizing angle(FR)                " << endl;
                    cout << "26. Get stb recognition stabilizing angle(FR)                " << endl;
                    cout << "27. Set stb recognition stabilizing complete frame count(FR) " << endl;
                    cout << "28. Get stb recognition stabilizing complete frame count(FR) " << endl;
                    cout << "29. Set stb recognition minimum account ratio(FR)            " << endl;
                    cout << "30. Get stb recognition minimum account ratio(FR)            " << endl;
                    cout << "31. Clear stb frame result                                   " << endl;
                    cout << "*************************************************************" << endl;

	                displaySamples = 0;
                    ret = takeSelectedInput("Select from the list of settings : ",sub_choice,1,31);

                    if(ret != true){
                        break;
                    }

                    switch(sub_choice){
                        case 1: // Get camera angle
                            returnStatus =  sensor.getCameraAngle(timeout, cameraAngle);

                            if(returnStatus != B5T_STATUS_SUCCESS){
                                cout << "Failed to get camera Angle with error code : " << returnStatus << endl;
                                break;
                            }

                            switch(cameraAngle){

                                case 0: // 0
                                    cout << "Camera Angle is 0 degree." << endl;
                                    break;
                                case 1: // 90
                                    cout << "Camera Angle is 90 degree." << endl;
                                    break;
                                case 2: // 180
                                    cout << "Camera Angle is 180 degree." << endl;
                                    break;
                                case 3: // 270
                                    cout << "Camera Angle is 270 degree." << endl;
                                    break;
                                default:
                                    cout << "Invalid Camera Angle Received." << endl;

                            }//end switch getCameraAngle

                            break;
                        case 2: // Set camera angle

                             cout << "*************************************************************" << endl;
                             cout << "*************************************************************" << endl;
                             cout << "                        Camera Angles                        " << endl;
                             cout << "*************************************************************" << endl;
                             cout << "*************************************************************" << endl;
                             cout << "1. 0   degree                                                " << endl;
                             cout << "2. 90  degree                                                " << endl;
                             cout << "3. 180 degree                                                " << endl;
                             cout << "4. 270 degree                                                " << endl;
                             cout << "*************************************************************" << endl;

                             ret = takeSelectedInput("Select from the list of Camera Angles :",input,1,4);

                             if(ret != true){
                                 break;
                             }

                             cameraAngle = (input - 1);

                             returnStatus = sensor.setCameraAngle(timeout,cameraAngle);

                             if(returnStatus != B5T_STATUS_SUCCESS){
                                 cout << "Failed to set camera angle with error code : " << returnStatus << endl;
                                 break;
                             }

                             if(cameraAngle == 0){
                                 cout << "Camera Angle : 0 set successfully" << endl;
                             }else if(cameraAngle == 1){
                                 cout << "Camera Angle : 90 set successfully" << endl;
                             }else if(cameraAngle == 2){
                                 cout << "Camera Angle : 180 set successfully" << endl;
                             }else if(cameraAngle == 3){
                                 cout << "Camera Angle : 270 set successfully" << endl;
                             }else{
                                 cout << "Invalid Camera Angle set." << endl;
                             }

                            break;
                        case 3: // Get threshold value

                            returnStatus =  sensor.getThreshold(timeout, threshold);

                            if(returnStatus != B5T_STATUS_SUCCESS){
                                cout << "Failed to get Threshold value with error code : " << returnStatus << endl;
                                break;
                            }

                            cout << "Threshold Value of Human Body Detection is : " << threshold.bdThreshold << endl;
                            cout << "Threshold Value of Hand Detection is : "       << threshold.hdThreshold << endl;
                            cout << "Threshold Value of Face Detection is : "       << threshold.dtThreshold << endl;
                            cout << "Threshold Value of Face Recognition is : "     << threshold.rsThreshold << endl;

                            break;

                        case 4: // Set threshold value

                            // Human Body Detection
                            ret = takeSelectedInput("Input Threshold value for Human Body Detection : ",input,1,1000);

                            if(ret != true){
                                break;
                            }

                            threshold.bdThreshold = input;

                            // Hand Detection
                            ret = takeSelectedInput("Input Threshold value for Hand Detection : ",input,1,1000);

                            if(ret != true){
                                break;
                            }

                            threshold.hdThreshold = input;

                            // Face Detection
                            ret = takeSelectedInput("Input Threshold value for Face Detection : ",input,1,1000);

                            if(ret != true){
                                break;
                            }

                            threshold.dtThreshold = input;

                            // Face Recognition
                            ret = takeSelectedInput("Input Threshold value for Face Recognition : ",input,1,1000);

                            if(ret != true){
                                break;
                            }

                            threshold.rsThreshold = input;

                            returnStatus = sensor.setThreshold(timeout,threshold);

                            if(returnStatus != B5T_STATUS_SUCCESS){
                                cout << "Failed to set threshold value with error code : " << returnStatus << endl;
                                break;
                            }

                            cout << "Threshold values set successfully." << endl;

                            break;

                        case 5: // Get detection size

                            returnStatus =  sensor.getSizeRange(timeout,sizeRange);

                            if(returnStatus != B5T_STATUS_SUCCESS){
                                cout << "Failed to get detection size with error code : " << returnStatus << endl;
                                break;
                            }

                            cout << "Minimum detection size of Human Body Detection is : " << sizeRange.bdMinSize << endl;
                            cout << "Maximum detection size of Human Body Detection is : " << sizeRange.bdMaxSize << endl;
                            cout << "Minimum detection size of Hand Detection is : "       << sizeRange.hdMinSize << endl;
                            cout << "Maximum detection size of Hand Detection is : "       << sizeRange.hdMaxSize << endl;
                            cout << "Minimum detection size of Face Detection is : "       << sizeRange.dtMinSize << endl;
                            cout << "Maximum detection size of Face Detection is : "       << sizeRange.dtMaxSize << endl;

                            break;

                        case 6: // Set detection size

                            ret = takeSelectedInput("Minimum detection size of Human Body Detection : ",input,20,8192);

                            if(ret != true){
                                break;
                            }

                            sizeRange.bdMinSize = input;

                            ret = takeSelectedInput("Maximum detection size of Human Body Detection : ",input,20,8192);

                            if(ret != true){
                                break;
                            }

                            sizeRange.bdMaxSize = input;

                            ret = takeSelectedInput("Minimum detection size of Hand Detection : ",input,20,8192);

                            if(ret != true){
                                break;
                            }

                            sizeRange.hdMinSize = input;

                            ret = takeSelectedInput("Maximum detection size of Hand Detection : ",input,20,8192);

                            if(ret != true){
                                break;
                            }

                            sizeRange.hdMaxSize = input;

                            ret = takeSelectedInput("Minimum detection size of Face Detection : ",input,20,8192);

                            if(ret != true){
                                break;
                            }

                            sizeRange.dtMinSize = input;

                            ret = takeSelectedInput("Maximum detection size of Face Detection : ",input,20,8192);

                            if(ret != true){
                                break;
                            }

                            sizeRange.dtMaxSize = input;

                            returnStatus = sensor.setSizeRange(timeout,sizeRange);

                            if(returnStatus != B5T_STATUS_SUCCESS){
                                cout << "Failed to set detection size with error code : " << returnStatus << endl;
                                break;
                            }

                            cout << "Detection size set successfully." << endl;

                            break;

                        case 7: // Get face angle

                            returnStatus = sensor.getFaceDetectionAngle(timeout,pose,angle);

                            if(returnStatus != B5T_STATUS_SUCCESS){
                                cout << "Failed to get Face Detetion Angle with error code : " << returnStatus << endl;
                                break;
                            }

                            if(pose == 0) cout << "Face Detection pose is : 30 degree" << endl;
                            else if(pose == 1) cout << "Face Detection pose is : 60 degree" << endl;
                            else if(pose == 2) cout << "Face Detection pose is : 90 degree" << endl;
                            else cout << "Invalid Face Detection pose." << endl;

                            if(angle == 0) cout << "Face Detection angle is : 15 degree" << endl;
                            else if(angle == 1) cout << "Face Detection angle is : 45 degre" << endl;
                            else cout << "Invalid Face Detection angle." << endl;

                            break;
                        case 8: // Set face angle

                            //Pose
                            cout << "*************************************************************" << endl;
                            cout << "*************************************************************" << endl;
                            cout << "                          Pose                               " << endl;
                            cout << "*************************************************************" << endl;
                            cout << "*************************************************************" << endl;
                            cout << "1. 30 degree                                                 " << endl;
                            cout << "2. 60 degree                                                 " << endl;
                            cout << "3. 90 degree                                                 " << endl;
                            cout << "*************************************************************" << endl;

                            ret = takeSelectedInput("Select from the list of Pose : ",input,1,3);

                            if(ret != true){
                                break;
                            }

                            pose = (input - 1);

                            //Angle
                            cout << "*************************************************************" << endl;
                            cout << "*************************************************************" << endl;
                            cout << "                          Angle                              " << endl;
                            cout << "*************************************************************" << endl;
                            cout << "*************************************************************" << endl;
                            cout << "1. 15 degree                                                 " << endl;
                            cout << "2. 45 degree                                                 " << endl;
                            cout << "*************************************************************" << endl;

                            ret = takeSelectedInput("Select from the list of Angle : ",input,1,2);

                            if(ret != true){
                                break;
                            }

                            angle = (input - 1);

                            returnStatus = sensor.setFaceDetectionAngle(timeout,pose,angle);

                            if(returnStatus != B5T_STATUS_SUCCESS){
                                cout << "Failed to set face detection angle with error code : " << returnStatus << endl;
                                break;
                            }

                            cout << "Face Detection Angle set Successfully." << endl;

                            break;

                        case 9: // Set uart forwarding rate

                             cout << "*************************************************************" << endl;
                             cout << "*************************************************************" << endl;
                             cout << "                        BaudRates                            " << endl;
                             cout << "*************************************************************" << endl;
                             cout << "*************************************************************" << endl;
                             cout << "1. 9600                                                      " << endl;
                             cout << "2. 38400                                                     " << endl;
                             cout << "3. 115200                                                    " << endl;
                             cout << "4. 230400                                                    " << endl;
                             cout << "5. 460800                                                    " << endl;
                             cout << "6. 921600                                                    " << endl;
                             cout << "*************************************************************" << endl;

                             ret = takeSelectedInput("Select from the list of BaudRates : ",input,1,6);

                             if(ret != true){
                                 break;
                             }

                             switch(input){

                                 case 1: // 9600
                                     baudRate = HVC_UART_BAUD_RATE_9600;
                                     break;

                                 case 2: // 38400
                                     baudRate = HVC_UART_BAUD_RATE_38400;
                                     break;

                                 case 3: // 115200
                                     baudRate = HVC_UART_BAUD_RATE_115200;
                                     break;

                                 case 4: // 230400
                                     baudRate = HVC_UART_BAUD_RATE_230400;
                                     break;

                                 case 5: // 460800
                                     baudRate = HVC_UART_BAUD_RATE_460800;
                                     break;

                                 case 6: // 921600
                                     baudRate = HVC_UART_BAUD_RATE_921600;
                                     break;

                                 default:
                                     cout << "Invalid choice. Please select from 1 to 6." << endl;

                             }//end input switch set baudrate

                             returnStatus = sensor.setBaudRate(timeout,baudRate);

                             if(returnStatus != B5T_STATUS_SUCCESS){
                                 cout << "Failed to set BaudRate with error code : " << returnStatus << endl;
                                 break;
                             }

                            cout << "BaudRate " << baudRate << " set successfully." << endl;

                            break;

                        case 10: // Get stb library status

                            returnStatus = sensor.getSTBStatus(stbStatus);

                            if(returnStatus != B5T_STATUS_SUCCESS){
                                cout << "Failed to get stablization library status with error code : " << returnStatus << endl;
                                break;
                            }

                            if(stbStatus == 0){
                                cout << "STB Library status is : OFF" << endl;
                            }else if(stbStatus){
                                cout << "STB Library status is : ON" << endl;
                            }

                            break;

                        case 11: // Set stb library status
                             cout << "*************************************************************" << endl;
                             cout << "*************************************************************" << endl;
                             cout << "                   STB Library Status                        " << endl;
                             cout << "*************************************************************" << endl;
                             cout << "*************************************************************" << endl;
                             cout << "1. Body Tracking                                             " << endl;
                             cout << "2. Face Tracking                                             " << endl;
                             cout << "3. Face Direction                                            " << endl;
                             cout << "4. Age Estimation                                            " << endl;
                             cout << "5. Gender Estimation                                         " << endl;
                             cout << "6. Gaze Estimation                                           " << endl;
                             cout << "7. Blink Estimation                                          " << endl;
                             cout << "8. Expression Estimation                                     " << endl;
                             cout << "9. Face Recognition                                          " << endl;
                             cout << "10. All                                                      " << endl;
                             cout << "11. Disable                                                  " << endl;
                             cout << "*************************************************************" << endl;
                             ret = takeSelectedMultiChoiceInput("select values separated by spaces if multi choice : ",digitValues,1,11);

                             if(ret != true){
                                 break;
                             }
                             stbStatus = 0;
                             for(i=0;i < digitValues.size();i++){
#if DEBUG_PRINT
                                 std::cout << "DEBUG: Processing values " << digitValues[i] << std::endl;
#endif
                                 switch(digitValues[i]){
                                     case 1:  // Body Tracking
                                         stbStatus |= STB_FUNC_BD;
                                         break;
                                     case 2: // Face Tracking
                                         stbStatus |= STB_FUNC_DT;
                                         break;
                                     case 3: // Face Direction
                                         stbStatus |= STB_FUNC_PT;
                                         break;
                                     case 4: // Age Estimation
                                         stbStatus |= STB_FUNC_AG;
                                         break;
                                     case 5: // Gender Estimation
                                         stbStatus |= STB_FUNC_GN;
                                         break;
                                     case 6: // Gaze Estimation
                                         stbStatus |= STB_FUNC_GZ;
                                         break;
                                     case 7: // Blink Estimation
                                         stbStatus |= STB_FUNC_BL;
                                         break;
                                     case 8: // Expression Estimation
                                         stbStatus |= STB_FUNC_EX;
                                         break;
                                     case 9: // Face Recognition
                                         stbStatus |= STB_FUNC_FR;
                                         break;
                                     case 10: // All
                                         stbStatus |= STB_FUNC_BD | STB_FUNC_DT | STB_FUNC_PT | STB_FUNC_AG | STB_FUNC_GN | STB_FUNC_GZ | STB_FUNC_BL | STB_FUNC_EX | STB_FUNC_FR;
                                         break;
                                     case 11: // Disable
                                         stbStatus = 0;
                                         break;
                                     default:
                                        cout << "Invalid choice. Please select from 1 to 11." << endl;
                                        break;
                                   }//end input switch set baudrate
                             }//End of for loop
                             returnStatus = sensor.setSTBStatus(stbStatus);

                             if(returnStatus != B5T_STATUS_SUCCESS){
                                 cout << "Failed to set STB library status with error code : " << returnStatus << endl;
                                 break;
                             }

                            cout << "STB library status set successfully." << endl;
                            break;
                            case 12://  Get stb version
                                returnStatus = sensor.stbGetVersion(&majorVersion, &minorVersion);
                                if(returnStatus != B5T_STATUS_SUCCESS){
                                    break;
                                }
                                cout << "STB version:" << unsigned(majorVersion) << "." << unsigned(minorVersion) << endl;
                                break;
                            case 13://  Set stb maximum retry count(TR)
                                ret = takeDigitInput("Enter value of maximum retry count: ", maxRetryCount);
                                if(ret != true){
                                    break;
                                }
                                returnStatus = sensor.stbSetTrRetryCount(maxRetryCount);
                                if(returnStatus != B5T_STATUS_SUCCESS){
                                    cout << "Failed to set stb maximum retry count(TR)" << endl;
                                    break;
                                }
                                cout << "Stb maximum retry count(TR) set successfully" << endl;
                                break;
                            case 14://  Get stb maximum retry count(TR)
                                returnStatus = sensor.stbGetTrRetryCount(&maxRetryCount);
                                if(returnStatus != B5T_STATUS_SUCCESS){
                                    cout << "Failed to get stb maximum retry count(TR)" << endl;
                                    break;
                                }
                                cout << "Stb maximum retry count(TR) = " << maxRetryCount << endl;
                                break;
                            case 15://  Set stb rectangle steadiness parameter(TR)
                                ret = takeDigitInput("Enter value of Rectangle position steadiness parameter: ", posSteadinessParam);
                                if(ret != true){
                                    break;
                                }
                                ret = takeDigitInput("Enter value of Rectangle size steadiness parameter: ", sizeSteadinessParam);
                                if(ret != true){
                                    break;
                                }
                                returnStatus = sensor.stbSetTrSteadinessParam(posSteadinessParam,sizeSteadinessParam);
                                if(returnStatus != B5T_STATUS_SUCCESS){
                                    cout << "Failed to set stb rectangle steadiness parameter(TR)" << endl;
                                    break;
                                }
                                cout << "Stb rectangle steadiness parameter(TR) set successfully." << endl;
                                break;
                            case 16://  Get stb rectangle steadiness parameter(TR)
                                returnStatus = sensor.stbGetTrSteadinessParam(&posSteadinessParam, &sizeSteadinessParam);
                                if(returnStatus != B5T_STATUS_SUCCESS){
                                    cout << "Failed to get stb rectangle steadiness parameter(TR)" << endl;
                                    break;
                                }
                                cout << "Rectangle position steadiness = " << posSteadinessParam << endl;
                                cout << "Rectangle size steadiness = " << sizeSteadinessParam << endl;
                                break;
                            case 17://  Set stb estimation result stabilizing threshold value(PE)
                                ret = takeDigitInput("Enter threshold value : ", thresholdValue);
                                if(ret != true){
                                    break;
                                }
                                returnStatus = sensor.stbSetPeThresholdUse(thresholdValue);
                                if(returnStatus != B5T_STATUS_SUCCESS){
                                    cout << "Failed to set stb estimation result stabilizing threshold value(PE)" << endl;
                                    break;
                                }
                                cout << "Stb estimation result stabilizing threshold value(PE) set successfully." << endl;
                                break;
                            case 18://  Get stb estimation result stabilizing threshold value(PE)
                                returnStatus = sensor.stbGetPeThresholdUse(&thresholdValue);
                                if(returnStatus != B5T_STATUS_SUCCESS){
                                    cout << "Failed to get stb estimation result stabilizing threshold value(PE)" << endl;
                                    break;
                                }
                                cout << "Stb estimation result stabilizing threshold value(PE) =" << thresholdValue;
                                break;
                            case 19://  Set stb estimation result stabilizing angle(PE)
                                ret = takeDigitInput("Enter Minimum up-down angle of face: ",udAngleMin);
                                if(ret != true){
                                    break;
                                }
                                ret = takeDigitInput("Enter Maximum up-down angle of face: ",udAngleMax);
                                if(ret != true){
                                    break;
                                }
                                ret = takeDigitInput("Enter Minimum left-right angle of face: ",lrAngleMin);
                                if(ret != true){
                                    break;
                                }
                                ret = takeDigitInput("Enter Maximum left-right angle of face: ",lrAngleMax);
                                if(ret != true){
                                    break;
                                }
                                returnStatus = sensor.stbSetPeAngleUse(udAngleMin, udAngleMax, lrAngleMin, lrAngleMax);
                                if(returnStatus != B5T_STATUS_SUCCESS){
                                    cout << "Failed to set stb estimation result stabilizing angle(PE)" << endl;
                                    break;
                                }
                                cout << "Stb estimation result stabilizing angle(PE) set successfully" << endl;
                                break;
                            case 20://  Get stb estimation result stabilizing angle(PE)
                                returnStatus = sensor.stbGetPeAngleUse(&udAngleMin,&udAngleMax, &lrAngleMin, &lrAngleMax);
                                if(returnStatus != B5T_STATUS_SUCCESS){
                                    cout << "Failed to get stb estimation result stabilizing angle(PE)" << endl;
                                    break;
                                }
                                cout << "Minimum up-down angle of face = "  << udAngleMin << endl;
                                cout << "Maximum up-down angle of face = "  << udAngleMax << endl;
                                cout << "Minimum left-right angle of face = "  << lrAngleMin << endl;
                                cout << "Maximum left-right angle of face = "   << lrAngleMax << endl;
                                break;
                            case 21://  Set stb age/gender estimation complete frame count(PE)
                                ret = takeDigitInput("Enter frame count : ",frameCount);
                                if(ret != true){
                                    break;
                                }
                                returnStatus = sensor.stbSetPeCompleteFrameCount(frameCount);
                                if(returnStatus != B5T_STATUS_SUCCESS){
                                    cout << "Failed to set stb age/gender estimation complete frame count(PE)" << endl;
                                    break;
                                }
                                cout << "Stb age/gender estimation complete frame count(PE) set successfully." << endl;
                                break;
                            case 22://  Get stb age/gender estimation complete frame count(PE)
                                returnStatus = sensor.stbGetPeCompleteFrameCount(&frameCount);
                                if(returnStatus != B5T_STATUS_SUCCESS){
                                    cout << "Failed to get stb age/gender estimation complete frame count(PE)" << endl;
                                    break;
                                }
                                cout << "Stb age/gender estimation complete frame count(PE) = " << frameCount << endl;
                                break;
                            case 23://  Set stb recognition stabilizing threshold value(FR)
                                ret = takeDigitInput("Enter threshold value : ",thresholdValue);
                                if(ret != true){
                                    break;
                                }
                                returnStatus = sensor.stbSetFrThresholdUse(thresholdValue);
                                if(returnStatus != B5T_STATUS_SUCCESS){
                                    cout << "Failed to set stb recognition stabilizing threshold value(FR)" << endl;
                                    break;
                                }
                                cout << "Stb recognition stabilizing threshold value(FR) set successfully." << endl;
                                break;
                            case 24://  Get stb recognition stabilizing threshold value(FR)
                                returnStatus = sensor.stbGetFrThresholdUse(&thresholdValue);
                                if(returnStatus != B5T_STATUS_SUCCESS){
                                    cout << "Failed to get stb recognition stabilizing threshold value(FR)" << endl;
                                    break;
                                }
                                cout << "Threshold value = " << thresholdValue << endl;
                                break;
                            case 25://  Set stb recognition stabilizing angle(FR)
                                ret = takeDigitInput("Enter Minimum up-down angle of face: ",udAngleMin);
                                if(ret != true){
                                    break;
                                }
                                ret = takeDigitInput("Enter Maximum up-down angle of face: ",udAngleMax);
                                if(ret != true){
                                    break;
                                }
                                ret = takeDigitInput("Enter Minimum left-right angle of face: ",lrAngleMin);
                                if(ret != true){
                                    break;
                                }
                                ret = takeDigitInput("Enter Maximum left-right angle of face: ",lrAngleMax);
                                if(ret != true){
                                    break;
                                }
                                returnStatus = sensor.stbSetFrAngleUse(udAngleMin, udAngleMax, lrAngleMin, lrAngleMax);
                                if(returnStatus != B5T_STATUS_SUCCESS){
                                    cout << "Failed to set stb recognition stabilizing angle(FR)" << endl;
                                    break;
                                }
                                cout << "Stb recognition stabilizing angle(FR) set successfully." << endl;
                                break;
                            case 26://  Get stb recognition stabilizing angle(FR)
                                returnStatus = sensor.stbGetFrAngleUse(&udAngleMin,&udAngleMax, &lrAngleMin, &lrAngleMax);
                                if(returnStatus != B5T_STATUS_SUCCESS){
                                    cout << "Failed to get stb estimation result stabilizing angle(PE)" << endl;
                                    break;
                                }
                                cout << "Minimum up-down angle of face = "  << udAngleMin << endl;
                                cout << "Maximum up-down angle of face = "  << udAngleMax << endl;
                                cout << "Minimum left-right angle of face = "  << lrAngleMin << endl;
                                cout << "Maximum left-right angle of face = "   << lrAngleMax << endl;
                                break;
                            case 27://  Set stb recognition stabilizing complete frame count(FR)
                                ret = takeDigitInput("Enter frame count : ",frameCount);
                                if(ret != true){
                                    break;
                                }
                                returnStatus = sensor.stbSetFrCompleteFrameCount(frameCount);
                                if(returnStatus != B5T_STATUS_SUCCESS){
                                    cout << "Failed to set stb age/gender estimation complete frame count(PE)" << endl;
                                    break;
                                }
                                cout << "Stb recognition stabilizing complete frame count(FR) set successfully." << endl;
                                break;
                            case 28://  Get stb recognition stabilizing complete frame count(FR)
                                returnStatus = sensor.stbGetFrCompleteFrameCount(&frameCount);
                                if(returnStatus != B5T_STATUS_SUCCESS){
                                    cout << "Failed to get stb recognition stabilizing complete frame count(FR)" << endl;
                                    break;
                                }
                                cout << "Frame count = " << frameCount << endl;
                                break;
                            case 29://  Set stb recognition minimum account ratio(FR)
                                ret = takeDigitInput("Enter minimum account ratio : ", minRatio);
                                if(ret != true){
                                    break;
                                }
                                returnStatus = sensor.stbSetFrMinRatio(minRatio);
                                if(returnStatus != B5T_STATUS_SUCCESS){
                                    cout << "Failed to set stb recognition minimum account ratio(FR)" << endl;
                                    break;
                                }
                                cout << "Stb recognition minimum account ratio(FR) set successfully" << endl;
                                break;
                            case 30://  Get stb recognition minimum account ratio(FR)
                                returnStatus = sensor.stbGetFrMinRatio(&minRatio);
                                if(returnStatus != B5T_STATUS_SUCCESS){
                                    cout << "Failed to get stb recognition minimum account ratio(FR)" << endl;
                                    break;
                                }
                                cout << "Account ratio = " << minRatio << endl;
                                break;
                            case 31://  Clear stb frame result
                                returnStatus = sensor.stbClear();
                                if(returnStatus != B5T_STATUS_SUCCESS){
                                    cout << "Failed to clear stb frame result" << endl;
                                    break;
                                }
                                cout << "Stb frame result cleared successfully." << endl;
                                break;
                    }//end sub_choice switch
		    if(ret == true){
                       takeEnterInput();
		    }
            }// end choice switch
        }//End of for

    if(displaySamples){
        cout << "************************************************************\r\n";
        cout << "Please enter display time in ms \r\n";
        cout << "************************************************************\r\n";

        try{
            isCinWaitState = 1;
            // Update sleep time variable
            cin.clear(); // reset badbit
            cin.exceptions(std::ios_base::badbit);
            cin >> sleep_time;
        }catch (std::ios_base::failure& fail){
#if DEBUG_PRINT
            std::cout << "Exception called display time" << std::endl;
#endif
        }
        isCinWaitState = 0;
	//exitFlag = 0;
        //Check STB current status
        stbStatus = 0;
        returnStatus = sensor.getSTBStatus(stbStatus);
        if((stbStatus & STB_FUNC_DT) && (stbStatus & STB_FUNC_PT)){
            stbStatus = stbStatus | STB_FUNC_AG | STB_FUNC_GN | STB_FUNC_GZ | STB_FUNC_BL | STB_FUNC_EX | STB_FUNC_FR;
        }
        if(returnStatus != B5T_STATUS_SUCCESS){
                cout << "Failed to get stablization library status with error code : " << returnStatus << endl;
                //break;
        }
#if DEBUG_PRINT
        std::cout << "DEBUG: stbStatus = " << stbStatus  << std::endl;
#endif
        while(exitFlag < 2){
            if(exitFlag == 1){
                try{
                    isCinWaitState = 1;
                    cin.exceptions(std::ios_base::badbit);
                    std::cin.ignore(1000,'\n');
    		        cin.clear();
                    cout << "Press enter to continue Main menu or press ctrl + c to exit from application." << endl;
                    cin.get();
                    exitFlag = 0;
                }catch (std::ios_base::failure& fail){
#if DEBUG_PRINT
                    std::cout << "Exception called" << std::endl;
#endif
                }
                isCinWaitState = 0;
                break;
            }
            cout << "*************************************************************" << endl;
            cout << "Detecting user. Please pay attention to camera, and wait for sometime." << endl;
            cout << "*************************************************************" << endl;
            cout << "Timestamp : ";
            // Get date and time
            gettimeofday(&value, NULL);
            curtime=value.tv_sec;
            strftime(dateAndTime,30,"[%F %T.",localtime(&curtime));
            cout << dateAndTime << std::setfill('0') << std::setw(3) << std::dec << value.tv_usec/1000 << "] " << endl;
            cout << "*************************************************************" << endl;

            // Call exececute detection API based on STB init status
            if(stbStatus == 0){
                returnStatus = sensor.executeDetection(timeout, execFlag, imageNum, hvcResult);
            }
            else{
                returnStatus = sensor.executeDetection(timeout, execFlag, imageNum, hvcResult, &outSTBFaceCount, &outSTBFaceResult, &outSTBBodyCount, &outSTBBodyResult);
                INT32 i;
                // Assign STB results
                if(returnStatus == B5T_STATUS_SUCCESS){
#if DEBUG_PRINT
                    std::cout << "DEBUG: outSTBBodyCount= " << outSTBBodyCount  << std::endl;
                    std::cout << "DEBUG: outSTBFaceCount= " << outSTBFaceCount << std::endl;
#endif
                    if((execFlag & HVC_ACTIV_BODY_DETECTION) && (stbStatus & STB_FUNC_BD)){
                        for(i = 0; i < outSTBBodyCount; i++ ){
                            if ( hvcResult.bdResult.num <= i ) break;
                            nIndex = outSTBBodyResult[i].nDetectID;
                            hvcResult.bdResult.bdResult[nIndex].posX = (short)outSTBBodyResult[i].center.x;
                            hvcResult.bdResult.bdResult[nIndex].posY = (short)outSTBBodyResult[i].center.y;
                            hvcResult.bdResult.bdResult[nIndex].size = outSTBBodyResult[i].nSize;
                        }
                    }
                    for(i = 0; i < outSTBFaceCount; i++ ){
                        if ( hvcResult.fdResult.num <= i ) break;
                        nIndex = outSTBFaceResult[i].nDetectID;
#if DEBUG_PRINT
                        std::cout << "DEBUG: outSTBFaceResult[i].nDetectID= " << outSTBFaceResult[i].nDetectID << std::endl;
                        std::cout << "DEBUG: outSTBFaceResult[i].nTrackingID= " << outSTBFaceResult[i].nTrackingID << std::endl;
#endif
                        if((execFlag & HVC_ACTIV_FACE_DETECTION) && (stbStatus & STB_FUNC_DT)){
#if DEBUG_PRINT
                            std::cout << "outSTBFaceResult[i].center.x = " << outSTBFaceResult[i].center.x<< std::endl;
                            std::cout << "outSTBFaceResult[i].center.y = " << outSTBFaceResult[i].center.y<< std::endl;
                            std::cout << "outSTBFaceResult[i].nSize = " << outSTBFaceResult[i].nSize << std::endl;
#endif
                            hvcResult.fdResult.fcResult[nIndex].dtResult.posX = (short)outSTBFaceResult[i].center.x;
                            hvcResult.fdResult.fcResult[nIndex].dtResult.posY = (short)outSTBFaceResult[i].center.y;
                            hvcResult.fdResult.fcResult[nIndex].dtResult.size = outSTBFaceResult[i].nSize;
                        }
                        if((execFlag & HVC_ACTIV_FACE_DIRECTION) && (stbStatus & STB_FUNC_PT) && (outSTBFaceResult[i].direction.status >= STB_STATUS_COMPLETE)){
#if DEBUG_PRINT
                            std::cout << "outSTBFaceResult[i].direction.yaw = "<< outSTBFaceResult[i].direction.yaw << std::endl;
                            std::cout << "outSTBFaceResult[i].direction.pitch = "<< outSTBFaceResult[i].direction.pitch << std::endl;
                            std::cout << "outSTBFaceResult[i].direction.roll = "<< outSTBFaceResult[i].direction.roll << std::endl;
                            std::cout << "outSTBFaceResult[i].direction.confidence = "<< outSTBFaceResult[i].direction.conf << std::endl;
#endif
                            hvcResult.fdResult.fcResult[nIndex].dirResult.yaw = outSTBFaceResult[i].direction.yaw;
                            hvcResult.fdResult.fcResult[nIndex].dirResult.pitch = outSTBFaceResult[i].direction.pitch;
                            hvcResult.fdResult.fcResult[nIndex].dirResult.roll = outSTBFaceResult[i].direction.roll;
                            hvcResult.fdResult.fcResult[nIndex].dirResult.confidence = outSTBFaceResult[i].direction.conf;
                        }
                        if((execFlag & HVC_ACTIV_AGE_ESTIMATION) && (stbStatus & STB_FUNC_AG) && (outSTBFaceResult[i].age.status >= STB_STATUS_COMPLETE)){
                            hvcResult.fdResult.fcResult[nIndex].ageResult.confidence += 10000; // During
                            if ( outSTBFaceResult[i].age.status >= STB_STATUS_COMPLETE ) {
#if DEBUG_PRINT
                                std::cout << "outSTBFaceResult[i].age.value = " << outSTBFaceResult[i].age.value << std::endl;
#endif
                                hvcResult.fdResult.fcResult[nIndex].ageResult.age = outSTBFaceResult[i].age.value;
                                hvcResult.fdResult.fcResult[nIndex].ageResult.confidence += 10000; // Complete
                            }
                        }
                        if((execFlag & HVC_ACTIV_GENDER_ESTIMATION) && (stbStatus & STB_FUNC_GZ) && (outSTBFaceResult[i].gender.status >= STB_STATUS_COMPLETE)){
                            hvcResult.fdResult.fcResult[nIndex].genderResult.confidence += 10000; // During
                            if ( outSTBFaceResult[i].gender.status >= STB_STATUS_COMPLETE ) {
#if DEBUG_PRINT
                                std::cout << "outSTBFaceResult[i].gender.value"<< outSTBFaceResult[i].gender.value <<std::endl;
#endif
                                hvcResult.fdResult.fcResult[nIndex].genderResult.gender = outSTBFaceResult[i].gender.value;
                                hvcResult.fdResult.fcResult[nIndex].genderResult.confidence += 10000; // Complete
                            }
                        }
#if DEBUG_PRINT
                            std::cout << "outSTBFaceResult[i].gaze.LR = " << outSTBFaceResult[i].gaze.LR<< std::endl;
                            std::cout << "outSTBFaceResult[i].gaze.UD = " << outSTBFaceResult[i].gaze.UD<< std::endl;
#endif
                        if((execFlag & HVC_ACTIV_GAZE_ESTIMATION) && (stbStatus & STB_FUNC_GZ) && (outSTBFaceResult[i].gaze.status >= STB_STATUS_COMPLETE)){
                            hvcResult.fdResult.fcResult[nIndex].gazeResult.gazeLR = outSTBFaceResult[i].gaze.LR;
                            hvcResult.fdResult.fcResult[nIndex].gazeResult.gazeUD = outSTBFaceResult[i].gaze.UD;
                        }
#if DEBUG_PRINT
                            std::cout << "outSTBFaceResult[i].blink.ratioL = " << outSTBFaceResult[i].blink.ratioL << std::endl;
                            std::cout << "outSTBFaceResult[i].blink.ratioR = " << outSTBFaceResult[i].blink.ratioR << std::endl;
#endif
                        if((execFlag & HVC_ACTIV_BLINK_ESTIMATION) && (stbStatus & STB_FUNC_BL) && (outSTBFaceResult[i].blink.status >= STB_STATUS_COMPLETE) ){
                            hvcResult.fdResult.fcResult[nIndex].blinkResult.ratioL = outSTBFaceResult[i].blink.ratioL;
                            hvcResult.fdResult.fcResult[nIndex].blinkResult.ratioR = outSTBFaceResult[i].blink.ratioR;
                        }
#if DEBUG_PRINT
                            std::cout << "DEBUG: outSTBFaceResult[i].expression.conf= " << outSTBFaceResult[i].expression.conf  << std::endl;
                            std::cout << "DEBUG: outSTBFaceResult[i].expression.value= " << outSTBFaceResult[i].expression.value  << std::endl;
                            std::cout << "DEBUG: outSTBFaceResult[i].expression.status= " << outSTBFaceResult[i].expression.status  << std::endl;
#endif
                        if((execFlag & HVC_ACTIV_EXPRESSION_ESTIMATION) && (stbStatus & STB_FUNC_EX) && (outSTBFaceResult[i].expression.status >= STB_STATUS_COMPLETE)){
                            hvcResult.fdResult.fcResult[nIndex].expressionResult.topExpression = outSTBFaceResult[i].expression.value;
                            //hvcResult.fdResult.fcResult[nIndex].expressionResult.score[0] = -1;
                        }
#if DEBUG_PRINT
                            std::cout << "DEBUG: outSTBFaceResult[i].recognition.value=" << outSTBFaceResult[i].recognition.value << std::endl;
                            std::cout << "DEBUG: outSTBFaceResult[i].recognition.status=" << outSTBFaceResult[i].recognition.status << std::endl;
                            std::cout << "DEBUG: outSTBFaceResult[i].recognition.conf=" << outSTBFaceResult[i].recognition.conf << std::endl;

#endif
                        if((execFlag & HVC_ACTIV_FACE_RECOGNITION) && (stbStatus & STB_FUNC_FR) && (outSTBFaceResult[i].recognition.status >= STB_STATUS_COMPLETE)){
                            hvcResult.fdResult.fcResult[nIndex].recognitionResult.uid = outSTBFaceResult[i].recognition.value;
                            hvcResult.fdResult.fcResult[nIndex].recognitionResult.confidence = outSTBFaceResult[i].recognition.conf;
                        }
                    }
                }
            }

            if(returnStatus != B5T_STATUS_SUCCESS){
                    cout << "Failed to execute detection with error code : " << returnStatus << endl;
                    continue;
            }
            // Check received executed flag
            if(hvcResult.executedFunc & HVC_ACTIV_BODY_DETECTION){

                // Print detected body result count
                cout << "Body result count : " << (unsigned) hvcResult.bdResult.num << endl;

                // Print detected body result parameter
                for(loop = 0; loop < hvcResult.bdResult.num; loop++){
                    cout << "*************************************************************" << endl;
                    cout << "   Index : " << loop << endl;
                    cout << "*************************************************************" << endl;
                    if(stbStatus & STB_FUNC_BD){
                        cout << "   Tracking Id : " << outSTBBodyResult[loop].nTrackingID << endl;
                    }
                    cout << "   X : " << hvcResult.bdResult.bdResult[loop].posX << endl;
                    cout << "   Y : " << hvcResult.bdResult.bdResult[loop].posY << endl;
                    cout << "   Size : " << hvcResult.bdResult.bdResult[loop].size << endl;
                    cout << "   Confidence : " << hvcResult.bdResult.bdResult[loop].confidence << endl;
                }
            }
            if(hvcResult.executedFunc & HVC_ACTIV_HAND_DETECTION){

                // Print detected hand result count
                cout << "Hand result count : " << (unsigned) hvcResult.hdResult.num << endl;

                // Print detected hand result parameter
                for(loop = 0; loop < hvcResult.hdResult.num; loop++){
                    cout << "*************************************************************" << endl;
                    cout << "   Index : " << loop << endl;
                    cout << "*************************************************************" << endl;
                    cout << "   X : " << hvcResult.hdResult.hdResult[loop].posX << endl;
                    cout << "   Y : " << hvcResult.hdResult.hdResult[loop].posY << endl;
                    cout << "   Size : " << hvcResult.hdResult.hdResult[loop].size << endl;
                    cout << "   Confidence : " << hvcResult.hdResult.hdResult[loop].confidence << endl;
                }
            }
            if(hvcResult.executedFunc & HVC_ACTIV_FACE_DETECTION){
                cout << "Face result count : " << (unsigned) hvcResult.fdResult.num << endl;
            }
            for(loop = 0; loop < hvcResult.fdResult.num; loop++){
                cout << "*************************************************************" << endl;
                cout << "   Index : " << loop << endl;
                cout << "*************************************************************" << endl;

                if(stbStatus){
                    ret = getSTBFaceIndex(outSTBFaceResult[loop].nDetectID, outSTBFaceResult, outSTBFaceCount, nIndex);
#if DEBUG_PRINT
                    cout << "loop : " << loop << endl;
                    cout << "nIndex : " << nIndex << endl;
                    cout << "outSTBFaceResult[loop].nDetectID : " << outSTBFaceResult[loop].nDetectID << endl;
#endif
                    if(ret != true){
                        continue;
                    }
                }

                if(hvcResult.executedFunc & HVC_ACTIV_FACE_DETECTION){
                    // Print detected face result count
                    // Print detected face result parameter
                    cout << "   Face detection:" << endl;
                    if(stbStatus & STB_FUNC_DT){
                        cout << "       Tracking Id : " << outSTBFaceResult[nIndex].nTrackingID << endl;
                    }
                    cout << "       X : " << hvcResult.fdResult.fcResult[loop].dtResult.posX << endl;
                    cout << "       Y : " << hvcResult.fdResult.fcResult[loop].dtResult.posY << endl;
                    cout << "       Size : " << hvcResult.fdResult.fcResult[loop].dtResult.size << endl;
                    cout << "       Confidence : " << hvcResult.fdResult.fcResult[loop].dtResult.confidence << endl;
                }
                if(hvcResult.executedFunc & HVC_ACTIV_AGE_ESTIMATION){
                    cout << "   Age estimation:" << endl;
                    // Print estimated age and related parameter
                    if(hvcResult.fdResult.fcResult[loop].ageResult.age == -128){
                        cout << "       Age Estimation not possible" << endl;
                    }
                    else{
                        if(stbStatus & STB_FUNC_AG){
                            cout << "       Status : " << printSTBStatus(outSTBFaceResult[nIndex].age.status) << endl;
                        }

                        cout << "       Estimated Age : " << hvcResult.fdResult.fcResult[loop].ageResult.age << endl;

                        if(stbStatus){

                            if(hvcResult.fdResult.fcResult[loop].ageResult.confidence >= 20000){
                                    cout << "       Confidence : " << hvcResult.fdResult.fcResult[loop].ageResult.confidence - 20000 << " (*)"<< endl;
                            }else if(hvcResult.fdResult.fcResult[loop].ageResult.confidence >= 10000){
                                    cout << "       Confidence : " << hvcResult.fdResult.fcResult[loop].ageResult.confidence - 10000 << " (-)"<< endl;
                            }else{
                                    cout << "       Confidence : " << hvcResult.fdResult.fcResult[loop].ageResult.confidence << " (x)"<<endl;
                            }
                        }
                        else{
                               cout << "       Confidence : " << hvcResult.fdResult.fcResult[loop].ageResult.confidence << endl;
                        }
                    }
                }
                if(hvcResult.executedFunc & HVC_ACTIV_FACE_DIRECTION){
                    // Print face direction related parameter
                    cout << "   Face direction:" << endl;
                    if(stbStatus & STB_FUNC_DT){
                        cout << "       Status : " << printSTBStatus(outSTBFaceResult[nIndex].direction.status) << endl;
                    }
                    cout << "       LR : " << hvcResult.fdResult.fcResult[loop].dirResult.yaw << endl;
                    cout << "       UD : " << hvcResult.fdResult.fcResult[loop].dirResult.pitch << endl;
                    cout << "       Roll : " << hvcResult.fdResult.fcResult[loop].dirResult.roll << endl;
                    cout << "       Confidence : " << hvcResult.fdResult.fcResult[loop].dirResult.confidence << endl;
                }
                if(hvcResult.executedFunc & HVC_ACTIV_GENDER_ESTIMATION){
                    // Print estimated age and related parameter
                    cout << "   Gender estimation:" << endl;
                    if(hvcResult.fdResult.fcResult[loop].genderResult.gender == -128){
                            cout << "       Gender estimation not possible." << endl;
                    }else{
                        if(stbStatus & STB_FUNC_GN){
                            cout << "       Status : " << printSTBStatus(outSTBFaceResult[nIndex].gender.status) << endl;
                        }

                        if(hvcResult.fdResult.fcResult[loop].genderResult.gender == 1){
                            cout << "       Gender : Male" << endl;
                        }else{
                            cout << "       Gender : female" << endl;
                        }
                        if(stbStatus){
                            if(hvcResult.fdResult.fcResult[loop].genderResult.confidence >= 20000){
                                cout << "       Confidence : " << hvcResult.fdResult.fcResult[loop].genderResult.confidence - 20000 << " (*)"<< endl;
                            }else if(hvcResult.fdResult.fcResult[loop].genderResult.confidence >= 10000){
                                cout << "       Confidence : " << hvcResult.fdResult.fcResult[loop].genderResult.confidence - 10000 << " (-)" <<endl;
                            }else{
                                cout << "       Confidence : " << hvcResult.fdResult.fcResult[loop].genderResult.confidence << " (x)"<< endl;
                            }
                        }
                        else{
                           cout << "       Confidence : " << hvcResult.fdResult.fcResult[loop].genderResult.confidence << endl;
                        }
                    }
                }
                if(hvcResult.executedFunc & HVC_ACTIV_GAZE_ESTIMATION){
                    cout << "   Gaze estimation:" << endl;
                    if((hvcResult.fdResult.fcResult[loop].gazeResult.gazeLR == -128) ||
                                    (hvcResult.fdResult.fcResult[loop].gazeResult.gazeUD == -128)){
                        cout << "       Gaze estimation not possible" << endl;
                    }else{
                        if(stbStatus & STB_FUNC_GZ){
                        cout << "       Status : " << printSTBStatus(outSTBFaceResult[nIndex].gaze.status) << endl;
                        }
                        cout << "       LR : " << hvcResult.fdResult.fcResult[loop].gazeResult.gazeLR << endl;
                        cout << "       UD : " << hvcResult.fdResult.fcResult[loop].gazeResult.gazeUD << endl;
                    }
                }
                if(hvcResult.executedFunc & HVC_ACTIV_BLINK_ESTIMATION){
                    cout << "   Blink estimation:" << endl;
                    if((hvcResult.fdResult.fcResult[loop].blinkResult.ratioL == -128) ||
                                    (hvcResult.fdResult.fcResult[loop].blinkResult.ratioR == -128)){
                        cout << "       Blink estimation not possible" << endl;
                    }else{
                        if(stbStatus & STB_FUNC_BL){
                            cout << "       Status : " << printSTBStatus(outSTBFaceResult[nIndex].blink.status) << endl;
                        }
                        cout << "       Left : " << hvcResult.fdResult.fcResult[loop].blinkResult.ratioL << endl;
                        cout << "       Right : " << hvcResult.fdResult.fcResult[loop].blinkResult.ratioR << endl;
                    }
                }
                if(hvcResult.executedFunc & HVC_ACTIV_EXPRESSION_ESTIMATION){
                   // Print expression estimater related parameter
                   cout << "   Expression estimation:" << endl;

                   if((stbStatus & STB_FUNC_EX)){
                       cout << "       Status : " << printSTBStatus(outSTBFaceResult[nIndex].expression.status) << endl;
                       exEnum = (EXPRESSION)hvcResult.fdResult.fcResult[loop].expressionResult.topExpression;
                       if(exEnum == EX_NEUTRAL){
                           cout << "       Expression : NEUTRAL" << endl;
                       }else if(exEnum == EX_HAPPINESS){
                           cout << "       Expression : HAPPINESS" << endl;
                       }else if(exEnum == EX_SURPRISE){
                           cout << "       Expression : SURPRISE" << endl;
                       }else if(exEnum == EX_ANGER){
                           cout << "       Expression : ANGER" << endl;
                       }else if(exEnum == EX_SADNESS){
                           cout << "       Expression : SADNESS" << endl;
                       }else{
                           cout << "       Invalid Expression" << endl;
                       }
                    }
                    else if(hvcResult.fdResult.fcResult[loop].expressionResult.score[0] == -128){
                        cout << "       Expression estimation not possible" << endl;
                    }
                    else{
                        exEnum = (EXPRESSION)hvcResult.fdResult.fcResult[loop].expressionResult.topExpression;
                        if(exEnum == EX_NEUTRAL){
                                cout << "       Expression : NEUTRAL" << endl;
                        }else if(exEnum == EX_HAPPINESS){
                                cout << "       Expression : HAPPINESS" << endl;
                        }else if(exEnum == EX_SURPRISE){
                                cout << "       Expression : SURPRISE" << endl;
                        }else if(exEnum == EX_ANGER){
                                cout << "       Expression : ANGER" << endl;
                        }else if(exEnum == EX_SADNESS){
                                cout << "       Expression : SADNESS" << endl;
                        }else{
                                cout << "       Invalid Expression" << endl;
                        }
                        cout << "       Score : ";
                        cout << hvcResult.fdResult.fcResult[loop].expressionResult.score[0] << ", ";
                        cout << hvcResult.fdResult.fcResult[loop].expressionResult.score[1] << ", ";
                        cout << hvcResult.fdResult.fcResult[loop].expressionResult.score[2] << ", ";
                        cout << hvcResult.fdResult.fcResult[loop].expressionResult.score[3] << ", ";
                        cout << hvcResult.fdResult.fcResult[loop].expressionResult.score[4];
                        cout << endl;
                        cout << "       Degree : " << hvcResult.fdResult.fcResult[loop].expressionResult.degree << endl;
                    }
                }
                if(hvcResult.executedFunc & HVC_ACTIV_FACE_RECOGNITION){
                    // Print expression estimater related parameter
                    cout << "   Face recognition:" << endl;
                    if(hvcResult.fdResult.fcResult[loop].recognitionResult.uid == -128){
                        cout << "       Recognition not possible" << endl;
                    }else if(hvcResult.fdResult.fcResult[loop].recognitionResult.uid == -127){
                        cout << "       Not registered" << endl;
                    }else if(hvcResult.fdResult.fcResult[loop].recognitionResult.uid == -1){
                        cout << "       ID not available" << endl;
                    }
                    else{

                        if(stbStatus & STB_FUNC_FR){
                            cout << "       Status : " << printSTBStatus(outSTBFaceResult[nIndex].recognition.status) << endl;
                        }

                        cout << "       User ID : " << hvcResult.fdResult.fcResult[loop].recognitionResult.uid << endl;
                        if(stbStatus){

                            if (hvcResult.fdResult.fcResult[loop].recognitionResult.confidence >= 20000){
                                cout << "       Confidence : " << hvcResult.fdResult.fcResult[loop].recognitionResult.confidence - 20000 << endl;
                            }else if(hvcResult.fdResult.fcResult[loop].recognitionResult.confidence >= 10000){
                                cout << "       Confidence : " << hvcResult.fdResult.fcResult[loop].recognitionResult.confidence - 10000 << endl;
                            }else{
                                cout << "       Confidence : " << hvcResult.fdResult.fcResult[loop].recognitionResult.confidence << endl;
                            }
                        }else{
                            cout << "       Confidence : " << hvcResult.fdResult.fcResult[loop].recognitionResult.confidence << endl;
                        }
                    }
                 }
            }//face count for loop
            usleep(sleep_time);
        }//end while

	}//End of if displaySample
    }while((exitFlag < 2));

    return 0;

}//end main



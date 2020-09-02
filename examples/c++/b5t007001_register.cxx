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

/* omron sensor header */
#include "b5t007001.hpp"

/* Define */
#define CLEAR_SCREEN "\033[2J\033[1;1H"                   /* Clear screen */
//#define USE_LIBRARY_TIMEOUT                               /* Enable to use library timeout */

#define ADD_USER           1
#define MODIFY_DELETE_USER 2
#define LIST_USER          3
#define SETTINGS           4

#define UART_SETTING_TIMEOUT              1000            /* HVC setting command signal timeout period */
#define UART_DETECT_EXECUTE_TIMEOUT       6000            /* HVC Face Detection command signal timeout period */
#define UART_REGIST_EXECUTE_TIMEOUT       7000            /* HVC registration command signal timeout period */
#define LIBRARY_TIMEOUT                   -1              /* Library timeout */

using namespace upm;
using namespace std;

volatile sig_atomic_t flag = true;
UINT8 isCinWaitState = 0; //State for cin is waiting for input or not.

void
sig_handler(INT32 signum)
{
    // Check for Interrupt signal
    if (signum == SIGINT || signum == SIGTERM){
        cout << "exit" << endl;
        flag = false;
        if(isCinWaitState == 1){
           std::cin.setstate(std::ios_base::badbit);
        }
    }
}

UINT8
takeSelectedInput(STRING str, UINT32 &value, UINT32 startIndex, UINT32 endIndex){// Take input

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

            // Check start index and end index with input value
            if(value < startIndex || value > endIndex){
                cout << "*************************************************************" << endl;
                cout << "Invalid choice. Please select from " << std::dec << startIndex << " to " << endIndex << "." << endl;
                cout << "*************************************************************" << endl;
            }else{
                isCinWaitState = 0;
                return true;
            }

        }while((value < startIndex || value > endIndex) && flag);

    }catch (std::ios_base::failure& fail){

        isCinWaitState = 0;
        return false;
    }

    isCinWaitState = 0;
    return false;
}

INT32
main(INT32 argc, INT8** argv)
{

    signal(SIGINT, sig_handler);

    // Variable initialize
    UINT32 choice,sub_choice,input,sub_input;
    UINT32 loop,i,tmpDataId,tmplen;
    UINT8 ret;

    #ifdef USE_LIBRARY_TIMEOUT
    INT32 timeout = LIBRARY_TIMEOUT;
    #else
    INT32 timeout = UART_SETTING_TIMEOUT;
    #endif

    HVC_VERSION version;
    HVC_IMAGE image;
    B5T_STATUS_T returnStatus;

    HVC_UART_BAUD_RATE_T baudRate;
    UINT8 cameraAngle;
    HVC_THRESHOLD threshold;
    HVC_SIZERANGE sizeRange;
    INT32 pose, angle;

    UINT8 *pAlbumData = NULL;
    INT32 albumDataSize = 0;
    string inFileName;

    fstream pFile;

    vector<UINT8> userId;
    vector<UINT8> userDataId;

    // Print baudrates for selection
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

    ret = takeSelectedInput("Select from the list of BaudRates : ", input, 1,6);

    if(ret != true){
       return 0;
    }

    switch(input){// Baudrate variable update as per input

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
    string tmpstring((INT8*)version.string, sizeof(version.string));

    // Album data Allocatoion
    pAlbumData = (UINT8*)malloc(sizeof(UINT8) * (HVC_ALBUM_SIZE_MAX + 8));

    if (pAlbumData == NULL){
            cout << "Album data memeory allocation error." << endl;
            return 0;
    }

    while(flag)
    {
        cout << CLEAR_SCREEN;
        cout << "*************************************************************" << endl;
        cout << " HVC Version : " // Print HVC version
             << tmpstring
             << (unsigned) version.major   << "."
             << (unsigned) version.minor   << "."
             << (unsigned) version.relese  << "."
             << (unsigned) (version.revision[0]        +
                           (version.revision[1] << 8)  +
                           (version.revision[2] << 16) +
                           (version.revision[3] << 24))                         << endl;
        // Print main menu for selection
        cout << "*************************************************************" << endl;
        cout << "*************************************************************" << endl;
        cout << "                        MAIN MENU                            " << endl;
        cout << "*************************************************************" << endl;
        cout << "*************************************************************" << endl;
        cout << "1. Add user                                                  " << endl;
        cout << "2. Modify/Delete user                                        " << endl;
        cout << "3. List user                                                 " << endl;
        cout << "4. Settings                                                  " << endl;
        cout << "*************************************************************" << endl;

        ret = takeSelectedInput("Select from main menu : ", choice, 1, 4);

        if(ret != true){
            break;
        }

        switch(choice){

            case ADD_USER:

                // Get register user ids and data ids
                returnStatus = sensor.getRegisteredUsersInfo(timeout,userId,userDataId);

                if(returnStatus != B5T_STATUS_SUCCESS){
                    cout << "Failed to get registered user info with error code : " << returnStatus << endl;
                    break;
                }

                if(userId.size() > 0) // Check registered user ids size
                {
                    for(loop = 0; loop < userId.size(); loop++){
                        if(userId[loop] != loop){
                            break;
                        }
                    }
                }else{
                    // Init 0 for non-registered user application
                    loop = 0;
                }

                #ifndef USE_LIBRARY_TIMEOUT
                // Update timeout variable
                timeout = UART_REGIST_EXECUTE_TIMEOUT;
                #endif

                if(loop > 99){// Check total number of register user id
                    cout << "Maximum user id registration number reached." << endl;
                    break;
                }

                tmpDataId = 0;

                // Add user
                returnStatus =  sensor.addUser(timeout,loop,tmpDataId,image);

                #ifndef USE_LIBRARY_TIMEOUT
                // Update timeout variable
                timeout = UART_SETTING_TIMEOUT;
                #endif

                if(returnStatus != B5T_STATUS_SUCCESS){// Check return status
                    cout << "Failed to add user with error code : " << returnStatus << endl;
                    break;
                }

                // Print message with registered user id and data id
                cout << "User added successfully with user id : " << loop <<  " ,and data id : " << tmpDataId << "." << endl;

                break;

            case MODIFY_DELETE_USER:

                // Get register user ids and data ids
                returnStatus = sensor.getRegisteredUsersInfo(timeout,userId,userDataId);

                if(returnStatus != B5T_STATUS_SUCCESS){// Check return status
                    cout << "Failed to get Registered User Info with error code : " << returnStatus << endl;
                    cin.ignore();
                    break;
                }

                // Print for selection of modify and delete user
                cout << "*************************************************************" << endl;
                cout << "*************************************************************" << endl;
                cout << "                  Modify or Delete Users ID                  " << endl;
                cout << "*************************************************************" << endl;
                cout << "*************************************************************" << endl;
                cout << "1. Modify user                                               " << endl;
                cout << "2. Delete user                                               " << endl;
                cout << "*************************************************************" << endl;

                ret = takeSelectedInput("Select modify user or delete user : ",sub_choice,1,2);

                if(ret != true){
                    break;
                }

                if(sub_choice == 1){// Modify user

                    if(userId.size() > 0){// Check registered user ids size
                        for(loop = 0; loop < userId.size(); loop++){
                            cout << (loop+1) << "." << "User ID : " << unsigned(userId[loop]) << endl;
                        }
                    }else{
                        cout << "Users id list is empty. Please add user and try again." << endl;
                        break;
                    }

                    ret = takeSelectedInput("Select from the list of user ID : ",sub_choice,1,userId.size());

                    if(ret != true){
                        break;
                    }

                    // Print for selection of modify and delete data ids
                    cout << "*************************************************************" << endl;
                    cout << "*************************************************************" << endl;
                    cout << "                   Modify/Delete Data IDs                    " << endl;
                    cout << "*************************************************************" << endl;
                    cout << "*************************************************************" << endl;
                    cout << "1. Add/Modify Data ID                                        " << endl;
                    cout << "2. Delete Data ID                                            " << endl;
                    cout << "*************************************************************" << endl;

                    ret = takeSelectedInput("Select from the list : ",input,1,2);

                    if(ret != true){
                        break;
                    }

                    if(input == 1){

                        // Print registered data ids
                        cout << "*************************************************************" << endl;
                        cout << "*************************************************************" << endl;
                        cout << "                        Data IDs                             " << endl;
                        cout << "*************************************************************" << endl;
                        cout << "*************************************************************" << endl;
                        cout << "Registered Data IDs."                                          << endl;

                        tmpDataId = 0;
                        tmplen = 0;

                        for(loop=0, i=0x01; i < 0x400; loop++, i<<=1){// Check and print registered data ids

                            if ((userDataId[sub_choice-1] & i) == 0 ){
                                if(tmplen < __builtin_popcountll(userDataId[sub_choice-1])){
                                    tmpDataId++;
                                    continue;
                                }else{
                                    break;
                                }
                            }
                            cout << "Data ID : " << tmpDataId << endl;
                            tmplen++;
                            tmpDataId++;
                        }

                        // Check registration number
                        if ( tmpDataId >= 10 ){
                            cout << "Maximum data id registration number reached." << endl;
                            cin.ignore();
                            break;
                        }

                        // Print for input from user for add/modify data id
                        ret = takeSelectedInput("Select data id(0 to 9) for add/modify : ",input,0,9);

                        if(ret != true){
                            break;
                        }

                        // Check user input
                        if((input >= 0) && (input <= 9)){

                            #ifndef USE_LIBRARY_TIMEOUT
                            // Update timeout variable
                            timeout = UART_REGIST_EXECUTE_TIMEOUT;
                            #endif

                            // Add user
                            returnStatus =  sensor.addUser(timeout,userId[sub_choice-1],input,image);

                            #ifndef USE_LIBRARY_TIMEOUT
                            // Update timeout variable
                            timeout = UART_SETTING_TIMEOUT;
                            #endif

                            if(returnStatus != B5T_STATUS_SUCCESS){
                                cout << "Failed to modify user with error code : " << returnStatus << endl;
                                break;
                            }

                            cout << "Data id "<< input << " of user id " << (unsigned)userId[sub_choice-1] << " add/modify successfully." << endl;
                            break;
                        }

                    }else if(input == 2){//Delete Data Id

                        cout << "*************************************************************" << endl;
                        cout << "*************************************************************" << endl;
                        cout << "                        Data IDs                             " << endl;
                        cout << "*************************************************************" << endl;
                        cout << "*************************************************************" << endl;

                        tmpDataId = 0;
                        tmplen = 0;

                        for(loop=0,i=0x01; i < 0x400; loop++, i<<=1){// Check and print registered data ids

                            if ((userDataId[sub_choice-1] & i) == 0 ){
                                if(tmplen < __builtin_popcountll(userDataId[sub_choice-1])){
                                    tmpDataId++;
                                    continue;
                                }else{
                                    break;
                                }
                            }
                            cout << "Data ID : " << tmpDataId << endl;
                            tmplen++;
                            tmpDataId++;
                        }

                        ret = takeSelectedInput("Select data ID for delete : ",sub_input,0,9);

                        if(ret != true){
                            break;
                        }

                        if((sub_input >= 0) && (sub_input <= (tmpDataId+1))){// Check input

                            #ifndef USE_LIBRARY_TIMEOUT
                            // Update timeout variable
                            timeout = UART_REGIST_EXECUTE_TIMEOUT;
                            #endif

                            // Delete user data id
                            returnStatus = sensor.deleteUserData(timeout,userId[sub_choice-1], sub_input);

                            #ifndef USE_LIBRARY_TIMEOUT
                            // Update timeout variable
                            timeout = UART_SETTING_TIMEOUT;
                            #endif

                            if(returnStatus != B5T_STATUS_SUCCESS){
                                cout << "Failed to modify user with error code : " << returnStatus << endl;
                                break;
                            }

                            cout << "Data id "<< sub_input << " of user id " << unsigned(userId[sub_choice-1]) << " deleted Successfully." << endl;
                            break;
                        }

                    }else{

                        cout << "Invalid input." << endl;
                    }
                }else if(sub_choice == 2){// Delete user

                    // Print for delete users
                    cout << "*************************************************************" << endl;
                    cout << "*************************************************************" << endl;
                    cout << "                       Delete Users                          " << endl;
                    cout << "*************************************************************" << endl;
                    cout << "*************************************************************" << endl;
                    cout << "1. Delete specific user id                                   " << endl;
                    cout << "2. Delete all user id                                        " << endl;
                    cout << "*************************************************************" << endl;

                    // Print for input from user
                    ret = takeSelectedInput("Select from the list of Delete user : ",input,1,2);

                    if(ret != true){
                        break;
                    }

                    if(input == 2){// Delete all user

                        if(userId.size() > 0){// Check registered user ids size

                            sensor.deleteUserAll(timeout);
                            cout << "All user id delete successfully." << endl;

                        }else{
                            cout << "Users id list is empty. Please add user and try again." << endl;
                        }

                    }else if(input == 1){// Delete specific user id

                        if(userId.size() > 0){// Check registered user ids size

                            for(loop = 0; loop < userId.size(); loop++){
                                cout << (loop+1) << "." << "User ID : " << unsigned(userId[loop]) << endl;
                            }

                        }else{

                            cout << "Users id list is empty. Please add user and try again." << endl;
                            break;
                        }

                        // Take a input choice from user
                        ret = takeSelectedInput("Select from the list of user ID : ",input,1,userId.size());

                        if(ret != true){
                            break;
                        }

                        // Delete user id
                        returnStatus = sensor.deleteUser(timeout,userId[input-1]);

                        if(returnStatus != B5T_STATUS_SUCCESS){
                            cout << "Failed to modify user with error code : " << returnStatus << endl;
                            cin.ignore();
                            break;
                        }
                        cout << "User id " << unsigned(userId[input-1]) << " deteled successfully." << endl;
                    }
                }

                break;

            case LIST_USER:

                // Get register user ids and data ids
                returnStatus = sensor.getRegisteredUsersInfo(timeout,userId,userDataId);

                if(returnStatus != B5T_STATUS_SUCCESS){
                    cout << "Failed to get Registered User Info with error code : " << returnStatus << endl;
                    break;
                }

                cout << "*************************************************************" << endl;
                cout << "*************************************************************" << endl;
                cout << "                        Users ID                             " << endl;
                cout << "*************************************************************" << endl;
                cout << "*************************************************************" << endl;

                if(userId.size() > 0){// Check registered user ids size

                    for(loop = 0; loop < userId.size(); loop++){

                        cout << (loop+1) << "." << "User ID : " << unsigned(userId[loop]) << endl;
                    }
                }else{

                    cout << "Users id list is empty. Please add user and try again." << endl;
                    break;
                }

                ret = takeSelectedInput("Select from the list of user ID : ",sub_choice,1,userId.size());

                if(ret != true){
                    break;
                }

                tmplen = 0;

                if((sub_choice > 0) && (sub_choice <= userId.size())){// Check input

                    for(loop=0,i=0x01; i < 0x400; loop++, i<<=1){// Check and print registered data ids

                        if ((userDataId[sub_choice-1] & i) == 0 ){

                            if(tmplen < __builtin_popcountll(userDataId[sub_choice-1])){
                                continue;
                            }else{
                                break;
                            }
                        }

                        cout << "Data ID : " << loop << endl;
                        tmplen++;
                    }
                    break;
                }

                break;

            case SETTINGS:

                // Print settings
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
                cout << "10. Save Album from HVC Device to Host                       " << endl;
                cout << "11. Load Album from Host to HVC Device RAM                   " << endl;
                cout << "12. Save Album from HVC Device RAM to HVC device ROM         " << endl;
                cout << "*************************************************************" << endl;

                ret = takeSelectedInput("Select from the list of settings : ",sub_choice,1,12);

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

                        ret = takeSelectedInput("Select from the list of Camera Angles : ",input,1,4);

                        if(ret != true){
                            break;
                        }

                        // Set Camera angle
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

                        if(pose == 0){
                            cout << "Face Detection pose is : 30 degree" << endl;
                        }else if(pose == 1){
                            cout << "Face Detection pose is : 60 degree" << endl;
                        }else if(pose == 2){
                            cout << "Face Detection pose is : 90 degree" << endl;
                        }else{
                            cout << "Invalid Face Detection pose." << endl;
                        }

                        if(angle == 0){
                            cout << "Face Detection angle is : 15 degree" << endl;
                        }else if(angle == 1){
                            cout << "Face Detection angle is : 45 degre" << endl;
                        }else{
                            cout << "Invalid Face Detection angle." << endl;
                        }

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
                                cout << "Invalid choice. Please Select from 1 to 6." << endl;

                        }//end input switch set baudrate

                        // Set Baudrate
                        returnStatus = sensor.setBaudRate(timeout,baudRate);

                        if(returnStatus != B5T_STATUS_SUCCESS){
                            cout << "Failed to set BaudRate with error code : " << returnStatus << endl;
                            break;
                        }

                        cout << "BaudRate " << baudRate << " set successfully." << endl;

                        break;

                    case 10: // Save Album from HVC Device to Host

                        #ifndef USE_LIBRARY_TIMEOUT
                        // Update timeout variable
                        timeout = UART_SAVE_ALBUM_TIMEOUT;
                        #endif

                        // Save Album to host
                        returnStatus =  sensor.saveAlbumToHost(timeout, pAlbumData, albumDataSize);

                        #ifndef USE_LIBRARY_TIMEOUT
                        // Update timeout variable
                        timeout = UART_SETTING_TIMEOUT;
                        #endif

                        if(returnStatus != B5T_STATUS_SUCCESS){
                            cout << "Failed to Save Album Data with error code : " << returnStatus << endl;
                            break;
                        }

                        do{
                            cout << "Input File Path for save Album : ";

                            // Set path of file

                            getline(cin, inFileName,'\n');

                            // Open file
                            pFile.open(inFileName.c_str(), fstream::out);

                            if(!pFile){// Check file or path exists
                                cout << "*************************************************************" << endl;
                                cout << "Invalid Path." << endl;
                                cout << "*************************************************************" << endl;
                            }

                        }while(!pFile && flag);

                        // Write data in file
                        pFile.write((INT8*)pAlbumData,albumDataSize);

                        // Close file
                        pFile.close();

                        cout << "Save Album Data successfully." << endl;

                        break;

                    case 11: // Load Album from Host to HVC Device RAM

                        #ifndef USE_LIBRARY_TIMEOUT
                        // Update timeout variable
                        timeout = UART_LOAD_ALBUM_TIMEOUT;
                        #endif

                        do{

                            cout << "Input File Path from load Album : ";

                            // Set path of file
                            getline(cin, inFileName,'\n');

                            // Open file
                            pFile.open(inFileName.c_str(), fstream::out | fstream::in);

                            if(!pFile){// Check file or path exists
                                cout << "*************************************************************" << endl;
                                cout << "Invalid Path." << endl;
                                cout << "*************************************************************" << endl;
                            }

                        }while(!pFile && flag);

                        // Calculate size of ablbum data
                        pFile.seekg (0, pFile.end);
                        albumDataSize = pFile.tellg();
                        pFile.seekg (0, pFile.beg);

                        // Read album data
                        pFile.read((INT8*)pAlbumData,albumDataSize);

                        // Close file
                        pFile.close();

                        // Load album
                        returnStatus =  sensor.loadAlbumFromHost(timeout, pAlbumData,albumDataSize);

                        #ifndef USE_LIBRARY_TIMEOUT
                        // Update timeout variable
                        timeout = UART_SETTING_TIMEOUT;
                        #endif

                        if(returnStatus != B5T_STATUS_SUCCESS){
                            cout << "Failed to load album data with error code : " << returnStatus << endl;
                            break;
                        }

                        cout << "Load album data successfully." << endl;

                        break;

                    case 12: // Save Album from HVC Device RAM to HVC device ROM

                        // Save album into ROM
                        returnStatus =  sensor.saveAlbumToROM(timeout);

                        if(returnStatus != B5T_STATUS_SUCCESS){
                            cout << "Failed to aave album to ROM memory with error code : " << returnStatus << endl;
                            break;
                        }

                        cout << "Save album to ROM memory successfully." << endl;

                        break;

                    default:
                        cout << "Invalid choice. Please select from 1 to 12." << endl;

                }//end sub_choice switch

                break;

            default:
                cout << "Invalid choice. Please select from 1 to 4." << endl;

        }//end choice switch

        if(flag){
            cout << "Press enter to continue...";
            //cin.ignore();
            cin.get();
        }
    }

    // Free album data
    if(pAlbumData != NULL){
        free(pAlbumData);
    }

    return 0;
}

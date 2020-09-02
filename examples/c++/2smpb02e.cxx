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

/* headers */
#include "2smpb02e.hpp"

#include <limits>
#include <signal.h>
#include <upm_utilities.h>
#include <sys/time.h>
#include <iomanip>


/* Define */
#define CLEAR_SCREEN "\033[2J\033[1;1H"                   /* Clear screen */

using namespace upm;
using namespace std;

volatile sig_atomic_t flag = true;
UINT8 isCinWaitState = 0; //State for cin is waiting for input or not.

void
sig_handler(int signum)
{
    // Check for Interrupt signal
    if (signum == SIGINT || signum == SIGTERM){
        cout << "exit" << endl;
        flag = false;
        if(isCinWaitState ==1){
           std::cin.setstate(std::ios_base::badbit);
        }
    }
}

UINT8
takeInput(STRING str, INT32 &value, INT32 startIndex, INT32 endIndex){// Take input

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

int
main(int argc, char** argv)
{
    INT32 bus = OM2SMPB02E_DEFAULT_I2C_BUS_NUM;
    UINT32 display_time = 1000;
    UINT8 addr = OM2SMPB02E_DEFAULT_I2C_ADDR;
    TemperatureUnit iTempscale = TemperatureUnit::CELSIUS;
    PressureUnit iPresscale = PressureUnit::PA;
    INT32 choice,sub_choice,input,sub_input;
    UINT8 chipId;
    OM2SMPB02E_STATUS_T returnStatus;
    UINT8 ret;

    OM2SMPB02E_VAL_POWERMODE_VALUES_T         powerMode;
    OM2SMPB02E_VAL_MEASMODE_VALUES_T          measMode;
    OM2SMPB02E_VAL_IIR_VALUES_T               iirVal;
    OM2SMPB02E_VAL_IOSETUP_STANDBY_VALUES_T   standbyTimeValue;

    FLOAT temperatureData;
    FLOAT pressureData;

    // Data and Time
    INT8 dateAndTime[30];
    struct timeval value;
    time_t curtime;

    signal(SIGINT, sig_handler);

    // Instantiate a 2SMPB02E sensor on I2C using bus and address.
    OM2SMPB02E sensor(bus,addr);

    while(flag){

        // Clear screen
        cout << CLEAR_SCREEN;

        // Get Device Chip Id
        returnStatus = sensor.getDeviceId(chipId);

        // Check return value
        if(returnStatus != OM2SMPB02E_STATUS_SUCCESS){
            cout << "Failed to get Device Id with error code : " << returnStatus << endl;
            return 0;
        }

        cout << "*************************************************************" << endl;
        cout << " Barometric Pressure Sensor Chip ID (Device ID) : " <<   hex
             << uppercase << (unsigned) chipId << endl;

        // Print main menu for selection
        cout << "*************************************************************" << endl;
        cout << "*************************************************************" << endl;
        cout << "                        MAIN MENU                            " << endl;
        cout << "*************************************************************" << endl;
        cout << "*************************************************************" << endl;
        cout << "1. Display Temperature                                       " << endl;
        cout << "2. Display Pressure                                          " << endl;
        cout << "3. Display Temperature and Pressure                          " << endl;
        cout << "4. Settings                                                  " << endl;
        cout << "*************************************************************" << endl;

        ret = takeInput("Select from main menu : ",choice,1,4);

        if(ret != true){
            break;
        }

        switch(choice){

            case 1:// Display Temperature

                // Print Temperature scale for selection
                cout << "*************************************************************" << endl;
                cout << "*************************************************************" << endl;
                cout << "                        Temperature Scale                    " << endl;
                cout << "*************************************************************" << endl;
                cout << "*************************************************************" << endl;
                cout << "1. CELSIUS      [deg C]                                      " << endl;
                cout << "2. FAHRENHEIT   [deg F]                                      " << endl;
                cout << "3. KELVIN       [deg K]                                      " << endl;
                cout << "*************************************************************" << endl;

                ret = takeInput("Select from the list of temperature scale : ",sub_choice,1,3);

                if(ret != true){
                    break;
                }

                // Update Temperature scale variable
                switch(sub_choice){

                    case 1:// Celsius
                        iTempscale = TemperatureUnit::CELSIUS;
                        break;

                    case 2:// Fahrenheit
                        iTempscale = TemperatureUnit::FAHRENHEIT;
                        break;

                    case 3:// Kelvin
                        iTempscale = TemperatureUnit::KELVIN;
                        break;

                    default:
                        break;

                }// end switch select temperature scale

                // Set temperature scale
                returnStatus = sensor.setTemperatureScale(iTempscale);

                // Check return status
                if(returnStatus != OM2SMPB02E_STATUS_SUCCESS){
                    cout << "Failed to set Temperature scale with error code : " << returnStatus << endl;
                    break;
                }

                // Print display time
                cout << "*************************************************************" << endl;
                cout << "*************************************************************" << endl;
                cout << "                        Display time                         " << endl;
                cout << "*************************************************************" << endl;
                cout << "*************************************************************" << endl;

                ret = takeInput("Please enter display time [ms] : ",input,1,1000000);

                if(ret != true){
                    break;
                }

                display_time = input;

                // Set precision
                cout << fixed << setprecision(2);

                cout << "*************************************************************" << endl;
                cout << "Timestamp                  Temperature                       " << endl;
                cout << "*************************************************************" << endl;

                while(flag){

                    // Get date and time
                    gettimeofday(&value, NULL);
                    curtime=value.tv_sec;
                    strftime(dateAndTime,30,"[%F %T.",localtime(&curtime));
                    cout << dateAndTime << std::setfill('0') << std::setw(3) << std::dec << value.tv_usec/1000 << "] " << setfill(' ') << setw(1) << " ";

                    // Get temperature value
                    returnStatus = sensor.getTemperature(temperatureData);

                    // Check return status
                    if(returnStatus != OM2SMPB02E_STATUS_SUCCESS){
                        cout << "Failed to get temperatue value with error code : " << returnStatus << endl;
                    }else{
                        // Print temperature data
                        cout << temperatureData << endl;
                    }

                    //Delay
                    upm_delay_ms(display_time);
                }

                break;

            case 2:// Display Pressure

                // Print Pressure scale for selection
                cout << "*************************************************************" << endl;
                cout << "*************************************************************" << endl;
                cout << "                        Pressure Scale                       " << endl;
                cout << "*************************************************************" << endl;
                cout << "*************************************************************" << endl;
                cout << "1. Pascal                   [pa]                             " << endl;
                cout << "2. Bar                      [bar]                            " << endl;
                cout << "3. Standard atmosphere      [atm]                            " << endl;
                cout << "4. Torr                     [torr]                           " << endl;
                cout << "5. Pounds per square inch   [psi]                            " << endl;
                cout << "*************************************************************" << endl;

                ret = takeInput("Select from the list of pressure scale : ",sub_choice,1,5);

                if(ret != true){
                    break;
                }

                // Update Pressure scale variable
                switch(sub_choice){

                    case 1:// Pascal
                        iPresscale = PressureUnit::PA;
                        break;

                    case 2:// Bar
                        iPresscale = PressureUnit::BAR;
                        break;

                    case 3:// Standard Atmosphere
                        iPresscale = PressureUnit::ATM;
                        break;

                    case 4:// Torr
                        iPresscale = PressureUnit::TORR;
                        break;

                    case 5:// Pounds per square inch
                        iPresscale = PressureUnit::PSI;
                        break;

                    default:
                        break;

                }// end switch select pressure scale

                // Set Preesure scale
                returnStatus = sensor.setPressureScale(iPresscale);

                // Check return status
                if(returnStatus != OM2SMPB02E_STATUS_SUCCESS){
                    cout << "Failed to set pressure scale with error code : " << returnStatus << endl;
                    break;
                }

                // Print display time
                cout << "*************************************************************" << endl;
                cout << "*************************************************************" << endl;
                cout << "                        Display time                         " << endl;
                cout << "*************************************************************" << endl;
                cout << "*************************************************************" << endl;

                ret = takeInput("Please enter display time [ms] : ",input,1,1000000);

                if(ret != true){
                    break;
                }

                display_time = input;

                cout << "*************************************************************" << endl;
                cout << "Timestamp                  Pressure                          " << endl;
                cout << "*************************************************************" << endl;

                while(flag){

                    // Get date and time
                    gettimeofday(&value, NULL);
                    curtime=value.tv_sec;
                    strftime(dateAndTime,30,"[%F %T.",localtime(&curtime));
                    cout << dateAndTime << std::setfill('0') << std::setw(3) << std::dec << value.tv_usec/1000 << "] " << setfill(' ') << setw(1) << " ";

                    // Get pressure data
                    returnStatus = sensor.getPressure(pressureData);

                    // Check return status
                    if(returnStatus != OM2SMPB02E_STATUS_SUCCESS){
                        cout << "Failed to get pressure value with error code : " << returnStatus << endl;
                    }else{
                        // Print pressure data
                        cout << fixed << (iPresscale==PressureUnit::PA?setprecision(2):setprecision(7)) << pressureData << endl;
                    }

                    //Delay
                    upm_delay_ms(display_time);
                }

                break;

            case 3:// Display Temperature and Pressure

                // Print Temperature scale for selection
                cout << "*************************************************************" << endl;
                cout << "*************************************************************" << endl;
                cout << "                        Temperature Scale                    " << endl;
                cout << "*************************************************************" << endl;
                cout << "*************************************************************" << endl;
                cout << "1. CELSIUS      [deg C]                                      " << endl;
                cout << "2. FAHRENHEIT   [deg F]                                      " << endl;
                cout << "3. KELVIN       [deg K]                                      " << endl;
                cout << "*************************************************************" << endl;

                ret = takeInput("Select from the list of temperature scale : ",sub_choice,1,3);

                if(ret != true){
                    break;
                }

                // Update Temperature scale variable
                switch(sub_choice){

                    case 1:// Celsius
                        iTempscale = TemperatureUnit::CELSIUS;
                        break;

                    case 2:// Fahrenheit
                        iTempscale = TemperatureUnit::FAHRENHEIT;
                        break;

                    case 3:// Kelvin
                        iTempscale = TemperatureUnit::KELVIN;
                        break;

                    default:
                        break;

                }// end switch select temperature scale

                // Set Temeperature scale
                returnStatus = sensor.setTemperatureScale(iTempscale);

                // Check return status
                if(returnStatus != OM2SMPB02E_STATUS_SUCCESS){
                    cout << "Failed to set Temperature scale with error code : " << returnStatus << endl;
                    break;
                }

                // Print Pressure scale for selection
                cout << "*************************************************************" << endl;
                cout << "*************************************************************" << endl;
                cout << "                        Pressure Scale                       " << endl;
                cout << "*************************************************************" << endl;
                cout << "*************************************************************" << endl;
                cout << "1. Pascal                   [pa]                             " << endl;
                cout << "2. Bar                      [bar]                            " << endl;
                cout << "3. Standard atmosphere      [atm]                            " << endl;
                cout << "4. Torr                     [torr]                           " << endl;
                cout << "5. Pounds per square inch   [psi]                            " << endl;
                cout << "*************************************************************" << endl;

                ret = takeInput("Select from the list of pressure scale : ",input,1,5);

                if(ret != true){
                    break;
                }

                // Update Pressure scale variable
                switch(input){

                    case 1:// Pascal
                        iPresscale = PressureUnit::PA;
                        break;

                    case 2:// Bar
                        iPresscale = PressureUnit::BAR;
                        break;

                    case 3:// Standard Atmosphere
                        iPresscale = PressureUnit::ATM;
                        break;

                    case 4:// Torr
                        iPresscale = PressureUnit::TORR;
                        break;

                    case 5:// Pounds per square inch
                        iPresscale = PressureUnit::PSI;
                        break;

                    default:
                        break;

                }// end switch select pressure scale

                // Set pressure scale
                returnStatus = sensor.setPressureScale(iPresscale);

                // Check return status
                if(returnStatus != OM2SMPB02E_STATUS_SUCCESS){
                    cout << "Failed to set pressure scale with error code : " << returnStatus << endl;
                    break;
                }

                // Print display time
                cout << "*************************************************************" << endl;
                cout << "*************************************************************" << endl;
                cout << "                       Display time                          " << endl;
                cout << "*************************************************************" << endl;
                cout << "*************************************************************" << endl;

                ret = takeInput("Please enter display time [ms] : ",sub_input,1,1000000);

                if(ret != true){
                    break;
                }

                display_time = sub_input;

                cout << "*************************************************************" << endl;
                cout << "Timestamp                  Temperature            Pressure   " << endl;
                cout << "*************************************************************" << endl;

                while(flag){

                    // Get date and time
                    gettimeofday(&value, NULL);
                    curtime=value.tv_sec;
                    strftime(dateAndTime,30,"[%F %T.",localtime(&curtime));
                    cout << dateAndTime << std::setfill('0') << std::setw(3) << std::dec << value.tv_usec/1000 << "]" << setfill(' ') << setw(3) << " ";

                    // Get pressura and temperature data
                    returnStatus = sensor.getPresAndTempValues(pressureData,temperatureData);

                    // Check return status
                    if(returnStatus != OM2SMPB02E_STATUS_SUCCESS){
                        cout << "Failed to get pressure and temperature value with error code : " << returnStatus << endl;
                    }else{
                        // Print pressure and temperature data
                        cout << fixed << setprecision(2) << temperatureData << " " << setfill(' ') << setw(15) << " " << fixed << (iPresscale==PressureUnit::PA?setprecision(2):setprecision(7)) << pressureData << endl;
                    }

                    //Delay
                    upm_delay_ms(display_time);
                }

                break;
            case 4:// Settings

                // Print main menu for selection
                cout << "*************************************************************" << endl;
                cout << "*************************************************************" << endl;
                cout << "                        SETTINGS                             " << endl;
                cout << "*************************************************************" << endl;
                cout << "*************************************************************" << endl;
                cout << "1. Get Power Mode                                            " << endl;
                cout << "2. Set Power Mode                                            " << endl;
                cout << "3. Get IIR filter value                                      " << endl;
                cout << "4. Set IIR filter value                                      " << endl;
                cout << "5. Get Measurement Mode                                      " << endl;
                cout << "6. Set Measurement Mode                                      " << endl;
                cout << "7. Set I2C Frequency                                         " << endl;
                cout << "*************************************************************" << endl;

                ret = takeInput("Select from the list of settings : ",sub_choice,1,7);

                if(ret != true){
                    break;
                }
                switch(sub_choice){

                    case 1:// Get Power Mode

                        // Get Power mode
                        returnStatus = sensor.getPowerMode(powerMode);

                        // Check return status
                        if(returnStatus != OM2SMPB02E_STATUS_SUCCESS){
                            cout << "Failed to get power mode with error code : " << returnStatus << endl;
                            break;
                        }

                        switch(powerMode){

                            case OM2SMPB02E_VAL_POWERMODE_SLEEP:
                                cout << "Power Mode : SLEEP" << endl;
                                break;

                            case OM2SMPB02E_VAL_POWERMODE_FORCED:
                                cout << "Power Mode : FORCED" << endl;
                                break;

                            case OM2SMPB02E_VAL_POWERMODE_NORMAL:

                                returnStatus = sensor.getStandbyTimeValue(standbyTimeValue);


                                if(returnStatus != OM2SMPB02E_STATUS_SUCCESS){
                                    cout << "Failed to get standby time value with error code : " << returnStatus << endl;
                                    break;
                                }

                                cout << "Power Mode : NORMAL, and Standby Time Value : ";

                                switch(standbyTimeValue){

                                    case OM2SMPB02E_VAL_IOSETUP_STANDBY_0001MS:
                                        cout << "1 ms." << endl;
                                        break;

                                    case OM2SMPB02E_VAL_IOSETUP_STANDBY_0005MS:
                                        cout << "5 ms." << endl;
                                        break;

                                    case OM2SMPB02E_VAL_IOSETUP_STANDBY_0050MS:
                                        cout << "50 ms." << endl;
                                        break;

                                    case OM2SMPB02E_VAL_IOSETUP_STANDBY_0250MS:
                                        cout << "250 ms." << endl;
                                        break;

                                    case OM2SMPB02E_VAL_IOSETUP_STANDBY_0500MS:
                                        cout << "500 ms." << endl;
                                        break;

                                    case OM2SMPB02E_VAL_IOSETUP_STANDBY_1000MS:
                                        cout << "1 s." << endl;
                                        break;

                                    case OM2SMPB02E_VAL_IOSETUP_STANDBY_2000MS:
                                        cout << "2 s." << endl;
                                        break;

                                    case OM2SMPB02E_VAL_IOSETUP_STANDBY_4000MS:
                                        cout << "4 s." << endl;
                                        break;

                                    default:
                                        cout << "Invalid value received for standby time." << endl;

                                }// end switch get standby time value

                                break;

                            default:
                                cout << "Invalid power mode value receive." << endl;

                        }// end switch getPowerMode

                        break;

                    case 2:// Set Power Mode

                        cout << "*************************************************************" << endl;
                        cout << "*************************************************************" << endl;
                        cout << "                        POWER MODE                           " << endl;
                        cout << "*************************************************************" << endl;
                        cout << "*************************************************************" << endl;
                        cout << "1. Sleep                                                     " << endl;
                        cout << "2. Forced                                                    " << endl;
                        cout << "3. Normal                                                    " << endl;
                        cout << "*************************************************************" << endl;

                        ret = takeInput("Select from the list of power mode : ",input,1,3);

                        if(ret != true){
                            break;
                        }

                        switch(input){

                            case 1:// Sleep
                                powerMode = OM2SMPB02E_VAL_POWERMODE_SLEEP;
                                break;

                            case 2:// Forced
                                powerMode = OM2SMPB02E_VAL_POWERMODE_FORCED;
                                break;

                            case 3:// Normal
                                powerMode = OM2SMPB02E_VAL_POWERMODE_NORMAL;

                                // Set Standby time Value
                                cout << "*************************************************************" << endl;
                                cout << "*************************************************************" << endl;
                                cout << "                    STANDBY TIME VALUES                      " << endl;
                                cout << "*************************************************************" << endl;
                                cout << "*************************************************************" << endl;
                                cout << "1. 1    ms                                                   " << endl;
                                cout << "2. 5    ms                                                   " << endl;
                                cout << "3. 50   ms                                                   " << endl;
                                cout << "4. 250  ms                                                   " << endl;
                                cout << "5. 500  ms                                                   " << endl;
                                cout << "6. 1     s                                                   " << endl;
                                cout << "7. 2     s                                                   " << endl;
                                cout << "8. 4     s                                                   " << endl;
                                cout << "*************************************************************" << endl;

                                ret = takeInput("Select from the list of standby time value : ",sub_input,1,8);

                                if(ret != true){
                                    break;
                                }

                                switch(sub_input){

                                    case 1:// 1 MS
                                        standbyTimeValue = OM2SMPB02E_VAL_IOSETUP_STANDBY_0001MS;
                                        break;

                                    case 2:// 5 MS
                                        standbyTimeValue = OM2SMPB02E_VAL_IOSETUP_STANDBY_0005MS;
                                        break;

                                    case 3:// 50 MS
                                        standbyTimeValue = OM2SMPB02E_VAL_IOSETUP_STANDBY_0050MS;
                                        break;

                                    case 4:// 250 MS
                                        standbyTimeValue = OM2SMPB02E_VAL_IOSETUP_STANDBY_0250MS;
                                        break;

                                    case 5:// 500 MS
                                        standbyTimeValue = OM2SMPB02E_VAL_IOSETUP_STANDBY_0500MS;
                                        break;

                                    case 6:// 1000 MS
                                        standbyTimeValue = OM2SMPB02E_VAL_IOSETUP_STANDBY_1000MS;
                                        break;

                                    case 7:// 2000 MS
                                        standbyTimeValue = OM2SMPB02E_VAL_IOSETUP_STANDBY_2000MS;
                                        break;

                                    case 8:// 4000 MS
                                        standbyTimeValue = OM2SMPB02E_VAL_IOSETUP_STANDBY_4000MS;
                                        break;

                                    default:
                                        break;
                                }//end switch set standby time value

                                break;

                            default:
                                break;

                        }//end switch set power mode

                        // Set power mode
                        returnStatus = sensor.setPowerMode(powerMode);

                        // Check return status
                        if(returnStatus != OM2SMPB02E_STATUS_SUCCESS){
                            cout << "Failed to set power mode with error code : " << returnStatus << endl;
                            break;
                        }

                        if(input == 3){// Check Power mode input is Normal
                            returnStatus = sensor.setStandbyTimeValue(standbyTimeValue);

                            if(returnStatus != OM2SMPB02E_STATUS_SUCCESS){
                                cout << "Failed to set standby time value with error code : " << returnStatus << endl;
                                break;
                            }

                            cout << "Power Mode set successfully with standby time value." << endl;

                        }else{
                            cout << "Power Mode successfully changed." << endl;
                        }

                        break;

                    case 3:// Get IIR filter value

                        // Get IIR Filter value
                        returnStatus = sensor.getIIRFilterValue(iirVal);

                        // Check return status
                        if(returnStatus != OM2SMPB02E_STATUS_SUCCESS){
                            cout << "Failed to get IIR filter value with error code : " << returnStatus << endl;
                            break;
                        }

                        switch(iirVal){

                            case OM2SMPB02E_VAL_IIR_OFF:
                                cout << "IIR Value : 0 Times (OFF)" << endl;
                                break;

                            case OM2SMPB02E_VAL_IIR_02TIMES:
                                cout << "IIR Value : 2 Times" << endl;
                                break;

                            case OM2SMPB02E_VAL_IIR_04TIMES:
                                cout << "IIR Value : 4 Times" << endl;
                                break;

                            case OM2SMPB02E_VAL_IIR_08TIMES:
                                cout << "IIR Value : 8 Times" << endl;
                                break;

                            case OM2SMPB02E_VAL_IIR_16TIMES:
                                cout << "IIR Value : 16 Times" << endl;
                                break;

                            case OM2SMPB02E_VAL_IIR_32TIMES:
                                cout << "IIR Value : 32 Times" << endl;
                                break;

                            default:
                                cout << "Invalid iir value receive." << endl;

                        }// end switch getIIRFilterValue

                        break;

                    case 4:// Set IIR filter value

                        cout << "*************************************************************" << endl;
                        cout << "*************************************************************" << endl;
                        cout << "                     IIR Filter Value                        " << endl;
                        cout << "*************************************************************" << endl;
                        cout << "*************************************************************" << endl;
                        cout << "1. 0  Times (OFF)                                            " << endl;
                        cout << "2. 2  Times                                                  " << endl;
                        cout << "3. 4  Times                                                  " << endl;
                        cout << "4. 8  Times                                                  " << endl;
                        cout << "5. 16 Times                                                  " << endl;
                        cout << "6. 32 Times                                                  " << endl;
                        cout << "*************************************************************" << endl;

                        ret = takeInput("Select from the list of iir filter value : ",input,1,6);

                        if(ret != true){
                            break;
                        }

                        switch(input){

                            case 1:// 0 Times (OFF)
                                iirVal = OM2SMPB02E_VAL_IIR_OFF;
                                break;

                            case 2:// 2 Times
                                iirVal = OM2SMPB02E_VAL_IIR_02TIMES;
                                break;

                            case 3:// 4 Times
                                iirVal = OM2SMPB02E_VAL_IIR_04TIMES;
                                break;

                            case 4:// 8 Times
                                iirVal = OM2SMPB02E_VAL_IIR_08TIMES;
                                break;

                            case 5:// 16 Times
                                iirVal = OM2SMPB02E_VAL_IIR_16TIMES;
                                break;

                            case 6:// 32 Times
                                iirVal = OM2SMPB02E_VAL_IIR_32TIMES;
                                break;

                            default:
                                break;

                        }//end switch set iir filter value

                        // Set IIR Filter value
                        returnStatus = sensor.setIIRFilterValue(iirVal);

                        // Check return status
                        if(returnStatus != OM2SMPB02E_STATUS_SUCCESS){
                            cout << "Failed to set iir filter value with error code : " << returnStatus << endl;
                            break;
                        }

                        cout << "IIR filter value set successfully." << endl;

                        break;

                    case 5:// Get Measurement Mode

                        // Get measurement mode
                        returnStatus = sensor.getMeasurementMode(measMode);

                        // Check return status
                        if(returnStatus != OM2SMPB02E_STATUS_SUCCESS){
                            cout << "Failed to get measurement mode value with error code : " << returnStatus << endl;
                            break;
                        }

                        cout << "*************************************************************" << endl;

                        switch(measMode){

                            case OM2SMPB02E_VAL_MEASMODE_HIGHSPEED:// High Speed
                                cout << "Measurement Mode : High Speed ";
                                break;

                            case OM2SMPB02E_VAL_MEASMODE_LOWPOWER:// Low Power
                                cout << "Measurement Mode : Low Power ";
                                break;

                            case OM2SMPB02E_VAL_MEASMODE_STANDARD:// Standard
                                cout << "Measurement Mode : Standard ";
                                break;

                            case OM2SMPB02E_VAL_MEASMODE_HIGHACCURACY:// High Accuracy
                                cout << "Measurement Mode : High Accuracy ";
                                break;

                            case OM2SMPB02E_VAL_MEASMODE_ULTRAHIGH:// Ultra High
                                cout << "Measurement Mode : Ultra High ";
                                break;

                            default:
                                cout << "Measurement Mode : Custom ";

                        }// end switch getMeasurementMode

                        cout << "[Pressure Measurement Avg time : ";

                        switch(measMode & OM2SMPB02E_MASK_PRESAVERAGE){

                            case OM2SMPB02E_VAL_PRESAVERAGE_00:
                                cout << "Skip";
                                break;

                            case OM2SMPB02E_VAL_PRESAVERAGE_01:
                                cout << "1";
                                break;

                            case OM2SMPB02E_VAL_PRESAVERAGE_02:
                                cout << "2";
                                break;

                            case OM2SMPB02E_VAL_PRESAVERAGE_04:
                                cout << "4";
                                break;

                            case OM2SMPB02E_VAL_PRESAVERAGE_08:
                                cout << "8";
                                break;

                            case OM2SMPB02E_VAL_PRESAVERAGE_16:
                                cout << "16";
                                break;

                            case OM2SMPB02E_VAL_PRESAVERAGE_32:
                                cout << "32";
                                break;

                            case OM2SMPB02E_VAL_PRESAVERAGE_64:
                                cout << "64";
                                break;

                            default:
                                cout << "Invalud pressure average value";

                        }//end switch print pressure average value

                        cout << ", Temperature Measurement Avg time : ";

                        switch(measMode & OM2SMPB02E_MASK_TEMPAVERAGE){

                            case OM2SMPB02E_VAL_TEMPAVERAGE_00:
                                cout << "Skip]" << endl;
                                break;

                            case OM2SMPB02E_VAL_TEMPAVERAGE_01:
                                cout << "1]" << endl;
                                break;

                            case OM2SMPB02E_VAL_TEMPAVERAGE_02:
                                cout << "2]" << endl;
                                break;

                            case OM2SMPB02E_VAL_TEMPAVERAGE_04:
                                cout << "4]" << endl;
                                break;

                            case OM2SMPB02E_VAL_TEMPAVERAGE_08:
                                cout << "8]" << endl;
                                break;

                            case OM2SMPB02E_VAL_TEMPAVERAGE_16:
                                cout << "16]" << endl;
                                break;

                            case OM2SMPB02E_VAL_TEMPAVERAGE_32:
                                cout << "32]" << endl;
                                break;

                            case OM2SMPB02E_VAL_TEMPAVERAGE_64:
                                cout << "64]" << endl;
                                break;

                            default:
                                cout << "Invalid temperature average value" << endl;

                        }//end switch print temperature average value

                        break;

                    case 6:// Set Measurement Mode

                        cout << "*************************************************************" << endl;
                        cout << "*************************************************************" << endl;
                        cout << "                    Measurement Mode                         " << endl;
                        cout << "*************************************************************" << endl;
                        cout << "*************************************************************" << endl;
                        cout << "1. High Speed                                                " << endl;
                        cout << "2. Low Power                                                 " << endl;
                        cout << "3. Standard                                                  " << endl;
                        cout << "4. High Accuracy                                             " << endl;
                        cout << "5. Ultra High                                                " << endl;
                        cout << "6. Custom                                                    " << endl;
                        cout << "*************************************************************" << endl;

                        ret = takeInput("Select from the list of measurement mode : ",input,1,6);

                        if(ret != true){
                            break;
                        }

                        switch(input){

                            case 1:// High Speed
                                measMode = OM2SMPB02E_VAL_MEASMODE_HIGHSPEED;
                                break;

                            case 2:// Low Power
                                measMode = OM2SMPB02E_VAL_MEASMODE_LOWPOWER;
                                break;

                            case 3:// Standard
                                measMode = OM2SMPB02E_VAL_MEASMODE_STANDARD;
                                break;

                            case 4:// High Accuracy
                                measMode = OM2SMPB02E_VAL_MEASMODE_HIGHACCURACY;
                                break;

                            case 5:// Ultra High
                                measMode = OM2SMPB02E_VAL_MEASMODE_ULTRAHIGH;
                                break;

                            case 6:// Custom

                                cout << "*************************************************************" << endl;
                                cout << "*************************************************************" << endl;
                                cout << "                  Temperature Average Value                  " << endl;
                                cout << "*************************************************************" << endl;
                                cout << "*************************************************************" << endl;
                                cout << "1. 1                                                         " << endl;
                                cout << "2. 2                                                         " << endl;
                                cout << "3. 4                                                         " << endl;
                                cout << "4. 8                                                         " << endl;
                                cout << "5. 16                                                        " << endl;
                                cout << "6. 32                                                        " << endl;
                                cout << "7. 64                                                        " << endl;
                                cout << "*************************************************************" << endl;

                                ret = takeInput("Select from the list of temperature average value : ",sub_input,1,7);

                                if(ret != true){
                                    break;
                                }

                                switch(sub_input){

                                    case 1:// 1
                                        measMode = (OM2SMPB02E_VAL_MEASMODE_VALUES_T) OM2SMPB02E_VAL_TEMPAVERAGE_01;
                                        break;

                                    case 2:// 2
                                        measMode = (OM2SMPB02E_VAL_MEASMODE_VALUES_T) OM2SMPB02E_VAL_TEMPAVERAGE_02;
                                         break;

                                    case 3:// 4
                                        measMode = (OM2SMPB02E_VAL_MEASMODE_VALUES_T) OM2SMPB02E_VAL_TEMPAVERAGE_04;
                                        break;

                                    case 4:// 8
                                        measMode = (OM2SMPB02E_VAL_MEASMODE_VALUES_T) OM2SMPB02E_VAL_TEMPAVERAGE_08;
                                        break;

                                    case 5:// 16
                                        measMode = (OM2SMPB02E_VAL_MEASMODE_VALUES_T) OM2SMPB02E_VAL_TEMPAVERAGE_16;
                                        break;

                                    case 6:// 32
                                        measMode = (OM2SMPB02E_VAL_MEASMODE_VALUES_T) OM2SMPB02E_VAL_TEMPAVERAGE_32;
                                        break;

                                    case 7:// 64
                                        measMode = (OM2SMPB02E_VAL_MEASMODE_VALUES_T) OM2SMPB02E_VAL_TEMPAVERAGE_64;
                                        break;

                                    default:
                                        break;

                                }// end sub input temperature average value

                                cout << "*************************************************************" << endl;
                                cout << "*************************************************************" << endl;
                                cout << "                  Pressure Average Value                     " << endl;
                                cout << "*************************************************************" << endl;
                                cout << "*************************************************************" << endl;
                                cout << "1. 1                                                         " << endl;
                                cout << "2. 2                                                         " << endl;
                                cout << "3. 4                                                         " << endl;
                                cout << "4. 8                                                         " << endl;
                                cout << "5. 16                                                        " << endl;
                                cout << "6. 32                                                        " << endl;
                                cout << "7. 64                                                        " << endl;
                                cout << "*************************************************************" << endl;

                                ret = takeInput("Select from the list of pressure average value : ",sub_input,1,7);

                                if(ret != true){
                                    break;
                                }

                                switch(sub_input){

                                    case 1:// 1
                                        measMode = OM2SMPB02E_VAL_MEASMODE_VALUES_T (measMode | OM2SMPB02E_VAL_PRESAVERAGE_01);
                                        break;

                                    case 2:// 2
                                        measMode = OM2SMPB02E_VAL_MEASMODE_VALUES_T (measMode | OM2SMPB02E_VAL_PRESAVERAGE_02);
                                         break;

                                    case 3:// 4
                                        measMode = OM2SMPB02E_VAL_MEASMODE_VALUES_T (measMode | OM2SMPB02E_VAL_PRESAVERAGE_04);
                                        break;

                                    case 4:// 8
                                        measMode = OM2SMPB02E_VAL_MEASMODE_VALUES_T (measMode | OM2SMPB02E_VAL_PRESAVERAGE_08);
                                        break;

                                    case 5:// 16
                                        measMode = OM2SMPB02E_VAL_MEASMODE_VALUES_T (measMode | OM2SMPB02E_VAL_PRESAVERAGE_16);
                                        break;

                                    case 6:// 32
                                        measMode = OM2SMPB02E_VAL_MEASMODE_VALUES_T (measMode | OM2SMPB02E_VAL_PRESAVERAGE_32);
                                        break;

                                    case 7:// 64
                                        measMode = OM2SMPB02E_VAL_MEASMODE_VALUES_T (measMode | OM2SMPB02E_VAL_PRESAVERAGE_64);
                                        break;

                                    default:
                                        break;

                                }// end sub input pressure average value

                                break;

                            default:
                                break;

                        }//end switch set Measurement Mode

                        // Set measurement mode
                        returnStatus = sensor.setMeasurementMode(measMode);

                        // Check return status
                        if(returnStatus != OM2SMPB02E_STATUS_SUCCESS){
                            cout << "Failed to set measurement mode value with error code : " << returnStatus << endl;
                            break;
                        }

                        cout << "Measurement mode value set successfully." << endl;

                        break;

                    case 7:// Set I2C Frequency

                        cout << "*************************************************************" << endl;
                        cout << "*************************************************************" << endl;
                        cout << "                      I2C Frequency                          " << endl;
                        cout << "*************************************************************" << endl;
                        cout << "*************************************************************" << endl;
                        cout << "1. 100 KHz                                                   " << endl;
                        cout << "2. 400 KHz                                                   " << endl;
                        cout << "3. 3.4 MHz                                                   " << endl;
                        cout << "*************************************************************" << endl;

                        ret = takeInput("Select from the list of i2c frequency : ",input,1,3);

                        if(ret != true){
                            break;
                        }

                        // Set I2C Frequency
                        returnStatus = sensor.setI2cFrequency((mraa::I2cMode)(input-1));

                        // Check return status
                        if(returnStatus != OM2SMPB02E_STATUS_SUCCESS){
                            cout << "Failed to set I2C frequency with error code : " << returnStatus << endl;
                            break;
                        }

                        cout << "I2C frequency set successfully." << endl;
                        break;

                    default:
                        break;

                }// end sub_choice

                break;

            default:
                break;

        }//end switch

        if(flag){
            cout << "Press enter to continue...";
            //cin.ignore();
            cin.get();
        }
    }//end while
    return 0;
}

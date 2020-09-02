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

/* standard headers */
#include <iostream>
#include <signal.h>
#include <upm_utilities.h>
#include <sys/time.h>
#include <iomanip>
#include <cmath>

/* omron sensor header */
#include "d6t.hpp"

using namespace upm;
using namespace std;

volatile sig_atomic_t flag = true;

/**
 * 1) Using first macro, call displayTemperature() method call and
 *    display the temperature
 * 2) Using second macro, call API
 *    getTemperature(float *ptat, float* temperatureData,
 *    			uint8_t length, uint8_t *pec);
 * 3) Using third macro, call API
 *    getTemperature(float &ptat, std::vector<float> &temperatureData,
 *                      uint8_t &pec);
 */
#define DISP_TEMP_API
#define DISP_TEMP_API_USING_ARRAY
#define DISP_TEMP_API_USING_VECTOR

/**
 * delay values channel 1
 */
float channel_1_delay_values[]=
{0.4, 0.8, 1.6, 3.2, 6.4, 12.5, 25, 50, 100, 200, 400, 800, 1600, 3200, 0.4, 0.8, 1.6, 3.2, 6.4, 12.5, 25, 50, 100, 200, 400, 800, 1600, 3200};

/**
 * delay values channel 8
 */
double channel_8_delay_values[]=
{1, 2, 4, 8, 16, 32, 64, 125, 250, 500, 1000, 2000, 4000, 8000, 1, 2, 4, 8, 16, 32, 64, 125, 250, 500, 1000, 2000, 4000, 8000};

/**
 * delay values channel 16
 */
double channel_16_delay_values[]=
{43, 86, 130, 172, 215, 258, 300, 344, 386, 420, 472, 514, 558, 600, 642, 685, 728, 772, 814, 856, 900, 942, 986, 1028, 1070, 1114, 1156, 1200, 1242, 1284, 1328, 1370};


void
sig_handler(int signum)
{
    // Check for Abort signal
    if(signum == SIGABRT){
	cout << "abort signal" << endl;
    }

    // Check for Interrupt signal
    if (signum == SIGINT || signum == SIGTERM){
       cout << "exit" << endl;
       flag = false;
       exit(1);
    }
}

int
main(int argc, char** argv)
{
    //char option = 0;
    int bus = D6T_DEFAULT_I2C_BUS_NUM;
    uint32_t sleep_time = 1000;
    int interval_value;
    uint32_t intervalValueMS = 0;
    uint8_t addr = D6T_DEFAULT_I2C_ADDR;
    D6T_SENSOR_TYPES_T s_type;
    TemperatureUnit iTempscale = TemperatureUnit::CELSIUS;
    int ret = D6T_STATUS_FAILURE;
    uint8_t iInput;

    signal(SIGINT, sig_handler);

    // Print sensor for selection
    cout << "************************************************************\r\n";
    cout << "Please select type of thermal sensor \r\n";
    cout << "1) 1X1 Channel\r\n";
    cout << "2) 1X8 Channel\r\n";
    cout << "3) 4X4 Channel\r\n";
    cout << "************************************************************\r\n";

    cin >> iInput;

    // Update sensor type variable
    switch(iInput){
	    case '1':
		    s_type = D6T_1X1_CHANNEL;
		    break;
	    case '2':
		    s_type = D6T_1X8_CHANNEL;
		    break;
	    case '3':
		    s_type = D6T_4X4_CHANNEL;
		    break;
            default:
                    cout << "Invalid Selection\r\n";
		    exit(1);
    }

    // Print Temperature scale for selection
    cout << "************************************************************\r\n";
    cout << "Please select Temperature scale \r\n";
    cout << "1) CELCIUS\r\n";
    cout << "2) FAHRENHEIT\r\n";
    cout << "3) KELVIN\r\n";
    cout << "************************************************************\r\n";

    cin >> iInput;

    // Update Temperature scale variable
    switch(iInput){
	    case '1':
		    iTempscale = TemperatureUnit::CELSIUS;
		    break;
	    case '2':
		    iTempscale = TemperatureUnit::FAHRENHEIT;
		    break;
	    case '3':
		    iTempscale = TemperatureUnit::KELVIN;
		    break;
	    default:
		    cout << "Invalid Selection\r\n";
		    exit(1);
    }

    // Instantiate a D6T sensor on I2C using bus and address.
    D6T sensor(s_type,bus,addr);

    uint8_t Interval = 0;
    // Get current interval setting
    sensor.getMeasurementInterval(Interval);

    if(s_type == D6T_4X4_CHANNEL){
          // Print for measurement value
          cout << "************************************************************\r\n";
          cout << "Please select interval measurement value between 1 to 32 \r\n";
	  cout << "Current Measurement Interval value is : " << unsigned(Interval) << endl;
          cout << "************************************************************\r\n";
          cout << "1  : 43 ms\r\n";
	  cout << "2  : 86 ms\r\n";
	  cout << "3  : 130 ms\r\n";
          cout << "4  : 172 ms\r\n";
	  cout << "5  : 215 ms\r\n";
	  cout << "6  : 258 ms\r\n";
          cout << "7  : 300 ms(Recommended)\r\n";
	  cout << "8  : 344 ms\r\n";
	  cout << "9  : 386 ms\r\n";
          cout << "10 : 420 ms\r\n";
	  cout << "11 : 472 ms\r\n";
	  cout << "12 : 514 ms\r\n";
          cout << "13 : 558 ms\r\n";
	  cout << "14 : 600 ms\r\n";
	  cout << "15 : 642 ms\r\n";
          cout << "16 : 685 ms\r\n";
	  cout << "17 : 728 ms\r\n";
	  cout << "18 : 772 ms\r\n";
          cout << "19 : 814 ms\r\n";
	  cout << "20 : 856 ms\r\n";
	  cout << "21 : 900 ms\r\n";
          cout << "22 : 942 ms\r\n";
	  cout << "23 : 986 ms\r\n";
	  cout << "24 : 1028 ms\r\n";
          cout << "25 : 1070 ms\r\n";
	  cout << "26 : 1114 ms\r\n";
	  cout << "27 : 1156 ms\r\n";
          cout << "28 : 1200 ms\r\n";
	  cout << "29 : 1242 ms\r\n";
	  cout << "30 : 1284 ms\r\n";
          cout << "31 : 1328 ms\r\n";
	  cout << "32 : 1370 ms\r\n";
          cout << "************************************************************\r\n";
	  cin >> interval_value;

	  // Inteval Value
	  intervalValueMS =  channel_16_delay_values[interval_value - 1];
    }else{
          // Print for measurement value
          cout << "************************************************************\r\n";
          cout << "Please select interval measurement value between 1 to 28 \r\n";
	  cout << "Current Measurement Interval value is : " << unsigned(Interval) << endl;
          cout << "************************************************************\r\n";

	  if(s_type == D6T_1X1_CHANNEL){
              cout << "1  : 8 ms\r\n";
	      cout << "2  : 8.5 ms\r\n";
	      cout << "3  : 10 ms\r\n";
	      cout << "4  : 12 ms\r\n";
	      cout << "5  : 15 ms\r\n";
	      cout << "6  : 20 ms\r\n";
	      cout << "7  : 35 ms\r\n";
	      cout << "8  : 55 ms\r\n";
	      cout << "9  : 100 ms\r\n";
	      cout << "10 : 200 ms\r\n";
	      cout << "11 : 400 ms\r\n";
	      cout << "12 : 800 ms\r\n";
	      cout << "13 : 1600 ms\r\n";
	      cout << "14 : 3200 ms\r\n";
	      cout << "15 : 8 ms(2 times average)\r\n";
	      cout << "16 : 8.5 ms(2 times average)\r\n";
	      cout << "17 : 10 ms(2 times average)\r\n";
	      cout << "18 : 12 ms(2 times average)\r\n";
	      cout << "19 : 15 ms(2 times average)\r\n";
	      cout << "20 : 20 ms(2 times average)\r\n";
	      cout << "21 : 35 ms(2 times average)\r\n";
	      cout << "22 : 55 ms(2 times average)\r\n";
	      cout << "23 : 100 ms(2 times average)(Recommended)\r\n";
	      cout << "24 : 200 ms(2 times average)\r\n";
	      cout << "25 : 400 ms(2 times average)\r\n";
	      cout << "26 : 800 ms(2 times average)\r\n";
	      cout << "27 : 1600 ms(2 times average)\r\n";
	      cout << "28 : 3200 ms(2 times average)\r\n";
	      cout << "************************************************************\r\n";
	      cin >> interval_value;

	      // Inteval Value
	      intervalValueMS =  ceil(channel_1_delay_values[interval_value - 1]);
	  }else{
              cout << "1  : 9 ms\r\n";
	      cout << "2  : 10 ms\r\n";
	      cout << "3  : 12 ms\r\n";
	      cout << "4  : 16 ms\r\n";
	      cout << "5  : 24 ms\r\n";
	      cout << "6  : 40 ms\r\n";
	      cout << "7  : 70 ms\r\n";
	      cout << "8  : 140 ms\r\n";
	      cout << "9  : 250 ms\r\n";
	      cout << "10 : 500 ms\r\n";
	      cout << "11 : 1000 ms\r\n";
	      cout << "12 : 2000 ms\r\n";
	      cout << "13 : 4000 ms\r\n";
	      cout << "14 : 8000 ms\r\n";
	      cout << "15 : 9 ms(2 times average)\r\n";
	      cout << "16 : 10 ms(2 times average)\r\n";
	      cout << "17 : 12 ms(2 times average)\r\n";
	      cout << "18 : 16 ms(2 times average)\r\n";
	      cout << "19 : 24 ms(2 times average)\r\n";
	      cout << "20 : 40 ms(2 times average)\r\n";
	      cout << "21 : 70 ms(2 times average)\r\n";
	      cout << "22 : 140 ms(2 times average)\r\n";
	      cout << "23 : 250 ms(2 times average)(Recommended)\r\n";
	      cout << "24 : 500 ms(2 times average)\r\n";
	      cout << "25 : 1000 ms(2 times average)\r\n";
	      cout << "26 : 2000 ms(2 times average)\r\n";
	      cout << "27 : 4000 ms(2 times average)\r\n";
	      cout << "28 : 8000 ms(2 times average)\r\n";
	      cout << "************************************************************\r\n";
	      cin >> interval_value;

	      // Inteval Value
	      intervalValueMS =  channel_8_delay_values[interval_value - 1];
	  }
    }

    while(true){
        // Print for enter sleep time
        cout << "************************************************************\r\n";
        cout << "Please enter display sleep time in ms \r\n";
	cout << "Value entered should be greater than or equal to the sensor measurement interval\r\n";
        cout << "************************************************************\r\n";

        // Update sleep time variable
        cin >> sleep_time;

        //Compare sleep time interval with sensor measurement interval
        if(sleep_time >= intervalValueMS){
		break;
	}else{
		cout << "ERROR: Please enter the value greater than or equal to sensor measurement interval" << endl;
	}

    }


    //set interval value
    ret = sensor.setMeasurementInterval(interval_value);

    // Display error message if measurement interval set fail
    if(ret != D6T_STATUS_SUCCESS){
	    cout << "Measurement interval set fail" << endl;
	    return ret;
    }

    // Set temperature scale
    ret = sensor.setTemperatureScale(iTempscale);

    // Display error message if temperature scale set fail
    if(ret != D6T_STATUS_SUCCESS){
	    cout << "Temperature scale set fail" << endl;
	    return ret;
    }

     //Get Temperature scale
     sensor.getTemperatureScale(iTempscale);

     //Print temperature scale
     if(iTempscale == TemperatureUnit::CELSIUS){
	     cout << "Temperature scale - CELSIUS\r\n";
     }
     else if(iTempscale == TemperatureUnit::FAHRENHEIT){
	     cout << "Temperature scale - FAHRENHEIT\r\n";
     }
     else if(iTempscale == TemperatureUnit::KELVIN){
	     cout << "Temperature scale - KELVIN\r\n";
     }else{
	     cout << "Temperature scale set fail\r\n";
     }

     // Variables declaration
     //float temperatureData[s_type];
     vector<float> VtemperatureData;
     //D6T_STATUS_T returnValue;
     //uint8_t counter;

     // Data and Time
     char dateAndTime[30];
     struct timeval value;

     time_t curtime;

    // Print the Temperature, PTAT, and PEC values
    // every perticular time as configure in sleep time
    while(flag)
    {

	    gettimeofday(&value, NULL);
            curtime=value.tv_sec;
            strftime(dateAndTime,30,"[%F %T.",localtime(&curtime));
            cout << dateAndTime << std::setfill('0') << std::setw(3) << value.tv_usec/1000 << "] ";

#ifdef DISP_TEMP_API
	        sensor.displayTemperature();
#elif defined (DISP_TEMP_API_USING_ARRAY)

            //Get current thermal sensor temperature values
            returnValue = sensor.getTemperature(&ptat, temperatureData, s_type, &pec);

	    if(D6T_STATUS_SUCCESS != returnValue){
		    std::cout <<  "displayTemperature failed with error code: " << returnValue << std::endl;
	    }

	    //Print values
	    cout << "TEMP_DATA: ";
    	    for(counter = 0; counter < s_type; counter ++) {
		    cout << temperatureData[counter] << " ";
	    }
	    cout << "PTAT: " << ptat;
	    cout << " PEC: " << (unsigned) pec << endl;

#elif defined (DISP_TEMP_API_USING_VECTOR)

	    returnValue = sensor.getTemperature(ptat, VtemperatureData, pec);

	    if(D6T_STATUS_SUCCESS != returnValue){
		    std::cout <<  "displayTemperature failed with error code: " << returnValue << std::endl;
                    return returnValue;
	    }

	    //Print values
	    cout << "TEMP_DATA: ";
	    for(counter = 0; counter < s_type; counter ++) {
		    cout << VtemperatureData[counter] << " ";
	    }
	    cout << "PTAT: " << ptat;
	    cout << " PEC: " << (unsigned) pec << endl;

	    //Clear Vector
	    VtemperatureData.clear();
#endif
            //Delay
	    upm_delay_ms(sleep_time);

    }

    return 0;
}

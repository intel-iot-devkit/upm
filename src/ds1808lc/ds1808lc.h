/*
 * Author: Henry Bruce <henry.bruce@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "upm/iLightController.h"
#include "mraa/i2c.h"

namespace upm
{
/**
 * @brief DS1808LC lighting controller library
 * @defgroup DS1808LC libupm-ds1808lc
 */

/**
 * @brief C++ API for DS1808 Dual Log Digital Potentiometer as a Light Controller
 *
 * The Dallas Semiconductor
 * [DS1808](http://www.maximintegrated.com/en/products/analog/data-converters/digital-potentiometers/DS1808.html)
 * Dual Log Digital Potentiometer
 *
 * @ingroup ds1808lc i2c
 * @snippet ds1808lc.cxx Interesting
 */
class DS1808LC : public upm::ILightController
{
public:
   DS1808LC(int gpioPower, int i2cBus);
   ~DS1808LC();

protected:
   bool isConfigured();
   bool isPowered();
   bool setPowerOn();
   bool setPowerOff();
   bool getBrightness(int* percent);
   bool setBrightness(int dutyPercent);
   bool getBrightnessRange(int* percentMin, int* percentMax);

private:
   int getPercentBrightness(uint8_t val1, uint8_t val2);
   uint8_t getPot1Value(int dutyPercent);
   uint8_t getPot2Value(int dutyPercent);
   int getScaledResistance(int dutyPercent);

   mraa_result_t status;
   mraa_i2c_context i2c;
   int pinPower;
   bool m_isPowered;
};


}


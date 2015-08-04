/*
 * The MIT License (MIT)
 *
 * Author: Daniel Mosquera
 * Copyright (c) 2013 Daniel Mosquera
 *
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * Contributions: Jon Trulson <jtrulson@ics.com>
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

#include <string>
#include <unistd.h>

#include "hd44780_bits.h"
#include "lcm1602.h"

using namespace upm;

Lcm1602::Lcm1602(int bus_in, int addr_in) : 
  m_i2c_lcd_control(new mraa::I2c(bus_in)),
  m_gpioRS(0), m_gpioEnable(0), m_gpioD0(0),
  m_gpioD1(0), m_gpioD2(0), m_gpioD3(0)
{
    mraa_result_t error = MRAA_SUCCESS;
    m_name = "Lcm1602 (I2C)";
    m_isI2C = true;

    m_lcd_control_address = addr_in;

    error = m_i2c_lcd_control->address(m_lcd_control_address);
    if (error != MRAA_SUCCESS) {
        fprintf(stderr, "Failed to initialize i2c bus\n");
        return;
    }

    usleep(50000);
    expandWrite(LCD_BACKLIGHT);
    usleep(100000);

    write4bits(0x03 << 4);
    usleep(4500);
    write4bits(0x30);
    usleep(4500);
    write4bits(0x30);
    usleep(150);

    // Put into 4 bit mode
    write4bits(0x20);

    // Set numeber of lines
    send(LCD_FUNCTIONSET | 0x0f, 0);
    send(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF, 0);
    clear();

    // Set entry mode.
    send(LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT, 0);

    home();
}

Lcm1602::Lcm1602(uint8_t rs,  uint8_t enable, uint8_t d0, 
                 uint8_t d1, uint8_t d2, uint8_t d3) :
  m_i2c_lcd_control(0),  
  m_gpioRS(new mraa::Gpio(rs)), m_gpioEnable(new mraa::Gpio(enable)), 
  m_gpioD0(new mraa::Gpio(d0)), m_gpioD1(new mraa::Gpio(d1)),
  m_gpioD2(new mraa::Gpio(d2)), m_gpioD3(new mraa::Gpio(d3))
{
    mraa_result_t error = MRAA_SUCCESS;
    m_name = "Lcm1602 (4-bit GPIO)";
    m_isI2C = false;

    // setup our gpios

    m_gpioRS->dir(mraa::DIR_OUT);
    m_gpioEnable->dir(mraa::DIR_OUT);

    m_gpioD0->dir(mraa::DIR_OUT);
    m_gpioD1->dir(mraa::DIR_OUT);
    m_gpioD2->dir(mraa::DIR_OUT);
    m_gpioD3->dir(mraa::DIR_OUT);


    // set RS and Enable low to begin issuing commands
    m_gpioRS->write(0);
    m_gpioEnable->write(0);

    // wait to stabilize
    usleep(100000);

    // set 4bit mode

    // These steps are adapted from the HD44780 datasheet, figure 24

    // try 1
    write4bits(0x03);
    usleep(4500);

    // try 2
    write4bits(0x03);
    usleep(4500);

    // try 3
    write4bits(0x03);
    usleep(150);

    // Finally, put into 4 bit mode
    write4bits(0x02);

    // Set number of lines
    send(LCD_FUNCTIONSET | LCD_2LINE | LCD_4BITMODE | LCD_5x8DOTS, 0);
    send(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF, 0);
    usleep(2000);
    clear();

    // Set entry mode.
    send(LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT, 0);

    home();
}

Lcm1602::~Lcm1602()
{
  // clean up after ourselves
  if (m_isI2C)
    {
      delete m_i2c_lcd_control;
    }
  else
    {
      delete m_gpioRS;
      delete m_gpioEnable;

      delete m_gpioD0;
      delete m_gpioD1;
      delete m_gpioD2;
      delete m_gpioD3;
    }
}

/*
 * **************
 *  virtual area
 * **************
 */
mraa_result_t
Lcm1602::write(std::string msg)
{
    mraa_result_t error = MRAA_SUCCESS;
    for (std::string::size_type i = 0; i < msg.size(); ++i) {
        error = send(msg[i], LCD_RS);
    }
    return error;
}

mraa_result_t
Lcm1602::setCursor(int row, int column)
{
    mraa_result_t error = MRAA_SUCCESS;

    int row_addr[] = { 0x80, 0xc0, 0x14, 0x54 };
    uint8_t offset = ((column % 16) + row_addr[row]);

    return send(LCD_CMD | offset, 0);
}

mraa_result_t
Lcm1602::clear()
{
    mraa_result_t ret;
    ret = send(LCD_CLEARDISPLAY, 0);
    usleep(2000); // this command takes awhile
    return ret;
}

mraa_result_t
Lcm1602::home()
{
    mraa_result_t ret;
    ret = send(LCD_RETURNHOME, 0);
    usleep(2000); // this command takes awhile
    return ret;
}

mraa_result_t
Lcm1602::createChar(uint8_t charSlot, uint8_t charData[])
{
    mraa_result_t error = MRAA_SUCCESS;
    charSlot &= 0x07; // only have 8 positions we can set
    error = send(LCD_SETCGRAMADDR | (charSlot << 3), 0);
    if (error == MRAA_SUCCESS) {
        for (int i = 0; i < 8; i++) {
          error = send(charData[i], LCD_RS);
        }
    }

    return error;
}

/*
 * **************
 *  private area
 * **************
 */
mraa_result_t
Lcm1602::send(uint8_t value, int mode)
{
    mraa_result_t ret = MRAA_SUCCESS;
    uint8_t h;
    uint8_t l;

    if (m_isI2C)
      {
        h = value & 0xf0;
        l = (value << 4) & 0xf0;
        ret = write4bits(h | mode);
        ret = write4bits(l | mode);
        return ret;
      }

    // else, gpio (4 bit)

    // register select
    m_gpioRS->write(mode);
    
    h = value >> 4;
    l = value & 0x0f;

    ret = write4bits(h);
    ret = write4bits(l);
    return ret;
}

mraa_result_t
Lcm1602::write4bits(uint8_t value)
{
    mraa_result_t ret = MRAA_SUCCESS;

    if (m_isI2C)
      {
        ret = expandWrite(value);
        ret = pulseEnable(value);
        return ret;
      }

    // else gpio
    ret = m_gpioD0->write( ((value >> 0) & 0x01) );
    ret = m_gpioD1->write( ((value >> 1) & 0x01) );
    ret = m_gpioD2->write( ((value >> 2) & 0x01) );
    ret = m_gpioD3->write( ((value >> 3) & 0x01) );

    ret = pulseEnable(value); // value is ignored here for gpio

    return ret;
}

mraa_result_t
Lcm1602::expandWrite(uint8_t value)
{
    // invalid for gpio
    if (!m_isI2C)
        return MRAA_ERROR_INVALID_RESOURCE;

    uint8_t buffer = value | LCD_BACKLIGHT;
    return m_i2c_lcd_control->writeByte(buffer);
}

mraa_result_t
Lcm1602::pulseEnable(uint8_t value)
{
    mraa_result_t ret = MRAA_SUCCESS;

    if (m_isI2C)
      {
        ret = expandWrite(value | LCD_EN);
        usleep(1);
        ret = expandWrite(value & ~LCD_EN);
        usleep(50);
        return ret;
      }

    // else gpio

    ret = m_gpioEnable->write(0);
    usleep(1);
    ret = m_gpioEnable->write(1);
    usleep(1); // must be > 450ns
    ret = m_gpioEnable->write(0);
    usleep(100); // must be >37us

    return ret;
}

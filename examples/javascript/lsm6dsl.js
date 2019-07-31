/*
 * Author: Jon Trulson <jtrulson@ics.com>
 * Copyright (c) 2016-2017 Intel Corporation.
 *
 * This program and the accompanying materials are made available under the
 * terms of the The MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 */

var sensorObj = require('jsupm_lsm6dsl');

// Instantiate a LSM6DSL instance using default i2c bus and address
var sensor = new sensorObj.LSM6DSL();

// For SPI, bus 0, you would pass -1 as the address, and a valid pin for CS:
// LSM6DSL(0, -1, 10);

// now output data every 250 milliseconds
setInterval(function()
{
    // update our values from the sensor
    sensor.update();

    var data = sensor.getAccelerometer();
    console.log("Accelerometer x: "
                + data.get(0)
                + " y: " + data.get(1)
                + " z: " + data.get(2)
                + " g");

    data = sensor.getGyroscope();
    console.log("Gyroscope x: "
                + data.get(0)
                + " y: " + data.get(1)
                + " z: " + data.get(2)
                + " dps");

    // we show both C and F for temperature
    console.log("Compensation Temperature: "
                + sensor.getTemperature()
                + " C / "
                + sensor.getTemperature(true)
                + " F");

    console.log();

}, 250);

// exit on ^C
process.on('SIGINT', function()
{
    sensor = null;
    sensorObj.cleanUp();
    sensorObj = null;
    console.log("Exiting.");
    process.exit(0);
});

/*
 * Author: Stefan Andritoiu <stefan.andritoiu@intel.com>
 * Copyright (c) 2015 Intel Corporation.
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

public class Adxl345 {
    static {
        try {
            System.loadLibrary("javaupm_adxl345");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("error in loading native library");
            System.exit(-1);
        }
    }

    public static void main(String argv[]) throws InterruptedException {
        upm_adxl345.Adxl345 obj = new upm_adxl345.Adxl345(0);
        int[] raw = new int[3];
        float[] accel = new float[3];

        while (true) {
            obj.update();
            raw = obj.getRawValues();
            accel = obj.getAcceleration();

            System.out.println("raw data: " + raw[0] + " " + raw[1] + " " +
                    raw[2]);
            System.out.println("accel data: " + accel[0] + " " + accel[1] + " "
                    + accel[2]);

            Thread.sleep(1000);
        }
    }
}

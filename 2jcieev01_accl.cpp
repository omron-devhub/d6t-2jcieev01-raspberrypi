/*
 * MIT License
 * Copyright (c) 2019, 2018 - present OMRON Corporation
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include "lis2dw.h"

#define LIS2DW_DEVICEID     0x44
#define LIS2DW_CONV(x) ((double)(x) * 4000.0 / 32767.0)

#define PIN_CSB 8

bool lis2dw_setup(void) {
    uint8_t outdat[6] = {0};

    int fds  ;
    fds = wiringPiSPISetup(0,500000);
    if(fds < 0 ) {
       printf("SPI NG\n");
    }

    outdat[0] = 0x0F;
    outdat[0] |= 0x80;
    pinMode(PIN_CSB,OUTPUT);
    digitalWrite(PIN_CSB,HIGH);
    digitalWrite(PIN_CSB,LOW);
    wiringPiSPIDataRW(0,outdat,2);
    digitalWrite(PIN_CSB,HIGH);
//    printf("SPI WHO AM %X\n",outdat[1]);

    outdat[0]=0x20;
    outdat[1]=0x54;
    outdat[2]=0x06;
    outdat[3]=0x00;
    outdat[4]=0x00;
    outdat[5]=0x00;
    outdat[6]=0x14;

    digitalWrite(PIN_CSB,LOW);
    wiringPiSPIDataRW(0,outdat,7);
    digitalWrite(PIN_CSB,HIGH);
    return false;
}

bool lis2dw_read(int16_t* accl) {
    uint8_t outdat[7] = {0};

    outdat[0]=0x28;
    outdat[0] |= 0x80;
    digitalWrite(PIN_CSB,LOW);
    wiringPiSPIDataRW(0,outdat,7);
    digitalWrite(PIN_CSB,HIGH);
    accl[0] = (int16_t)(((int16_t)outdat[2] << 8) | outdat[1]);
    accl[1] = (int16_t)(((int16_t)outdat[4] << 8) | outdat[3]);
    accl[2] = (int16_t)(((int16_t)outdat[6] << 8) | outdat[5]);
    return false;
}






/** <!-- main - humidity sensor {{{1 -->
 * 1. setup sensor
 * 2. output results, format is: [mg]
 */
int main() {
    int16_t accl[3];
    double mg[3];

    if (lis2dw_setup()) {
        return 1;
    }
    if (lis2dw_read(accl)) {
        return 2;
    }
    mg[0] = LIS2DW_CONV(accl[0]);  // x
    mg[1] = LIS2DW_CONV(accl[1]);  // y
    mg[2] = LIS2DW_CONV(accl[2]);  // z
    printf("%7.1f, %7.1f, %7.1f\n", mg[0], mg[1], mg[2]);
    #if defined(OUTPUT_RAW)  // raw output
    printf("%7.1f, %7.1f, %7.1f\n", accl[0], accl[1], accl[2]);
    #endif
    return 0;
}
// vi: ft=arduino:fdm=marker:et:sw=4:tw=80

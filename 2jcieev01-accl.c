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

#define LIS2DW_FIFO_SIZE   1
#define LIS2DW_VAL_DEVICEID 0x44
#define LIS2DW_CONV(x) ((double)(x) * 4000.0 / 32767.0)

#define PIN_CSB 8

#define conv8s_s16_le(b, n) ((int16_t)b[n] | ((int16_t)b[n + 1] << 8))

/** <!-- lis2dw_setup {{{1 --> setup a accerelometer sensor.
 */
bool lis2dw_setup(void) {
    uint32_t retry = 100;
    uint8_t wbuf[8] = {0};

    int fds;
    fds = wiringPiSPISetup(0, 500000);
    if (fds < 0) {
       printf("SPI NG\n");
       return true;
    }

    pinMode(PIN_CSB, OUTPUT);
    digitalWrite(PIN_CSB, HIGH);

    /* Check connection */
    while ((wbuf[1] != LIS2DW_VAL_DEVICEID) && (retry > 0)) {
        wbuf[0] = LIS2DW_REG_WHOAMI;
        digitalWrite(PIN_CSB, LOW);
        wiringPiSPIDataRW(0, wbuf, 2);
        digitalWrite(PIN_CSB, HIGH);
        retry--;
    }
    if (retry <= 0) {
        printf("cannot find LIS2DW on SPI-bus.");
        return true;
    }

    wbuf[0] = LIS2DW_REG_CTRL1;
    wbuf[1] = 0x54;   // REG1: 100Hz, High-Performance
    wbuf[2] = 0x06;   // REG2:
    wbuf[3] = 0x00;   // REG3:
    wbuf[4] = 0x00;   // REG4: INT1
    wbuf[5] = 0x00;   // REG5: INT2
    wbuf[6] = 0x14;   // REG6: FS 4g

    digitalWrite(PIN_CSB, LOW);
    wiringPiSPIDataRW(0, wbuf, 7);
    digitalWrite(PIN_CSB, HIGH);
    return false;
}

/** <!-- lis2dw_read_and_avg {{{1 --> get accerelo values from FIFO and
 * make average values.
 */
bool lis2dw_read_and_avg(int16_t* accl) {
    uint8_t accbuf[1 + 3 * 2 * LIS2DW_FIFO_SIZE] = {0};
    int32_t accsum[3] = {0, 0, 0};

    accbuf[0] = LIS2DW_REG_OUT_X_L;
    digitalWrite(PIN_CSB, LOW);
    wiringPiSPIDataRW(0, accbuf, sizeof(accbuf));
    digitalWrite(PIN_CSB, HIGH);
    for (uint8_t i = 0; i < LIS2DW_FIFO_SIZE; i++) {
        int n = i * 6;
        accsum[0] += (int32_t)conv8s_s16_le(accbuf, n + 1);  // 1=skip addr.
        accsum[1] += (int32_t)conv8s_s16_le(accbuf, n + 3);
        accsum[2] += (int32_t)conv8s_s16_le(accbuf, n + 5);
    }
    accl[0] = (int16_t)(accsum[0] / LIS2DW_FIFO_SIZE);
    accl[1] = (int16_t)(accsum[1] / LIS2DW_FIFO_SIZE);
    accl[2] = (int16_t)(accsum[2] / LIS2DW_FIFO_SIZE);
    return false;
}


/** <!-- main - accelerometer sensor {{{1 -->
 * 1. setup sensor
 * 2. output results, format is: [mg]
 */
int main() {
    int16_t accl[3];
    double mg[3];

    if (lis2dw_setup()) {
        return 1;
    }
    delay(100);
    if (lis2dw_read_and_avg(accl)) {
        return 2;
    }
    mg[0] = LIS2DW_CONV(accl[0]);  // x
    mg[1] = LIS2DW_CONV(accl[1]);  // y
    mg[2] = LIS2DW_CONV(accl[2]);  // z
    printf("%7.1f, %7.1f, %7.1f\n", mg[0], mg[1], mg[2]);
    #if defined(OUTPUT_RAW)  // raw output
    printf("%7.1f, %7.1f, %7.1f\n", accl[0], accl[1], accl[2]);
    #endif
    pinModeAlt(PIN_CSB, 0b100);
    return 0;
}
// vi: ft=arduino:fdm=marker:et:sw=4:tw=80

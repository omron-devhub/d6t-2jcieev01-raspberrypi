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
/* includes */
#include "baro_2smpb02e.h"

/* defines */
#define AP_2SMPB02E_CHIP_ID     0x5C

/* values */
ap_2smpb02e_setting_t ap_2smpb02e_setting;

/* macros */
#define conv8s_s24_be(a, b, c) \
        (int32_t)((((uint32_t)a << 16) & 0x00FF0000) | \
                  (((uint32_t)b << 8) & 0x0000FF00) | \
                   ((uint32_t)c & 0x000000FF))

#define ap_halt(a) {fprintf(stderr, a); return true;}

/* functions */
static double ap_2smpb02e_conv16_dbl(
        double a, double s, uint8_t* buf, int offset);
static double ap_2smpb02e_conv20q4_dbl(uint8_t* buf, uint8_t ex, int offset);
static bool ap_2smpb02e_trigger_measurement(uint8_t mode);

#define RASPBERRY_PI_I2C    "/dev/i2c-1"
#define I2CDEV              RASPBERRY_PI_I2C

int fd ;

uint32_t i2c_write_reg8(uint8_t devAddr, uint8_t regAddr, uint8_t* data , uint8_t length) {
    int8_t count = 0;
    uint8_t buf[128];
    if (length > 127) {
        fprintf(stderr, "Byte write count (%d) > 127\n", length);
        return(1);
    }

    fd = open(I2CDEV , O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
        return(1);
    }
    if (ioctl(fd, I2C_SLAVE, devAddr) < 0) {
        fprintf(stderr, "Failed to select device: %s\n", strerror(errno));
        close(fd);
        return(1);
    }
    buf[0] = regAddr;
    if(length > 0 ) {
        memcpy(buf+1,data,length);
    }
    count = write(fd, buf, length+1);
    if (count < 0) {
        fprintf(stderr, "Failed to write device(%d): %s\n", count, strerror(errno));
        close(fd);
        return(1);
    } else if (count != length+1) {
        fprintf(stderr, "Short write to device, expected %d, got %d\n", length+1, count);
        close(fd);
        return(1);
    }
    close(fd);

    return 0;
}

uint32_t i2c_read_reg8(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t length) {
    int8_t count = 0;
    fd = open(I2CDEV, O_RDWR);

    if (fd < 0) {
        fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
        return(1);
    }
    if (ioctl(fd, I2C_SLAVE, devAddr) < 0) {
        fprintf(stderr, "Failed to select device: %s\n", strerror(errno));
        close(fd);
        return(1);
    }
    if (write(fd, &regAddr, 1) != 1) {
        fprintf(stderr, "Failed to write reg: %s\n", strerror(errno));
        close(fd);
        return(1);
    }
    count = read(fd, data, length);

    if (count < 0) {
        fprintf(stderr, "Failed to read device(%d): %s\n", count, strerror(errno));
        close(fd);
        return(1);
    } else if (count != length) {
        fprintf(stderr, "Short read  from device, expected %d, got %d\n", length, count);
        close(fd);
        return(1);
    }
    close(fd);

    return 0;
}


/** <!-- ap_2smpb02e_setup {{{1 --> setup for 2SMPB-02E
 * 1. check CHIP_ID to confirm I2C connections.
 * 2. read coefficient values for compensations.
 * 3. sensor setup and start to measurements.
 */
bool ap_2smpb02e_setup(void) {
    bool result;
    uint8_t rbuf[32] = {0};
    uint8_t ex;

    // 1.
    result = i2c_read_reg8(AP_2SMPB02E_ADDRESS,
                           AP_2SMPB02E_REGI2C_CHIP_ID, rbuf, 1);
    if (result || rbuf[0] != AP_2SMPB02E_CHIP_ID) {
        ap_halt("cannot find 2SMPB-02E sensor, halted...");
    }

    // 2.
    result = i2c_read_reg8(AP_2SMPB02E_ADDRESS,
            AP_2SMPB02E_REGI2C_COEFS, rbuf, 25);
    if (result) {
        ap_halt("failed to read 2SMPB-02E coeffients, halted...");
    }

    // pressure parameters
    ex = (rbuf[24] & 0xf0) >> 4;
    ap_2smpb02e_setting._B00 = ap_2smpb02e_conv20q4_dbl(rbuf, ex, 0);
    ap_2smpb02e_setting._BT1 = ap_2smpb02e_conv16_dbl(
            AP_2SMPB02E_COEFF_A_BT1, AP_2SMPB02E_COEFF_S_BT1, rbuf, 2);
    ap_2smpb02e_setting._BT2 = ap_2smpb02e_conv16_dbl(
            AP_2SMPB02E_COEFF_A_BT2, AP_2SMPB02E_COEFF_S_BT2, rbuf, 4);
    ap_2smpb02e_setting._BP1 = ap_2smpb02e_conv16_dbl(
            AP_2SMPB02E_COEFF_A_BP1, AP_2SMPB02E_COEFF_S_BP1, rbuf, 6);
    ap_2smpb02e_setting._B11 = ap_2smpb02e_conv16_dbl(
            AP_2SMPB02E_COEFF_A_B11, AP_2SMPB02E_COEFF_S_B11, rbuf, 8);
    ap_2smpb02e_setting._BP2 = ap_2smpb02e_conv16_dbl(
            AP_2SMPB02E_COEFF_A_BP2, AP_2SMPB02E_COEFF_S_BP2, rbuf, 10);
    ap_2smpb02e_setting._B12 = ap_2smpb02e_conv16_dbl(
            AP_2SMPB02E_COEFF_A_B12, AP_2SMPB02E_COEFF_S_B12, rbuf, 12);
    ap_2smpb02e_setting._B21 = ap_2smpb02e_conv16_dbl(
            AP_2SMPB02E_COEFF_A_B21, AP_2SMPB02E_COEFF_S_B21, rbuf, 14);
    ap_2smpb02e_setting._BP3 = ap_2smpb02e_conv16_dbl(
            AP_2SMPB02E_COEFF_A_BP3, AP_2SMPB02E_COEFF_S_BP3, rbuf, 16);

    // temperature parameters
    ex = (rbuf[24] & 0x0f);
    ap_2smpb02e_setting._A0 = ap_2smpb02e_conv20q4_dbl(rbuf, ex, 18);
    ap_2smpb02e_setting._A1 = ap_2smpb02e_conv16_dbl(
            AP_2SMPB02E_COEFF_A_A1, AP_2SMPB02E_COEFF_S_A1, rbuf, 20);
    ap_2smpb02e_setting._A2 = ap_2smpb02e_conv16_dbl(
            AP_2SMPB02E_COEFF_A_A2, AP_2SMPB02E_COEFF_S_A2, rbuf, 22);

    // 3. setup a sensor at 125msec sampling and 32-IIR filter.
    rbuf[0] = AP_2SMPB02E_VAL_IOSETUP_STANDBY_0125MS;
    i2c_write_reg8(AP_2SMPB02E_ADDRESS, AP_2SMPB02E_REGI2C_IO_SETUP,
                   rbuf, sizeof(rbuf));

    rbuf[0] = AP_2SMPB02E_VAL_IIR_32TIMES;
    i2c_write_reg8(AP_2SMPB02E_ADDRESS, AP_2SMPB02E_REGI2C_IIR,
                   rbuf, sizeof(rbuf));

    // then, start to measurements.
    result = ap_2smpb02e_trigger_measurement(
            AP_2SMPB02E_VAL_MEASMODE_ULTRAHIGH);
    if (result) {
        ap_halt("failed to wake up 2SMPB-02E sensor, halted...");
    }
    return false;
}

/** <!-- ap_2smpb02e_conv16_dbl {{{1 --> convert bytes buffer to double.
 * bytes buffer format is a signed-16bit Big-Endian.
 */
static double ap_2smpb02e_conv16_dbl(double a, double s,
                                     uint8_t* buf, int offset) {
    uint16_t val;
    int16_t ret;

    val = (uint16_t)(
            (uint16_t)(buf[offset] << 8) | (uint16_t)buf[offset + 1]);
    if ((val & 0x8000) != 0) {
        ret = (int16_t)((int32_t)val - 0x10000);
    } else {
        ret = val;
    }
    return a + (double)ret * s / 32767.0;
}

/** <!-- ap_2smpb02e_conv20q4_dbl {{{1 --> convert bytes buffer to double.
 * bytes buffer format is signed 20Q4, from -32768.0 to 32767.9375
 *
 * ### bit field of 20Q4
 * ```
 * |19,18,17,16|15,14,13,12|11,10, 9, 8| 7, 6, 5, 4| 3, 2, 1, 0|
 * | buf[offset]           | buf[offset+1]         | ex        |
 *                                                 A
 *                                                 |
 *                                                 +-- Decimal point
 * ```
 */
static double ap_2smpb02e_conv20q4_dbl(uint8_t* buf, uint8_t ex, int offset) {
    int32_t ret;
    uint32_t val;

    val = (uint32_t)((buf[offset] << 12) | (buf[offset + 1] << 4) | ex);
    if ((val & 0x80000) != 0) {
        ret = (int32_t)val - 0x100000;
    } else {
        ret = val;
    }
    return (double)ret / 16.0;
}

/** <!-- ap_2smpb02e_trigger_measurement {{{1 --> start the sensor
 */
static bool ap_2smpb02e_trigger_measurement(uint8_t mode) {
    uint8_t wbuf[1] = {
        (uint8_t)(mode | AP_2SMPB02E_VAL_POWERMODE_NORMAL)};

    i2c_write_reg8(AP_2SMPB02E_ADDRESS, AP_2SMPB02E_REGI2C_CTRL_MEAS,
                   wbuf, sizeof(wbuf));
    return false;
}

/** <!-- ap_2smpb02e_read {{{1 --> read the sensor digit and convert to
 * physical values.
 */
bool ap_2smpb02e_read(uint32_t* pres, int16_t* temp,
                      uint32_t* dp, uint32_t* dt) {
    bool ret;
    uint8_t rbuf[6] = {0};
    uint32_t rawtemp, rawpres;

    ret = i2c_read_reg8(
            AP_2SMPB02E_ADDRESS, AP_2SMPB02E_REGI2C_PRES_TXD2,
            rbuf, sizeof(rbuf));
    if (ret) {
        return true;
    }

    *dp = rawpres = conv8s_s24_be(rbuf[0], rbuf[1], rbuf[2]);
    *dt = rawtemp = conv8s_s24_be(rbuf[3], rbuf[4], rbuf[5]);
    return ap_2smpb02e_output_compensation(rawtemp, rawpres, pres, temp);
}

/** <!-- ap_2smpb02e_output_compensation {{{1 --> compensate sensors
 * raw output digits to [Pa] and [degC].
 */
bool ap_2smpb02e_output_compensation(uint32_t raw_temp_val,
                                     uint32_t raw_press_val,
                                     uint32_t* pres, int16_t* temp
) {
    double Tr, Po;
    double Dt, Dp;

    Dt = (int32_t)raw_temp_val - 0x800000;
    Dp = (int32_t)raw_press_val - 0x800000;

    // temperature compensation
    ap_2smpb02e_setting_t* c = &ap_2smpb02e_setting;
    Tr = c->_A0 + c->_A1 * Dt + c->_A2 * (Dt * Dt);

    // barometer compensation
    Po = c->_B00 + (c->_BT1 * Tr) + (c->_BP1 * Dp) +
         (c->_B11 * Tr * Dp) + c->_BT2 * (Tr * Tr) +
         (c->_BP2 * (Dp * Dp)) + (c->_B12 * Dp * (Tr * Tr)) +
         (c->_B21 * (Dp * Dp) * Tr) + (c->_BP3 * (Dp * Dp * Dp));

    *temp = (int16_t)(Tr / 2.56);     // x100degC
    *pres = (uint32_t)(Po * 10.0);    // x10Pa
    return false;
}


/** <!-- main - barometer sensor {{{1 -->
 * 1. setup sensor
 * 2. output results, format is: [Pa],[degC],[digit],[digit]
 */
int main() {
    uint32_t pres;
    int16_t temp;
    uint32_t dp, dt;

    // i2c_setup();
    if (ap_2smpb02e_setup()) {
        return 1;
    }
    delay(100);
    ap_2smpb02e_read(&pres, &temp, &dp, &dt);
    printf("%10.1f, %7.3f, %x, %x\n", pres / 10.0, temp / 100.0, dp, dt);
    return 0;
}
// vi: ft=arduino:fdm=marker:et:sw=4:tw=80
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
#include "sht30.h"

#define RASPBERRY_PI_I2C    "/dev/i2c-1"
#define I2CDEV              RASPBERRY_PI_I2C

#define conv8s_u16_be(b, n) \
    (uint16_t)(((uint16_t)b[n] << 8) | (uint16_t)b[n + 1])


uint32_t i2c_write_reg16(uint8_t devAddr, uint16_t regAddr,
                         uint8_t* data , uint8_t length
) {
    uint8_t buf[128];
    if (length > 127) {
        fprintf(stderr, "Byte write count (%d) > 127\n", length);
        return 11;
    }

    int fd = open(I2CDEV , O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
        return 12;
    }
    int err = 0;
    do {
        if (ioctl(fd, I2C_SLAVE, devAddr) < 0) {
            fprintf(stderr, "Failed to select device: %s\n", strerror(errno));
            err = 13; break;
        }
        buf[0] = regAddr >> 8;
        buf[1] = regAddr & 0xFF;
        if (length > 0) {
            memcpy(buf + 2, data, length);
        }
        length += 2;
        int count = write(fd, buf, length);
        if (count < 0) {
            fprintf(stderr, "Failed to write device(%d): %s\n",
                    count, strerror(errno));
            err = 14; break;
        } else if (count != length) {
            fprintf(stderr, "Short write to device, expected %d, got %d\n",
                    length, count);
            err = 15; break;
        }
    } while (false);
    close(fd);
    return err;
}

/** <!-- i2c_read_reg16 {{{1 --> I2C read function for bytes transfer.
 */
uint32_t i2c_read_reg16(uint8_t devAddr, uint16_t regAddr,
                        uint8_t *data, uint8_t length
) {
    int fd = open(I2CDEV, O_RDWR);

    if (fd < 0) {
        fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
        return 21;
    }
    int err = 0;
    do {
        if (ioctl(fd, I2C_SLAVE, devAddr) < 0) {
            fprintf(stderr, "Failed to select device: %s\n", strerror(errno));
            err = 22; break;
        }
        uint8_t buf[2] = {regAddr >> 8, regAddr & 0xFF};
        if (write(fd, buf, 2) != 2) {
            fprintf(stderr, "Failed to write reg: %s\n", strerror(errno));
            err = 23; break;
        }
        int count = read(fd, data, length);
        if (count < 0) {
            fprintf(stderr, "Failed to read device(%d): %s\n",
                    count, strerror(errno));
            err = 24; break;
        } else if (count != length) {
            fprintf(stderr, "Short read  from device, expected %d, got %d\n",
                    length, count);
            err = 25; break;
        }
    } while (false);
    close(fd);
    return err;
}

static uint32_t sht30_write_verify_user_register(void);
static uint32_t sht30_read_triggered_value(
        uint16_t *value_T_raw,
        uint16_t *value_RH_raw);
static int32_t sht30_convert_temperature_value_x100(uint16_t value_raw);
static int32_t sht30_convert_humidity_value_x100(uint16_t value_raw);

/** <!-- sht30_setup {{{1 --> setup a humidity sensor.
 */
bool sht30_setup(void) {
    int ret = sht30_write_verify_user_register();
    if (ret) {
        printf("sht NG=>%d", ret);
        return true;
    }
    return false;
}

/** <!-- sht30_write_verify_user_register {{{1 --> start to measurement.
 */
static uint32_t sht30_write_verify_user_register(void) {
    uint32_t result;
    uint8_t read_buff[3];

    /* Clear status register */
    result = i2c_write_reg16(SHT30_SLAVE_ADDR, SHT30_CLEAR_STATUS, NULL, 0);
    if (result) {
        return result;
    }

    /* Read Out of status register */
    int retry = 10;
    do {
        result = i2c_read_reg16(SHT30_SLAVE_ADDR, SHT30_READ_STATUS,
                                read_buff, 3);
        delay(10);

    /* Read Data Check */
    } while ((result || (read_buff[0] != 0x00) || (read_buff[1] != 0x00)) &&
             (retry-- > 0));

    if (result) {return result;}
    if (retry) {return 31;}

    /* Measurement Commands for Periodic Data Acquisition Mode */
    do {
        result = i2c_write_reg16(SHT30_SLAVE_ADDR, SHT30_CMD_MEASURE, NULL, 0);
        delay(5);

    /* Read Data Check */
    } while (result && (retry-- > 0));

    if (result) {return 100 + result;}
    if (retry) {return 32;}
    return 0;
}

/** <!-- sht30_read_triggered_TRH_x100 {{{1 --> read raw digits and
 * convert them to physical values.
 */
int32_t sht30_read_triggered_TRH_x100(int32_t *value_T, int32_t *value_RH) {
    int32_t result;
    uint16_t value_T_raw = 0;
    uint16_t value_RH_raw = 0;

    /* Measure the temperature value */
    result = sht30_read_triggered_value(&value_T_raw, &value_RH_raw);
    if (result) {
        return result;
    }

    /* Convert the value to centidegrees Celsius */
    *value_T = sht30_convert_temperature_value_x100(value_T_raw);
    *value_RH = sht30_convert_humidity_value_x100(value_RH_raw);
    return 0;
}

/** <!-- sht30_read_triggered_value {{{1 --> just read sensor raw values.
 */
static uint32_t sht30_read_triggered_value(
    uint16_t *value_T_raw,
    uint16_t *value_RH_raw) {
    uint32_t result;
    uint8_t read_buff[6];

    result = i2c_read_reg16(SHT30_SLAVE_ADDR,
                            SHT30_READ_MEASURE, read_buff, 6);
    if (result) {
        return result;
    }

    *value_T_raw = conv8s_u16_be(read_buff, 0);
    *value_RH_raw = conv8s_u16_be(read_buff, 3);
    return 0;
}

/** <!-- sht30_convert_humidity_value_x100 {{1 --> convert raw digit to
 * physical value [x100]
 */
static int32_t sht30_convert_humidity_value_x100(uint16_t value_raw) {
    int32_t value_converted = 0;

    /* Convert the value to centi-percent RH */
    value_converted = (((int32_t)value_raw * 2500) >> 14);
    return (int32_t)value_converted;
}

/** <!-- sht30_convert_temperature_value_x100 {{1 --> convert raw digit to
 * physical value [x100]
 */
static int32_t sht30_convert_temperature_value_x100(uint16_t value_raw) {
    int32_t value_converted = 0;

    /* Convert the value to centi-degC */
    value_converted = (((int32_t)value_raw * 4375) >> 14) - 4500;
    return (int32_t)value_converted;
}

/** <!-- main - humidity sensor {{{1 -->
 * 1. setup sensor
 * 2. output results, format is: [%RH], [degC]
 */
int main() {
    int32_t humi, temp;

    if (sht30_setup()) {
        return 1;
    }
    delay(100);
    int ret = sht30_read_triggered_TRH_x100(&temp, &humi);
    printf("%5.2f, %7.1f, return code:%d\n",
           (double)humi / 100.0, (double)temp / 100.0, ret);
    return 0;
}
// vi: ft=arduino:fdm=marker:et:sw=4:tw=80

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
/** @Includes ----------------------------------------------------------------*/
#include "sht30.h"

#define RASPBERRY_PI_I2C    "/dev/i2c-1"
#define I2CDEV              RASPBERRY_PI_I2C

int fd ;

uint32_t i2c_write_reg16(uint8_t devAddr, uint16_t regAddr, uint8_t* data , uint8_t length) {
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
    uint8_t regAddr2 = regAddr >> 8;
    buf[0] = regAddr2 ;
//    printf("buf0:%X\n",buf[0]);
    regAddr2 = regAddr & 0x00FF;
    buf[1] = regAddr2 ;
//    printf("buf1:%X\n",buf[1]);
    if(length > 0 ) {
        memcpy(buf+2,data,length);
    }
    count = write(fd, buf, length+2);
    if (count < 0) {
        fprintf(stderr, "Failed to write device(%d): %s\n", count, strerror(errno));
        close(fd);
        return(1);
    } else if (count != length+2) {
        fprintf(stderr, "Short write to device, expected %d, got %d\n", length+2, count);
        close(fd);
        return(1);
    }
    close(fd);

    return 0;

}

/*
bool i2c_write_8(uint8_t devAddr, uint8_t regAddr, uint8_t* data , uint8_t length) {
    if(wiringPiI2CWriteReg8(devAddr,regAddr,data) < 0 ) {
        printf("Error i2c write");
    }
    else return(TRUE);
}*/

uint32_t i2c_read_reg16(uint8_t devAddr, uint16_t regAddr, uint8_t *data, uint8_t length) {
  /*修正*/
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
    uint8_t regAddr2 = regAddr >> 8 ;   
    if (write(fd, &regAddr2, 1) != 1) {
        fprintf(stderr, "Failed to write reg: %s\n", strerror(errno));
        close(fd);
        return(1);
    }
    regAddr2 = regAddr ;
    if (write(fd, &regAddr2, 1) != 1) {
        fprintf(stderr, "Failed to write reg: %s\n", strerror(errno));
        close(fd);
        return(1);
    }
    count = read(fd, data, length);
    //printf("%d",regAddr);

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

#if !defined(BARO_SAMPLE)
// vi: ft=c:fdm=marker:et:sw=4:tw=80
#endif
static uint32_t sht30_write_verify_user_register(void);
static uint32_t sht30_read_triggered_value(
        uint16_t *value_T_raw,
        uint16_t *value_RH_raw);
static int32_t sht30_convert_temperature_value_x100(uint16_t value_raw);
static int32_t sht30_convert_humidity_value_x100(uint16_t value_raw);

/** <!-- sht30_setup {{{1 --> setup a humidity sensor.
 */
bool sht30_setup(void) {
    if (sht30_write_verify_user_register()) {
        printf("sht NG");
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
    uint8_t retry = 10;
    do {
        result = i2c_read_reg16(SHT30_SLAVE_ADDR, SHT30_READ_STATUS,
                                read_buff, 3);
        if (result) {
            return result;
        }
        retry--;

    /* Read Data Check */
    } while (
        result &&
        ((read_buff[0] != 0x00) ||
        (read_buff[1] != 0x00)) &&
        (retry > 0));

    if (retry == 0) {
        return result;
    }

    /* Measurement Commands for Periodic Data Acquisition Mode */
    do {
        result = i2c_write_reg16(SHT30_SLAVE_ADDR, SHT30_CMD_MASURE, NULL, 0);
        retry--;

    /* Read Data Check */
    } while (result && (retry > 0));

    if (retry == 0) {
        return result;
    }
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
    return result;
}

/** <!-- sht30_read_triggered_value {{{1 --> just read sensor raw values.
 */
static uint32_t sht30_read_triggered_value(
    uint16_t *value_T_raw,
    uint16_t *value_RH_raw) {
    uint32_t result;
    uint8_t read_buff[6];

    result = i2c_read_reg16(SHT30_SLAVE_ADDR, SHT30_READ_MASURE, read_buff, 6);
    if (result) {
        return result;
    }

    *value_T_raw = (uint16_t)(((uint16_t)read_buff[0] << 8) | read_buff[1]);
    *value_RH_raw = (uint16_t)(((uint16_t)read_buff[3] << 8) | read_buff[4]);
    return result;
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
    if (sht30_read_triggered_TRH_x100(&temp, &humi)) {
        return 2;
    }
    printf("%5.2f, %7.1f\n", (double)humi / 100.0, (double)temp / 100.0);
    return 0;
}
// vi: ft=arduino:fdm=marker:et:sw=4:tw=80

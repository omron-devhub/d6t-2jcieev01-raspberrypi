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
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdbool.h>
#include <time.h>
#include <linux/i2c.h> //add

/* defines */
#define D6T_ADDR 0x0A  // for I2C 7bit address
#define D6T_CMD 0x4D  // for D6T-32L-01A, compensated output.
#define D6T_SET_ADD 0x01

#define N_ROW 32
#define N_PIXEL (32 * 32)
#define N_READ ((N_PIXEL + 1) * 2 + 1)
#define RASPBERRY_PI_I2C    "/dev/i2c-1"
#define I2CDEV              RASPBERRY_PI_I2C

uint8_t rbuf[N_READ];
double ptat;
double pix_data[N_PIXEL];

/******* setting parameter *******/
#define D6T_IIR 0x00 
#define D6T_AVERAGE 0x04  
/*********************************/

/* I2C functions */
/** <!-- i2c_read_reg8 {{{1 --> I2C read function for bytes transfer.
 */
uint32_t i2c_read_reg8(uint8_t devAddr, uint8_t regAddr,
	uint8_t *data, int length
) {
	int fd = open(I2CDEV, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
		return 21;
	}
	int err = 0;
	do {
		struct i2c_msg messages[] = {
			{ devAddr, 0, 1, &regAddr },
			{ devAddr, I2C_M_RD, length, data },
		};
		struct i2c_rdwr_ioctl_data ioctl_data = { messages, 2 };
		if (ioctl(fd, I2C_RDWR, &ioctl_data) != 2) {
			fprintf(stderr, "i2c_read: failed to ioctl: %s\n", strerror(errno));
		}

	} while (false);
	close(fd); //change
	return err;
}

uint32_t i2c_write_reg8(uint8_t devAddr,
                        uint8_t *data, int length) {
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
        if (write(fd, data, length) != length) {
            fprintf(stderr, "Failed to write reg: %s\n", strerror(errno));
            err = 23; break;
        }
    } while (false);
    close(fd);
    return err;
}

uint8_t calc_crc(uint8_t data) {
	int index;
	uint8_t temp;
	for (index = 0; index < 8; index++) {
		temp = data;
		data <<= 1;
		if (temp & 0x80) { data ^= 0x07; }
	}
	return data;
}

/** <!-- D6T_checkPEC {{{ 1--> D6T PEC(Packet Error Check) calculation.
 * calculate the data sequence,
 * from an I2C Read client address (8bit) to thermal data end.
 */
bool D6T_checkPEC(uint8_t buf[], int n) {
	int i;
	uint8_t crc = calc_crc((D6T_ADDR << 1) | 1);  // I2C Read address (8bit)
	for (i = 0; i < n; i++) {
		crc = calc_crc(buf[i] ^ crc);
	}
	bool ret = crc != buf[n];
	if (ret) {
		fprintf(stderr,
			"PEC check failed: %02X(cal)-%02X(get)\n", crc, buf[n]);
	}
	return ret;
}

/** <!-- conv8us_s16_le {{{1 --> convert a 16bit data from the byte stream.
 */
int16_t conv8us_s16_le(uint8_t* buf, int n) {
    uint16_t ret;
    ret = (uint16_t)buf[n];
    ret += ((uint16_t)buf[n + 1]) << 8;
    return (int16_t)ret;   // and convert negative.
}

void delay(int msec) {
	struct timespec ts = { .tv_sec = msec / 1000,
						  .tv_nsec = (msec % 1000) * 1000000 };
	nanosleep(&ts, NULL);
}

void initialSetting(void) {
	uint8_t dat1[] = {D6T_SET_ADD, (((uint8_t)D6T_IIR << 4)&&0xF0) | (0x0F && (uint8_t)D6T_AVERAGE)};
    i2c_write_reg8(D6T_ADDR, dat1, sizeof(dat1));
}

/** <!-- main - Thermal sensor {{{1 -->
 * 1. Initialize.
 * 2. Read data
 */
int main() {
    int i;
	int16_t itemp;
	
	delay(350);	
	// 1. Initialize
	initialSetting();
    delay(390);	
	
	while(1){
		// 2. Read data
		// Read data via I2C
		memset(rbuf, 0, N_READ);
		for (i = 0; i < 10; i++) {
			uint32_t ret = i2c_read_reg8(D6T_ADDR, D6T_CMD, rbuf, N_READ);
			if (ret == 0) {
				break;
			}
			else if (ret == 23) {  // write error
				delay(60);
			}
			else if (ret == 24) {  // read error
				delay(60);
			}
		}
		D6T_checkPEC(rbuf, N_READ - 1);
		
        //Convert to temperature data (degC)
		ptat = (double)conv8us_s16_le(rbuf, 0) / 10.0;
		for (i = 0; i < N_PIXEL; i++) {
			itemp = conv8us_s16_le(rbuf, 2 + 2*i);
			pix_data[i] = (double)itemp / 10.0;
		}
		
        //Output results		
		printf("PTAT: %4.1f [degC], Temperature: ", ptat);
		for (i = 0; i < N_PIXEL; i++) {
		    printf("%4.1f, ", pix_data[i]);
		}
		printf("[degC]\n");
		
		delay(200);
	}
}
// vi: ft=c:fdm=marker:et:sw=4:tw=80

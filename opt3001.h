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
#if !defined(__OPT3001_H__)
#define __OPT3001_H__

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdbool.h>

#define OPT3001_SLAVE_ADDR                 ((uint8_t)0x45)  /* Slave address */

/* Commands */
/* Register values */
#define OPT3001_REG_RESULT                   (uint8_t)0x00
#define OPT3001_REG_CONFIG                   (uint8_t)0x01
#define OPT3001_REG_MANUFACTUREID            (uint8_t)0x7E
#define OPT3001_DEVICEID                     (uint8_t)0x7F
/* Settings */
#define OPT3001_CMD_CONFIG_MSB                   (uint8_t)0xC6
#define OPT3001_CMD_CONFIG_LSB                   (uint8_t)0x10

uint32_t opt3001_trigger_measurement(void);
uint32_t opt3001_read_data(uint16_t *light_x100);
#endif  // OPT3001_H__


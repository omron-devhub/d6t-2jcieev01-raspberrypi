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
#if !defined(SHT20_H__)
#define SHT20_H__

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define SHT30_SLAVE_ADDR (uint8_t)0x44   /* Slave address */

#define SHT20_STATUS_BIT_MASK (uint16_t)0xFFFC
#define SHT20_MEAS_TYPE_BIT (uint16_t)0x0002

/* Commands */
/* Trigger RH measurement (hold master)*/
#define SHT20_CMD_MEASURE_HUMIDITY_HOLD            (uint8_t)0xE5
/* Trigger RH measurement */
#define SHT20_CMD_MEASURE_HUMIDITY_NO_HOLD         (uint8_t)0xF5
/* Trigger T measurement (hold master)*/
#define SHT20_CMD_MEASURE_TEMPERATURE_HOLD         (uint8_t)0xE3
/* Trigger T measurement */
#define SHT20_CMD_MEASURE_TEMPERATURE_NO_HOLD      (uint8_t)0xF3
/* Write user register */
#define SHT20_CMD_WRITE_REGISTER                   (uint8_t)0xE6
/* Read user register */
#define SHT20_CMD_READ_REGISTER                    (uint8_t)0xE7
/* Soft reset */
#define SHT20_CMD_RESET                            (uint8_t)0xFE

/* Register values */
#define SHT20_REG_RESOLUTION_MASK                  (uint8_t)0x81
#define SHT20_REG_RESOLUTION_H12_T14               (uint8_t)0x00
#define SHT20_REG_RESOLUTION_H08_T12               (uint8_t)0x01
#define SHT20_REG_RESOLUTION_H10_T13               (uint8_t)0x80
#define SHT20_REG_RESOLUTION_H11_T11               (uint8_t)0x81
#define SHT20_REG_LOW_VOLTAGE                      (uint8_t)0x40
#define SHT20_REG_OK_VOLTAGE                       (uint8_t)0x00
#define SHT20_REG_ENABLE_HEATER                    (uint8_t)0x04
#define SHT20_REG_DISABLE_HEATER                   (uint8_t)0x66

/* Measurement Range */
#define SHT30_HUMIDITY_MIN_X100                    0  /* centi-percent RH */
#define SHT30_HUMIDITY_MAX_X100                    10000  /* centi-percent RH */
#define SHT30_TEMPERATURE_MIN_X100                -4000  /* centi-degC */
#define SHT30_TEMPERATURE_MAX_X100                12500  /* centi-degC */

/* Serial Number */
#define SHT20_CMD_READ_SN_1ST_BYTES                (uint16_t)0xFA0F
#define SHT20_CMD_READ_SN_2ND_BYTES                (uint16_t)0xFCC9


/* Settings */
#define SHT20_RESOLUTION                            SHT20_REG_RESOLUTION_H12_T14
#define SHT20_HEATER                                SHT20_REG_DISABLE_HEATER

#define SHT30_SET_HEATER_DISABLE                        (uint8_t)0x66
#define SHT30_SET_HEATER_MSB                            (uint8_t)0x30
#define SHT30_SET_HEATER_LSB                            SHT30_SET_HEATER_DISABLE
#define SHT30_READ_STATUS  (uint16_t)0xF32D
#define SHT30_CLEAR_STATUS (uint16_t)0x3041
#define SHT30_CMD_MASURE   (uint16_t)0x2334
#define SHT30_READ_MASURE  (uint16_t)0xE000

//------------------------
// kurihara
#define SHT30_ONESHOT_MASURE_MSB                            (uint8_t)0x2C
#define SHT30_ONESHOT_MASURE_LSB                            (uint8_t)0x06
//------------------------

/** @Typedef -----------------------------------------------------------------*/
/** @Macro -------------------------------------------------------------------*/
/** @Extern ------------------------------------------------------------------*/
/** @Functions ---------------------------------------------------------------*/
bool sht30_Init(void);
int32_t sht30_read_triggered_TRH_x100(int32_t *value_T, int32_t *value_RH);
extern uint32_t i2c_write_8(uint8_t slave_addr, uint8_t register_addr, uint8_t *write_buff, uint8_t len);
extern uint32_t i2c_read_16(uint8_t slave_addr, uint16_t register_addr, uint8_t *read_buff, uint8_t len);
extern int32_t value_T;
extern int32_t value_RH;

#endif  // SHT20_H__

/******************* (C) COPYRIGHT 2016 OMRON Corporation ******END OF FILE****/

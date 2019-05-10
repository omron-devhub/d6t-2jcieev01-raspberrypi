/*
 * MIT License
 * Copyright (c) 2018 - present OMRON Corporation
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
#if !defined(__BARO_2SMPB02E_H__)
#define __BARO_2SMPB02E_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdbool.h>

#include <wiringPi.h>

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define BARO_2SMPB02E_ADDRESS                (0x56)
/*=========================================================================*/

/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
    #define BARO_2SMPB02E_REGI2C_PRES_TXD2               0xF7
    #define BARO_2SMPB02E_REGI2C_IO_SETUP                0xF5
    #define BARO_2SMPB02E_REGI2C_CTRL_MEAS               0xF4
    #define BARO_2SMPB02E_REGI2C_IIR                     0xF1
    #define BARO_2SMPB02E_REGI2C_CHIP_ID                 0xD1
    #define BARO_2SMPB02E_REGI2C_COEFS                   0xA0

    /* Register values */
    #define BARO_2SMPB02E_VAL_IOSETUP_STANDBY_0001MS       ((uint8_t)0x00)
    #define BARO_2SMPB02E_VAL_IOSETUP_STANDBY_0125MS       ((uint8_t)0x20)
    #define BARO_2SMPB02E_VAL_IOSETUP_STANDBY_0250MS       ((uint8_t)0x40)
    #define BARO_2SMPB02E_VAL_IOSETUP_STANDBY_0500MS       ((uint8_t)0x60)
    #define BARO_2SMPB02E_VAL_IOSETUP_STANDBY_1000MS       ((uint8_t)0x80)
    #define BARO_2SMPB02E_VAL_IOSETUP_STANDBY_2000MS       ((uint8_t)0xA0)
    #define BARO_2SMPB02E_VAL_IOSETUP_STANDBY_4000MS       ((uint8_t)0xC0)
    #define BARO_2SMPB02E_VAL_IOSETUP_STANDBY_8000MS       ((uint8_t)0xE0)

    #define BARO_2SMPB02E_VAL_TEMPAVERAGE_01     ((uint8_t)0x20)
    #define BARO_2SMPB02E_VAL_TEMPAVERAGE_02     ((uint8_t)0x40)
    #define BARO_2SMPB02E_VAL_TEMPAVERAGE_04     ((uint8_t)0x60)

    #define BARO_2SMPB02E_VAL_PRESAVERAGE_01    ((uint8_t)0x04)
    #define BARO_2SMPB02E_VAL_PRESAVERAGE_02    ((uint8_t)0x08)
    #define BARO_2SMPB02E_VAL_PRESAVERAGE_04    ((uint8_t)0x0C)
    #define BARO_2SMPB02E_VAL_PRESAVERAGE_08    ((uint8_t)0x10)
    #define BARO_2SMPB02E_VAL_PRESAVERAGE_16    ((uint8_t)0x14)
    #define BARO_2SMPB02E_VAL_PRESAVERAGE_32    ((uint8_t)0x18)

    #define BARO_2SMPB02E_VAL_POWERMODE_SLEEP  ((uint8_t)0x00)
    #define BARO_2SMPB02E_VAL_POWERMODE_FORCED ((uint8_t)0x01)
    #define BARO_2SMPB02E_VAL_POWERMODE_NORMAL ((uint8_t)0x03)

    #define BARO_2SMPB02E_VAL_IIR_OFF     ((uint8_t)0x00)
    #define BARO_2SMPB02E_VAL_IIR_02TIMES ((uint8_t)0x01)
    #define BARO_2SMPB02E_VAL_IIR_04TIMES ((uint8_t)0x02)
    #define BARO_2SMPB02E_VAL_IIR_08TIMES ((uint8_t)0x03)
    #define BARO_2SMPB02E_VAL_IIR_16TIMES ((uint8_t)0x04)
    #define BARO_2SMPB02E_VAL_IIR_32TIMES ((uint8_t)0x05)

    /* Coeff */
    #define BARO_2SMPB02E_COEFF_S_A1   ((double)( 4.3E-04))
    #define BARO_2SMPB02E_COEFF_A_A1   ((double)(-6.3E-03))
    #define BARO_2SMPB02E_COEFF_S_A2   ((double)( 1.2E-10))
    #define BARO_2SMPB02E_COEFF_A_A2   ((double)(-1.9E-11))
    #define BARO_2SMPB02E_COEFF_S_BT1  ((double)( 9.1E-02))
    #define BARO_2SMPB02E_COEFF_A_BT1  ((double)( 1.0E-01))
    #define BARO_2SMPB02E_COEFF_S_BT2  ((double)( 1.2E-06))
    #define BARO_2SMPB02E_COEFF_A_BT2  ((double)( 1.2E-08))
    #define BARO_2SMPB02E_COEFF_S_BP1  ((double)( 1.9E-02))
    #define BARO_2SMPB02E_COEFF_A_BP1  ((double)( 3.3E-02))
    #define BARO_2SMPB02E_COEFF_S_B11  ((double)( 1.4E-07))
    #define BARO_2SMPB02E_COEFF_A_B11  ((double)( 2.1E-07))
    #define BARO_2SMPB02E_COEFF_S_BP2  ((double)( 3.5E-10))
    #define BARO_2SMPB02E_COEFF_A_BP2  ((double)(-6.3E-10))
    #define BARO_2SMPB02E_COEFF_S_B12  ((double)( 7.6E-13))
    #define BARO_2SMPB02E_COEFF_A_B12  ((double)( 2.9E-13))
    #define BARO_2SMPB02E_COEFF_S_B21  ((double)( 1.2E-14))
    #define BARO_2SMPB02E_COEFF_A_B21  ((double)( 2.1E-15))
    #define BARO_2SMPB02E_COEFF_S_BP3  ((double)( 7.9E-17))
    #define BARO_2SMPB02E_COEFF_A_BP3  ((double)( 1.3E-16))

    #define BARO_2SMPB02E_VAL_MEASMODE_HIGHSPEED \
        (BARO_2SMPB02E_VAL_PRESAVERAGE_02 | BARO_2SMPB02E_VAL_TEMPAVERAGE_01)
    #define BARO_2SMPB02E_VAL_MEASMODE_LOWPOWER \
        (BARO_2SMPB02E_VAL_PRESAVERAGE_04 | BARO_2SMPB02E_VAL_TEMPAVERAGE_01)
    #define BARO_2SMPB02E_VAL_MEASMODE_STANDARD \
        (BARO_2SMPB02E_VAL_PRESAVERAGE_08 | BARO_2SMPB02E_VAL_TEMPAVERAGE_01)
    #define BARO_2SMPB02E_VAL_MEASMODE_HIGHACCURACY \
        (BARO_2SMPB02E_VAL_PRESAVERAGE_16 | BARO_2SMPB02E_VAL_TEMPAVERAGE_02)
    #define BARO_2SMPB02E_VAL_MEASMODE_ULTRAHIGH \
        (BARO_2SMPB02E_VAL_PRESAVERAGE_32 | BARO_2SMPB02E_VAL_TEMPAVERAGE_04)
/*=========================================================================*/

/*=========================================================================
    CALIBRATION DATA
    -----------------------------------------------------------------------*/
    typedef struct baro_2smpb02e_setting {
        /* Compensation Factor */
        double _A0, _A1, _A2;
        double _B00, _BT1, _BP1;
        double _B11, _BT2, _BP2;
        double _B12, _B21, _BP3;
    } baro_2smpb02e_setting_t;

bool baro_2smpb02e_output_compensation(
        uint32_t raw_temp_val, uint32_t raw_press_val,
        uint32_t* pres, int16_t* temp);
int baro_2smpb02e_read(uint32_t* pres, int16_t* temp,
                       uint32_t* dp, uint32_t* dt);
#endif

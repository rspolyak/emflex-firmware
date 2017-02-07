/*****************************************************************************
* File:       i2c.c
*
* Created on: Aug 21, 2015
* Author:     rostokus
******************************************************************************

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
******************************************************************************
 */

#include <string.h>
#include <ctype.h>
#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "common.h"
#include "logging.h"

#if defined STM32L1XX_MD || defined STM32F10X_MD_VL 
const I2CConfig i2cCfg =
{
  OPMODE_I2C,
  100000,
  STD_DUTY_CYCLE
};
#endif

#if defined STM32F303xC
static const I2CConfig i2cCfg = {
  STM32_TIMINGR_PRESC(15U) |
  STM32_TIMINGR_SCLDEL(4U) | STM32_TIMINGR_SDADEL(2U) |
  STM32_TIMINGR_SCLH(15U)  | STM32_TIMINGR_SCLL(21U),
  0,
  0
};
#endif

void i2cDrvInit(void)
{
  palSetPadMode(GPIOB, 8, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN); //SCL
  palSetPadMode(GPIOB, 9, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN); //SCA

  i2cStart(&I2CD1, &i2cCfg);
}


/******************************************************************************
* File:        bsp.c
*
* Created on:  May 02, 2017
*
* Description: platform specific routines for custom STM32F103 dev board
*
* Author:      Rostyslav Spolyak
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

#include "common.h"
#include "bsp.h"

RV_t bspInit(void)
{
  /* Set serial port TX pin to alternative mode */
  palSetPadMode(GPIOA, 9, PAL_MODE_STM32_ALTERNATE_PUSHPULL);

  return RV_SUCCESS;
}

RV_t bspRegisterEventCb(bsp_event_t ev, bsp_cb_t cb)
{
  (void) ev;
  (void) cb;

  return RV_SUCCESS;
}

void bspGsmPowerOnOff(void)
{
}

void bspSystemPowerOn(void)
{
}

void bspSystemPowerOff(void)
{
}

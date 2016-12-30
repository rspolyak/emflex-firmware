/**
******************************************************************************
* File:         main.c
* Description:  Entry point of GSM tracker. Creates tasks and
*               goes to infinite loop
*
* Created on:   May 09, 2015
* Author:       rostokus
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
#include "common.h"
#include "logging.h"
#include "bsp.h"
#include "cnfgr_api.h"
#include "button.h"
#include "stepper.h"
#include "bl_api.h"
#include "ext_api.h"

uint32_t logTimeStampGet(void)
{
  RTCDateTime timp;
  static uint32_t timestamp_g = 0;

  rtcGetTime(&RTCD1, &timp);

  if (timestamp_g == 0)
  {
    timestamp_g = timp.millisecond;
  }

  return (timp.millisecond - timestamp_g) / 60000;
}

void profileInit(void)
{
    cnfgrRegister("BLT", blInit);
    cnfgrRegister("Button", buttonAppInit);
    cnfgrRegister("Stepper", stepperAppInit);
    cnfgrRegister("Ext", extAppInit);
}

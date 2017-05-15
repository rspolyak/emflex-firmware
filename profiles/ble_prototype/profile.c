/**
******************************************************************************
* File:         profile.c
* Description:  Entry point of BLE prototype
*
* Created on:   20 April 2017
* Author:       Denys Haryachyy
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
#include "bl_api.h"
#include "cli.h"
#include "ext_api.h"

static RV_t gerkonMessage(void)
{
  static uint32_t counter = 0;
  char str[20];

  snprintf(str, 20, "%u: gerkon\r\n", counter++);

  blModuleSend(str);

  return RV_SUCCESS;
}

void profileInit(void)
{
    extAppCbRegister(2,
                EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOC,
                gerkonMessage);

    cliCmdRegister("gerkon", &gerkonMessage);
    cnfgrRegister("BLT", blInit);
    cnfgrRegister("Ext", extAppInit);
}

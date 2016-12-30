/**
******************************************************************************
* File:         profile.c
* Description:  Entry point of GPS tracker.
*
* Created on:   May 09, 2015
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
#include "bsp.h"
#include "cnfgr_api.h"
#include "logging.h"
#include "cli.h"

RV_t helloWorld(void)
{
  LOG_TRACE(CMP, "Hello world!");

  return RV_SUCCESS;
}

void profileInit()
{
  cliCmdRegister("hello", &helloWorld);
}

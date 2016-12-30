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
#include "gsm_api.h"
#include "control_api.h"
#include "cli.h"
#include "data_eeprom.h"

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

#if 0
RV_t gsmCmdSend(void)
{
  char buf[32] = {0};

  strncpy(buf, cmdBuf, len);
  buf[len] = '\r';
  buf[len+1] = '\0';

  gsmATCmdSend(buf, sizeof(buf));
  return RV_SUCCESS;
}
#endif

RV_t cliEventLogsOn(void)
{
  //logMutexLock();

  //logStdOut = true;
  //memset(debug_flags_g, 0xff, sizeof(debug_flags_g));

  //logMutexUnlock();

  return RV_SUCCESS;
}

RV_t cliEventLogsOff(void)
{
  //logMutexLock();

  //logStdOut = false;
  //memset(debug_flags_g, 0x00, sizeof(debug_flags_g));

  //logMutexUnlock();

  return RV_SUCCESS;
}

RV_t cliEventShowEeprom(void)
{
  uint8_t val = 0;
  uint32_t i = 1;

  LOG_TRACE(CLI_CMP, "======START LOG=====");
  while(i < EEPROM_SIZE)
  {
    DATAEEPROM_ReadByte(EEPROM_START+i, &val);
    //LOG_INLINE("%c", val);
    i++;
  }
  LOG_TRACE(CLI_CMP, "======END LOG=====");

  return RV_SUCCESS;
}

void profileInit(void)
{
  cnfgrRegister("Logging", loggingAppInit);
  cnfgrRegister("Control", ctrlAppInit);
  cnfgrRegister("GSM", gsmInit);

//  cliCmdRegister("AT", gsmCmdSend);
  cliCmdRegister("log_console",  cliEventLogsOn);
  cliCmdRegister("log_eeprom", cliEventLogsOff);
  cliCmdRegister("show_eeprom", cliEventShowEeprom);
}

/******************************************************************************
* File:         wifi.c
* Description:  Wifi driver.
*
* Created on:   January 19, 2017
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
#include <ctype.h>

#include "ch.h"
#include "hal.h"
#include "bsp.h"
#include "chprintf.h"
#include "common.h"
#include "logging.h"
#include "wifi.h"
#include "serial_port.h"

static THD_WORKING_AREA(wifiThread, WIFI_TASK_STACK_SIZE);
static THD_FUNCTION(wifiTask, arg)
{
  (void) arg;
  uint32_t i = 0;
  char buf[WIFI_MAX_BUF_SIZE + 1] = {0};

  chRegSetThreadName("wifiTask");

  while (1)
  {
    buf[i] = sdGet(&WIFI_SERIAL_PORT);

    if (buf[i] == '\n')
    {
      continue;
    }

    if (buf[i] == '\r' || i >= WIFI_MAX_BUF_SIZE)
    {
      i = 0;

      LOG_TRACE(WIFI_CMP, "%s\r\n", buf);
      continue;
    }

    i++;
  }
}

/*
 * Wifi entry point.
 */
RV_t wifiInit(void)
{
  if (serialInit(WIFI_SERIAL_SPEED, &WIFI_SERIAL_PORT) < 0)
  {
    LOG_TRACE(WIFI_CMP, "Serial is already occupied");
    return -1;
  }

  LOG_TRACE(WIFI_CMP, "Hello from WIFI!!");

  /*
   * Creates the GPS read thread.
   */
  chThdCreateStatic(wifiThread, sizeof(wifiThread), NORMALPRIO+1, wifiTask, NULL);

  return 0;
}

/*****************************************************************************
* File:       gps_api.c
*
* Created on: May 09, 2015
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
#include "bsp.h"
#include "chprintf.h"
#include "common.h"
#include "logging.h"
#include "gps.h"
#include "serial_port.h"

static THD_WORKING_AREA(gpsThread, GPS_TASK_STACK_SIZE);
static THD_FUNCTION(gpsTask, arg)
{
  (void) arg;
  uint32_t i = 0;
  char buf[GPS_MAX_BUF_SIZE + 1] = {0};

  chRegSetThreadName("gpsTask");

  while (1)
  {
    buf[i] = sdGet(&GPS_SERIAL_PORT);

    if (buf[i] == '\n')
    {
      continue;
    }

    if (buf[i] == '\r' || i >= GPS_MAX_BUF_SIZE)
    {
      buf[i] = 0;
      i = 0;
      LOG_TRACE(GPS_CMP, "%s\r\n", buf);
      //nmeaParse(buf);
      continue;
    }

    i++;
  }
}

/*
 * GPS entry point.
 */
RV_t gpsInit(void)
{
  if (serialInit(GPS_SERIAL_SPEED, &GPS_SERIAL_PORT) < 0)
  {
    LOG_TRACE(GPS_CMP, "Serial is already occupied");
    return -1;
  }

  LOG_TRACE(GPS_CMP, "Hello from GPS!!");

  /*
   * Creates the GPS read thread.
   */
  chThdCreateStatic(gpsThread, sizeof(gpsThread), NORMALPRIO+1, gpsTask, NULL);

  return 0;
}

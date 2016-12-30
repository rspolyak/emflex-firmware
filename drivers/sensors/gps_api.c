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
#include "chprintf.h"
#include "common.h"
#include "logging.h"

static THD_WORKING_AREA(gpsAppThread, 256);

static THD_FUNCTION(gpsAppTask, arg)
{
  (void)arg;

  while (1)
  {

  }
}

void gpsAppInit(void)
{
  /* Create thread */
  chThdCreateStatic(gpsAppThread, sizeof(gpsAppThread), NORMALPRIO+1, gpsAppTask, 0);
}


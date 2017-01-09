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
#include "control_api.h"
#include "gps.h"

typedef enum
{
  START_EVENT = 0,
} gps_sm_event_t;

typedef enum
{
  IDLE_STATE = 0,
  ACTIVE_STATE,
} gps_sm_state_t;

RV_t doStateIdle(ctrl_sm_event_t ev, ctrl_sm_state_t* state)
{
  switch (ev)
  {
    case START_EVENT:
      LOG_TRACE(CONTROL_CMP, "Started!\r\n");
      *state = ACTIVE_STATE;
      break;

    default:
      LOG_TRACE(CONTROL_CMP, "UNSUPPORTED event!\r\n");
      break;
  }

  return RV_SUCCESS;
}

RV_t doStateActive(ctrl_sm_event_t ev, ctrl_sm_state_t* state)
{
  (void)ev;
  (void)state;

  return RV_SUCCESS;
}

RV_t helloWorld(void)
{
  LOG_TRACE(CMP, "Hello world!");

  ctrlEventPost(START_EVENT);

  return RV_SUCCESS;
}

void profileInit()
{
  ctrlStateAdd(IDLE_STATE, doStateIdle, "IDLE");
  ctrlStateAdd(ACTIVE_STATE, doStateActive, "ACTIVE");

  cliCmdRegister("hello", &helloWorld);

  cnfgrRegister("Control", ctrlAppInit);
  cnfgrRegister("Gps", gpsInit);
}

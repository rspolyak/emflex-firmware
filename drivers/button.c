/*****************************************************************************
* File:       button.c
*
* Created on: Apr 24, 2016
*
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
#include "common.h"
#include "stepper.h"
#include "logging.h"

RV_t extcb1(void)
{
  msg_t resp = Q_OK;

  LOG_TRACE(BUT_CMP, "UP button");

  resp = chMBPostI(&stepMsg, STEP_UP);
  if (resp < Q_OK)
  {
    LOG_TRACE(BUT_CMP, "rv = %i", resp);
  }

  return RV_SUCCESS;
}

RV_t extcb2(void)
{
  msg_t resp = Q_OK;

  LOG_TRACE(BUT_CMP, "DOWN button");

  resp = chMBPostI(&stepMsg, STEP_DOWN);
  if (resp < Q_OK)
  {
    LOG_TRACE(BUT_CMP, "rv = %i", resp);
  }

  return RV_SUCCESS;
}

RV_t buttonAppInit(void)
{
   return RV_SUCCESS;
}

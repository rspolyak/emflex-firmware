/******************************************************************************
* File:         ext_api.c
*
* Description:  External interrupt handlers
*
* Created on:   May 20, 2016
*
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

#include "common.h"
#include "button.h"
#include "stepper.h"

static EXTConfig but_config = {{}};

RV_t extAppSetCb(uint32_t channel, uint32_t mode, extcallback_t cb)
{
  but_config.channels[channel].mode = mode;
  but_config.channels[channel].cb = cb;

  return RV_SUCCESS;
}

RV_t extAppInit(void)
{
  uint32_t channel = 0;

  extStart(&EXTD1, &but_config);

  for (channel = 0; channel < EXT_MAX_CHANNELS; channel++)
  {
      if (but_config.channels[channel].cb != NULL)
      {
          extChannelEnable(&EXTD1, channel);
      }
  }

  return RV_SUCCESS;
}

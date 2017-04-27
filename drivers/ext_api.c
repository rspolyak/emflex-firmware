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
#include "logging.h"
#include "ext_api.h"

static mailbox_t extMailbox;
static msg_t extMsgQueue[EXT_MAX_CHANNELS];
static ext_cb_t cb_array[EXT_MAX_CHANNELS];

static EXTConfig ext_config = {{}};

static void extApp_cb(EXTDriver *extp, expchannel_t channel)
{
  (void)extp;

  chSysLockFromISR();

  chMBPostI(&extMailbox, channel);

  chSysUnlockFromISR();

  return;
}

RV_t extAppCbRegister(uint32_t channel, uint32_t mode, ext_cb_t cb)
{
  ext_config.channels[channel].mode = mode;
  ext_config.channels[channel].cb = extApp_cb;
  cb_array[channel] = cb;

  return RV_SUCCESS;
}

static THD_WORKING_AREA(extThread, 512);

static THD_FUNCTION(extTask, arg)
{
  (void) arg;

  msg_t resp = Q_OK;
  msg_t val = 0;

  while (1)
  {
    /* wait for event */
    if ((resp = chMBFetch(&extMailbox, &val, TIME_INFINITE)) >= Q_OK)
    {
      cb_array[val]();
    }
  }
}

RV_t extAppInit(void)
{
  /* create message queue */
  chMBObjectInit(&extMailbox, extMsgQueue, EXT_MAX_CHANNELS);

  /* Create thread */
  chThdCreateStatic(extThread, sizeof(extThread), NORMALPRIO+1, extTask, 0);

  extStart(&EXTD1, &ext_config);

  return RV_SUCCESS;
}

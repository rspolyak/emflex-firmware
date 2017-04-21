/*****************************************************************************
* File:         bl_ll.c
*
* Description:
*               Low level functionality that communicates with Bluetooth module.
*
* Created on:   Apr 14, 2016
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "common.h"
#include "bl_ll_api.h"
#include "utils.h"
#include "bsp.h"
#include "logging.h"
#include "serial_port.h"

#define MAX_CMD_LEN 128

typedef enum
{
  PARSE_DATA_STATE,
  FINISH_STATE
} bl_parse_state_t;

static THD_WORKING_AREA(blThread, 1024);

#if 0
static RV_t bleResponseParse(const char *buf, int32_t len)
{
  static bl_parse_state_t state = PARSE_DATA_STATE;
  static char blCmdBuf[MAX_CMD_LEN];
  static char *p = blCmdBuf;
  int resp = 0;

  if (!buf)
  {
    return RV_FAILURE;
  }

  switch (state)
  {
    case PARSE_DATA_STATE:
      while (len > 0)
      {
        if (*buf == '\n')
        {
          buf++;
          len--;
          state = FINISH_STATE;
          break;
        }
        *p++ = *buf++;
        len--;
        if (len == 0)
        {
          LOG_TRACE(BLT_CMP, "len %u", len);
          return RV_NOT_COMPLETED;
        }
      }

    case FINISH_STATE:
      *p = '\0';
      state = PARSE_DATA_STATE;

      if (0 == strncmp(blCmdBuf, "up", sizeof("up")-1))
      {
        LOG_TRACE(BLT_CMP, "UP button");

        resp = chMBPost(&stepMsg, STEP_UP, TIME_IMMEDIATE);
        if (resp < Q_OK)
        {
          LOG_TRACE(BLT_CMP, "rv = %i", resp);
        }
      }
      else if (0 == strncmp(blCmdBuf, "down", sizeof("down")-1))
      {
        LOG_TRACE(BLT_CMP, "DOWN button");

        resp = chMBPost(&stepMsg, STEP_DOWN, TIME_IMMEDIATE);
        if (resp < Q_OK)
        {
          LOG_TRACE(BLT_CMP, "rv = %i", resp);
        }
      }
      else if (0 == strncmp(blCmdBuf, "open", sizeof("open")-1))
      {
        LOG_TRACE(BLT_CMP, "OPEN button");

        resp = chMBPost(&stepMsg, FULLOPEN, TIME_IMMEDIATE);
        if (resp < Q_OK)
        {
          LOG_TRACE(BLT_CMP, "rv = %i", resp);
        }
      }
      else if (0 == strncmp(blCmdBuf, "close", sizeof("close")-1))
      {
        LOG_TRACE(BLT_CMP, "CLOSE button");

        resp = chMBPost(&stepMsg, FULLCLOSED, TIME_IMMEDIATE);
        if (resp < Q_OK)
        {
          LOG_TRACE(BLT_CMP, "rv = %i", resp);
        }
      }

      p = blCmdBuf;
      break;
  }

  return RV_SUCCESS;
}
#endif

static THD_FUNCTION(blTask, arg)
{
  (void) arg;

  uint32_t blInByteNum = 0;
  char buf[MAX_CMD_LEN] = {0};

  while (1)
  {
    /*Decrease the read speed from UART*/
    chThdSleepMilliseconds(100);

    /* get data from serial port if any. Get up to sizeof(buf) bytes */
    memset(buf, 0, sizeof(buf));
    blInByteNum = sdAsynchronousRead(&SD1, (uint8_t *) buf, MAX_CMD_LEN - 1);
    if ((blInByteNum > 0) && (blInByteNum < MAX_CMD_LEN))
    {
      LOG_TRACE(BLT_CMP, "Bluetooth returned %d bytes:%s", blInByteNum, buf);

      //bleResponseParse(buf, blInByteNum);
    }
  }
}

RV_t blTaskInit(void)
{
  if (serialInit(BL_SERIAL_SPEED, &BL_SERIAL_PORT) < 0)
  {
    LOG_TRACE(BLT_CMP, "Serial is already occupied");
    return RV_FAILURE;
  }

  /* Create thread */
  chThdCreateStatic(blThread, sizeof(blThread), NORMALPRIO+1, blTask, 0);

  return RV_SUCCESS;
}

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

#include "common.h"
#include "bl_ll_api.h"
#include "bl_api.h"
#include "utils.h"
#include "bsp.h"
#include "logging.h"
#include "serial_port.h"

#define MAX_CMD_LEN 128

static THD_WORKING_AREA(blThread, 1024);

blCbFunc_t blCallback_g = NULL;

RV_t blRegisterEventCb(blCbFunc_t cb)
{
  blCallback_g = cb;

  return RV_SUCCESS;
}

static RV_t blCallCb(const char *buf, int32_t len)
{
  if (blCallback_g == NULL)
  {
    LOG_TRACE(BL_CMP,"Callback is not registered");
    return RV_FAILURE;
  }

  return blCallback_g(buf, len);
}

RV_t blModuleSend(const char *val)
{
  msg_t resp = Q_OK;

  if (!val)
  {
    return RV_FAILURE;
  }

  if ((resp = sdWriteTimeout(&BL_SERIAL_PORT, (uint8_t *) val, strlen(val),
                             BL_WRITE_TIMEOUT)) < Q_OK)
  {
    LOG_TRACE(GSM_CMP,"Error. rv=%i", resp);
  }

  return RV_SUCCESS;
}

static THD_FUNCTION(blTask, arg)
{
  (void) arg;

  uint32_t blInByteNum = 0;
  char buf[MAX_CMD_LEN] = {0};

  while (1)
  {
    /* get data from serial port if any. Get up to sizeof(buf) bytes */
    memset(buf, 0, sizeof(buf));
    blInByteNum = sdReadTimeout(&BL_SERIAL_PORT, (uint8_t *) buf,
                                MAX_CMD_LEN - 1, BL_READ_TIMEOUT);
    if ((blInByteNum > 0) && (blInByteNum < MAX_CMD_LEN))
    {
      LOG_TRACE(BLT_CMP, "Bluetooth returned %d bytes:%s", blInByteNum, buf);

      blCallCb(buf, blInByteNum);
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

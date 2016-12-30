/*****************************************************************************
* File:       gsm_api.c
*
* Created on: May 01, 2015
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

#include "common.h"
#include "logging.h"
#include "gsm_common.h"
#include "gsm_ll_api.h"
//#include "cli.h"

extern gsmCbFunc_t gsmCbArray_g[GSM_EVENT_LAST];
extern phoneBook_t phoneBook_g;

RV_t gsmEventModuleReboot(void)
{
  LOG_TRACE(GSM_CMP, "Powering down!\r\n");
  if (RV_SUCCESS != gsmModuleSend(GSM_POWER_DOWN))
  {
    LOG_TRACE(GSM_CMP,"gsmModuleSend failed\r\n");
  }

  return RV_SUCCESS;
}

/* Initialize gsm module */
RV_t gsmInit(void)
{
  /* create task, msg queue, etc. */
  if (RV_SUCCESS != gsmTaskInit())
  {
    return RV_FAILURE;
  }

  /* initialize GSM module */
  if (RV_SUCCESS != gsmModuleInit())
  {
    return RV_FAILURE;
  }

  return RV_SUCCESS;
}

/* Send sms to gsm module */
RV_t gsmSmsSend(const char* smsStr)
{
  if (smsStr == 0)
  {
    return RV_FAILURE;
  }

  if (phoneBook_g.resp_is_set == FALSE)
  {
    LOG_TRACE(GSM_CMP,"No number to respond to\r\n");
    return RV_FAILURE;
  }

  return gsmSendSmsToNumber(phoneBook_g.resp_number, smsStr);
}

/* Call a number */
RV_t gsmCall(const char *telNum)
{
  (void) telNum;

  return RV_SUCCESS;
}

/* Register callback to receive asynchronous events from gsm module */
RV_t gsmRegisterEventCb(gsmEvent_t event, gsmCbFunc_t cb)
{
  if ((event == GSM_EVENT_UNKNOWN) || (event == GSM_EVENT_LAST))
  {
      return RV_FAILURE;
  }

  gsmCbArray_g[event] = cb;

  return RV_SUCCESS;
}

RV_t gsmStateReqSend(void)
{
  if (RV_SUCCESS == gsmCmdSend(GSM_SIGNAL_LEVEL) &&
      RV_SUCCESS == gsmCmdSend(GSM_BATTERY_DISCHARGE) &&
      RV_SUCCESS == gsmCmdSend(GSM_BALANCE_CHECK))
  {
    return RV_SUCCESS;
  }
  else
  {
    return RV_FAILURE;
  }
}

RV_t gsmATCmdSend(const char *buf, uint32_t len)
{
  (void) len;

  gsmCmdSend(buf);

  return RV_SUCCESS;
}


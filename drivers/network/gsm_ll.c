/*****************************************************************************
* File:         gsm_ll.c
* Description:
*               Low level functionality that communicates with GSM module:
*               transmit new commands to GSM, parse response to sent commands
*               (i.e. OK, FAIL) and handle incoming asynchronous messages
*               (like events about new SMS
*
* Created on:   Mar 29, 2015
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
#include "logging.h"
#include "gsm_ll.h"
#include "gsm_ll_api.h"
#include "gsm_common.h"
#include "utils.h"
#include "bsp.h"
#include "serial_port.h"

/* Handle errors returned by GSM module,
 * like out of balance, network unreachable */
//#define GSM_ERROR_HANDLE

/* Enables GSM low power mode */
#define GSM_SLEEP

gsmCbFunc_t gsmCbArray_g[GSM_EVENT_LAST];
phoneBook_t phoneBook_g;

static mailbox_t gsm_tx_mb_s;
static msg_t gsm_tx_msg_queue_s[MAILBOX_QUEUE_TX_SIZE];
static uint32_t signal = 0;
static uint32_t battery = 0;
static bool gsmReady = false;
static command_t cur_command = {0, true};

static struct
{
  bool state;
  float balance;
} balance_s = {.state = false, .balance = 0.0};

static THD_WORKING_AREA(gsmThread, GSM_THREAD_STACK_SIZE);

static bool gsm_is_ready = FALSE;
static MUTEX_DECL(gsm_ready_mutex);
static CONDVAR_DECL(gsm_ready_cond_var);

RV_t gsmReadySet(void)
{
  chMtxLock(&gsm_ready_mutex);

  gsm_is_ready = TRUE;

  chCondSignal(&gsm_ready_cond_var);

  chMtxUnlock(&gsm_ready_mutex);

  return RV_SUCCESS;
}

RV_t gsmReadyGet(void)
{
  chMtxLock(&gsm_ready_mutex);

  while (gsm_is_ready == FALSE)
  {
    chCondWait(&gsm_ready_cond_var);
  }

  chMtxUnlock(&gsm_ready_mutex);

  return RV_SUCCESS;
}

/* check if received event from GSM is sms content */
RV_t gsmCmpCommand(const char *inBuf, const char *cmpBuf)
{
  if ((char *) 0 != strstr(inBuf, cmpBuf))
  {
    return RV_SUCCESS;
  }
  else
  {
    return RV_FAILURE;
  }
}

RV_t gsmStateReqGet(uint32_t *sign, uint32_t *batt, char *bal, uint32_t len)
{
  if (!sign || !batt || !bal)
  {
    return RV_FAILURE;
  }

  *sign = signal;
  *batt = battery;
   if (balance_s.state == false)
   {
     strncpy(bal, "not ready", len);
   }
   else
   {
     if (RV_SUCCESS != floatToStr(balance_s.balance, bal, len))
     {
       return RV_FAILURE;
     }
   }

  return RV_SUCCESS;
}

/* parse received event about new sms and form SMS request command */ 
static RV_t gsmFormSmsGetRqst(char *inBuf, char *outBuf)
{
  uint8_t i = 0;
  uint8_t len = 0;

  strncpy(outBuf, "AT+CMGR=", sizeof("AT+CMGR="));
  i = strlen(outBuf);

  len = strlen(inBuf);
  if (isdigit((uint8_t) inBuf[len-2]))
  {
    outBuf[i++] = inBuf[len-2];
  }
  outBuf[i++] = inBuf[len-1];

  outBuf[i++] = ',';
  outBuf[i++] = '0';
  outBuf[i++] = '\r';
  outBuf[i] = '\0';

  return RV_SUCCESS;
}

RV_t gsmModuleSend(const char *val)
{
  msg_t resp = Q_OK;

  if (!val)
  {
    return RV_FAILURE;
  }

  if ((resp = sdWriteTimeout(&GSM_SERIAL_PORT, (uint8_t *) val, strlen(val),
                             GSM_WRITE_TIMEOUT)) < Q_OK)
  {
    LOG_TRACE(GSM_CMP,"Error. rv=%i", resp);
  }

  return RV_SUCCESS;
}

RV_t gsmModuleCmdSend(const char *buf)
{
  msg_t rdymsg = MSG_OK;
  char *p = 0;
  uint32_t len = 0;

  if (buf == 0)
  {
    return RV_FAILURE;
  }

  len = strnlen(buf, MAX_BUF_LEN) + 1;

  p = (char *) chHeapAlloc(0, len);

  LOG_TRACE(GSM_CMP, "buf len=%u 0x%x heap\r\n", strnlen(buf, MAX_BUF_LEN), p);

  if (p)
  {
    strncpy(p, buf, len);
  }
  else
  {
    LOG_TRACE(GSM_CMP,"GSM cmd pool is empty\r\n");
    return RV_NOT_READY;
  }

  rdymsg = chMBPost(&gsm_tx_mb_s, (msg_t) p, TIME_IMMEDIATE);
  if (rdymsg != MSG_OK)
  {
    LOG_TRACE(GSM_CMP,"Failed to queue TX msg, RV=%u\r\n", rdymsg);
    return RV_FAILURE;
  }

  return RV_SUCCESS;
}

void gsmModuleConnectGprs(void)
{
    LOG_TRACE(GSM_CMP, "Configure GPRS ...\r\n");

#ifdef GPRS_ENABLED
    gsmCmdSend(GSM_CHECK_SYGNAL_GPRS);
    gsmCmdSend(GSM_CHECK_GPRS_NETWORK);
    gsmCmdSend(GSM_ATTACH_GPRS_NETWORK);
    gsmCmdSend(GSM_CONTYPE_GPRS);
    gsmCmdSend(GSM_CONTYPE_APN);
    gsmCmdSend(GSM_GPRS_CONNECT);
    gsmCmdSend(GSM_GPRS_CHECK);
    gsmCmdSend(GSM_ENABLE_HTTP_SERVICE);
#endif
}

void gsmModuleDisconnectGprs(void)
{
    LOG_TRACE(GSM_CMP, "Disconnect from GPRS...\r\n");

#ifdef GPRS_ENABLED
    gsmCmdSend(GSM_DISABLE_HTTP_SERVICE);
    gsmCmdSend(GSM_GPRS_DISCONNECT);
#endif
}

void gsmModuleSendGetHttpRequest(void)
{
#ifdef GPRS_ENABLED
    gsmCmdSend(GSM_HTTP_SET_BEARER_PROFILE_ID);
    gsmCmdSend(GSM_HTTP_SET_URL);
    gsmCmdSend(GSM_HTTP_SET_GET_METHOD);
    gsmCmdSend(GSM_HTTP_READ_DATA);
#endif
}

void gsmModuleCfg(void)
{
  gsmCmdSend(GSM_ECHO_DISABLE);
  gsmCmdSend(GSM_FIXED_BAUDRATE);
  gsmCmdSend(GSM_TA_RESPONSE_FORMAT_ENABLE);
  gsmCmdSend(GSM_SMS_TEXT_MESSAGE_FORMAT);
  gsmCmdSend(GSM_NEW_SMS_MESSAGE_INDICATION);
  gsmCmdSend(GSM_REPORT_ERROR_CODE_VERBOSE_ENABLE);
  gsmCmdSend(GSM_LEGACY_SMS_CLEAR);

  //gsmCmdSend(GSM_PHONEBOOK_READ_ALL);

  gsmCmdSend(GSM_SLEEP_MODE_DTR);
}

void gsmModulePhoneNumberAdd(char* number, char* name)
{
  char buf[100] = {0};

  sprintf(buf, GSM_PHONEBOOK_WRITE_ENTRY, number, name);

  gsmCmdSend(buf);
}

/* handle asynchronous event about new SMS from GSM module */
static RV_t gsmModuleAsyncEventHandle(char *str)
{
  char gsmCommGetSms[GSM_CTRL_CMD_LEN] = {0};

  /* notification about new message received.
     create command to retrieve the message itself */
  if (RV_SUCCESS == gsmCmpCommand(str, GSM_MSG_CMTISM))
  {
    if (RV_SUCCESS == gsmFormSmsGetRqst(str, gsmCommGetSms))
    {
      gsmCmdSend(gsmCommGetSms);
    }
  }
  /* message content received*/
  else if (RV_SUCCESS == gsmCmpCommand(str, GSM_MSG_CMT))
  {
    LOG_TRACE(GSM_CMP, "Rec sms: %s", str);
    gsmTaskCb(str);
  }
  else
  {
    LOG_TRACE(GSM_CMP, "Rec event:%s", str);
  }

  return RV_SUCCESS;
}

/* Analyze input command from GSM module and send to corresponding receiver */
static RV_t gsmModuleCmdAnalyze(char *buf, uint32_t len, uint32_t *val)
{
  (void) len;

  //*val = 0;

  if ((!buf) || (!val))
  {
    return RV_FAILURE;
  }

  /* gsm module is switched on */
  if (RV_SUCCESS == gsmCmpCommand(buf, GSM_CALL_RDY_EVENT) ||
      RV_SUCCESS == gsmCmpCommand(buf, GSM_MODULE_VENDOR_NAME))
  {
    gsmReady = true;

    /* allow commands to be dispatched to GSM */
    cur_command.ack = true;

    gsmReadySet();

    //gsmModuleConnectGprs();

    return RV_SUCCESS;
  }
  if (RV_SUCCESS == gsmCmpCommand(buf, GSM_NETWORK_STATUS_STR))
  {
    uint32_t resp = strnlen(buf, len);
    uint32_t data = buf[resp-1] - '0';

    LOG_TRACE(GSM_CMP, "returned %s", buf);

    if (data == 1)
    {
      gsmReady = true;
    }
    return RV_SUCCESS;
  }
  else if (RV_SUCCESS == gsmCmpCommand(buf, GSM_POWER_DOWN_EVENT))
  {
    if (RV_SUCCESS != gsmCallEventCb(GSM_EVENT_DOWN))
    {
      LOG_TRACE(GSM_CMP,"Failed to power off GSM");
      return RV_FAILURE;
    }

    LOG_TRACE(GSM_CMP, "GSM IS DISABLED");

    gsmReady = false;

    return RV_SUCCESS;
  }
  else if (RV_SUCCESS == gsmCmpCommand(buf, GSM_RDY_EVENT))
  {
    return RV_SUCCESS;
  }
  else if (RV_SUCCESS == gsmCmpCommand(buf, GSM_CPIN_RDY_EVENT))
  {
    return RV_SUCCESS;
  }
  else if (RV_SUCCESS == gsmCmpCommand(buf, GSM_CFUN_EVENT))
  {
    return RV_SUCCESS;
  }
  else if (RV_SUCCESS == gsmCmpCommand(buf, GSM_POWER_WARN_EVENT))
  {
    /* change supply voltage range from normal: 3.4 ... 4.5V to extreme: 3.1…4.7V
     * this will increase our chances to successfully send last notification msg */
    gsmCmdSend("AT+VR=1\r\r\n");

    /* wait until GSM module process the command and send response */
    chThdSleepMilliseconds(300);

    if (RV_SUCCESS != gsmCallEventCb(GSM_EVENT_POWER_LOW))
    {
      LOG_TRACE(GSM_CMP,"Failed send under-voltage event");
      return RV_FAILURE;
    }

    LOG_TRACE(GSM_CMP, "Received under-voltage event");

    return RV_SUCCESS;
  }
  else if (RV_SUCCESS == gsmCmpCommand(buf, GSM_BALANCE_RESPONSE))
  {
    /* possible return types:
     * +CUSD: 4  or  +CUSD: 2
     * +CUSD: 1 or 0,"Na rahunku 13.05 grn.
     *  */
    char* pMatch = strstr(buf, GSM_BALANCE_MATCH_TYPE);
    uint32_t ret_type = atoi(pMatch + strlen(GSM_BALANCE_MATCH_TYPE));

    if ((ret_type == 1) || (ret_type == 0))
    {
      pMatch = strstr(buf, GSM_BALANCE_MATCH_STR);
      balance_s.balance = atof(pMatch + strlen(GSM_BALANCE_MATCH_STR));
      balance_s.state = true;
    }
    else if ((ret_type == 2) || (ret_type == 4))
    {
      balance_s.balance = 0.0;
      balance_s.state = false;
    }
    else
    {
      LOG_TRACE(GSM_CMP,"GSM returned unexpected balance type - %u", ret_type);
    }

    LOG_TRACE(GSM_CMP, "Balance = %f", balance_s.balance);

    return gsmCallEventCb(GSM_EVENT_BALANCE_SIGN_BATT);
  }
  else if (RV_SUCCESS == gsmCmpCommand(buf, GSM_BATTERY_CMD_RESPONCE))
  {
    /* +CBC: 0,80,3916 */
    char* sMatch = 0;
    char *eMatch = 0;
    sMatch = strchr(buf, ',');
    eMatch = strrchr(buf, ',');
    *eMatch = '\0';
    battery = atoi(sMatch+1);

    LOG_TRACE(GSM_CMP, "Battery discharge: %u", battery);

    /* notify user after battery discharge level has reached 3% */
    if (battery < 3)
    {
      if (RV_SUCCESS != gsmCallEventCb(GSM_EVENT_POWER_LOW))
      {
        LOG_TRACE(GSM_CMP,"Failed send low battery event\r\n");
        return RV_FAILURE;
      }
    }

    return RV_SUCCESS;
  }
  else if (RV_SUCCESS == gsmCmpCommand(buf, GSM_SIGNAL_CMD_RESPONCE))
  {
    /* +CSQ: 9,0 */
    char* sMatch = 0;
    char *eMatch = 0;
    sMatch = strchr(buf, ' ');
    eMatch = strrchr(buf, ',');
    *eMatch = '\0';
    signal = atoi(sMatch+1);

    LOG_TRACE(GSM_CMP, "Signal level: %u", signal);

    return RV_SUCCESS;
  }
#if 0
  else if (RV_SUCCESS == gsmCmpCommand(buf, GSM_PHONE_BOOK_READ_MATCH_STR))
  {
    char number[MAX_BUF_LEN] = {};

    gsmPhoneNumberParse(buf, number);
    gsmPhoneNumberAdd(number);
  }
#endif
  else if (0 == strncmp(buf, GSM_NEW_MSG_EVENT, sizeof(GSM_NEW_MSG_EVENT)-1))
  {
    if (RV_SUCCESS != gsmModuleAsyncEventHandle(buf))
    {
      LOG_TRACE(GSM_CMP,"Failed to sent parsed cmd\r\n");
      return RV_FAILURE;
    }
    return RV_SUCCESS;
  }
  else
  {
    /* reply to previously sent command */
    if (!strcmp(buf, "OK"))
    {
      *val = GSM_CMD_OK;
    }
    else if (!strcmp(buf, "ERROR"))
    {
      *val = GSM_CMD_ERROR;
      LOG_TRACE(GSM_CMP, "ERROR");
    }
    else if (!strncmp(buf, "+CMS ERROR:", strlen("+CMS ERROR:")))
    {
      char* sMatch = 0;
      sMatch = strchr(buf, ':');
      sMatch++;

      if (strncmp(sMatch, "PS busy", sizeof("PS busy")))
      {
        *val = GSM_PS_BUSY;
      }
      else if (strncmp(sMatch, "operation not allowed", sizeof("operation not allowed")))
      {
        *val = GSM_OP_NOT;
      }
      else
      {
        *val = GSM_CMS_ERROR;
      }

      LOG_TRACE(GSM_CMP, "CMS ERROR");
    }
    else if (!strncmp(buf, "+CME ERROR:", strlen("+CME ERROR:")))
    {
      *val = GSM_CME_ERROR;

      LOG_TRACE(GSM_CMP, "CME ERROR");
    }
    /* while sending  SMS '>' can be considered as SMS send command acknowledgment */
    else if (!strcmp(buf, ">"))
    {
      *val = GSM_SMS_PROMPT;
    }
    else
    {
      LOG_TRACE(GSM_CMP, "CMD:%s\r\n", buf);
    }
  }
  return RV_SUCCESS;
}

/* Parse input value and form GSM reply command.
 * State machine handles following cases:
 * "\r\nRESPONSE\r\n"
 * "\r\n> "
 * "\r\n+CMT*RESPONSE*\r\n*RESPONSE*\r\n" -> skip \r\n at the middle of response.
 *  */
static RV_t gsmModuleCmdParse(const char *buf, int32_t len, gsmCmdAnalyze_t cb, uint32_t *val)
{
  static gsm_parse_state state = WAIT_FOR_SOS_CR_STATE;
  static char gsmCmdBuf[MAX_GSM_CMD_LEN];
  static char *p = gsmCmdBuf;
  static uint8_t cmtPresence = 0;

  if (!buf)
  {
    return RV_FAILURE;
  }

  while (len > 0)
  {
    switch (state)
    {
      case WAIT_FOR_SOS_CR_STATE:
        /* unlike other states, this state handle the case when junk bytes
         * are received before \r\n (like at gsm module startup) */
        if (*buf == CR)
        {
          state = WAIT_FOR_SOS_LF_STATE;
        }
        buf++;
        len--;
        if (len < 0)
        {
          ENABLE_GSM_TRACE(len, state, 0, 0);
          return RV_NOT_COMPLETED;
        }
        ENABLE_GSM_TRACE(len, state, 0, 0);
        break;

      case WAIT_FOR_SOS_LF_STATE:
        if (*buf == LF)
        {
          buf++;
          len--;
          state = STORE_DATA_STATE;
          if (len < 0)
          {
            ENABLE_GSM_TRACE(len, state, 0, 0);
            return RV_NOT_COMPLETED;
          }
        }
        else
        {
          LOG_TRACE(GSM_CMP,"char=%x\r\n", *buf);
          state = WAIT_FOR_SOS_CR_STATE;
        }
        ENABLE_GSM_TRACE(len, state, 0, 0);
        break;

      case STORE_DATA_STATE:
        while (len > 0)
        {
          if (*buf == '>')
          {
            *p++ = *buf++;
            len--;
            state = WAIT_FOR_SPACE_STATE;
            break;
          }
          else if (*buf == CR)
          {
            buf++;
            len--;
            state = WAIT_FOR_LF_STATE;
            break;
          }
          *p++ = *buf++;
          len--;
          if (len == 0)
          {
            ENABLE_GSM_TRACE(len, state, 0, 0);
            return RV_NOT_COMPLETED;
          }
        }
        ENABLE_GSM_TRACE(len, state, 0, buf);
        break;

      case WAIT_FOR_SPACE_STATE:
        if (*buf == ' ')
        {
          buf++;
          state = FINISH_STATE;
        }
        else
        {
          LOG_TRACE(GSM_CMP,"char=%x\r\n", *buf);
          state = WAIT_FOR_SOS_CR_STATE;
        }
        ENABLE_GSM_TRACE(len, state, 0, 0);
        break;

      case WAIT_FOR_LF_STATE:
        if (*buf == LF)
        {
          buf++;
          if ((0 == memcmp(gsmCmdBuf, GSM_MSG_CMT, strlen(GSM_MSG_CMT))) &&
              (cmtPresence == 0))
          {
            state = STORE_DATA_STATE;
            cmtPresence = 1;
            len--;
          }
          else
          {
            state = FINISH_STATE;
            cmtPresence = 0;
          }

          if (len < 0)
          {
            ENABLE_GSM_TRACE(len, state, 0, 0);
            return RV_NOT_COMPLETED;
          }
        }
        else
        {
          LOG_TRACE(GSM_CMP,"char=%x\r\n", *buf);
          state = WAIT_FOR_SOS_CR_STATE;
        }
        ENABLE_GSM_TRACE(len, state, 0, 0);
        break;

      case FINISH_STATE:
        *p = '\0';
        state = WAIT_FOR_SOS_CR_STATE;
        len--;
        ENABLE_GSM_TRACE(len, state, 0, gsmCmdBuf);
        if (RV_SUCCESS != cb(gsmCmdBuf, (p-gsmCmdBuf)+1, val))
        {
          LOG_TRACE(GSM_CMP,"Failed to analyze GSM reply\r\n");
        }
        p = gsmCmdBuf;
        break;
    }
  }
  return RV_SUCCESS;
}

static THD_FUNCTION(gsmTask, arg)
{
  (void) arg;
  uint32_t gsmInByteNum = 0;
  char buf[MAX_GSM_CMD_LEN] = {0};
  char* pBuf = 0;
  msg_t res = 0;
  uint32_t val = 0;
  uint32_t currTime = 0;
  RV_t rv = RV_FAILURE;
  uint32_t time = {0};
  char lastCmd[GSM_CTRL_CMD_LEN] = {0};
#if GSM_ERROR_HANDLE
  BOOL firstTime = TRUE;
#endif

  while (1)
  {
    /*Decrease the read speed from UART*/
    chThdSleepMilliseconds(100);

    /* dispatch next cmd to GSM if response to previous command was received */
    if (cur_command.ack == true)
    {
      res = chMBFetch(&gsm_tx_mb_s, (msg_t*)&pBuf, 100);
      if (res == MSG_OK)
      {
        cur_command.id++;
#ifdef GSM_SLEEP
        /* exit sleep mode. serial port will be active after about 50ms */
        palClearPad(GPIOA, GPIOA_PIN6);
        chThdSleepMilliseconds(100);
#endif
        LOG_TRACE(GSM_CMP, "Sending a command #%d:%s", cur_command.id, pBuf);
        if (RV_SUCCESS != gsmModuleSend(pBuf))
        {
          LOG_TRACE(GSM_CMP,"gsmModuleSend failed\r\n");
        }

        cur_command.ack = false;

        /* store current command to be resent in case of GSM module error */
        strncpy(lastCmd, pBuf, sizeof(lastCmd));

        /* return memory block to heap pool */
        if (pBuf)
        {
          LOG_TRACE(GSM_CMP, "0x%x free\r\n", pBuf);
          chHeapFree((void *) pBuf);
        }
      }
    }

    /* get data from serial port if any. Get up to sizeof(buf) bytes */
    memset(buf, 0, sizeof(buf));
    gsmInByteNum = sdAsynchronousRead(&GSM_SERIAL_PORT, (uint8_t *) buf, MAX_GSM_CMD_LEN - 1);
    if ((gsmInByteNum > 0) && (gsmInByteNum < MAX_GSM_CMD_LEN))
    {
      LOG_TRACE(GSM_CMP, "GSM returned %d bytes:%s", gsmInByteNum, buf);

      rv = gsmModuleCmdParse(buf, gsmInByteNum, gsmModuleCmdAnalyze, &val);
      if (rv != RV_SUCCESS)
      {
          LOG_TRACE(GSM_CMP,"GSM parser error: %u", rv);
      }

      /* check if response to previously sent command was received */
      if (val)
      {
#if GSM_ERROR_HANDLE
        if (val != GSM_CMD_OK)
        {
          /* if out of balance error, then wait for user to pay */
          /* if network is unreachable, then just wait */

          /* in case of unrecoverable GSM error, send command again
           * and reboot GSM if did not help */
          if ((val == GSM_CME_ERROR) || (val == GSM_CMS_ERROR))
          {
            LOG_TRACE(GSM_CMP,"GSM replied with ERROR. rv=%u", rv);

            if (firstTime)
            {
              chMBPostAhead(&gsm_tx_mb_s, (msg_t) lastCmd, TIME_IMMEDIATE);
              firstTime = FALSE;
            }
            else
            {
              /* looks like we do not need to send GSM init commands during
               * module reboot, so no need to save commands from GSM queue */

              firstTime = TRUE;

              /* reboot GSM module */

              cur_command.ack = false;
              continue;
            }
          }
        }
#endif
        /* allow next command to be dispatched to GSM module */
        cur_command.ack = true;
#ifdef GSM_SLEEP
        /* enter sleep mode */
        palSetPad(GPIOA, GPIOA_PIN6);
#endif
      }
      val = 0;
    }

    /* detect low battery voltage level */
    currTime = logTimeStampGet();
    if ((currTime - time) >= 30)
    {
      time = currTime;
      gsmCmdSend(GSM_BATTERY_DISCHARGE);
    }
  }
}

RV_t gsmTaskInit(void)
{
  /* create message queue to send asynchronous requests */
  chMBObjectInit(&gsm_tx_mb_s, gsm_tx_msg_queue_s, MAILBOX_QUEUE_TX_SIZE);

  if (serialInit(GSM_SERIAL_SPEED, &GSM_SERIAL_PORT) < 0)
  {
    LOG_TRACE(GSM_CMP, "Serial is already occupied");
    return RV_FAILURE;
  }

  /* Create thread */
  chThdCreateStatic(gsmThread, sizeof(gsmThread), NORMALPRIO+1, gsmTask, 0);

  memset(&phoneBook_g, 0, sizeof(phoneBook_g));

  phoneBook_g.resp_is_set = TRUE;
  strncpy(phoneBook_g.resp_number, "+380982297151", sizeof(phoneBook_g.resp_number));

  /* add predefined phone number to allow out-of-box configuration */
  gsmPhoneNumberAdd("+380982297151");

  return RV_SUCCESS;
}

/*
 * Create message and sent to GSM
 */
RV_t gsmSendSmsToNumber(const char *telNum, const char *text)
{
  char buf[MAX_BUF_LEN] = {0};
  uint32_t len = 0;

  strcpy(buf, SEND_SMS_STR);
  strcat(buf, telNum);
  strcat(buf, "\"\r");

  gsmCmdSend(buf);

  strncpy(buf, text, sizeof(buf));
  len = strlen(buf);

  /* transmit EOF character */
  if ((len+1) >= sizeof(buf))
  {
    buf[len-1] = EOF_SMS;
    buf[len] = NULL_SYM;
  }
  else
  {
    buf[len] = EOF_SMS;
    buf[len+1] = NULL_SYM;
  }

  gsmCmdSend(buf);

  return RV_SUCCESS;
}

/* Call a callback */
RV_t gsmCallEventCb(gsmEvent_t event)
{
  if ((event > GSM_EVENT_UNKNOWN) && (event < GSM_EVENT_LAST))
  {
      return gsmCbArray_g[event]();
  }

  return RV_FAILURE;
}

bool gsmReadyCheck(void)
{
  return gsmReady;
}

RV_t gsmModuleInit()
{
    uint32_t count = 0;

    /* Check whether GSM module is already running.
     * Send test command and wait for GSM reply.
     */
    if (RV_SUCCESS != gsmModuleCmdSend(GSM_MODULE_MODEL_GET))
    {
      return RV_FAILURE;
    }

    /* add 1 sec delay */
    while (count < 10)
    {
      chThdSleepMilliseconds(100);

      if (gsmReady == true)
      {
        return RV_SUCCESS;
      }

      count++;
    }

    /* If GSM module does not response within timeout,
     * proceed to switch it on.
     */
    bspGsmPowerOnOff();

    /* block on cond var until received "Call Ready" from GSM */
    gsmReadyGet();

    /* GSM is ready to be configured. Send initialization commands */
    gsmModuleCfg();

  return RV_SUCCESS;
}

RV_t gsmPhoneNumberParse(const char* buf, char* number)
{
  char* pMatchStart = NULL;
  char* pMatchEnd   = NULL;
  int   length      = 0;

  pMatchStart = strchr(buf, '"');
  pMatchStart++;
  pMatchEnd = strchr(pMatchStart, '"');
  length = pMatchEnd - pMatchStart;

  memcpy(number, pMatchStart, length);

  return RV_SUCCESS;
}

RV_t gsmPhoneNumberAdd(const char* number)
{
  strncpy(phoneBook_g.data[phoneBook_g.size].number, number, GSM_PHONE_NUMBER_LEN);

  phoneBook_g.size++;

  return RV_SUCCESS;
}

RV_t gsmPhoneNumberFind(const char* number)
{
  uint32_t i = 0;

  for (i = 0; i < GSM_PHONE_BOOK_SIZE; i++)
  {
    if (0 == strncmp(phoneBook_g.data[i].number, number, GSM_PHONE_NUMBER_LEN))
    {
      return RV_SUCCESS;
    }
  }

  LOG_TRACE(GSM_CMP,"Phone number is not found\r\n");
  return RV_FAILURE;
}

/******************************************************
* @purpose Receive ctrl message from user and
*          parse cmds
*
* @notes   1) ctrl message format: 1 - start, 2 - stop
*          2) get tel number and compare it with predefined
*             numbers from internal database
*          3) input data example:
*             +CMT: "+12223334444","","14/05/29,01:04:18-32"
*             The text message body goes here
*
* @end
******************************************************/
RV_t gsmTaskCb(const char *in)
{
  char senderTelNum[GSM_PHONE_NUMBER_LEN] = {0};

  /* First of all extract tel number and ensure that we trust this sender */
  gsmPhoneNumberParse(in, senderTelNum);

  if (gsmPhoneNumberFind(senderTelNum) != RV_SUCCESS)
  {
    LOG_TRACE(GSM_CMP,"Unauthorized user access!\r\n");
    return RV_FAILURE;
  }

  //phoneBook_g.resp_is_set = TRUE;

  //strncpy(phoneBook_g.resp_number, senderTelNum, sizeof(senderTelNum));

  if (RV_SUCCESS == gsmCmpCommand(in, START_CMD))
  {
    return gsmCallEventCb(GSM_EVENT_SMS_START);
  }

  if (RV_SUCCESS == gsmCmpCommand(in, STOP_CMD))
  {
    return gsmCallEventCb(GSM_EVENT_SMS_STOP);
  }

  if (RV_SUCCESS == gsmCmpCommand(in, STATE_CMD))
  {
    return gsmCallEventCb(GSM_EVENT_SMS_STATE);
  }

  LOG_TRACE(GSM_CMP,"Unsupported SMS command\r\n");

  return RV_FAILURE;
}

RV_t gsmCmdSend(const char *gsm_command)
{
  RV_t rv = RV_FAILURE;

  if (gsmReadyCheck() == true)
  { 
    if (RV_SUCCESS == (rv = gsmModuleCmdSend(gsm_command)))
    {
      return RV_SUCCESS;
    }
    else if (RV_NOT_READY == rv)
    {
      chThdSleepMilliseconds(300);
      rv = gsmModuleCmdSend(gsm_command);
    }

    if (RV_FAILURE == rv)
    {
      LOG_TRACE(GSM_CMP,"Sending %s failed\r\n", gsm_command);
      return RV_FAILURE;
    }
  }
  else
  {
    LOG_TRACE(GSM_CMP, "GSM not ready\r\n");
    return RV_FAILURE;
  }
  return RV_SUCCESS;
}

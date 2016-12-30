/*****************************************************************************
* File:       control.c
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
#include <math.h>
#include "common.h"
#include "logging.h"
#include "control_api.h"
#include "accelGyro.h"
#include "gsm_api.h"
#include "bsp.h"
#include "cli.h"
#include "utils.h"

static RV_t ctrlGsmEventUpProcess(void);
static RV_t ctrlGsmEventDownProcess(void);
static RV_t ctrlGsmEventBalanceProcess(void);
static RV_t ctrlGsmEventSmsStartProcess(void);
static RV_t ctrlGsmEventSmsStopProcess(void);
static RV_t ctrlGsmEventSmsStateProcess(void);
static RV_t ctrlImuEventAlarmProcess(void);
static RV_t ctrlVoltageEventAlarmProcess(void);
static RV_t ctrlGsmStateSend(ctrl_sm_state_t state);
static RV_t ctrlStateToStr(ctrl_sm_state_t state, char *str, uint32_t len);
static RV_t underVoltageProcess(void);

#define ALERT_MSG_QUEUE_SIZE 5

mailbox_t alertMsg;
static msg_t alertMsgQueue[ALERT_MSG_QUEUE_SIZE];

/* state machine section */
ctrl_sm_state_t doStateIdle(ctrl_sm_event_t ev);
ctrl_sm_state_t doStateActive(ctrl_sm_event_t ev);
ctrl_sm_state_t doStateAlarm(ctrl_sm_event_t ev);
ctrl_sm_state_t doStateTest(ctrl_sm_event_t ev);

runState stateArray[] = {doStateIdle, doStateActive, doStateAlarm, doStateTest};

ctrl_sm_state_t doStateIdle(ctrl_sm_event_t ev)
{
  ctrl_sm_state_t st = IDLE_STATE;
  //dof_t dof;

  switch (ev)
  {
    case START_EVENT:
      LOG_TRACE(CONTROL_CMP, "START event!\r\n");

      st = ACTIVE_STATE;

      LOG_TRACE(CONTROL_CMP, "ACTIVE state!\r\n");

      /* enable collection of statistics from gyro and accel */
      //imuSumAngleGet(&dof);

      //LOG_TRACE(CONTROL_CMP, "x=%f y=%f", dof.x, dof.y);

      //imuThresholdSet(dof, 0.5);
      //imuEnable();

      if (RV_SUCCESS != gsmSmsSend("ARM\r\n"))
      {
        LOG_TRACE(CONTROL_CMP, "Sms msg send failed!\r\n");
      }
      break;

    case STOP_EVENT:
      LOG_TRACE(CONTROL_CMP, "STOP event!\r\n");
      break;

    case ALARM_EVENT:
      LOG_TRACE(CONTROL_CMP, "ALARM event!\r\n");
      break;

    case STATE_EVENT:
      /* request GSM battery discharge, signal level, SIM card balance */
      if (RV_SUCCESS != gsmStateReqSend())
      {
        LOG_TRACE(CONTROL_CMP, "Could not send state request commands\r\n");
      }

      /* wait for balance event */

      break;

    case BALANCE_EVENT:
      if (RV_SUCCESS != ctrlGsmStateSend(st))
      {
        LOG_TRACE(CONTROL_CMP, "Could not get gTrack state\r\n");
      }
      break;

    case LOW_VOLTAGE_EVENT:
      if (RV_SUCCESS != underVoltageProcess())
      {
        LOG_TRACE(CONTROL_CMP, "Ctrl task: under voltage event failed\r\n");
      }
      break;

    default:
      LOG_TRACE(CONTROL_CMP, "UNSUPPORTED event!\r\n");
      break;
  }

  return st;
}

ctrl_sm_state_t doStateActive(ctrl_sm_event_t ev)
{
  ctrl_sm_state_t st = ACTIVE_STATE;

  switch (ev)
  {
    case START_EVENT:
      LOG_TRACE(CONTROL_CMP, "START event!\r\n");
      break;

    case STOP_EVENT:
      LOG_TRACE(CONTROL_CMP, "STOP event!\r\n");

      st = IDLE_STATE;

      LOG_TRACE(CONTROL_CMP, "IDLE state!\r\n");

      /* disable collection of statistics from gyro and accel */
      imuDisable();

      if (RV_SUCCESS != gsmSmsSend("DISARM\r\n"))
      {
        LOG_TRACE(CONTROL_CMP, "Sms msg send failed!\r\n");
      }

      break;

    case ALARM_EVENT:
      LOG_TRACE(CONTROL_CMP, "ALARM event!\r\n");

      st = ALARM_STATE;

      if (RV_SUCCESS != gsmSmsSend("ALARM\r\n"))
      {
        LOG_TRACE(CONTROL_CMP,"Sms msg send failed!\r\n");
      }

      break;

    case STATE_EVENT:
      /* request GSM battery discharge, signal level, SIM card balance */
      if (RV_SUCCESS != gsmStateReqSend())
      {
        LOG_TRACE(CONTROL_CMP,"Could not send state request commands\r\n");
      }

      /* wait for balance event */

      break;

    case BALANCE_EVENT:
      if (RV_SUCCESS != ctrlGsmStateSend(st))
      {
        LOG_TRACE(CONTROL_CMP,"Could not get gTrack state\r\n");
      }
      break;

    case LOW_VOLTAGE_EVENT:
      if (RV_SUCCESS != underVoltageProcess())
      {
        LOG_TRACE(CONTROL_CMP,"Ctrl task: under voltage event failed\r\n");
      }
      break;

    default:
      LOG_TRACE(CONTROL_CMP,"UNSUPPORTED event!\r\n");
      break;
  }

  return st;
}

ctrl_sm_state_t doStateAlarm(ctrl_sm_event_t ev)
{
  ctrl_sm_state_t st = ALARM_STATE;

  switch (ev)
  {
    case START_EVENT:
      LOG_TRACE(CONTROL_CMP, "START event!\r\n");
      break;

    case STOP_EVENT:
      LOG_TRACE(CONTROL_CMP, "STOP event!\r\n");

      st = IDLE_STATE;

      LOG_TRACE(CONTROL_CMP, "IDLE state!\r\n");

      /* disable collection of statistics from gyro and accel */
      imuDisable();

      if (RV_SUCCESS != gsmSmsSend("DISARM\r\n"))
      {
        LOG_TRACE(CONTROL_CMP,"Sms msg send failed!\r\n");
      }

      break;

    case ALARM_EVENT:
      LOG_TRACE(CONTROL_CMP, "ALARM event!\r\n");

      /*if (RV_SUCCESS != gsmSmsSend("ALARM\r\n"))
      {
        LOG_TRACE(CONTROL_CMP,"Sms msg send failed!\r\n");
      }*/

      break;

    case STATE_EVENT:
      /* request GSM battery discharge, signal level, SIM card balance */
      if (RV_SUCCESS != gsmStateReqSend())
      {
        LOG_TRACE(CONTROL_CMP,"Could not send state request commands\r\n");
      }

      /* wait for balance event */

      break;

    case BALANCE_EVENT:
      if (RV_SUCCESS != ctrlGsmStateSend(st))
      {
        LOG_TRACE(CONTROL_CMP,"Could not get gTrack state\r\n");
      }
      break;

    case LOW_VOLTAGE_EVENT:
      if (RV_SUCCESS != underVoltageProcess())
      {
        LOG_TRACE(CONTROL_CMP,"Under voltage event send failed\r\n");
      }
      break;

    default:
      LOG_TRACE(CONTROL_CMP,"UNSUPPORTED event!\r\n");
      break;
  }

  return st;
}

ctrl_sm_state_t doStateTest(ctrl_sm_event_t ev)
{
  (void) ev;
  /* this state does not have an handler. Test state is used only to trigger
     get functions for each sensor only once and then return to previous state */

  //chprintf(SER_STR, "\r\nSM: invalid state TEST, ev=%d!", ev);

  return IDLE_STATE;
}

ctrl_sm_state_t changeState(ctrl_sm_state_t st, ctrl_sm_event_t ev)
{
  return (stateArray[st])(ev);
}
/* end of state machine section */


static THD_WORKING_AREA(ctrlAppThread, 1024);

static THD_FUNCTION(ctrlAppTask, arg)
{
  (void)arg;

  msg_t resp = Q_OK;
  msg_t val = 0;
  ctrl_sm_state_t c_state = IDLE_STATE;

  while (1)
  {
    /* wait for event */
    if ((resp = chMBFetch(&alertMsg, &val, TIME_INFINITE)) >= Q_OK)
    {
      LOG_TRACE(CONTROL_CMP, "nst = %u, val = %d\r\n", c_state, val);
      c_state = changeState(c_state, val);
    }
  }
}

RV_t ctrlAppInit(void)
{
  /* create message queue to retrieve alert messages from other tasks */
  chMBObjectInit(&alertMsg, alertMsgQueue, ALERT_MSG_QUEUE_SIZE);

  /* Create thread */
  chThdCreateStatic(ctrlAppThread, sizeof(ctrlAppThread), NORMALPRIO+1, ctrlAppTask, 0);

  gsmRegisterEventCb(GSM_EVENT_UP, ctrlGsmEventUpProcess);
  gsmRegisterEventCb(GSM_EVENT_DOWN, ctrlGsmEventDownProcess);
  gsmRegisterEventCb(GSM_EVENT_SMS_START, ctrlGsmEventSmsStartProcess);
  gsmRegisterEventCb(GSM_EVENT_SMS_STOP, ctrlGsmEventSmsStopProcess);
  gsmRegisterEventCb(GSM_EVENT_SMS_STATE, ctrlGsmEventSmsStateProcess);
  gsmRegisterEventCb(GSM_EVENT_BALANCE_SIGN_BATT, ctrlGsmEventBalanceProcess);
  gsmRegisterEventCb(GSM_EVENT_POWER_LOW, ctrlVoltageEventAlarmProcess);

  imuRegisterEventCb(IMU_EVENT_ALARM, ctrlImuEventAlarmProcess);

  bspRegisterEventCb(BSP_LOW_VOLTAGE_EVENT, ctrlVoltageEventAlarmProcess);

  cliCmdRegister("start", ctrlGsmEventSmsStartProcess);
  cliCmdRegister("stop", ctrlGsmEventSmsStopProcess);
  cliCmdRegister("state", ctrlGsmEventSmsStateProcess);

  return RV_SUCCESS;
}

RV_t ctrlEventPost(ctrl_sm_event_t event)
{
  msg_t resp = Q_OK;

  LOG_TRACE(CONTROL_CMP, "Posting event %d\r\n", event);

  resp = chMBPost(&alertMsg, event, TIME_IMMEDIATE);
  if (resp < Q_OK)
  {
    LOG_TRACE(CONTROL_CMP,"rv = %i\r\n", resp);
    return RV_FAILURE;
  }

  return RV_SUCCESS;
}

static RV_t ctrlGsmEventUpProcess(void)
{
  return ctrlEventPost(START_EVENT);
}

static RV_t ctrlGsmEventDownProcess(void)
{
  return ctrlEventPost(STOP_EVENT);
}

static RV_t ctrlGsmEventBalanceProcess(void)
{
  return ctrlEventPost(BALANCE_EVENT);
}

static RV_t ctrlGsmEventSmsStartProcess(void)
{
  return ctrlEventPost(START_EVENT);
}

static RV_t ctrlGsmEventSmsStopProcess(void)
{
  return ctrlEventPost(STOP_EVENT);
}

static RV_t ctrlGsmEventSmsStateProcess(void)
{
  return ctrlEventPost(STATE_EVENT);
}

static RV_t ctrlImuEventAlarmProcess(void)
{
  return ctrlEventPost(ALARM_EVENT);
}

static RV_t ctrlVoltageEventAlarmProcess(void)
{
  return ctrlEventPost(LOW_VOLTAGE_EVENT);
}

static RV_t ctrlStateToStr(ctrl_sm_state_t state, char *str, uint32_t len)
{
  const char ctrlStateLookup[][10] =
  {
    "IDLE", "ACTIVE", "ALARM", "TEST"
  };

  if (!str)
  {
    return RV_FAILURE;
  }
  if (state >= LAST_STATE)
  {
    return RV_FAILURE;
  }

  strncpy(str, ctrlStateLookup[state], len);

  return RV_SUCCESS;
}

/* read GSM battery discharge, signal level, SIM card balance */
static RV_t ctrlGsmStateSend(ctrl_sm_state_t state)
{
  uint32_t signal = 0;
  uint32_t battery = 0;
  char buf[96] = "Balance (UAH): ";
  char temp[32] = {0};

  if (RV_SUCCESS == gsmStateReqGet(&signal, &battery, temp, sizeof(temp)))
  {
    STRCAT_SAFE(buf, temp);

    osapiItoa(signal, temp, sizeof(temp));
    STRCAT_SAFE(buf, "\r\nSIGNAL: \r\n");
    STRCAT_SAFE(buf, temp);

    osapiItoa(battery, temp, sizeof(temp));
    STRCAT_SAFE(buf, "\r\nBATTERY (%): \r\n");
    STRCAT_SAFE(buf, temp);

    STRCAT_SAFE(buf, "\r\nSTATE: \r\n");
    if (RV_SUCCESS != ctrlStateToStr(state, temp, sizeof(temp)))
    {
      STRCAT_SAFE(buf, "not ready\r\n");
    }
    else
    {
      STRCAT_SAFE(buf, temp);
    }

    if (RV_SUCCESS != gsmSmsSend(buf))
    {
      LOG_TRACE(CONTROL_CMP,"Failed to send GSM state!\r\n");
      return RV_FAILURE;
    }
  }
  else
  {
    LOG_TRACE(CONTROL_CMP,"Failed to get GSM state\r\n");
    return RV_FAILURE;
  }
  return RV_SUCCESS;
}

static RV_t underVoltageProcess(void)
{
  LOG_TRACE(CONTROL_CMP, "Low battery voltage! Powering off...\r\n");

  if (RV_SUCCESS != gsmSmsSend("Low battery voltage! Powering off...\r\n"))
  {
    LOG_TRACE(CONTROL_CMP,"Low voltage msg send failed!\r\n");
    return RV_FAILURE;
  }

  chThdSleepSeconds(20);

  /* switch off GSM module */
  gsmPowerOnOff();

  /* before switching device off some delay is required until
   * user receives power down notification via SMS.
   * Such delay is implicitly added by gsmPowerOnOff() routine,
   * so no need to add another one */

  /* switch off device */
  systemPowerOff();

  return RV_SUCCESS;
}

/******************************************************************************
* File:       control_api.h
*
* Created on: May 01, 2015
* Author:     rostokus
******************************************************************************/
#ifndef CONTROL_API
#define CONTROL_API

#include "common.h"

typedef enum
{
  START_EVENT = 0,
  STOP_EVENT,
  STATE_EVENT,
  ALARM_EVENT,
  BALANCE_EVENT,
  LOW_VOLTAGE_EVENT
} ctrl_sm_event_t;

typedef enum
{
  IDLE_STATE = 0,
  ACTIVE_STATE,
  ALARM_STATE,
  TEST_STATE,
  LAST_STATE,
} ctrl_sm_state_t;

typedef ctrl_sm_state_t (*runState)(ctrl_sm_event_t ev);

RV_t ctrlEventPost(ctrl_sm_event_t event);

RV_t ctrlAppInit(void);

#endif

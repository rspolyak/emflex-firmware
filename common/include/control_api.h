/******************************************************************************
* File:       control_api.h
*
* Created on: May 01, 2015
* Author:     rostokus
******************************************************************************/
#ifndef CONTROL_API
#define CONTROL_API

#include "common.h"

typedef int ctrl_sm_event_t;

typedef int ctrl_sm_state_t;

typedef RV_t (*runState)(ctrl_sm_event_t ev, ctrl_sm_state_t* state);

typedef struct ctrl_state_info_s
{
  const char* name;
  runState cb;
} ctrl_state_info_t;

RV_t ctrlEventPost(ctrl_sm_event_t event);

RV_t ctrlAppInit(void);

RV_t ctrlStateInitStateSet(ctrl_sm_state_t state);

RV_t ctrlStateAdd(ctrl_sm_state_t state, runState cb, const char* name);

const char* ctrlCurStateToStr(void);

#endif

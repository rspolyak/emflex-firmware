/*****************************************************************************
* File:       stepper.h
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

#ifndef STEP_H
#define STEP_H

#include "common.h"

typedef enum
{
  STEP_UP,
  STEP_DOWN,
  FULLCLOSED,
  FULLOPEN
} step_pos_t;

extern mailbox_t stepMsg;

extern RV_t stepperAppInit(void);

void extcb3(EXTDriver *extp, expchannel_t channel);

#endif

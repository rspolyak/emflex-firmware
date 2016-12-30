/*****************************************************************************
* File:       accelGyro.h
*
* Created on: Aug 21, 2015
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

#ifndef ACCELGYRO
#define ACCELGYRO

#include <stdint.h>
#include "common.h"

typedef struct dof_s
{
  float x;
  float y;
  float z;
} dof_t;

typedef struct rawImu_s
{
  int16_t x;
  int16_t y;
  int16_t z;
} rawImu_t;

extern RV_t accelGyroInit(void);
extern RV_t imuEnable(void);
extern RV_t imuDisable(void);
extern RV_t imuWait(void);
extern RV_t imuThresholdSet(dof_t dof, float k);
extern RV_t imuSumAngleGet(dof_t *dof);

typedef enum
{
  IMU_EVENT_UNKNOWN,
  IMU_EVENT_ALARM,
  IMU_EVENT_LAST
} imuEvent_t;

typedef RV_t (*imuCbFunc_t)(void);

RV_t imuRegisterEventCb(imuEvent_t event, imuCbFunc_t cb);

#endif

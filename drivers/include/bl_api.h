/*****************************************************************************
* File:       bl_api.h
*
* Created on: Apr 14, 2016
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

#ifndef BL_API
#define BL_API

#include "common.h"

typedef RV_t (*blCbFunc_t)(const char *buf, int32_t len);

extern RV_t blInit(void);

RV_t blModuleSend(const char *val);

RV_t blRegisterEventCb(blCbFunc_t cb);

#endif



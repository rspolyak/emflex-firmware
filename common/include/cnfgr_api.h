/*****************************************************************************
* File:       cnfgr.h
*
* Created on: Apr 08, 2016
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

#ifndef CNFGR_API
#define CNFGR_API

#include "cnfgr.h"

RV_t cnfgrInit(void);
RV_t cnfgrRegister(const char* name, cnfgrFunc_t cb);
RV_t cnfgrInvoke(void);

#endif



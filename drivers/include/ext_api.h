/*****************************************************************************
* File:       ext_api.h
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

#ifndef EXT_API
#define EXT_API

#include "common.h"

typedef RV_t (*ext_cb_t)(void);

RV_t extAppInit(void);

RV_t extAppCbRegister(uint32_t channel, uint32_t mode, ext_cb_t cb);

#endif /* EXT_API */


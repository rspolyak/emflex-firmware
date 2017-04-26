/*****************************************************************************
* File:       button.h
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

#ifndef BUT_H
#define BUT_H

#include "common.h"

extern RV_t buttonAppInit(void);

RV_t extcb1(void);
RV_t extcb2(void);

#endif

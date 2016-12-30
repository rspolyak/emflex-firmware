/*****************************************************************************
* File:       bl_api.c
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

#include "common.h"
#include "bl_ll_api.h"

/* Initialize bluetooth module */
RV_t blInit(void)
{
  /* create task, msg queue, etc. */
  if (RV_SUCCESS != blTaskInit())
  {
    return RV_FAILURE;
  }

  return RV_SUCCESS;
}

/*****************************************************************************
* File:       logging.h
*
* Created on: Jul 26, 2016
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

#ifndef LOG_API
#define LOG_API

#include "common.h"

#define LOGGING_THREAD_STACK_SIZE 1024

RV_t loggingAppInit(void);

void logEvent(const char *msg, ...);

#define DEBUG

#ifdef DEBUG
#define LOG_TRACE(cmp, msg, ...)                                           \
   logEvent(msg, ##__VA_ARGS__);
#else
#define LOG_TRACE(cmp, msg, ...)
#endif

#endif


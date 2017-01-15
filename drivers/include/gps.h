/**
******************************************************************************
* File:         gps.h
* Description:  GPS engine
*
* Author:       Denys Haryachyy
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

#ifndef _GPS_H_
#define _GPS_H_

/*===========================================================================*/
/**
 * @name Global settings
 * @{
 */
/*===========================================================================*/

/**
 * @brief   GPS buffer size.
 */
#define GPS_MAX_BUF_SIZE    80

/**
 * @brief   GPS serial speed.
 */
#define GPS_SERIAL_SPEED    9600


RV_t gpsInit(void);

typedef enum
{
  GPS_EVENT_LAT_LONG,
  GPS_EVENT_LAST
} gpsEvent_t;

typedef struct gpsData_s
{
  int latitude;
  int longitude;
  int scale;
} gpsData_t;

typedef RV_t (*gpsCbFunc_t)(gpsData_t*);

RV_t gpsRegisterEventCb(gpsEvent_t event, gpsCbFunc_t cb);

/** @} */

#endif  /* _GPS_H_ */

/** @} */

/******************************************************************************
* File:         wifi.h
* Description:  Wifi driver header.
*
* Created on:   January 19, 2017
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

#ifndef _WIFI_H_
#define _WIFI_H_

/*===========================================================================*/
/**
 * @name Global settings
 * @{
 */
/*===========================================================================*/

/**
 * @brief   WIFI buffer size.
 */
#define WIFI_MAX_BUF_SIZE    80

/**
 * @brief   WIFI serial speed.
 */
#define WIFI_SERIAL_SPEED    115200

RV_t wifiInit(void);

/** @} */

#endif  /* _WIFI_H_ */

/** @} */

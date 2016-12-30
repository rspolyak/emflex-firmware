/*****************************************************************************
* File:       bl_ll_api.h
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
#ifndef BL_LL_API
#define BL_LL_API

/**
 * @brief   CLI serial port.
 * @note    Allowed values are SD1, SD2 or SD3.
 * @note    SD1 (PA9 - TX, PA10 - RX)
 */
#define BL_SERIAL_PORT     SD1

/**
 * @brief   CLI serial speed.
 */
#define BL_SERIAL_SPEED    9600

RV_t blTaskInit(void);

#endif

/*****************************************************************************
* File:       data_eeprom.h
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

#ifndef _DATA_EEPROM_H_
#define _DATA_EEPROM_H_

#include "stm32l1xx_flash.h"

/* first bytes is reserved and not used for storing data */
#define EEPROM_START		((uint32_t)0x08080000+1)

#define EEPROM_END		((uint32_t)0x08080FFF)	// 4kbytes
#define EEPROM_SIZE     4095

int DATAEEPROM_ReadByte(uint32_t addr, uint8_t *val);

#endif /* _DATA_EEPROM_H_ */

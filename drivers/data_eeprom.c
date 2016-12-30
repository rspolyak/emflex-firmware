/*****************************************************************************
* File:       data_eeprom.c
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

#include "ch.h"
#include "hal.h"
#include "data_eeprom.h"

typedef struct EepromSequentialStream
{
  const struct BaseSequentialStreamVMT *vmt;
  const size_t startAddr;
  uint32_t idx;
} EepromSequentialStream;

int DATAEEPROM_ReadByte(uint32_t addr, uint8_t *val)
{
  if (addr < EEPROM_START-1 || addr > EEPROM_END)
  {
    return -1;
  }

  *val = *((volatile uint8_t *) addr);

  return 0;
}

msg_t eeprom_putt(void *instance, uint8_t b)
{
  EepromSequentialStream *p = (EepromSequentialStream *) instance;

  uint32_t offset = p->idx % EEPROM_SIZE;

  p->idx++;

  DATA_EEPROM_Unlock();
  DATA_EEPROM_ProgramByte(p->startAddr + offset, b);
  DATA_EEPROM_Lock();

  return 0;
}

msg_t eeprom_gett(void *instance)
{
  (void) instance;

  return 0;
}

size_t eeprom_writet(void *instance, const uint8_t *bp, size_t n)
{
  (void) instance;
  (void) n;
  (void) bp;

  //return DATAEEPROM_ProgramByte(EEPROM_START, *bp);
  return 0;
}

size_t eeprom_readt(void *instance, uint8_t *bp, size_t n)
{
  (void) instance;
  (void) n;

  *bp = 0;

  return 0;
}

const struct BaseSequentialStreamVMT vmt = {.write=eeprom_writet, .read=eeprom_readt, .put=eeprom_putt, .get=eeprom_gett};

EepromSequentialStream EEPROM = {.vmt = &vmt, .idx = 0, .startAddr = EEPROM_START};


// #endif /* HAL_USE_DATA_EEPROM */

/** @} */

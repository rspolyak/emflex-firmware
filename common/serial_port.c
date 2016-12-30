/**
******************************************************************************
* File:         serial_port.c
* Description:  Serial port wrapper.
*
* Created on:   January 24, 2016
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

#include "common.h"

#define USART_ARRAY_SIZE 3

SerialDriver* usart_array[USART_ARRAY_SIZE] = {NULL};
int serial_number = 0;

/*
 * Reserve serial
 */
int serialVacantIdGet(void)
{
  int i = 0;

  for (i = 0; i < USART_ARRAY_SIZE; i++)
  {
    if (usart_array[i] == NULL)
    {
      return i;
    }
  }

  return -1;
}

/*
 * Reserve serial
 */
int serialReserve(SerialDriver* sdp)
{
  int id = serialVacantIdGet();

  if (id < 0)
  {
    return -1;
  }

  usart_array[id] = sdp;

  return 0;
}

/*
 * Check if Serial is used
 */
int isSerialInUse(SerialDriver* sdp)
{
  int i = 0;

  for (i = 0; i < USART_ARRAY_SIZE; i++)
  {
    if (usart_array[i] == sdp)
    {
      return true;
    }
  }

  return false;
}

/*
 * Serial port entry point.
 */
int serialInit(int speed, SerialDriver* sdp)
{
  SerialConfig sd_cfg;

  if (isSerialInUse(sdp) == true)
  {
      return -1;
  }

  if (serialReserve(sdp) < 0)
  {
      return -1;
  }

  sd_cfg.speed = speed;
  sd_cfg.cr1 = 0;
  sd_cfg.cr2 = USART_CR2_LINEN;
  sd_cfg.cr3 = 0;

  sdStart(sdp, &sd_cfg);

  return 0;
}

void serialPrint(SerialDriver* sdp, char *buf, uint32_t len)
{
  uint32_t i = 0;

  for (i = 0; i < len; i++)
  {
    if (buf[i] != '\r')
    {
      sdPut(sdp, buf[i]);
    }
    else
    {
      sdAsynchronousWrite(sdp, (uint8_t *) "\r\n>", sizeof("\r\n")-1);
    }
  }
}

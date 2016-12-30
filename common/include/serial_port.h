/**
 * @file    common.h
 * @brief   Common headers.
 */

#ifndef _SERIAL_PORT_H_
#define _SERIAL_PORT_H_

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "string.h"

/*
 * Serial port entry point.
 */
int serialInit(int speed, SerialDriver* sdp);

/*
 * Serial print.
 */
void serialPrint(SerialDriver* sdp, char *buf, uint32_t len);

#endif  /* _SERIAL_PORT_H_ */

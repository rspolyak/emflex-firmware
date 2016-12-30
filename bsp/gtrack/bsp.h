#ifndef BSP
#define BSP

#include "common.h"

typedef enum
{
  BSP_LOW_VOLTAGE_EVENT = 0,

  BSP_LAST_EVENT
} bsp_event_t;

typedef RV_t (*bsp_cb_t)(void);

extern RV_t bspInit(void);
extern RV_t bspRegisterEventCb(bsp_event_t ev, bsp_cb_t cb);
extern void gsmPowerOnOff(void);
extern void systemPowerOff(void);

/**
 * @brief   CLI serial port.
 * @note    Allowed values are SD1, SD2 or SD3.
 * @note    SD3 (PB10 - TX, PB11 - RX)
 */
#define CLI_SERIAL_PORT     SD3

#endif /* BSP */


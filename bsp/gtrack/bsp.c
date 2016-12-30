/******************************************************************************
* File:        bsp.c
*
* Created on:  Dec 27, 2015
*
* Description: platform specific routines for gtrack PCB
*
* Author:      rostokus
******************************************************************************/

#include "common.h"
#include "bsp.h"
//#include "cli.h"

static bsp_cb_t bsp_events[BSP_LAST_EVENT];

void gsmPowerOnOff(void)
{
  /* pull down PWRKEY pin in GSM module */
  palSetPad(GPIOC, GPIOC_PIN13);

  /* wait at least 1 sec */
  chThdSleepMilliseconds(1200);

  /* release PWRKEY (automatically raises HIGH) */
  palClearPad(GPIOC, GPIOC_PIN13);

  /* give GSM more time to ensure it is UP */
  chThdSleepMilliseconds(100);
}

void systemPowerOn(void)
{
  /* PWR IO is initialized in board.h file */
  palSetPad(SYS_PWR_PORT, SYS_PWR_PIN);

  return;
}

void systemPowerOff(void)
{
  /* PWR IO is initialized in board.h file */
  palClearPad(SYS_PWR_PORT, SYS_PWR_PIN);

  return;
}

RV_t bspRegisterEventCb(bsp_event_t ev, bsp_cb_t cb)
{
  if (ev >= BSP_LAST_EVENT)
  {
    return RV_FAILURE;
  }

  bsp_events[ev] = cb;

  return RV_SUCCESS;
}

static RV_t bspCallEventCb(bsp_event_t ev)
{
  if (ev >= BSP_LAST_EVENT)
  {
    return RV_FAILURE;
  }

  bsp_events[ev]();

  return RV_SUCCESS;
}

RV_t bspEventDeviceReboot(void)
{
  /* switch off GSM module */
  gsmPowerOnOff();

  /* before switching device off some delay is required until
   * user receives power down notification via SMS.
   * Such delay is implicitly added by gsmPowerOnOff() routine,
   * so no need to add another one */

  /* switch off device */
  systemPowerOff();

  return RV_SUCCESS;
}

static void extcb1(EXTDriver *extp, expchannel_t channel)
{
  (void)extp;
  (void)channel;

  /* send notification about low battery voltage */
  bspCallEventCb(BSP_LOW_VOLTAGE_EVENT);

  /* indicate low system voltage level */
  //palSetPad(GPIOC, 6);

  /* switch off GSM module */
  gsmPowerOnOff();

  /* before switching device off some delay is required until
   * user receives power down notification via SMS.
   * Such delay is implicitly added by gsmPowerOnOff() routine,
   * so no need to add another one */

  /* switch off device */
  systemPowerOff();

  return;
}

/* add interrupt for PA2 pin */
static const EXTConfig lowpwr_cfg = {
  {
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOA, extcb1},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL}
  }
};

RV_t bspInit(void)
{

  /* Activates the UART driver for debugging,
   * PB10 and PB11 are routed to USART3. */
  palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(7));

  /* set IO pin responsible for switching DC-DC converter */
  systemPowerOn();

  return RV_SUCCESS;
}

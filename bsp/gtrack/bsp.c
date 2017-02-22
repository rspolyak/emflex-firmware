/******************************************************************************
* File:        bsp.c
*
* Created on:  Dec 27, 2015
*
* Description: platform specific routines for gtrack PCB
*
* Author:      rostokus
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
#include "bsp.h"

#define BSP_PWR_OFF_CHANNEL GPIOB_PIN12

extern RV_t ctrlGsmEventSmsStartProcess(void);

static thread_reference_t trp = NULL;
static THD_WORKING_AREA(buttonThread, 64);

void bspGsmPowerOnOff(void)
{
  /* pull down PWRKEY pin in GSM module */
  palSetPad(GPIOC, BSP_GSM_PWR_PIN);

  /* wait at least 1 sec */
  chThdSleepMilliseconds(1200);

  /* release PWRKEY (automatically raises HIGH) */
  palClearPad(GPIOC, BSP_GSM_PWR_PIN);

  /* give GSM more time to ensure it is UP */
  chThdSleepMilliseconds(100);
}

void bspSystemPowerOn(void)
{
  /* PWR IO is initialized in board.h file */
  palSetPad(BSP_SYS_PWR_PORT, BSP_SYS_PWR_PIN);

  return;
}

void bspSystemPowerOff(void)
{
  /* PWR IO is initialized in board.h file */
  palClearPad(BSP_SYS_PWR_PORT, BSP_SYS_PWR_PIN);

  return;
}

static void bspExtcb1(EXTDriver *extp, expchannel_t channel)
{
  (void)extp;
  (void)channel;

  /* Wakes up the thread.*/
  chSysLockFromISR();
  chThdResumeI(&trp, 0);
  chSysUnlockFromISR();

  return;
}

/* External interrupt/event controller (EXTI) config.
   Each line corresponds to separate channel of EXTI */
static const EXTConfig bspPwrCfg = {
  {
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
    {EXT_CH_MODE_RISING_EDGE | EXT_MODE_GPIOB, bspExtcb1}, /* power on/off button is connected here */
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

THD_FUNCTION(buttonTask, arg)
{
  (void)arg;

  while (true) {

    /* Waiting for the wake up event from button EXTI.*/
    chSysLock();
    chThdSuspendS(&trp);
    chSysUnlock();

    /* wait 3 sec */
    chThdSleepSeconds(3);

    /* if button is still pressed - power off device.
     * Otherwise assume user want to enable Active state */
    if (PAL_LOW == palReadPad(GPIOB, GPIOB_PIN12))
    {
        /* let user complete his tasks (leave the room or a car) prior to enable alarm */
        chThdSleepSeconds(20);

        ctrlGsmEventSmsStartProcess();

        return;
    }
    else
    {
      /* switch off GSM module */
      /* pull down PWRKEY pin in GSM module */
      palSetPad(GPIOC, BSP_GSM_PWR_PIN);

      /* wait at least 1 sec */
      chThdSleepSeconds(1);

      /* release PWRKEY (automatically raises HIGH) */
      palClearPad(GPIOC, BSP_GSM_PWR_PIN);

      /* switch off device */
      bspSystemPowerOff();
    }
  }
}

RV_t bspInit(void)
{
  /* Device initialization has started.
     Switch on status LED */
  palSetPad(GPIOC, 6);

  /* Ensure power on button is pressed at least 3 sec.
     Otherwise device will be powered off */
  chThdSleepSeconds(3);

  /* Activates the UART driver for debugging,
   * PB10 and PB11 are routed to USART3. */
  palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(7));

  /* Activates GSM driver pins,
   * PA2 and PA3 are routed to USART2. */
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

  /* start EXTI driver that handles user button events */
  extStart(&EXTD1, &bspPwrCfg);

  /* set IO pin responsible for switching DC-DC converter */
  bspSystemPowerOn();

  /* Create thread */
  chThdCreateStatic(buttonThread, sizeof(buttonThread), NORMALPRIO+1, buttonTask, 0);

  return RV_SUCCESS;
}

/* GPT4 callback. */
static void bspGpt4cb(GPTDriver *gptp)
{
  (void) gptp;

  palTogglePad(GPIOC, 6);
}

/* GPT4 configuration. */
static const GPTConfig bspGpt4cfg = {
  1000,    /* 1kHz timer clock.*/
  bspGpt4cb,    /* Timer callback.*/
  0,
  0
};

RV_t bspInitComplete(void)
{
    /* Device initialization completed successfully */
    palClearPad(GPIOC, 6);

    /* enable processing power on/off button event */
    extChannelEnable(&EXTD1, BSP_PWR_OFF_CHANNEL);

    /* Display normal device activity.
     * Blink status LED each 3 sec */
    gptStart(&GPTD4, &bspGpt4cfg);
    gptStartContinuous(&GPTD4, 3000); /* 3000 / 1000 = trigger rate in seconds */

    return RV_SUCCESS;
}

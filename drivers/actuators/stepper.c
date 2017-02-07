/*****************************************************************************
* File:       stepper.c
*
* Created on: Apr 24, 2016
*
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

#include <string.h>
#include <ctype.h>
#include "common.h"
#include "stepper.h"
#include "logging.h"

#define DRIVE_QUEUE_SIZE 10
#define DRIVE_STEP       1
#define DRIVE_INFINITE  (50)

// motor pwm control
#define PWM_PORT        GPIOB
#define PWM_PIN         3
// motor direction 1
#define DIR1_PORT       GPIOB
#define DIR1_PIN        4
// motor direction 2
#define DIR2_PORT       GPIOB
#define DIR2_PIN        5

mailbox_t stepMsg;
static msg_t stepMsgQueue[DRIVE_QUEUE_SIZE];

static uint32_t dc_overc;

/* ADC conversion group settings */
#define ADC_GRP_NUM_CHANNELS   1
#define ADC_GRP_BUF_DEPTH      4

/* Buffer for ADC samples */
static adcsample_t samples[ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH];

/* ADC conversion group.
 * Mode:        Circular buffer, 1 samples of 1 channel, SW triggered.
 * Channels:    IN10 */
#ifdef STM32L1XX_MD
static const ADCConversionGroup adcgrpcfg = {
    TRUE,
    ADC_GRP_NUM_CHANNELS,
    NULL,
    NULL,
    0,                        /* CR1 */
    ADC_CR2_SWSTART,          /* CR2 */
    0,                        /* SMPR1 */
    ADC_SMPR2_SMP_AN10(ADC_SAMPLE_48),
    0,                        /* SMPR3 */
    ADC_SQR1_NUM_CH(ADC_GRP_NUM_CHANNELS),
    0, 0,                     /* SQR2, SQR3 */
    0,                        /* SQR4 */
    ADC_SQR5_SQ1_N(ADC_CHANNEL_IN10)
};
#else
static const ADCConversionGroup adcgrpcfg = {0};
#endif /* STM32L1XX_MD */

/* GPT2 callback. */
static void gpt4cb(GPTDriver *gptp)
{
  (void) gptp;

  if (samples[0] >= 10)
  {
    dc_overc = 1;
    palTogglePad(GPIOB, 7);
  }  
}

/* GPT2 configuration. */
static const GPTConfig gpt4cfg = {
  100000,    /* 100kHz timer clock.*/
  gpt4cb,    /* Timer callback.*/
  0,
  0
};

RV_t SetWindowPosition(step_pos_t position);

static THD_WORKING_AREA(stepThread, 512);

static THD_FUNCTION(stepTask, arg)
{
  (void) arg;

  msg_t resp = Q_OK;
  msg_t val = 0;

  while (1)
  {
    chThdSleepMilliseconds(100);

    /* wait for event */
    if ((resp = chMBFetch(&stepMsg, &val, TIME_INFINITE)) >= Q_OK)
    {
      LOG_TRACE(STEP_CMP, "val=%u", val);

      if (RV_SUCCESS != (resp = SetWindowPosition(val)))
      {
        LOG_TRACE(STEP_CMP, "SetWindowPosition() failed, resp=%u", resp);
      }
    }
  }
}

RV_t SetWindowPosition(step_pos_t position)
{
  int32_t on_time = 0;

  switch (position)
  {
    case FULLCLOSED:
      palClearPad(DIR1_PORT, DIR1_PIN);
      palSetPad(DIR2_PORT, DIR2_PIN);

      on_time = DRIVE_INFINITE;
      break;

    case STEP_UP:
      palClearPad(DIR2_PORT, DIR2_PIN);
      palSetPad(DIR1_PORT, DIR1_PIN);

      on_time = DRIVE_STEP;
      break;

    case STEP_DOWN:
      palClearPad(DIR1_PORT, DIR1_PIN);
      palSetPad(DIR2_PORT, DIR2_PIN);

      on_time = DRIVE_STEP;
      break;

    case FULLOPEN:
      palClearPad(DIR2_PORT, DIR2_PIN);
      palSetPad(DIR1_PORT, DIR1_PIN);

      on_time = DRIVE_INFINITE;
      break;

    default:
      return RV_FAILURE;
      break;
  }

  palSetPad(PWM_PORT, PWM_PIN);
  while (on_time && !dc_overc)
  {
    //palSetPad(GPIOB, 6);
    chThdSleepMilliseconds(500); // 1s delay
    on_time--;
    //palClearPad(GPIOB, 6);
  }
  dc_overc = 0;
  palClearPad(PWM_PORT, PWM_PIN);

  return RV_SUCCESS;
}

void InitMotor(void)
{
  /* already initialized in board.h file */
}

RV_t stepperAppInit(void)
{
  /* create message queue */
  chMBObjectInit(&stepMsg, stepMsgQueue, DRIVE_QUEUE_SIZE);

  /* Create thread */
  chThdCreateStatic(stepThread, sizeof(stepThread), NORMALPRIO+1, stepTask, 0);

  InitMotor();

  //ADC init
  /* Setup pins for ADC */
  palSetGroupMode(GPIOC, PAL_PORT_BIT(0), 0, PAL_MODE_INPUT_ANALOG);

  /* Start Sampling Timer Peripheral */
  gptStart(&GPTD4, &gpt4cfg);
  gptStartContinuous(&GPTD4, 100); /* 1kHz trigger rate .*/
  /* Start ADC and ADC Conversions */
  adcStart(&ADCD1, NULL);
  adcStartConversion(&ADCD1, &adcgrpcfg, samples, ADC_GRP_BUF_DEPTH);

  return RV_SUCCESS;
}

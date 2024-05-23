/*
 * Copyright (c) 2022 Hunan OpenValley Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <hdf_log.h>
#include <pwm_if.h>
#include "cmsis_os2.h"
#include "ohos_run.h"

#define HDF_PWM0_DUTY_OFFSET    10000
#define HDF_PWM0_PERIOD         500000

#define HDF_PWM_STACK_SIZE 0x1000
#define HDF_PWM_TASK_NAME "hdf_pwm_test_task"
#define HDF_PWM_TASK_PRIORITY 25

#define HDF_PWM_DELAY_TICKS 100

#define PWM_RED_LED_NUM 0

static void* HdfPwmTestEntry(int32_t arg)
{
    (void*)arg;
    int32_t ret;
    struct PwmConfig pcfg0 = { HDF_PWM0_PERIOD, HDF_PWM0_PERIOD, 0, PWM_NORMAL_POLARITY, PWM_ENABLE_STATUS };

    DevHandle pwm_led_handle = NULL;
    DevHandle blue_led_handle = NULL;
    pwm_led_handle = PwmOpen(PWM_RED_LED_NUM);
    if (pwm_led_handle == NULL) {
        printf("PwmOpen pwm%d failed!\n", PWM_RED_LED_NUM);
    }
    printf("open pwm%u success!\n", PWM_RED_LED_NUM);

    PwmSetConfig(pwm_led_handle, &pcfg0);
    
    while (1) {
        for (uint32_t duty = HDF_PWM0_PERIOD; duty > 0; duty -= HDF_PWM0_DUTY_OFFSET) {
            PwmSetDuty(pwm_led_handle, duty);
            LOS_TaskDelay(HDF_PWM_DELAY_TICKS);
        }
        LOS_TaskDelay(HDF_PWM_DELAY_TICKS);
        for (uint32_t duty = 0; duty <= HDF_PWM0_PERIOD; duty += HDF_PWM0_DUTY_OFFSET) {
            PwmSetDuty(pwm_led_handle, duty);
            LOS_TaskDelay(HDF_PWM_DELAY_TICKS);
        }
        LOS_TaskDelay(HDF_PWM_DELAY_TICKS);
    }
}

void StartHdfPwmTest(void)
{
    osThreadAttr_t attr;

    attr.name = HDF_PWM_TASK_NAME;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = HDF_PWM_STACK_SIZE;
    attr.priority = HDF_PWM_TASK_PRIORITY;

    if (osThreadNew((osThreadFunc_t)HdfPwmTestEntry, NULL, &attr) == NULL) {
        printf("Failed to create thread1!\n");
    }
}

OHOS_APP_RUN(StartHdfPwmTest);
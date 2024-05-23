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
#include "adc_if.h"
#include "cmsis_os2.h"
#include "hdf_log.h"
#include "ohos_run.h"
#include "osal_irq.h"

#define SYS_DELAY_TICKS 100
#define TASK_STACK_SIZE 4096
#define TASK_PRIO 25

#define ADC_DEVICE_NUM 1
#define ADC_CHANNEL_NUM 6

osThreadId_t g_taskID = NULL;

static void adc_test(void)
{
    DevHandle adcHandle;
    uint32_t readVal = 0;

    adcHandle = AdcOpen(ADC_DEVICE_NUM);
    if (adcHandle == NULL) {
        HDF_LOGE("AdcOpen fail!\r\n");
        return;
    }

    for (;;) {
        if (AdcRead(adcHandle, ADC_CHANNEL_NUM, &readVal) != HDF_SUCCESS) {
            HDF_LOGE("%s: read adc fail!\r\n", __func__);
            AdcClose(adcHandle);
            osThreadTerminate(g_taskID);
            return;
        }

        HDF_LOGE("Read adc value: %d\r\n", readVal);
        osDelay(SYS_DELAY_TICKS);
    }
}

static void adc_task(void)
{
    HDF_LOGE("into adc hdf example!\n");
    osThreadAttr_t attr;
    attr.name = "adc_test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TASK_STACK_SIZE;
    attr.priority = TASK_PRIO;
    g_taskID = osThreadNew((osThreadFunc_t)adc_test, NULL, &attr);
    if (g_taskID == NULL) {
        HDF_LOGE("Failed to create Test ADC thread!\n");
    }
}

OHOS_APP_RUN(adc_task);

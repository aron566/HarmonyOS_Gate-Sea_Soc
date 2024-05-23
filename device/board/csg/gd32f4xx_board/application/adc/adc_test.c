/*
 * Copyright (c) 2021 HiHope Open Source Organization.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "adc_if.h"
#include "los_task.h"
#include "cmsis_os2.h"
#include <hdf_log.h>

#define HDF_ADC_STACK_SIZE 0x1000
#define HDF_ADC_TASK_NAME "hdf_adc_test_task"
#define HDF_ADC_TASK_PRIORITY 27

VOID AdcTaskSampleEntry(VOID)
{
    uint32_t ret;
    uint32_t adcConvValue=0;

    // open multi channel adc device
    DevHandle adcHandler;
    adcHandler = AdcOpen(0);
    if(adcHandler == NULL) {
        printf("%s: adcHandler is NULL\n", __func__);
    }

    while (1) {
        /* ADC配置为DMA方式，按照hcs中adc_channel顺序读取，如：adc_channel = [11, 12, 13, 14, 15];，要读取12，则AdcRead传2*/
        //read ADC1 channel_1
        ret = AdcRead(adcHandler, 1, &adcConvValue);
        if (ret != 0) {
            printf("AdcRead failed, ret %u\n", ret);
        }
        printf("%s: [line = %d] [ADC_CH1 = %u]\n", __func__, __LINE__, adcConvValue);

        //read ADC1 channel_2
        ret = AdcRead(adcHandler, 2, &adcConvValue);
        if (ret != 0) {
            printf("AdcRead failed, ret %u\n", ret);
        }
        printf("%s: [line = %d] [ADC_CH2 = %u]\n", __func__, __LINE__, adcConvValue);

        //read ADC1 channel_3
        ret = AdcRead(adcHandler, 3, &adcConvValue);
        if (ret != 0) {
            printf("AdcRead failed, ret %u\n", ret);
        }
        printf("%s: [line = %d] [ADC_CH3 = %u]\n", __func__, __LINE__, adcConvValue);

        //read ADC1 channel_4
        ret = AdcRead(adcHandler, 4, &adcConvValue);
        if (ret != 0) {
            printf("AdcRead failed, ret %u\n", ret);
        }
        printf("%s: [line = %d] [ADC_CH4 = %u]\n", __func__, __LINE__, adcConvValue);

        //read ADC1 channel_5
        ret = AdcRead(adcHandler, 5, &adcConvValue);
        if (ret != 0) {
            printf("AdcRead failed, ret %u\n", ret);
        }
        printf("%s: [line = %d] [ADC_CH5 = %u]\n", __func__, __LINE__, adcConvValue);

        //AdcClose(adcHandler);  //关闭ADC
        (VOID)LOS_TaskDelay(500); 
    }
}

void StartHdfAdcTest(void)
{
    osThreadAttr_t attr;

    attr.name = HDF_ADC_TASK_NAME;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = HDF_ADC_STACK_SIZE;
    attr.priority = HDF_ADC_TASK_PRIORITY;

    if (osThreadNew((osThreadFunc_t)AdcTaskSampleEntry, NULL, &attr) == NULL) {
        HDF_LOGE("Failed to create thread1!\n");
    }
}

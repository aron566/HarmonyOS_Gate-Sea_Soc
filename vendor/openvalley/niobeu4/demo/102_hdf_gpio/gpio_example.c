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
#include "gpio_types.h"
#include "gpio_if.h"
#include "cmsis_os2.h"
#include "hdf_log.h"
#include "ohos_run.h"

#define SYS_DELAY_TICKS 200
#define TASK_STACK_SIZE 4096
#define TASK_PRIO 25

#define OFF 0
#define ON  1

 // 在hcs文件中对应具体的GPIO口
#define RED_LED_PIN_INDEX   0       // 红色LED
#define BLUE_LED_PIN_INDEX  1       // 蓝色LED
#define PWR_SW_PIN_INDEX    2       // VCC_3.3 电源使能
#define BUTTON_KEY_PIN_INDEX 3      // 独立按键

osThreadId_t g_taskID = NULL;


/**
 * @brief 中断回调函数
 *
 */
static int32_t GpioIrqCallback(uint16_t gpio, int data)
{
    (void *)data;
    HDF_LOGE("--->hdf gpio index %d interrupt!!", gpio);
}

static void gpio_test(void)
{
    uint16_t key_state = 0;
    GpioSetDir(PWR_SW_PIN_INDEX, GPIO_DIR_OUT);     // 设置VCC_3.3电源使能
    GpioWrite(PWR_SW_PIN_INDEX, ON);

    GpioSetDir(RED_LED_PIN_INDEX, GPIO_DIR_OUT);    // LED 配置为输出
    GpioSetDir(BLUE_LED_PIN_INDEX, GPIO_DIR_OUT);
     
    GpioSetDir(BUTTON_KEY_PIN_INDEX, GPIO_DIR_IN);  // 按键配置为输入
    GpioSetIrq(BUTTON_KEY_PIN_INDEX, GPIO_IRQ_TRIGGER_FALLING, (GpioIrqFunc)GpioIrqCallback, NULL); // 设置中断、上升下降沿触发
    GpioEnableIrq(BUTTON_KEY_PIN_INDEX);            // 使能中断
    while (1) {
        GpioWrite(RED_LED_PIN_INDEX, ON);       // 设置GPIO状态
        GpioWrite(BLUE_LED_PIN_INDEX, ON);      // 设置GPIO状态
        GpioRead(BUTTON_KEY_PIN_INDEX, &key_state); // 读取GPIO状态
        printf("--> The Button has been %s\r\n", key_state ? "released" : "pressed");
        osDelay(SYS_DELAY_TICKS);
        GpioWrite(RED_LED_PIN_INDEX, OFF);
        GpioWrite(BLUE_LED_PIN_INDEX, OFF);
        GpioRead(BUTTON_KEY_PIN_INDEX, &key_state);
        printf("--> The Button has been %s\r\n", key_state ? "released" : "pressed");
        osDelay(SYS_DELAY_TICKS);
    }
    GpioUnsetIrq(BUTTON_KEY_PIN_INDEX, NULL);
    GpioDisableIrq(BUTTON_KEY_PIN_INDEX);
}

static void gpio_example_task(void)
{
    HDF_LOGE("into gpio hdf example!\n");
    osThreadAttr_t attr;
    attr.name = "adc_test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TASK_STACK_SIZE;
    attr.priority = TASK_PRIO;
    g_taskID = osThreadNew((osThreadFunc_t)gpio_test, NULL, &attr);
    if (g_taskID == NULL) {
        HDF_LOGE("Failed to create Test GPIO thread!\n");
    }
}

OHOS_APP_RUN(gpio_example_task);

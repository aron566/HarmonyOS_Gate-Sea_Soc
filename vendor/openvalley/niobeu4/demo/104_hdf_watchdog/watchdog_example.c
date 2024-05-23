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
#include "esp_task_wdt.h"
#include "watchdog_if.h"
#include "cmsis_os2.h"
#include "hdf_log.h"
#include "ohos_run.h"

#define SYS_DELAY_TICKS 200
#define TASK_STACK_SIZE 4096
#define TASK_PRIO 25

#define HDF_WATCHDOG_TIMEOUT            5           // 5秒喂狗
#define HDF_WATCHDOG_TIMEOUT_START      100
#define HDF_WATCHDOG_TIMEOUT_OFFSET     100

osThreadId_t g_taskID = NULL;

void watchdog_api_test(DevHandle handle)
{
    int32_t ret;
    int32_t status;
    uint32_t timeOut = 0;
    ret = WatchdogGetStatus(handle, &status);
    if (ret != 0) {
        printf("WatchdogGetStatus: failed, ret %d\r\n", ret);
        return NULL;
    } else
        printf("WatchdogGetStatus: %d\r\n", status);

    ret = WatchdogGetTimeout(handle, &timeOut);
    if (ret != 0) {
        printf("WatchdogGetTimeout: failed, ret %d\r\n", ret);
        return NULL;
    } else
        printf("WatchdogGetTimeout: %d\r\n", timeOut);
    
    ret = WatchdogSetTimeout(handle, HDF_WATCHDOG_TIMEOUT);
    if (ret != 0) {
        printf("WatchdogSetTimeout: failed, ret %d\r\n", ret);
        return NULL;
    } else
        printf("WatchdogSetTimeout: %d\r\n", HDF_WATCHDOG_TIMEOUT);

    ret = WatchdogGetTimeout(handle, &timeOut);
    if (ret != 0) {
        printf("WatchdogGetTimeout: failed, ret %d\r\n", ret);
        return NULL;
    } else
        printf("WatchdogGetTimeout: %d\r\n", timeOut);

    ret = WatchdogStart(handle);
    if (ret != 0) {
        printf("WatchdogStart: failed, ret %d\r\n", ret);
        return NULL;
    } else
        printf("---> WatchdogStart.\r\n");

    ret = WatchdogGetStatus(handle, &status);
    if (ret != 0) {
        printf("WatchdogGetStatus: failed, ret %d\r\n", ret);
        return NULL;
    } else
        printf("WatchdogGetStatus: %d\r\n", status);
}

static void watchdog_test(void)
{
    int32_t ret;
    DevHandle handle = NULL;
    uint32_t delay_cnt = 0;

    ret = WatchdogOpen(0, &handle);
    if (HDF_SUCCESS != ret || handle == NULL) {
        printf("WatchdogOpen: failed, ret %d\r\n", ret);
        return NULL;
    }
    
    watchdog_api_test(handle);

    delay_cnt = HDF_WATCHDOG_TIMEOUT_START;
    while (1) {
        ret = WatchdogFeed(handle);
        if (ret != 0) {
            printf("WatchdogFeed: failed, ret %d\r\n", ret);
            return NULL;
        } else
            printf("--> WatchdogFeed success!\r\n");
        osDelay(delay_cnt);     // 延时越来越长，直到系统复位
        delay_cnt += HDF_WATCHDOG_TIMEOUT_OFFSET;
    }
}

static void watchdog_example_task(void)
{
    printf("into watchdog hdf example!\r\n");
    osThreadAttr_t attr;
    attr.name = "watchdog_test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TASK_STACK_SIZE;
    attr.priority = TASK_PRIO;
    g_taskID = osThreadNew((osThreadFunc_t)watchdog_test, NULL, &attr);
    if (g_taskID == NULL) {
        printf("Failed to create Test Watchdog thread!\r\n");
    }
}

OHOS_APP_RUN(watchdog_example_task);

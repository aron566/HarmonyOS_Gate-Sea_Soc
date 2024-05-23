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
#include <string.h>
#include <unistd.h>
#include "los_event.h"
#include "los_mux.h"
#include "los_task.h"
#include "ohos_run.h"

#ifndef osWaitForever
#define osWaitForever 0xFFFFFFFFU
#endif

#define EVENT_MASK_CLEAN 0x00000001
#define MAX_STR_LEN 256
#define DELAY_100_TICKS 100U
#define DELAY_400_TICKS (4 * DELAY_100_TICKS)
#define DELAY_600_TICKS (6 * DELAY_100_TICKS)
#define TASK_STACK_SIZE 4096
#define TASK_PRIO 5

UINT32 mutex1_id;
UINT32 mutex2_id;

static char g_str[MAX_STR_LEN];

void firstThread(UINT32 arg)
{
    int ret = -1;
    while (1) {
        LOS_MuxPend(mutex1_id, osWaitForever);
        ret = strcpy_s(g_str, MAX_STR_LEN, "firstThread");
        if (ret != 0) {
            printf("strcpy_s fail!\r\n");
        }
        printf("firstThread is Acquire. g_str=%s\r\n", g_str);
        LOS_MuxPost(mutex1_id);
        LOS_TaskDelay(DELAY_100_TICKS);
    }
}

void twoThread(UINT32 arg)
{
    int ret = -1;
    while (1) {
        LOS_MuxPend(mutex2_id, osWaitForever);
        char tmp[MAX_STR_LEN] = {0};
        ret = sprintf_s(tmp, MAX_STR_LEN, "%s+twoThread", g_str);
        if (ret < 0) {
            printf("sprintf_s fail!\r\n");
        }

        ret = memcpy_s(g_str, MAX_STR_LEN, tmp, sizeof(tmp));
        if (ret != 0) {
            printf("memcpy_s fail!\r\n");
        }
        printf("twoThread is Acquire. g_str=%s\r\n", g_str);
        LOS_MuxPost(mutex2_id);
        LOS_TaskDelay(DELAY_100_TICKS);
    }
}

void threeThread(UINT32 arg)
{
    int ret;
    while (1) {
        LOS_MuxPend(mutex1_id, osWaitForever);
        LOS_MuxPend(mutex2_id, osWaitForever);
        char tmp[MAX_STR_LEN] = {0};
        ret = sprintf_s(tmp, MAX_STR_LEN, "%s+threeThread", g_str);
        if (ret < 0) {
            printf("sprintf_s fail!\r\n");
        }

        ret = memcpy_s(g_str, MAX_STR_LEN, tmp, sizeof(tmp));
        if (ret != 0) {
            printf("memcpy_s fail!\r\n");
        }
        printf("threeThread is Acquire. g_str=%s\r\n", g_str);
        LOS_TaskDelay(DELAY_100_TICKS);
        LOS_MuxPost(mutex1_id);
        LOS_TaskDelay(DELAY_400_TICKS);
        LOS_MuxPost(mutex2_id);
        LOS_TaskDelay(DELAY_600_TICKS);
    }
}

void os_mutex_example(void)
{
    int ret = -1;
    ret = memset_s(g_str, MAX_STR_LEN, 0, MAX_STR_LEN);
    if (ret != 0) {
        printf("memset_s fail!\r\n");
    }
    TSK_INIT_PARAM_S attr = {0};
    attr.uwStackSize = TASK_STACK_SIZE;
    attr.usTaskPrio = TASK_PRIO;

    // 创建互斥锁
    if (LOS_MuxCreate(&mutex1_id) != LOS_OK) {
        printf("create Mutex1 failed!  ret=%x\n", ret);
        return;
    }

    if (LOS_MuxCreate(&mutex2_id) != LOS_OK) {
        printf("create Mutex2 failed!\n");
        LOS_MuxDelete(mutex1_id);
        return;
    }

    // 锁任务调度
    LOS_TaskLock();
    attr.pcName = "firstThread";
    attr.pfnTaskEntry = (TSK_ENTRY_FUNC)firstThread;
    UINT32 taskIDFirst;
    if (LOS_TaskCreate(&taskIDFirst, &attr) != LOS_OK) {
        printf("create firstThread failed!\n");
        LOS_MuxDelete(mutex1_id);
        LOS_MuxDelete(mutex2_id);
        return;
    }
    attr.pcName = "twoThread";
    attr.pfnTaskEntry = (TSK_ENTRY_FUNC)twoThread;
    UINT32 taskIDTwo;
    if (LOS_TaskCreate(&taskIDTwo, &attr) != LOS_OK) {
        printf("create twoThread failed!\n");
    }
    attr.pcName = "threeThread";
    attr.pfnTaskEntry = (TSK_ENTRY_FUNC)threeThread;
    UINT32 taskIDThree;
    if (LOS_TaskCreate(&taskIDThree, &attr) != LOS_OK) {
        printf("create threeThread failed!\n");
    }
    LOS_TaskUnlock(); // 解锁任务调度
}

OHOS_APP_RUN(os_mutex_example);

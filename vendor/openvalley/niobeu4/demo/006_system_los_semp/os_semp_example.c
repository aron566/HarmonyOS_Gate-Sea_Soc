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

#include "los_sem.h"
#include "ohos_run.h"

#ifndef osWaitForever
#define osWaitForever 0xFFFFFFFFU
#endif
#define TASK_STACK_SIZE 4096
#define TASK_PRIO 10
#define DELAY_100_TICKS 100U
#define DELAY_300_TICKS (3 * DELAY_100_TICKS)
#define DELAY_400_TICKS (4 * DELAY_100_TICKS)
#define SEM_COUNT   3

UINT32 semMutex;
UINT32 semSync;

static char g_str[] = "hello!";

void Thread_SempA(void)
{
    while (1) {
        LOS_SemPend(semMutex, osWaitForever);
        printf("Thread_SempA read g_str = %s \n", g_str);
        LOS_TaskDelay(DELAY_100_TICKS);
        LOS_SemPost(semMutex);
    }
}

void Thread_SempB(void)
{
    while (1) {
        LOS_SemPend(semMutex, osWaitForever);
        printf("Thread_SempB read g_str = %s\n", g_str);
        LOS_TaskDelay(DELAY_100_TICKS);
        LOS_SemPost(semMutex);
    }
}

void Thread_SempC(void)
{
    while (1) {
        LOS_SemPend(semMutex, osWaitForever);
        printf("Thread_SempC read g_str = %s\n", g_str);
        LOS_TaskDelay(DELAY_100_TICKS);
        LOS_SemPost(semMutex);
    }
}

void Thread_SempD(void)
{
    while (1) {
        LOS_SemPend(semMutex, osWaitForever);
        printf("Thread_SempD read g_str = %s\n", g_str);
        LOS_TaskDelay(DELAY_100_TICKS);
        LOS_SemPost(semMutex);
    }
}

void Thread_Semp1(void)
{
    LOS_TaskDelay(DELAY_300_TICKS);
    LOS_SemPost(semSync);
    printf("Thread_Semp1 Release semSync.\n");
    LOS_TaskDelay(DELAY_100_TICKS);
}

void Thread_Semp2(void)
{
    printf("entry Thread_Semp2 : try get semSync, timeout 100 ticks!\n");
    UINT32 ret = LOS_SemPend(semSync, DELAY_100_TICKS);
    if (ret == LOS_OK) {
        printf("Thread_Semp2 : got semSync.\n");
    } else if (ret == LOS_ERRNO_SEM_TIMEOUT) {
        printf("Thread_Semp2: failed to get semSync! and try get semSync wait forever.\n");
    }

    LOS_SemPend(semSync, osWaitForever);
    printf("Thread_Semp2: wait_forever and get semSync.\n");
    LOS_TaskDelay(DELAY_100_TICKS);
}

void semp_example(void)
{
    // 创建信号量，用户线程间同步
    if (LOS_SemCreate(0, &semSync) != LOS_OK) {
        printf("semSync create failed!\n");
        return;
    }

    // 创建信号量，用户线程间互斥
    if (LOS_SemCreate(SEM_COUNT, &semMutex) != LOS_OK) {
        printf("semMutex create failed!\n");
        LOS_SemDelete(semSync);
        return;
    }

    TSK_INIT_PARAM_S attr = {0};
    attr.uwStackSize = TASK_STACK_SIZE;
    attr.usTaskPrio = TASK_PRIO;

    // 创建两个线程，通过信号量实现线程间同步
    LOS_TaskLock();
    attr.pcName = "Thread_Semp1";
    attr.pfnTaskEntry = (TSK_ENTRY_FUNC)Thread_Semp1;
    UINT32 taskID1;
    if (LOS_TaskCreate(&taskID1, &attr) != LOS_OK) {
        printf("create Thread_Semp1 failed!\n");
        LOS_SemDelete(semSync);
        LOS_SemDelete(semMutex);
        return;
    }

    attr.pcName = "Thread_Semp2";
    attr.pfnTaskEntry = (TSK_ENTRY_FUNC)Thread_Semp2;
    UINT32 taskID2;
    if (LOS_TaskCreate(&taskID2, &attr) != LOS_OK) {
        printf("create Thread_Semp2 failed!\n");
        LOS_SemDelete(semSync);
        LOS_SemDelete(semMutex);
        LOS_TaskDelete(taskID1);
    }
    LOS_TaskUnlock();
    LOS_TaskDelay(DELAY_400_TICKS);
    LOS_SemDelete(semSync);
}

OHOS_APP_RUN(semp_example);
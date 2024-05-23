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
#include "los_task.h"
#include "los_swtmr.h"
#include "ohos_run.h"

#define TICKS_100 100U
#define TICKS_1000 1000U
#define TASK_PRIO 5

/* Timer count */
UINT32 g_timerCount1 = 0;
UINT32 g_timerCount2 = 0;

/* 任务ID */
UINT32 g_testTaskId01;

void Timer1_Callback(UINT32 arg) // 回调函数1
{
    UINT32 tick_last1;
    g_timerCount1++;
    tick_last1 = (UINT32)LOS_TickCountGet(); // 获取当前Tick数
    printf("g_timerCount1=%d, tick_last1=%d\n", g_timerCount1, tick_last1);
}

void Timer2_Callback(UINT32 arg) // 回调函数2
{
    UINT32 tick_last2;
    tick_last2 = (UINT32)LOS_TickCountGet();
    g_timerCount2++;
    printf("g_timerCount2=%d tick_last2=%d\n", g_timerCount2, tick_last2);
}

void Timer_example(UINT32 arg)
{
    UINT32 ret;
    UINT32 timerId1; // timer id1
    UINT32 timerId2; // timer id2
    UINT32 tickCount;
    /* 创建单次软件定时器，Tick数为1000，启动到1000Tick数时执行回调函数1 */
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
    LOS_SwtmrCreate(TICKS_1000, LOS_SWTMR_MODE_ONCE, Timer1_Callback, &timerId1, 1,
                    OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_INSENSITIVE);
#else
    LOS_SwtmrCreate(TICKS_1000, LOS_SWTMR_MODE_ONCE, Timer1_Callback, &timerId1, 1);
#endif
    /* 创建周期性软件定时器，每100Tick数执行回调函数2 */
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
    LOS_SwtmrCreate(TICKS_100, LOS_SWTMR_MODE_PERIOD, Timer2_Callback, &timerId2, 1,
                    OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_INSENSITIVE);
#else
    LOS_SwtmrCreate(TICKS_100, LOS_SWTMR_MODE_PERIOD, Timer2_Callback, &timerId2, 1);
#endif
    printf("create Timer1 success\n");
    LOS_SwtmrStart(timerId1); // 启动单次软件定时器
    printf("start Timer1 success\n");

    LOS_TaskDelay(200);                     // 延时200Tick数
    LOS_SwtmrTimeGet(timerId1, &tickCount); // 获得单次软件定时器剩余Tick数
    printf("tickCount=%d\n", tickCount);

    LOS_SwtmrStop(timerId1); // 停止软件定时器
    printf("stop Timer1 success\n");
    LOS_TaskDelay(TICKS_1000);

    printf("start Timer2\n");
    LOS_SwtmrStart(timerId1);
    LOS_SwtmrTimeGet(timerId1, &tickCount); // 获得单次软件定时器剩余Tick数
    printf("tickCount=%d\n", tickCount);
    LOS_TaskDelay(TICKS_1000);

    LOS_SwtmrStart(timerId2); // 启动周期性软件定时器
    printf("start Timer2\n");

    LOS_TaskDelay(TICKS_1000);
    LOS_SwtmrStop(timerId2);
    ret = LOS_SwtmrDelete(timerId2); // 删除软件定时器
    if (ret == LOS_OK) {
        printf("delete Timer2 success\n");
    }
}

UINT32 Example_TaskEntry(VOID)
{
    UINT32 ret;
    TSK_INIT_PARAM_S task1;
    /* 锁任务调度 */
    LOS_TaskLock();

    /* 创建任务1 */
    ret = memset_s(&task1, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    if (ret != 0) {
        printf("memset_s fail!\r\n");
    }
    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)Timer_example;
    task1.pcName = "TimerTsk";
    task1.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    task1.usTaskPrio = TASK_PRIO;
    ret = LOS_TaskCreate(&g_testTaskId01, &task1);
    if (ret != LOS_OK) {
        printf("TimerTsk create failed.\n");
        return LOS_NOK;
    }

    /* 解锁任务调度 */
    LOS_TaskUnlock();

    return LOS_OK;
}

OHOS_APP_RUN(Example_TaskEntry);

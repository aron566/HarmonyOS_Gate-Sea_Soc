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
#include "los_event.h"
#include "ohos_run.h"

#define FLAGS_MSK1 0x00000001U
#define FLAGS_MSK2 0x00000002U
#define DELAY_100_TICKS 100U
#define TASK_STACK_SIZE 4096
#define TASK_PRIO   5
EVENT_CB_S g_event;

// 发送事件
void OS_Thread_EventSender(UINT32 argument)
{
    (void)argument;
    while (1) {
        if (LOS_EventWrite(&g_event, FLAGS_MSK1) == LOS_OK) {
            printf("write eventFlags %d success.\n", FLAGS_MSK1);
        }
        LOS_TaskDelay(DELAY_100_TICKS);

        if (LOS_EventWrite(&g_event, FLAGS_MSK1 | FLAGS_MSK2) == LOS_OK) {
            printf("write eventFlags %d success.\n", FLAGS_MSK1 | FLAGS_MSK2);
        }

        LOS_TaskDelay(DELAY_100_TICKS);
    }
}

// 接收事件
void OS_Thread_EventReceiverOR(UINT32 argument)
{
    (void)argument;
    UINT32 flags;
    while (1) {
        flags = LOS_EventRead(&g_event, FLAGS_MSK1 | FLAGS_MSK2, LOS_WAITMODE_OR | LOS_WAITMODE_CLR, 0xFFFFFFFFU);
        printf("EventReceiverOR wait event! Receive Flags is %d\n", flags);
    }
}

// 接收事件
void OS_Thread_EventReceiverAND(UINT32 argument)
{
    (void)argument;
    UINT32 flags;
    while (1) {
        flags = LOS_EventRead(&g_event, FLAGS_MSK1 | FLAGS_MSK2, LOS_WAITMODE_AND | LOS_WAITMODE_CLR, 0xFFFFFFFFU);
        printf("EventReceiverAND wait event! Receive Flags is %d\n", flags);
    }
}

// 事件测试Example
static void OS_Event_example(void)
{
    if (LOS_EventInit(&g_event) != LOS_OK) {
        printf("Failed to create EventFlags!\n");
        return;
    }
    LOS_TaskLock();

    TSK_INIT_PARAM_S attr = {0};
    attr.pfnTaskEntry = (TSK_ENTRY_FUNC)OS_Thread_EventSender;
    attr.uwStackSize = TASK_STACK_SIZE;
    attr.usTaskPrio = TASK_PRIO;
    attr.pcName = "Thread_EventSender";
    UINT32 taskIDEventSender;
    if (LOS_TaskCreate(&taskIDEventSender, &attr) != LOS_OK) {
        printf("Failed to create Thread_EventSender!\n");
        return;
    }
    attr.pcName = "Thread_EventReceiverOR";
    attr.pfnTaskEntry = (TSK_ENTRY_FUNC)OS_Thread_EventReceiverOR;
    UINT32 taskIDEventRecvOR;
    if (LOS_TaskCreate(&taskIDEventRecvOR, &attr) != LOS_OK) {
        printf("Failed to create Thread_EventReceiverOR!\n");
        return;
    }
    attr.pcName = "Thread_EventReceiverAND";
    attr.pfnTaskEntry = (TSK_ENTRY_FUNC)OS_Thread_EventReceiverAND;
    UINT32 taskIDEventRecvAND;
    if (LOS_TaskCreate(&taskIDEventRecvAND, &attr) != LOS_OK) {
        printf("Failed to create Thread_EventReceiverAND!\n");
        return;
    }

    LOS_TaskUnlock();
}

OHOS_APP_RUN(OS_Event_example);

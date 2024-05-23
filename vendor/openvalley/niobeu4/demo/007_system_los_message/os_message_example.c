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

#include "los_queue.h"
#include "los_task.h"
#include "ohos_run.h"

#define BUFFER_LEN 50
#define USLEEP_1S (1000000)
#define TASK_PRIO_H   9
#define TASK_PRIO_L   10
#define QUEUE_LEN   5
#define QUEUE_MAX_SIZE  50
static UINT32 g_queue;

VOID SendEntry(VOID)
{
    UINT32 ret = 0;
    CHAR abuf[] = "test message";
    UINT32 len = sizeof(abuf);

    ret = LOS_QueueWriteCopy(g_queue, abuf, len, 0);
    if (ret != LOS_OK) {
        printf("send message failure, error: %x\n", ret);
    }
}

VOID RecvEntry(VOID)
{
    UINT32 ret = 0;
    CHAR readBuf[BUFFER_LEN] = {0};
    UINT32 readLen = BUFFER_LEN;

    // 休眠1s
    usleep(USLEEP_1S);
    ret = LOS_QueueReadCopy(g_queue, readBuf, &readLen, 0);
    if (ret != LOS_OK) {
        printf("recv message failure, error: %x\n", ret);
    }

    printf("recv message: %s\n", readBuf);

    ret = LOS_QueueDelete(g_queue);
    if (ret != LOS_OK) {
        printf("delete the queue failure, error: %x\n", ret);
    }

    printf("delete the queue success.\n");
}

UINT32 ExampleQueue(VOID)
{
    printf("start queue example.\n");
    UINT32 ret = 0;
    UINT32 task1, task2;
    TSK_INIT_PARAM_S initParam = {0};

    initParam.pfnTaskEntry = (TSK_ENTRY_FUNC)SendEntry;
    initParam.usTaskPrio = TASK_PRIO_L;
    initParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    initParam.pcName = "SendQueue";

    LOS_TaskLock();
    ret = LOS_TaskCreate(&task1, &initParam);
    if (ret != LOS_OK) {
        printf("create task1 failed, error: %x\n", ret);
        return ret;
    }

    initParam.pcName = "RecvQueue";
    initParam.pfnTaskEntry = (TSK_ENTRY_FUNC)RecvEntry;
    initParam.usTaskPrio = TASK_PRIO_H;
    ret = LOS_TaskCreate(&task2, &initParam);
    if (ret != LOS_OK) {
        printf("create task2 failed, error: %x\n", ret);
        return ret;
    }

    ret = LOS_QueueCreate("queue", QUEUE_LEN, &g_queue, 0, QUEUE_MAX_SIZE);
    if (ret != LOS_OK) {
        printf("create queue failure, error: %x\n", ret);
    }

    printf("create the queue success.\n");
    LOS_TaskUnlock();
    return ret;
}

OHOS_APP_RUN(ExampleQueue);

/*
 * Copyright (c) 2022 ASR Microelectronics (Shanghai) Co., Ltd. All rights reserved.
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
#include "cmsis_os2.h"
#include "ohos_init.h"
#include "log.h"

#define DEMO_TASK_STACK_SIZE 1024

static void DemoSdkTask(void *arg)
{
    (void)arg;
}

void DemoSdkMain(void)
{
    printf("example: %s\r\n", __func__);
    osThreadAttr_t attr = {0};
    attr.stack_size = DEMO_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    attr.name = "DemoSdk";
    if (osThreadNew((osThreadFunc_t)DemoSdkTask, NULL, &attr) == NULL) {
        printf("Failed to create DemoSdkTask\r\n");
    }
}

APP_FEATURE_INIT(DemoSdkMain);

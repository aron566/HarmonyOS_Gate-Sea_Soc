/*
 * Copyright (c) 2021 GOODIX.
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

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "shcmd.h"
#include "shell.h"

#define SHELL_TEST_TASK_STACK_SIZE   512
#define SHELL_TEST_TASK_PRIO         25
#define MS_1000                     1000

UINT32 CmdGetDeviceInfo(UINT32 argc, const CHAR **argv)
{
    printf("Board:GR5515 Starter Kit, Version:OpenHarmony master\r\n");
    return LOS_OK;
}

static void *ShellTestTask(const char *arg)
{
    (void)arg;

    osCmdReg(CMD_TYPE_EX, "devinfo", 0, CmdGetDeviceInfo);

    while (1) {
        osDelay(MS_1000);
    }
}

void ShellTestTaskEntry(void)
{
    osThreadAttr_t attr;

    attr.name = "ShellTestTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = SHELL_TEST_TASK_STACK_SIZE;
    attr.priority = SHELL_TEST_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)ShellTestTask, NULL, &attr) == NULL) {
        printf("[HelloDemo] Failed to create ShellTestTask!\n");
    }
}

SYS_RUN(ShellTestTaskEntry);

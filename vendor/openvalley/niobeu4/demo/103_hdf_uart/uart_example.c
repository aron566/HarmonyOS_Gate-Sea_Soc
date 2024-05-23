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
#include "uart_types.h"
#include "uart_if.h"
#include "cmsis_os2.h"
#include "hdf_log.h"
#include "ohos_run.h"

#define SYS_DELAY_TICKS 200
#define TASK_STACK_SIZE 4096
#define TASK_PRIO 25
#define BUFF_MAX_SIZE   128
#define UART_BAUDRATE        115200

#define UART_INDEX   0       // 串口号

osThreadId_t g_taskID = NULL;

static void uart_test(void)
{
    int32_t ret;
    DevHandle hd = UartOpen(UART_INDEX);    // 打开串口
    if (hd == NULL) {
        printf("%s UartOpen failed!\n", __func__);
    }

    uint32_t baudRate = UART_BAUDRATE;
    ret = UartSetBaud(hd, baudRate);      // 设置波特率
    if (ret < 0) {
        printf("HDF UartSetBaud %d fail!\r\n", baudRate);
    }

    baudRate = 0;   // 波特率参数清零
    ret = UartGetBaud(hd, &baudRate);             // 获取波特率
    if (ret >= 0) {
        printf("HDF UartGetBaud %d success!\r\n", baudRate);
    }

    ret = UartSetTransMode(hd, UART_MODE_RD_BLOCK);   // 设置串口为读取阻塞模式
    if (ret >= 0) {
        printf("UartSetTransMode UART_MODE_RD_BLOCK!\r\n");
    }

    while (1) {
        char buff[BUFF_MAX_SIZE] = { 0 };   // 用于接收串口数据的buffer
        ret = UartRead(hd, buff, BUFF_MAX_SIZE);
        if (ret > 0) {
            if (strstr(buff, "quit")) { // 接收到包含"quit"的字符串则退出任务
                break;
            } else {
                UartWrite(hd, buff, ret);   // 将接收到的数据写入串口
            }
        } else if (ret == 0) {
            printf("No data arrived!\r\n");
        } else {
            printf("Read Uart data fail! ret = %d\r\n", ret);
        }
    }
    UartClose(hd);
    printf("---> Exit uart_test task!\r\n");
    return;
}

static void uart_example_task(void)
{
    HDF_LOGE("into uart hdf example!\n");
    osThreadAttr_t attr;
    attr.name = "uart_test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TASK_STACK_SIZE;
    attr.priority = TASK_PRIO;
    g_taskID = osThreadNew((osThreadFunc_t)uart_test, NULL, &attr);
    if (g_taskID == NULL) {
        HDF_LOGE("Failed to create Test Uart thread!\n");
    }
}

OHOS_APP_RUN(uart_example_task);

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
#include "gpio_if.h"
#include "uart_if.h"

#define UART_BUAD_RATE 115200
#define LED_BLINK_DELAY 1000
#define HDF_TASK_STACK_SIZE 1024

enum case_test_index {
    SET_BAUD,
    GET_BAUD,
    UART_CONFIG,
    UART_WRITE,
    UART_READ
};

void GpioKeyIrqFunc(uint16_t gpio, void *data)
{
    (void)data;
    printf("user key %d pressed\n", gpio);
}

static void uart_test_case(DevHandle uart_handle)
{
    static int state = 0;
    int32_t ret;
    uint32_t baudrate;
    struct UartAttribute attr;
    uint8_t send_data[] = "Hello OpenHarmony\r\n";
    uint8_t recv_data[20];

    switch (state) {
        case SET_BAUD:
            ret = UartSetBaud(uart_handle, UART_BUAD_RATE);
            if (ret != 0) {
                printf("%s, state[%d] failed", __func__, state);
                return;
            }
            state++;
            break;
        case GET_BAUD:
            ret = UartGetBaud(uart_handle, &baudrate);
            if (ret != 0) {
                printf("%s, state[%d] failed", __func__, state);
                return;
            }
            state++;
            printf("get baudrate = %u", baudrate);
            break;
        case UART_CONFIG:
            attr.dataBits = UART_ATTR_DATABIT_8;
            attr.parity = UART_ATTR_PARITY_NONE;
            attr.stopBits = UART_ATTR_STOPBIT_1;
            ret = UartSetAttribute(uart_handle, &attr);
            if (ret != 0) {
                printf("%s, state[%d] failed", __func__, state);
                return;
            }
            state++;
            break;
        case UART_WRITE:
            ret = UartWrite(uart_handle, send_data, strlen(send_data));
            if (ret != 0) {
                printf("%s, state[%d] failed", __func__, state);
                return;
            }
            printf("send data done\n");
            state++;
            break;
        case UART_READ:
            ret = UartRead(uart_handle, recv_data, sizeof(recv_data) - 1);
            if (ret > 0) {
                recv_data[ret] = 0;
                printf("%s, state[%d] recv:%s", __func__, state, recv_data);
            } else {
                printf("%s, state[%d] no data received", __func__, state);
            }
            state = UART_WRITE;
            break;
        default:
            printf("%s, state[%d] is not support", __func__, state);
            break;
    }
}

void uart_test(void)
{
    static int uart_state = 0;
    static DevHandle uart_handle = NULL;
    switch (uart_state) {
        case 0:
            uart_handle = UartOpen(0);
            if (uart_handle == NULL) {
                printf (">>>uart open failed");
            } else {
                printf (">>>uart open success");
                uart_state++;
            }
            break;
        case 1:
            if (uart_handle) {
                uart_test_case(uart_handle);
            }
            break;
        default:
            if (uart_handle) {
                uart_test_case(uart_handle);
            }
            break;
    }
}

static void HDFTask(void *arg)
{
    (void)arg;
    uint32_t val = 0;
    GpioSetIrq(1, OSAL_IRQF_TRIGGER_FALLING, GpioKeyIrqFunc, NULL);
    GpioEnableIrq(1);
    while (1) {
        val++;
        GpioWrite(0, GPIO_VAL_LOW);
        lega_rtos_delay_milliseconds(LED_BLINK_DELAY);
        GpioWrite(0, GPIO_VAL_HIGH);
        lega_rtos_delay_milliseconds(LED_BLINK_DELAY);
        printf("blink led3,%u\n", val);
        uart_test();
    }
}

void HDF_App(void)
{
    printf("example: %s\r\n", __func__);
    osThreadAttr_t attr = {0};
    attr.stack_size = HDF_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    attr.name = "hdftest";
    if (osThreadNew((osThreadFunc_t)HDFTask, NULL, &attr) == NULL) {
        printf("Failed to create HDFTask\r\n");
    }
}

APP_FEATURE_INIT(HDF_App);

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
#include "gpio_if.h"
#include "driver_test.h"
#include "uart_if.h"

static DevHandle uart_handle = NULL;
#define baudrate_115200 115200
enum case_test_index {
    SET_BAUD,
    GET_BAUD,
    UART_CONFIG,
    UART_WRITE,
    UART_READ
};


int uart_test_case_read_write(int uart_state)
{
    int state_temp = uart_state;
    int32_t ret;
    uint8_t send_data[] = "Hello OpenHarmony\r\n";
    uint8_t recv_data[16];

    switch (state_temp) {
        case UART_WRITE:
            ret = UartWrite(uart_handle, send_data, strlen(send_data));
            if (ret != 0) {
                LOG_E("%s, state[%d] failed", __func__, state_temp);
                return;
            }
            break;
        case UART_READ:
            ret = UartRead(uart_handle, recv_data, sizeof(recv_data) - 1);
            if (ret > 0) {
                recv_data[ret] = 0;
                LOG_I("%s, state[%d] recv:%s", __func__, state_temp, recv_data);
            } else {
                LOG_I("%s, state[%d] no data received", __func__, state_temp);
            }
            state_temp = UART_WRITE;
            break;
        default:
            LOG_I("%s, state[%d] is not support", __func__, state_temp);
            break;
    }

    return state_temp;
}

static void uart_test_case(DevHandle uart_handle)
{
    static int state = 0;
    int32_t ret;
    uint32_t baudrate;
    struct UartAttribute attr;
    uint8_t recv_data[16];

    ret = UartSetBaud(uart_handle, baudrate_115200);
    if (ret != 0) {
        LOG_E("%s, state[%d] failed", __func__, state);
        return;
    }

    ret = UartGetBaud(uart_handle, &baudrate);
    if (ret != 0) {
        LOG_E("%s, state[%d] failed", __func__, state);
        return;
    }

    attr.dataBits = UART_ATTR_DATABIT_8;
    attr.parity = UART_ATTR_PARITY_NONE;
    attr.stopBits = UART_ATTR_STOPBIT_1;
    ret = UartSetAttribute(uart_handle, &attr);
    if (ret != 0) {
        LOG_E("%s, state[%d] failed", __func__, state);
        return;
    }

    ret = UartRead(uart_handle, recv_data, sizeof(recv_data) - 1);
    if (ret > 0) {
        recv_data[ret] = 0;
        LOG_I("%s, state[%d] recv:%s", __func__, state, recv_data);
    } else {
        LOG_I("%s, state[%d] no data received", __func__, state);
    }

    state = UART_WRITE;
    state = uart_test_case_read_write(state);
}

void uart_test(void)
{
    static int uart_state = 0;

    switch (uart_state) {
        case 0:
            uart_handle = UartOpen(1);
            if (uart_handle == NULL) {
                LOG_E (">>>uart open failed");
            } else {
                LOG_I (">>>uart open success");
                uart_state++;
            }
            break;
        case 1:
            if (uart_handle) {
                uart_test_case(uart_handle);
            }
            break;
        default:
            break;
    }
}

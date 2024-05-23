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
#include "securec.h"
#include "cmsis_os2.h"
#include "ohos_run.h"
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"
#include "wifi_device.h"

#define OPEN_WIFI_NAME "test"
#define SERVER_IP	"192.168.8.140"
#define SERVER_PORT 8080
#define OD_DELAY_1000 1000
#define OD_DELAY_100 100
#define RECV_LEN 511
#define STACK_SIZE 4096
#define PRIORITY 25

osThreadId_t wifi_test_id = NULL;

void wifi_test(void)
{
    int sock = -1;
    struct sockaddr_in client_addr;
    char recv_data[512] = {0};
    int recv_data_len;
    WifiConnect(OPEN_WIFI_NAME);
    printf("start wifi_test test\r\n");
    while (1) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            printf("Socket error\n");
            osDelay(OD_DELAY_100);
            continue;
        }
        memset_s(&(client_addr), sizeof(client_addr), 0, sizeof(client_addr));
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(SERVER_PORT);
        client_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
        printf("try connect to server "SERVER_IP" :%d \n", SERVER_PORT);
        if (connect(sock, (struct sockaddr *)&client_addr, sizeof(struct sockaddr)) == -1) {
            closesocket(sock);
            osDelay(OD_DELAY_1000);
            continue;
        }
        printf("Connect to tcp server successful!\n");
        while (1) {
            recv_data_len = recv(sock, recv_data, RECV_LEN, 0);
            if (recv_data_len <= 0) {
                break;
            } else {
                recv_data[recv_data_len] = '\0';
            }
            printf("recv: %s\n", recv_data);
            write(sock, recv_data, recv_data_len);
        }
    }
}

static void wifi_test_example(void)
{
    osThreadAttr_t attr;
    attr.name = "wifi_test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = STACK_SIZE;
    attr.priority = PRIORITY;
    wifi_test_id = osThreadNew((osThreadFunc_t)wifi_test, NULL, &attr);
    if (wifi_test_id == NULL) {
        printf("Failed to create wifi_test thread!\n");
    }
}

OHOS_APP_RUN(wifi_test_example);

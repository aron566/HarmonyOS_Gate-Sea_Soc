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

#include <stdlib.h>
#include <string.h>
#include "cmsis_os2.h"
#include "ohos_init.h"
#include "wifi_device.h"
#include "wifi_error_code.h"
#include "shell.h"
#include "shcmd.h"
#include "lwip_app_iperf.h"

#define ARG_IDX_1 1
#define ARG_IDX_2 2
#define ARG_IDX_3 3
#define ARG_IDX_4 4
#define ARG_IDX_5 5
#define ARG_IDX_6 6
#define ARG_IDX_7 7

static void Wifi_sta(WifiDeviceConfig conf)
{
    WifiErrorCode error;

    if (EnableWifi() != WIFI_SUCCESS) {
        printf("EnableWifi failed\r\n");
        return;
    }

    if (IsWifiActive() == WIFI_STA_NOT_ACTIVE) {
        printf("Wifi station is not actived.\n");
        return;
    }

    int result = 1;
    if (RemoveDevice(result) != WIFI_SUCCESS) {
        printf("RemoveDevice failed!\r\n");
        return;
    }
    if (AddDeviceConfig(&conf, &result) != WIFI_SUCCESS) {
        printf("AddDeviceConfig failed!\r\n");
        return;
    }
    printf("Connecting to %s...\r\n", conf.ssid);
    error = ConnectTo(result);
    printf("WiFi connect %s!\r\n", (error == WIFI_SUCCESS) ? "succeed" : "failed");
}

static int ap_conn_func(int argc, char **argv)
{
    WifiDeviceConfig conf;

    printf("doing...\n");
    switch (argc) {
        case ARG_IDX_1:
        case ARG_IDX_2:
            break;
        case ARG_IDX_3:
        case ARG_IDX_4:
        case ARG_IDX_5:
        case ARG_IDX_6:
        case ARG_IDX_7:
            if ((strcmp(argv[ARG_IDX_1], "sta") != 0)) {
                printf("param2 error,not support\r\n");
                return -1;
            }
            if (strlen((char *)argv[ARG_IDX_2]) >= sizeof(conf.ssid)) {
                return -1;
            }
            strcpy_s((char *)conf.ssid, sizeof(conf.ssid), (char *)argv[ARG_IDX_2], strlen(argv[ARG_IDX_2]));

            if (argc >= ARG_IDX_4) {
                if ((strlen(argv[ARG_IDX_3]) > ARG_IDX_4) && (strlen(argv[ARG_IDX_3]) < sizeof(conf.preSharedKey))) {
                    strcpy_s((char *)conf.preSharedKey, sizeof(conf.preSharedKey), (char *)argv[ARG_IDX_3],
                             strlen(argv[ARG_IDX_3]));
                    conf.securityType = WIFI_SEC_TYPE_PSK;
                } else if ((strlen(argv[ARG_IDX_3]) == 1) && (convert_str_to_int(argv[ARG_IDX_3]) == 0)) {
                    conf.securityType = WIFI_SEC_TYPE_OPEN;
                    memset_s(conf.preSharedKey, sizeof(conf.preSharedKey), 0, sizeof(conf.preSharedKey));
                } else {
                    printf("param4 error, input 0 or pwd(length >= 5)\n");
                    return -1;
                }
            }
            break;
        default:
            printf("param num error,1~7\r\n");
            return -1;
    }
    Wifi_sta(conf);

    return 0;
}

static int ap_close_func(int argc, char **argv)
{
    DisableWifi();
    return 0;
}
#ifdef LWIP_APP_IPERF
static int lega_at_wifi_iperf(int argc, char **argv)
{
    lega_wifi_iperf(argc, argv);
    return 0;
}
#endif
static void Wifi_App(void)
{
    printf("example: %s\r\n", __func__);

    osCmdReg(CMD_TYPE_STD, "wifi_open", 0, (CMD_CBK_FUNC)ap_conn_func);
    osCmdReg(CMD_TYPE_EX, "wifi_close", 0, (CMD_CBK_FUNC)ap_close_func);
#ifdef LWIP_APP_IPERF
    osCmdReg(CMD_TYPE_STD, "iperf", 0, (CMD_CBK_FUNC)lega_at_wifi_iperf);
    lega_wifi_iperf_init();
#endif
}

APP_FEATURE_INIT(Wifi_App);

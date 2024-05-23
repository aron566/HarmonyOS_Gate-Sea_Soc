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
#include "securec.h"
#include "cmsis_os2.h"
#include "ohos_init.h"
#include "wifi_device.h"
#include "lwip/ip4_addr.h"
#include "lwip/netif.h"
#include "lwip/netifapi.h"
#include "wifi_error_code.h"

#define DEF_TIMEOUT 15
#define ONE_SECOND 1
#define SELECT_WLAN_PORT "wlan0"
#define SELECT_WIFI_SECURITYTYPE WIFI_SEC_TYPE_OPEN
#define STD_TIMEZONE_OFFSET (+8)
#define OD_DELAY_100 100
#define OD_DELAY_200 200

static int g_staScanSuccess = 0;
static int g_ConnectSuccess = 0;
static int ssid_count = 0;
static WifiErrorCode wifi_error;
static WifiEvent g_wifiEventHandler = {0};
static int wifi_sta_init_state = 0;
int sock_fd;
int addr_length;
const int timeZone = 8;

static void WiFiInit(void);
static void WaitScanResult(void);
static int WaitConnectResult(void);
static void OnWifiScanStateChangedHandler(int state, int size);
static void OnWifiConnectionChangedHandler(int state, WifiLinkedInfo *info);
static void OnHotspotStaJoinHandler(StationInfo *info);
static void OnHotspotStateChangedHandler(int state);
static void OnHotspotStaLeaveHandler(StationInfo *info);

void DisableWIFI(void)
{
    DisableWifi();
}

static void OnHotspotStaJoinHandler(StationInfo *info)
{
    (void)info;
    printf("STA join AP\n");
    return;
}

static void OnHotspotStaLeaveHandler(StationInfo *info)
{
    (void)info;
    printf("HotspotStaLeave:info is null.\n");
    return;
}

static void OnHotspotStateChangedHandler(int state)
{
    printf("HotspotStateChanged:state is %d.\n", state);
    return;
}

static void WiFiInit(void)
{
    printf("<--Wifi Init-->\r\n");
    g_wifiEventHandler.OnWifiScanStateChanged = OnWifiScanStateChangedHandler;
    g_wifiEventHandler.OnWifiConnectionChanged = OnWifiConnectionChangedHandler;
    g_wifiEventHandler.OnHotspotStaJoin = OnHotspotStaJoinHandler;
    g_wifiEventHandler.OnHotspotStaLeave = OnHotspotStaLeaveHandler;
    g_wifiEventHandler.OnHotspotStateChanged = OnHotspotStateChangedHandler;
    wifi_error = RegisterWifiEvent(&g_wifiEventHandler);
    if (wifi_error != WIFI_SUCCESS) {
        printf("register wifi event fail!\r\n");
    } else {
        printf("register wifi event succeed!\r\n");
    }
}

static void OnWifiScanStateChangedHandler(int state, int size)
{
    (void)state;
    if (size > 0) {
        ssid_count = size;
        g_staScanSuccess = 1;
    }
    return;
}

static int result;
int WifiConnect(const char *ssid, const char *psk)
{
    WifiScanInfo *info = NULL;
    unsigned int size = WIFI_SCAN_HOTSPOT_LIMIT;
    static struct netif *g_lwip_netif = NULL;
    WifiDeviceConfig select_ap_config = {0};
    osDelay(OD_DELAY_200);
    printf("<--System Init-->\r\n");
    WiFiInit();
    if (EnableWifi() != WIFI_SUCCESS) {
        printf("EnableWifi failed, wifi_error = %d\n", wifi_error);
        return -1;
    }
    if (IsWifiActive() == 0) {
        printf("Wifi station is not actived.\n");
        return -1;
    }
    info = malloc(sizeof(WifiScanInfo) * WIFI_SCAN_HOTSPOT_LIMIT);
    if (info == NULL) {
        printf("faild to create wifiscanInfo.\n");
        return -1;
    }
    do {
        ssid_count = 0;
        g_staScanSuccess = 0;
        Scan();
        WaitScanResult();
        wifi_error = GetScanInfoList(info, &size);
    } while (g_staScanSuccess != 1);
    strcpy_s(select_ap_config.ssid, sizeof(select_ap_config.ssid), ssid);
    printf("[%s][%s] \r\n", select_ap_config.ssid, select_ap_config.preSharedKey);
    select_ap_config.securityType = SELECT_WIFI_SECURITYTYPE;
    if (AddDeviceConfig(&select_ap_config, &result) == WIFI_SUCCESS) {
        if (ConnectTo(result) == WIFI_SUCCESS && WaitConnectResult() == 1) {
            printf("WiFi connect succeed!\r\n");
            wifi_sta_init_state = 1;
        }
    }
    osDelay(OD_DELAY_100);
    return 0;
}

static int WaitConnectResult(void)
{
    int ConnectTimeout = DEF_TIMEOUT;
    while (ConnectTimeout > 0) {
        sleep(1);
        ConnectTimeout--;
        if (g_ConnectSuccess == 1) {
            printf("WaitConnectResult:wait success[%d]s\n", (DEF_TIMEOUT - ConnectTimeout));
            break;
        }
    }
    if (ConnectTimeout <= 0) {
        printf("WaitConnectResult:timeout!\n");
        return 0;
    }
    return 1;
}

static void OnWifiConnectionChangedHandler(int state, WifiLinkedInfo *info)
{
    (void)info;
    if (state > 0) {
        g_ConnectSuccess = 1;
        printf("callback function for wifi connect\r\n");
    } else {
        g_ConnectSuccess = 0;
        printf("connect wifi_error, please check password, state:%d, try connect again\r\n", state);
        esp_wifi_connect();
    }
    return;
}

static void WaitScanResult(void)
{
    int scanTimeout = DEF_TIMEOUT;
    while (scanTimeout > 0) {
        sleep(ONE_SECOND);
        scanTimeout--;
        if (g_staScanSuccess == 1) {
            printf("WaitScanResult:wait success[%d]s\n", (DEF_TIMEOUT - scanTimeout));
            break;
        }
    }
    if (scanTimeout <= 0) {
        printf("WaitScanResult:timeout!\n");
    }
}

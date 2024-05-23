# NiobeU4开发板WIFI使用说明文档

## 简介
WLAN组件为设备提供接入与使用WLAN的相关接口，包括开启、关闭WLAN，扫描、监听WLAN状态等。本案例程序将演示怎么在Niobeu4开发板上使用wifi，编写一个客户端连接socket发包的业务程序，实现开发板联网与服务端进行数据通信。

## 接口说明

### EnableWifi()

```
WifiErrorCode EnableWifi(void);
```

**描述：**

Wifi enable打开wifi函数。

### DisableWifi()

```
int32_t DisableWifi(void);
```

**描述：**

wifi disable关闭wifi函数。

### Scan()

```
int32_t Scan(void);
```

**描述：**

wifi scan扫描函数。

### GetScanInfoList()

```
int32_t GetScanInfoList(WifiScanInfo* result, unsigned int* size);
```

**描述：**

wifi GetScanInfoList获取扫描列表函数。

**参数：**

| 名字   | 描述         |
| :----- | :----------- |
| result | 扫描结果指针 |
| size   | 扫描wifi总数 |

### AddDeviceConfig()

```
int32_t AddDeviceConfig(const WifiDeviceConfig* config, int* result);
```

**描述：**

wifi GetScanInfoList获取扫描列表函数。

**参数：**

| 名字   | 描述                                       |
| :----- | :----------------------------------------- |
| config | wifi配置结构体指针,用于连接指定的Wi-Fi设备 |
| result | netid                                      |

### WiFiInit()

```
void WiFiInit(void);
```

**描述：**

wifi init初始化wifi函数。

### WifiConnect()

```
int32_t WifiConnect(const char *ssid, const char *psk);
```

**描述：**

wifi init初始化wifi函数。

**参数：**

| 名字 | 描述     |
| :--- | :------- |
| ssid | WiFi名称 |
| psk  | WiFi密码 |

## 约束
WLAN服务组件使用C语言编写，目前支持niobeu4开发板。

## 使用
- 设备需要完成以下开发步骤，以开启WLAN：

    ```
    //开启WLAN
    WifiErrorCode error = EnableWifi();
    if (error != WIFI_SUCCESS) {
        return;
    }
    ```

- 设备需要完成以下开发步骤，以触发WLAN扫描：

    ```
    //开启WLAN
    WifiErrorCode error = Scan();
    if (error != WIFI_SUCCESS) {
        return;
    }
    ```

- 设备需要完成以下开发步骤，以获取WLAN扫描结果：

    ```
    WifiScanInfo* infoList = malloc(sizeof(WifiScanInfo) * WIFI_SCAN_HOTSPOT_LIMIT);//WIFI_SCAN_HOTSPOT_LIMIT：wifi连接设备数量设置,默认64
    unsigned int size = WIFI_SCAN_HOTSPOT_LIMIT;
    error = GetScanInfoList(infoList, &size);
    if (error != WIFI_SUCCESS || size == 0) {
        return;
    }
    ```

- 设备需要完成以下开发步骤，以配置接入点的接入方式：

    ```
    int netId = 0;
    WifiDeviceConfig config = {0};
    config.freq = 20;
    config.securityType = WIFI_SEC_TYPE_PSK;
    config.wapiPskType = WIFI_PSK_TYPE_ASCII;
    memcpy_s(config.ssid, WIFI_MAX_SSID_LEN, "Sample", 10); //在这里输入待连接WLAN的名称
    unsigned char bssid[WIFI_MAC_LEN] = {0xac, 0x75, 0x1d, 0xd8, 0x55, 0xc1}; //在这里输入待连接WLAN的BSSID
    memcpy_s(config.bssid, WIFI_MAC_LEN, bssid, WIFI_MAC_LEN);
    memcpy_s(config.preSharedKey, WIFI_MAX_KEY_LEN, "1234567890", 10); //在这里输入待连接WLAN的口令
    WifiErrorCode error = AddDeviceConfig(&config, &netId);
    if (error != WIFI_SUCCESS || netId == 0) {
        return;
    }
    ```

- 设备需要完成以下开发步骤，以连接配置的WLAN：

    ```
    error = ConnectTo(netId);  //netId来自上一步
    if (error != WIFI_SUCCESS) {
        return;
    }
    ```

## 案例程序

### Socket发包业务Demo

在Niobeu4开发板上使用wifi，编写一个客户端连接socket发包的业务程序，实现开发板联网与服务端进行数据通信。

```
#include <stdio.h>
#include "cmsis_os2.h"
#include "ohos_run.h"
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"
#include "wifi_device.h"

#define WIFI_NAME "HUAWEIP40"
#define WIFI_PWD "houpengfei8"
#define SERVER_IP	"192.168.8.140"
#define SERVER_PORT 8080

osThreadId_t wifi_test_id = NULL;

void wifi_test(void *thread_param)
{
    int sock = -1;
    struct sockaddr_in client_addr;
    char recv_data[512]={0};
    int recv_data_len;

    WifiConnect(WIFI_NAME,WIFI_PWD);//连接指定设备

    printf("start wifi_test test\r\n");
    while(1) {
        /* 申请套接字，本质是netconn_new函数的封装 */
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0){
            printf("Socket error\n");
            osDelay(100);
            continue;
        } 
        /* 清空sockaddr_in结构体内存空间 */
        memset(&(client_addr), 0, sizeof(client_addr)); 

        /* 为sockaddr_in结构体成员赋值，用于以下的connect绑定 */
        client_addr.sin_family = AF_INET;      
        client_addr.sin_port = htons(SERVER_PORT);   
        client_addr.sin_addr.s_addr = inet_addr(SERVER_IP);


        printf("try connect to server "SERVER_IP":%d\n",SERVER_PORT);
        /* 将远端server的ip地址与端口进行绑定 */
        if (connect(sock, (struct sockaddr *)&client_addr, sizeof(struct sockaddr)) == -1) {
            closesocket(sock);
            osDelay(1000);
            continue;
        }

        printf("Connect to tcp server successful!\n");
        while(1){						
            /* 成功接收到数据，返回接收的数据长度 */
            recv_data_len = recv(sock, recv_data, 511, 0);
            if (recv_data_len <= 0) break;
            else
                recv_data[recv_data_len] = '\0';

            /* 串口打印接收的数据内容 */
            printf("recv:%s\n",recv_data);		
            /* 发送数据内容 */
            write(sock,recv_data,recv_data_len);
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
    attr.stack_size = 1024 * 4;
    attr.priority = 25;
    wifi_test_id = osThreadNew((osThreadFunc_t)wifi_test, NULL, &attr);
    if (wifi_test_id == NULL){
        printf("Failed to create wifi_test thread!\n");
    }
}

OHOS_APP_RUN(wifi_test_example);

```

通过百度搜索调试工具“NetAssist.exe”，下载以后配置SERVER_LISTEN_PORT和IP_ADDRESS，开启服务端接收niobeu4开发板发送的消息。

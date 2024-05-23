# 蓝牙示例

## 简介

本案例为蓝牙示例代码。该代码实现了蓝牙低能耗(BLE)通用属性(GATT)客户端，它扫描附近的外围服务器并连接到预定义的服务。然后，客户端搜索可用的特征并订阅已知的特征，以便接收通知或指示。该示例可以注册一个应用程序Profile文件并初始化一系列事件，这些事件可用于配置GAP参数，并处理诸如扫描、连接外设和读写特征等事件。

## 编译调试

- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:
  `(Top) → Platform → Board Selection → select board niobeu4 → use openvalley niobeu4 application → niobeu4 application choose`
- 选择 `201_bluetooth`
- 回到sdk根目录，执行`hb build`脚本进行编译。

## 案例程序

本例程位于`vendor/openvalley/niobeu4/demo/201_bluetooth`文件夹下。

```
#define REMOTE_SERVICE_UUID        0x00FF //定义远端蓝牙设备UUID
#define REMOTE_NOTIFY_CHAR_UUID    0xFF01 //定义远端蓝牙读写UUID
//应用配置文件
#define PROFILE_NUM 1                      //设置Profile数量
#define PROFILE_A_APP_ID 0                 //设置Profile对应的App ID
#define INVALID_HANDLE   0//特征句柄，当类型为GATT_DB_DESCRIPTOR时，该参数有效

static const char remote_device_name[] = "BLE_SLAVE";//设置远端蓝牙名称

/* 声明静态回调函数 */
static void gap_cb(GapSearchEvent event, BleGapParam *param);
static void gattc_cb(GattcBleCallbackEvent event, GattInterfaceType gattc_if, BleGattcParam *param);
static void gattc_profile_event_handler(GattcBleCallbackEvent event, GattInterfaceType gattc_if, BleGattcParam *param);

/* 设置对端设备蓝牙地址，用于蓝牙直连 */
uint8_t directAddr[6] = {0x44, 0x17, 0x93, 0xb4, 0x0b, 0xe2};

/* 设置过滤对端设备蓝牙服务UUID，用于蓝牙搜索服务 */
static BtUuid remote_filter_service_uuid = {
    .uuidLen = ESP_UUID_LEN_16,
    .uuid = REMOTE_SERVICE_UUID,
};

/* 设置过滤对端设备蓝牙读写UUID，用于蓝牙搜索服务 */
static esp_bt_uuid_t remote_filter_char_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = REMOTE_NOTIFY_CHAR_UUID,},
};

/* 客户端特征项配置描述符。默认为0x2902，以便能与手机app通信 */
static esp_bt_uuid_t notify_descr_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG,},
};

/* 设置ble扫描参数 */
static BleScanParams ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,  //扫描类型
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,  //拥有者的蓝牙设备地址类型
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,//扫描过滤器设置
    .scan_interval          = 0x0010,  //扫描间隔
    .scan_window            = 0x0050,  //扫描窗口
    .scan_duplicate         = BLE_SCAN_DUPLICATE_DISABLE  //扫描重复类型
};

/* GattcProfileInst是对为一个或多个服务器应用程序设计的功能进行分组 *\
struct GattcProfileInst {
    GattcBleCallback gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t char_handle;
    BdAddrs remote_bda;
};
```

此GattcProfileInst包括如下结构。

```
* ' gattc_cb ': GATT客户端回调函数
* ' gattc_if ':该配置文件的GATT客户端接口编号
* ' app_id ' :应用配置文件ID号
* ' conn_id ':连接ID
* ' service_start_handle ':服务启动句柄
* ' service_end_handle ':服务结束句柄
* ' char_handle ': Char句柄
* ' remote_bda ':连接到此客户端的远程设备地址。
```

### 主入口

程序的入口点是app_main_btc()函数。

```
    EnableBle();//开启蓝牙
    BleGattcConfigureMtuSize(500);//设置MTU大小
    BtGattClientCallbacks btGattClientCallbacks = {
        .gap_callback     = gap_cb,
        .gattc_callback   = gattc_cb,
        .PROFILE_APP_ID = PROFILE_A_APP_ID,
    };
    BleGattcRegister(btGattClientCallbacks);//注册Gatt客户端回调函数
```

应用Profile存储在' gl_profile_tab '数组中，初始化为OHOS_GATT_IF_NONE。

```
/* 一个基于gattc_if的配置文件，一个app_id和一个gattc_if，这个数组将存储OHOS_GATTS_REG_EVT返回的gattc_if */
static struct GattcProfileInst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gattc_cb = gattc_profile_event_handler,//gatt profile事件回调
        .gattc_if = OHOS_GATT_IF_NONE,       //没有获得gatt_if，所以初始值是OHOS_GATT_IF_NONE 
    },
};
```

注册回调以后会触发一个`OHOS_GATTC_REG_EVT`事件，该事件由`gattc_cb()` 事件处理程序处理。处理程序接受事件返回的GATT接口，并将其存储在配置文件表中(gl_profile_tab)。

```
static void gattc_cb(GattcBleCallbackEvent event, GattInterfaceType gattc_if, BleGattcParam *param)
{
    /* If event is register event, store the gattc_if for each profile */
    if (event == OHOS_GATTC_REG_EVT) {
        if (param->reg.status == BT_SUCCESS) {
            gl_profile_tab[param->reg.app_id].gattc_if = gattc_if;
        } else {
            ESP_LOGI(GATTC_TAG, "reg app failed, app_id %04x, status %d",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }
...
}
```

最后，回调函数为` gl_profile_tab `表中的每个配置文件调用相应的事件处理程序。

```
    /* If the gattc_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback */
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            if (gattc_if == OHOS_GATT_IF_NONE || /* OHOS_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                    gattc_if == gl_profile_tab[idx].gattc_if) {
                if (gl_profile_tab[idx].gattc_cb) {
                    gl_profile_tab[idx].gattc_cb(event, gattc_if, param);
                }
            }
        }
    } while (0);
```

#### 蓝牙扫描

蓝牙扫描参数设置见`组件参数配置`，注册回调以后需要设置扫描参数。

```
    case OHOS_GATTC_REG_EVT:
        BT_LOGE(GATTC_TAG, "REG_EVT");
        BtError scan_ret = BleGatSetScanParams(&ble_scan_params);
        if (scan_ret) {
            BT_LOGE(GATTC_TAG, "set scan params error, error code = %x", scan_ret);
        }
        break;
```

设置完扫描参数会有一个`OHOS_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT`事件，在这里开启蓝牙扫描。

```
    case OHOS_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
        BleStartScan();
        break;
    }
```

扫描结果会在`OHOS_GAP_BLE_SCAN_RESULT_EVT`事件中，如果扫描到remote_device_name，停止扫描，发起连接。

```
    case OHOS_GAP_BLE_SCAN_RESULT_EVT: {
        BleGapParam *scan_result = (BleGapParam *)param;
        switch (scan_result->scan_rst.search_evt) {
        case OHOS_GAP_SEARCH_INQ_RES_EVT:
            BT_LOGE(GATTC_TAG, "searched Adv Data Len %d, Scan Response Len %d", scan_result->scan_rst.adv_data_len, scan_result->scan_rst.scan_rsp_len);
            adv_name = BleResolveAdvData(scan_result->scan_rst.ble_adv,
                                                OHOS_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
            if (adv_name_len) {
                BT_LOGE(GATTC_TAG, "searched Device Name Len %d", adv_name_len);
                BtLogBufferChar(GATTC_TAG, adv_name, adv_name_len);

                BT_LOGE(GATTC_TAG, "\n");
            }

            if (adv_name != NULL) {
                if (strlen(remote_device_name) == adv_name_len && strncmp((char *)adv_name, remote_device_name, adv_name_len) == 0) {
                    BT_LOGE(GATTC_TAG, "searched device %s\n", remote_device_name);
                    if (connect == false) {
                        connect = true;
                        BT_LOGE(GATTC_TAG, "connect to the remote device.");
                        //蓝牙直连
                        BleStopScan();
                        memcpy(directAddr, scan_result->scan_rst.bda, sizeof(directAddr));
                        BleGattcConnect(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, NULL, scan_result->scan_rst.bda, true, BLE_ADDR_TYPE_PUBLIC);
                    }
                }
            }
            break;
```


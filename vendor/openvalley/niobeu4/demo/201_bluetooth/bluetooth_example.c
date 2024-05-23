/*
 * Copyright (c) 2022 Hunan OpenValley Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http:// www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "bluetooth_example.h"
#include "ohos_run.h"
#include "cmsis_os2.h"

static const char remote_device_name[] = "BLUETOOTH";
static bool connect = false;
static bool get_server = false;
static BleGattcCharElem *char_elem_result = NULL;
static BleGattcDescrElem *descr_elem_result = NULL;

/* Declare static functions */
static void gap_cb(GapSearchEvent event, BleGapParam *param);
static void gattc_cb(GattcBleCallbackEvent event, GattInterfaceType gattc_if, BleGattcParam *param);
static void gattc_event_handler(GattcBleCallbackEvent event,
                                GattInterfaceType gattc_if,
                                BleGattcParam *param);

uint8_t directAddr[6] = {0x3c, 0x61, 0x05, 0x7d, 0xdf, 0x66};

static BtUuid remote_filter_service_uuid = {
    .uuidLen = 2,
    .uuid = REMOTE_SERVICE_UUID,
};

static BtUuids remote_filter_char_uuid = {
    .len = 2,
    .uuid = {
        .uuid16 = REMOTE_NOTIFY_CHAR_UUID,
    },
};

static BtUuids notify_descr_uuid = {
    .len = 2,
    .uuid = {
        .uuid16 = OHOS_GATT_UUID_CHAR_CLIENT_CONFIG,
    },
};

static BleScanParams ble_scan_params = {
    .scan_type = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval = 0x0010,
    .scan_window = 0x0050,
    .scan_duplicate = BLE_SCAN_DUPLICATE_DISABLE};

static struct GattcProfileInst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gattc_cb = gattc_event_handler,
        .gattc_if = OHOS_GATT_IF_NONE,
    },
};

static void gattc_reg_evt(GattInterfaceType gattc_if, BleGattcParam *p_data)
{
    BT_LOGE("REG_EVT");
    BtError scan_ret = BleGatSetScanParams(&ble_scan_params);
    if (scan_ret) {
        BT_LOGE("set scan params error, error code = %x", scan_ret);
    }
}

static void open_evt(BleGattcParam *p_data, BleGattcParam *param)
{
    if (param->open.status != BT_SUCCESS) {
        BT_LOGE("open failed, status %d", p_data->open.status);
        return;
    }
    BT_LOGE("open success");
}

static void connect_evt(GattInterfaceType gattc_if, BleGattcParam *p_data)
{
    BT_LOGE("OHOS_GATTC_CONNECT_EVT conn_id %d, if %d", p_data->connect.conn_id, gattc_if);
    gl_profile_tab[PROFILE_A_APP_ID].conn_id = p_data->connect.conn_id;
    memcpy_s(gl_profile_tab[PROFILE_A_APP_ID].remote_bda.addr,
             sizeof(gl_profile_tab[PROFILE_A_APP_ID].remote_bda.addr),
             p_data->connect.remote_bda, sizeof(p_data->connect.remote_bda));
    BT_LOGE("REMOTE BDA:");
    BtError mtu_ret = BleGattcSendMtuReq(gattc_if, p_data->connect.conn_id);
    if (mtu_ret) {
        BT_LOGE("config MTU error, error code = %x", mtu_ret);
    }
}

static void dis_srvccmpl_evt(GattInterfaceType gattc_if, BleGattcParam *param)
{
    if (param->dis_srvc_cmpl.status != BT_SUCCESS) {
        BT_LOGE("discover service failed, status %d", param->dis_srvc_cmpl.status);
        return;
    }
    BT_LOGE("discover service complete conn_id %d", param->dis_srvc_cmpl.conn_id);
    BleGattcSearchServices(gattc_if, param->cfg_mtu.conn_id, &remote_filter_service_uuid);
}

static void cfg_mtu_evt(BleGattcParam *param)
{
    if (param->cfg_mtu.status != BT_SUCCESS) {
        BT_LOGE("config mtu failed, error status = %x", param->cfg_mtu.status);
    }
    BT_LOGE("OHOS_GATTC_CFG_MTU_EVT, Status %d, MTU %d, conn_id %d",
            param->cfg_mtu.status, param->cfg_mtu.mtu, param->cfg_mtu.conn_id);
}

static void search_res_evt(GattInterfaceType gattc_if, BleGattcParam *p_data)
{
    BT_LOGE("SEARCH RES: conn_id = %x is primary service %d",
            p_data->search_res.conn_id, p_data->search_res.is_primary);
    BT_LOGE("start handle %d end handle %d current handle value %d",
            p_data->search_res.start_handle, p_data->search_res.end_handle,
            p_data->search_res.srvc_id.inst_id);
    if (p_data->search_res.srvc_id.uuid.len == UUID16_BIT &&
        p_data->search_res.srvc_id.uuid.uuid.uuid16 == REMOTE_SERVICE_UUID) {
        BT_LOGE("service found");
        get_server = true;
        gl_profile_tab[PROFILE_A_APP_ID].service_start_handle = p_data->search_res.start_handle;
        gl_profile_tab[PROFILE_A_APP_ID].service_end_handle = p_data->search_res.end_handle;
        BT_LOGE("UUID16: %x", p_data->search_res.srvc_id.uuid.uuid.uuid16);
    }
}

static void evt_get_server(GattInterfaceType gattc_if, BleGattcParam *p_data)
{
    uint16_t count = 0;
    GattcGetAttr get_attr = {
        .gattc_if = gattc_if,
        .conn_id = p_data->search_cmpl.conn_id,
        .type = OHOS_GATT_DB_CHARACTERISTIC,
        .start_handle = gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
        .end_handle = gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
    };
    GattStatus status = BleGattcGetAttrCount(get_attr,
                                             INVALID_HANDLE,
                                             &count);
    if (status != BT_SUCCESS) {
        BT_LOGE("GattcGetAttrCount error");
    }

    if (count > 0) {
        char_elem_result = (BleGattcDescrElem *)malloc(sizeof(BleGattcDescrElem) * count);
        if (!char_elem_result) {
            BT_LOGE("gattc no mem");
        } else {
            GattcGetChar get_char = {
                .gattc_if = gattc_if,
                .conn_id = p_data->search_cmpl.conn_id,
                .start_handle = gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                .end_handle = gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
            };
            status = BleGattcGetCharByUuid(get_char,
                                           remote_filter_char_uuid,
                                           char_elem_result,
                                           &count);
            if (status != BT_SUCCESS) {
                BT_LOGE("BleGattcGetCharByUuid error");
            }

            if (count > 0 && (char_elem_result[0].properties & OHOS_GATT_CHAR_PROP_BIT_NOTIFY)) {
                gl_profile_tab[PROFILE_A_APP_ID].char_handle = char_elem_result[0].char_handle;
                BleGattcRegisterForNotify(gattc_if, gl_profile_tab[PROFILE_A_APP_ID].remote_bda,
                                          char_elem_result[0].char_handle);
            }
            free(char_elem_result);
            char_elem_result = NULL;
        }
    } else {
        BT_LOGE("no char found");
    }
}

static void search_cmpl_event(GattInterfaceType gattc_if, BleGattcParam *p_data)
{
    if (p_data->search_cmpl.status != BT_SUCCESS) {
        BT_LOGE("search service failed, error status = %x", p_data->search_cmpl.status);
        return;
    }
    if (p_data->search_cmpl.searched_service_source == OHOS_GATT_SERVICE_FROM_REMOTE_DEVICE) {
        BT_LOGE("Get service information from remote device");
    } else if (p_data->search_cmpl.searched_service_source == OHOS_GATT_SERVICE_FROM_NVS_FLASH) {
        BT_LOGE("Get service information from flash");
    } else {
        BT_LOGE("unknown service source");
    }
    BT_LOGE("OHOS_GATTC_SEARCH_CMPL_EVT");
    if (get_server) {
        evt_get_server(gattc_if, p_data);
    }
}

static GattStatus notify_handle_attr(GattInterfaceType gattc_if, uint16_t count)
{
    GattcGetAttr get_attr = {
        .gattc_if = gattc_if,
        .conn_id = gl_profile_tab[PROFILE_A_APP_ID].conn_id,
        .type = OHOS_GATT_DB_DESCRIPTOR,
        .start_handle = gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
        .end_handle = gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
    };
    return BleGattcGetAttrCount(get_attr,
                                gl_profile_tab[PROFILE_A_APP_ID].char_handle,
                                &count);
}

static void act_notify_handle(GattInterfaceType gattc_if, BleGattcParam *p_data)
{
    uint16_t count = 0;
    uint16_t notify_en = 1;
    GattStatus ret_status = notify_handle_attr(gattc_if, count);
    if (ret_status != BT_SUCCESS) {
        BT_LOGE("BleGattcGetAttrCount error");
    }
    if (count > 0) {
        descr_elem_result = malloc(sizeof(BleGattcDescrElem) * count);
        if (!descr_elem_result) {
            BT_LOGE("malloc error, gattc no mem");
        } else {
            GattcGetDescr get_descr = {
                .gattc_if = gattc_if,
                .conn_id = gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                .char_handle = p_data->reg_for_notify.handle,
                .descr_uuid = descr_elem_result,
            };
            ret_status = BleGattcGetDescrByCharHandle(get_descr,
                                                      descr_elem_result,
                                                      &count);
            if (ret_status != BT_SUCCESS) {
                BT_LOGE("BleGattcGetDescrByCharHandle error");
            }
            if (count > 0 && descr_elem_result[0].uuid.len == UUID16_BIT &&
                descr_elem_result[0].uuid.uuid.uuid16 == OHOS_GATT_UUID_CHAR_CLIENT_CONFIG) {
                GattcWriteChar write_char = {
                    .gattc_if = gattc_if,
                    .conn_id = gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                    .handle = descr_elem_result[0].handle,
                    .value_len = sizeof(notify_en),
                    .write_type = OHOS_GATT_WRITE_TYPE_NO_RSP,
                };
                ret_status = BleGattcWriteCharDescr(write_char,
                                                    (uint8_t *)&notify_en,
                                                    OHOS_BLE_AUTH_NO_BOND);
            }
            if (ret_status != BT_SUCCESS) {
                BT_LOGE("BleGattcWriteCharDescr error");
            }
            free(descr_elem_result);
            descr_elem_result = NULL;
        }
    } else {
        BT_LOGE("decsr not found");
    }
}

static void reg_notify_evt(GattInterfaceType gattc_if, BleGattcParam *p_data)
{
    BT_LOGE("OHOS_GATTC_REG_FOR_NOTIFY_EVT");
    if (p_data->reg_for_notify.status != BT_SUCCESS) {
        BT_LOGE("REG FOR NOTIFY failed: error status = %d", p_data->reg_for_notify.status);
    } else {
        act_notify_handle(gattc_if, p_data);
    }
}

static void notify_evt(GattInterfaceType gattc_if, BleGattcParam *p_data)
{
    if (p_data->notify.is_notify) {
        BT_LOGE("OHOS_GATTC_NOTIFY_EVT, receive notify value:");
    } else {
        BT_LOGE("OHOS_GATTC_NOTIFY_EVT, receive indicate value:");
    }
    int dataLength = p_data->notify.value_len;
    printf("\n receive value length: %d \n", dataLength);
    printf("receive value: ");
    for (int i = 0; i < dataLength; i++) {
        printf("%x ", p_data->notify.value[i]);
    }
    printf("\n");
}

static void write_descr_evt(GattInterfaceType gattc_if, BleGattcParam *p_data)
{
    if (p_data->write.status != BT_SUCCESS) {
        BT_LOGE("write descr failed, error status = %x", p_data->write.status);
        return;
    }
    BT_LOGE("write descr success ");
    uint8_t write_char_data[4] = {0x00, 0x01, 0x02, 0x03};
    GattcWriteChar write_char = {
        .gattc_if = gattc_if,
        .conn_id = gl_profile_tab[PROFILE_A_APP_ID].conn_id,
        .handle = descr_elem_result[0].handle,
        .value_len = sizeof(write_char_data),
        .write_type = OHOS_GATT_WRITE_TYPE_NO_RSP,
    };
    BleGattcWriteCharacteristic(write_char,
                                write_char_data,
                                OHOS_BLE_AUTH_NO_BOND);
}

static void srvc_chg_evt(GattInterfaceType gattc_if, BleGattcParam *p_data)
{
    uint32_t bda[OHOS_BD_ADDR_LEN];
    memcpy_s(bda, sizeof(bda), p_data->srvc_chg.remote_bda, sizeof(p_data->srvc_chg.remote_bda));
    BT_LOGE("OHOS_GATTC_SRVC_CHG_EVT, bd_addr:");
}

static void wrire_char_evt(GattInterfaceType gattc_if, BleGattcParam *p_data)
{
    if (p_data->write.status != BT_SUCCESS) {
        BT_LOGE("write char failed, error status = %x", p_data->write.status);
        return;
    }
    BT_LOGE("write char success ");
}

static void gattc_disconnect_evt(GattInterfaceType gattc_if, BleGattcParam *p_data)
{
    connect = false;
    get_server = false;
    BT_LOGE("OHOS_GATTC_DISCONNECT_EVT, reason = %d", p_data->disconnect.reason);
    BT_LOGE("GATTC RECONNECT!\n");
    BleGattcConnect(gl_profile_tab[PROFILE_A_APP_ID].gattc_if,
                    NULL, (unsigned char *)directAddr,
                    true,
                    BLE_ADDR_TYPE_PUBLIC);
}

static void gattc_event_handler(GattcBleCallbackEvent event, GattInterfaceType gattc_if, BleGattcParam *param)
{
    BleGattcParam *p_data = (BleGattcParam *)param;

    switch (event) {
        case OHOS_GATTC_REG_EVT:
            gattc_reg_evt(gattc_if, p_data);
            break;
        case OHOS_GATTC_CONNECT_EVT: {
            connect_evt(gattc_if, p_data);
            break;
        }
        case OHOS_GATTC_OPEN_EVT:
            open_evt(p_data, param);
            break;
        case OHOS_GATTC_DIS_SRVC_CMPL_EVT:
            dis_srvccmpl_evt(gattc_if, param);
            break;
        case OHOS_GATTC_CFG_MTU_EVT:
            cfg_mtu_evt(param);
            break;
        case OHOS_GATTC_SEARCH_RES_EVT: {
            search_res_evt(gattc_if, p_data);
            break;
        }
        case OHOS_GATTC_SEARCH_CMPL_EVT:
            search_cmpl_event(gattc_if, p_data);
            break;
        case OHOS_GATTC_REG_FOR_NOTIFY_EVT: {
            reg_notify_evt(gattc_if, p_data);
            break;
        }
        case OHOS_GATTC_NOTIFY_EVT:
            notify_evt(gattc_if, p_data);
            break;
        case OHOS_GATTC_WRITE_DESCR_EVT:
            write_descr_evt(gattc_if, p_data);
            break;
        case OHOS_GATTC_SRVC_CHG_EVT: {
            srvc_chg_evt(gattc_if, p_data);
            break;
        }
        case OHOS_GATTC_WRITE_CHAR_EVT:
            wrire_char_evt(gattc_if, p_data);
            break;
        case OHOS_GATTC_DISCONNECT_EVT:
            gattc_disconnect_evt(gattc_if, p_data);
            break;
        default:
            break;
    }
}

static void adv_name_conncet(uint8_t *adv_name, uint8_t adv_name_len,
                             BleGapParam *scan_result)
{
    if (strlen(remote_device_name) == adv_name_len &&
        strncmp((char *)adv_name, remote_device_name, adv_name_len) == 0) {
        BT_LOGE("searched device %s\n", remote_device_name);
        if (connect == false) {
            connect = true;
            BT_LOGE("connect to the remote device.");
            BleStopScan();
            memcpy_s(directAddr, sizeof(directAddr),
                        scan_result->scan_rst.bda, sizeof(directAddr));
            BleGattcConnect(gl_profile_tab[PROFILE_A_APP_ID].gattc_if,
                            NULL, scan_result->scan_rst.bda,
                            true, BLE_ADDR_TYPE_PUBLIC);
        }
    }
}

static void scan_result_evt(BleGapParam *param)
{
    uint8_t *adv_name = NULL;
    uint8_t adv_name_len = 0;
    BleGapParam *scan_result = (BleGapParam *)param;
    switch (scan_result->scan_rst.search_evt) {
        case OHOS_GAP_SEARCH_INQ_RES_EVT:
            BT_LOGE("searched Adv Data Len %d, Scan Response Len %d",
                    scan_result->scan_rst.adv_data_len, scan_result->scan_rst.scan_rsp_len);
            adv_name = BleResolveAdvData(scan_result->scan_rst.ble_adv,
                                         OHOS_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
            if (adv_name_len) {
                BT_LOGE("searched Device Name Len %d", adv_name_len);
                BtLogBufferChar(GATTC_TAG, adv_name, adv_name_len);
                BT_LOGE("\n");
            }
            if (adv_name != NULL) {
                adv_name_conncet(adv_name, adv_name_len, scan_result);
            }
            break;
        case OHOS_GAP_SEARCH_INQ_CMPL_EVT:
            break;
        default:
            break;
    }
}

static void gap_cb(GapSearchEvent event, BleGapParam *param)
{
    switch (event) {
        case OHOS_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
            BleStartScan();
            break;
        }
        case OHOS_GAP_BLE_SCAN_RESULT_EVT: {
            scan_result_evt(param);
            break;
        }
        default:
            break;
    }
}

static void gattc_cb_handle(GattcBleCallbackEvent event, GattInterfaceType gattc_if, BleGattcParam *param)
{
    int idx;
    for (idx = 0; idx < PROFILE_NUM; idx++) {
        if (gattc_if == OHOS_GATT_IF_NONE ||
            gattc_if == gl_profile_tab[idx].gattc_if) {
            if (gl_profile_tab[idx].gattc_cb) {
                gl_profile_tab[idx].gattc_cb(event, gattc_if, param);
            }
        }
    }
}

static void gattc_cb_circle(GattcBleCallbackEvent event, GattInterfaceType gattc_if, BleGattcParam *param)
{
    do {
        gattc_cb_handle(event, gattc_if, param);
    } while (0);
}

static void gattc_cb(GattcBleCallbackEvent event, GattInterfaceType gattc_if, BleGattcParam *param)
{
    /* If event is register event, store the gattc_if for each profile */
    if (event == OHOS_GATTC_REG_EVT) {
        if (param->reg.status == BT_SUCCESS) {
            gl_profile_tab[param->reg.app_id].gattc_if = gattc_if;
        } else {
            BT_LOGE("reg app failed, app_id %04x, status %d",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }
    gattc_cb_circle(event, gattc_if, param);
}

void bluetooth_example(void)
{
    EnableBle();
    BleGattcConfigureMtuSize(MTU_SIZE);
    BtGattClientCallbacks btGattClientCallbacks = {
        .gap_callback = gap_cb,
        .gattc_callback = gattc_cb,
        .PROFILE_APP_ID = PROFILE_A_APP_ID,
    };
    BleGattcRegister(btGattClientCallbacks);
}

OHOS_APP_RUN(bluetooth_example);
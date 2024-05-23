/*
 * Copyright (c) 2022 Hunan OpenValley Digital Industry Development Co., Ltd.
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

#ifndef __BLUETOOTH_EXAMPLE_H__
#define __BLUETOOTH_EXAMPLE_H__
#include "stdbool.h"
#include "securec.h"
#include "ohos_bt_def.h"
#include "ohos_bt_gatt.h"
#include "bluetooth_device.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#define REMOTE_SERVICE_UUID 0x00FF
#define REMOTE_NOTIFY_CHAR_UUID 0xFF01
#define PROFILE_NUM 1
#define PROFILE_A_APP_ID 0
#define INVALID_HANDLE 0
#define UUID16_BIT 2
#define MTU_SIZE 500

struct GattcProfileInst {
    GattcBleCallback gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t char_handle;
    BdAddr remote_bda;
};
#endif /* __BLUETOOTH_EXAMPLE_H__ */

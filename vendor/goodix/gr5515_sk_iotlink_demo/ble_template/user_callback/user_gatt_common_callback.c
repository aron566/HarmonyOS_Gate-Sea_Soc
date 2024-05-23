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

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "gr55xx_sys.h"

/*
* LOCAL FUNCTION DECLARATION
*****************************************************************************************
*/
static void app_gatt_mtu_exchange_cb(uint8_t conn_idx, uint8_t status, uint16_t mtu);

/*
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
const gatt_common_cb_fun_t app_gatt_common_callback = {
    .app_gatt_mtu_exchange_cb = app_gatt_mtu_exchange_cb,
};

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 * @brief This callback function will be called when the mtu has been exchanged.
 *
 * @param[in] conn_idx: The connection index.
 * @param[in] status:   The status of GATT operation.
 * @param[in] mtu:      The value of exchanged mtu.
 *****************************************************************************************
 */
static void app_gatt_mtu_exchange_cb(uint8_t conn_idx, uint8_t status, uint16_t mtu)
{
}

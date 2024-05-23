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
static void app_sec_rcv_enc_req_cb(uint8_t conn_idx, sec_enc_req_t *p_enc_req);
static void app_sec_rcv_enc_ind_cb(uint8_t conn_idx, sec_enc_ind_t enc_ind, uint8_t auth);
static void app_sec_rcv_keypress_nofify_cb(uint8_t conn_idx, sec_keypress_notify_t notify_type);

/*
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
const sec_cb_fun_t app_sec_callback = {
    .app_sec_enc_req_cb         = app_sec_rcv_enc_req_cb,
    .app_sec_enc_ind_cb         = app_sec_rcv_enc_ind_cb,
    .app_sec_keypress_notify_cb = app_sec_rcv_keypress_nofify_cb
};

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving encryption request.
 *
 * @param[in] conn_idx:  The connection index.
 * @param[in] p_enc_req: The information of SEC encryption request. See @ref sec_enc_req_t.
 *****************************************************************************************
 */
static void app_sec_rcv_enc_req_cb(uint8_t conn_idx, sec_enc_req_t *p_enc_req)
{
}

/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving pair indication.
 *
 * @param[in] conn_idx: The connection index.
 * @param[in] enc_ind:  The result of SEC pair. See @ref sec_enc_ind_t.
 * @param[in] auth:     Indicate the flag of bonding, MITM and secure connection.
 *                      See @ref SEC_AUTH_FLAG.
 *****************************************************************************************
 */
static void app_sec_rcv_enc_ind_cb(uint8_t conn_idx, sec_enc_ind_t enc_ind, uint8_t auth)
{
}

/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving key press notify.
 *
 * @param[in] conn_idx:    The connection index.
 * @param[in] notify_type: The type of SEC key press. See @ref sec_keypress_notify_t.
 *****************************************************************************************
 */
static void app_sec_rcv_keypress_nofify_cb(uint8_t conn_idx, sec_keypress_notify_t notify_type)
{
}

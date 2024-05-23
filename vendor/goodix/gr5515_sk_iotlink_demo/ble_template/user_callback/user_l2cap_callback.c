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
static void app_l2cap_lecb_conn_req_cb(uint8_t conn_idx, lecb_conn_req_ind_t *p_conn_req);
static void app_l2cap_lecb_conn_cb(uint8_t conn_idx, uint8_t status, lecb_conn_ind_t *p_conn_ind);
static void app_l2cap_lecb_add_credits_ind_cb(uint8_t conn_idx, lecb_add_credits_ind_t *p_add_credits_ind);
static void app_l2cap_lecb_disconn_cb(uint8_t conn_idx, uint8_t status, lecb_disconn_ind_t *p_disconn_ind);
static void app_l2cap_lecb_sdu_recv_cb(uint8_t conn_idx, lecb_sdu_t *p_sdu);
static void app_l2cap_lecb_sdu_send_cb(uint8_t conn_idx, uint8_t status, lecb_sdu_send_evt_t *p_sdu_send_evt);
static void app_l2cap_lecb_credit_add_cmp_cb(uint8_t conn_idx, uint8_t status, uint16_t local_cid);

/*
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
const l2cap_lecb_cb_fun_t app_l2cap_callback = {
    .app_l2cap_lecb_conn_req_cb        = app_l2cap_lecb_conn_req_cb,
    .app_l2cap_lecb_conn_cb            = app_l2cap_lecb_conn_cb,
    .app_l2cap_lecb_add_credits_ind_cb = app_l2cap_lecb_add_credits_ind_cb,
    .app_l2cap_lecb_disconn_cb         = app_l2cap_lecb_disconn_cb,
    .app_l2cap_lecb_sdu_recv_cb        = app_l2cap_lecb_sdu_recv_cb,
    .app_l2cap_lecb_sdu_send_cb        = app_l2cap_lecb_sdu_send_cb,
    .app_l2cap_lecb_credit_add_cmp_cb  = app_l2cap_lecb_credit_add_cmp_cb,
};

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving lecb connection request.
 *
 * @param[in] conn_idx:   The connection index.
 * @param[in] p_conn_req: The information of LE credit based connection request. See @ref lecb_conn_req_ind_t.
 *****************************************************************************************
 */
static void app_l2cap_lecb_conn_req_cb(uint8_t conn_idx, lecb_conn_req_ind_t *p_conn_req)
{
}

/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving lecb created indication.
 *
 * @param[in] conn_idx:   The connection index.
 * @param[in] status:     The status of L2cap operation.
 * @param[in] p_conn_ind: The information LE credit based connection created indication. See @ref lecb_conn_ind_t.
 *****************************************************************************************
 */
static void app_l2cap_lecb_conn_cb(uint8_t conn_idx, uint8_t status, lecb_conn_ind_t *p_conn_ind)
{
}

/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving lecb connection addition indication.
 *
 * @param[in] conn_idx:          The connection index.
 * @param[in] p_add_credits_ind: The information of LE credit based connection addition indication.
 * See @ref lecb_add_credits_ind_t.
 *****************************************************************************************
 */
static void app_l2cap_lecb_add_credits_ind_cb(uint8_t conn_idx, lecb_add_credits_ind_t *p_add_credits_ind)
{
}

/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving LE Credit Based disconnection indication.
 *
 * @param[in] conn_idx:      The connection index.
 * @param[in] status:        The status of L2cap operation.
 * @param[in] p_disconn_ind: The information of LE credit based disconnect indication. See @ref lecb_disconn_ind_t.
 *****************************************************************************************
 */
static void app_l2cap_lecb_disconn_cb(uint8_t conn_idx, uint8_t status, lecb_disconn_ind_t *p_disconn_ind)
{
}

/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving SDU packet.
 *
 * @param[in] conn_idx: The connection index.
 * @param[in] p_sdu:    SDU packet parameter. See @ref lecb_sdu_t.
 *****************************************************************************************
 */
static void app_l2cap_lecb_sdu_recv_cb(uint8_t conn_idx, lecb_sdu_t *p_sdu)
{
}

/**
 *****************************************************************************************
 * @brief This callback function will be called when sending SDU packet.
 *
 * @param[in] conn_idx:       The connection index.
 * @param[in] status:         The status of L2cap operation.
 * @param[in] p_sdu_send_evt: SDU packet parameter. @see lecb_sdu_send_evt_t.
 *****************************************************************************************
 */
static void app_l2cap_lecb_sdu_send_cb(uint8_t conn_idx, uint8_t status, lecb_sdu_send_evt_t *p_sdu_send_evt)
{
}

/**
 *****************************************************************************************
 * @brief This callback function will be called when adding lecb credit.
 *
 * @param[in] conn_idx:       The connection index.
 * @param[in] status:         The status of L2cap operation.
 *****************************************************************************************
 */
static void app_l2cap_lecb_credit_add_cmp_cb(uint8_t conn_idx, uint8_t status, uint16_t local_cid)
{
}


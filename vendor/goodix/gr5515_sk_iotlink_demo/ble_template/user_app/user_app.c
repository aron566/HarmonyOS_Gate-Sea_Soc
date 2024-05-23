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
#include "user_app.h"
#include "gr55xx_sys.h"
#include "app_log.h"
#include "app_error.h"

/*
 * DEFINES
 *****************************************************************************************
 */
/**@brief Gapm config data. */
#define DEVICE_NAME                        "Goodix_Tem" /**< Device Name which will be set in GAP. */

/**< The fast advertising min interval (in units of 0.625 ms). */
#define APP_ADV_FAST_MIN_INTERVAL          32
/**< The fast advertising max interval (in units of 0.625 ms). */
#define APP_ADV_FAST_MAX_INTERVAL          48
/**< The slow advertising min interval (in units of 0.625 ms). */
#define APP_ADV_SLOW_MIN_INTERVAL          160
/**< The slow advertising max interval (in units of 0.625 ms). */
#define APP_ADV_SLOW_MAX_INTERVAL          400

#define APP_ADV_TIMEOUT_IN_SECONDS         0            /**< The advertising timeout in units of seconds. */
#define MIN_CONN_INTERVAL                  320          /**< Minimum acceptable connection interval (0.4 seconds). */
#define MAX_CONN_INTERVAL                  520          /**< Maximum acceptable connection interval (0.65 second). */
#define SLAVE_LATENCY                      0            /**< Slave latency. */
#define CONN_SUP_TIMEOUT                   400          /**< Connection supervisory timeout (4 seconds). */

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
static gap_adv_param_t      s_gap_adv_param;            /**< Advertising parameters for legay advertising. */
static gap_adv_time_param_t s_gap_adv_time_param;       /**< Advertising time parameter. */

static const uint8_t s_adv_data_set[] = {               /**< Advertising data. */
    0x03,
    BLE_GAP_AD_TYPE_COMPLETE_LIST_16_BIT_UUID,
    0x01,
    0x01,

    // Manufacturer specific adv data type
    0x05,
    BLE_GAP_AD_TYPE_MANU_SPECIFIC_DATA,
    // Goodix SIG Company Identifier: 0x04F7
    0xF7,
    0x04,
    // Goodix specific adv data
    0x02, 0x03,
};

static const uint8_t s_adv_rsp_data_set[] = {                /**< Scan responce data. */
    0x0b,
    BLE_GAP_AD_TYPE_COMPLETE_NAME,
    'G', 'o', 'o', 'd', 'i', 'x', '_', 'T', 'e', 'm',
};

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 * @brief Initialize gap parameters.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile)parameters
 *          of the device including the device name, appearance, and the preferred connection parameters.
 *****************************************************************************************
 */
static void gap_params_init(void)
{
    sdk_err_t   error_code;

    ble_gap_pair_enable(false);

    s_gap_adv_param.adv_intv_max  = APP_ADV_SLOW_MAX_INTERVAL;
    s_gap_adv_param.adv_intv_min  = APP_ADV_FAST_MIN_INTERVAL;
    s_gap_adv_param.adv_mode      = GAP_ADV_TYPE_ADV_IND;
    s_gap_adv_param.chnl_map      = GAP_ADV_CHANNEL_37_38_39;
    s_gap_adv_param.disc_mode     = GAP_DISC_MODE_GEN_DISCOVERABLE;
    s_gap_adv_param.filter_pol    = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;

    uint8_t dev_name[32];
    uint16_t dev_name_len = 32;
    error_code = ble_gap_device_name_get(dev_name, &dev_name_len);
    APP_ERROR_CHECK(error_code);

    if (!strcmp((const char *)dev_name, BLE_GAP_DEVNAME_DEFAULT)) {
        // Set the default Device Name.
        error_code = ble_gap_device_name_set(BLE_GAP_WRITE_PERM_NOAUTH, DEVICE_NAME, strlen(DEVICE_NAME));
        APP_ERROR_CHECK(error_code);
    } else {
        // Set the Device Name is writable from the peer.
        error_code = ble_gap_device_name_set(BLE_GAP_WRITE_PERM_NOAUTH, NULL, 0);
        APP_ERROR_CHECK(error_code);
    }

    error_code = ble_gap_adv_param_set(0, BLE_GAP_OWN_ADDR_STATIC, &s_gap_adv_param);
    APP_ERROR_CHECK(error_code);

    error_code = ble_gap_adv_data_set(0, BLE_GAP_ADV_DATA_TYPE_DATA, s_adv_data_set, sizeof(s_adv_data_set));
    APP_ERROR_CHECK(error_code);

    error_code = ble_gap_adv_data_set(0, BLE_GAP_ADV_DATA_TYPE_SCAN_RSP, \
                                      s_adv_rsp_data_set, sizeof(s_adv_rsp_data_set));
    APP_ERROR_CHECK(error_code);

    s_gap_adv_time_param.duration     = 0;
    s_gap_adv_time_param.max_adv_evt  = 0;
}


/**
 *****************************************************************************************
 * @brief Initialize services that will be used by the application.
 *****************************************************************************************
 */
static void services_init(void)
{
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
void app_disconnected_handler(uint8_t conn_idx, uint8_t reason)
{
    sdk_err_t error_code;

    error_code = ble_gap_adv_start(0, &s_gap_adv_time_param);
    APP_ERROR_CHECK(error_code);
}

void app_connected_handler(uint8_t conn_idx, const gap_conn_cmp_t *p_param)
{
}

void ble_init_cmp_callback(void)
{
    sdk_err_t     error_code;
    gap_bdaddr_t  bd_addr;
    sdk_version_t version;

    sys_sdk_verison_get(&version);
    APP_LOG_INFO("Goodix GR551x SDK V%d.%d.%02d (commit %d)",
                 version.major, version.minor, version.build, version.commit_id);

    error_code = ble_gap_addr_get(&bd_addr);
    APP_ERROR_CHECK(error_code);
    APP_LOG_INFO("Local Board %02X:%02X:%02X:%02X:%02X:%02X.",
                 bd_addr.gap_addr.addr[5],      /* gap_addr byte5 */
                 bd_addr.gap_addr.addr[4],      /* gap_addr byte4 */
                 bd_addr.gap_addr.addr[3],      /* gap_addr byte3 */
                 bd_addr.gap_addr.addr[2],      /* gap_addr byte2 */
                 bd_addr.gap_addr.addr[1],      /* gap_addr byte1 */
                 bd_addr.gap_addr.addr[0]);     /* gap_addr byte0 */
    APP_LOG_INFO("Template application example started.");

    services_init();
    gap_params_init();

    error_code = ble_gap_adv_start(0, &s_gap_adv_time_param);
    APP_ERROR_CHECK(error_code);
}


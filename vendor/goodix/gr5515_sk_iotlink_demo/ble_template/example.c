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

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "gr55xx_sys.h"
#include "scatter_common.h"
#include "flash_scatter_config.h"
#include "user_app.h"
#include "app_callback.h"

#define BLE_TASK_STACK_SIZE   4096
#define BLE_TASK_PRIO         25
#define MS_1000     1000

/**@brief Stack global variables for Bluetooth protocol stack. */
STACK_HEAP_INIT(heaps_table);

static app_callback_t s_app_ble_callback = {
    .app_ble_init_cmp_callback  = ble_init_cmp_callback,
    .app_gap_callbacks          = &app_gap_callbacks,
    .app_gatt_common_callback   = &app_gatt_common_callback,
    .app_gattc_callback         = &app_gattc_callback,
    .app_sec_callback           = &app_sec_callback,
};

static void *BLE_Task(const char *arg)
{
    (void)arg;

    printf("Initialize the BLE stack.\r\n");

    /* init ble stack */
    ble_stack_init(&s_app_ble_callback, &heaps_table);

    while (1) {
        osDelay(MS_1000);
    }
}

void BLE_TaskEntry(void)
{
    osThreadAttr_t attr;

    attr.name       = "BLE_Task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = BLE_TASK_STACK_SIZE;
    attr.priority   = BLE_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)BLE_Task, NULL, &attr) == NULL) {
        printf("[HelloDemo] Failed to create HelloTask!\n");
    }
}

SYS_RUN(BLE_TaskEntry);

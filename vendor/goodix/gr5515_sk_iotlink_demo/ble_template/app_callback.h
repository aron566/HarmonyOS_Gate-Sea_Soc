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

#ifndef __USER_APP_CALLBACK_H__
#define __USER_APP_CALLBACK_H__

#include "gr55xx_sys.h"

extern gap_cb_fun_t         app_gap_callbacks;
extern gatt_common_cb_fun_t app_gatt_common_callback;
extern gattc_cb_fun_t       app_gattc_callback;
extern sec_cb_fun_t         app_sec_callback;

#endif
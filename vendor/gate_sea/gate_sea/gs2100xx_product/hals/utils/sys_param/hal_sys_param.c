/*
 * Copyright (c) 2024-2026 Gate-Sea Co., Ltd.
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

#include "hal_sys_param.h"

static const char OHOS_DEVICE_TYPE[]        = {"HPLC&HRF Board"};
static const char OHOS_DISPLAY_VERSION[]    = {"OpenHarmony 3.1"};
static const char OHOS_MANUFACTURE[]        = {"Gate-Sea"};
static const char OHOS_BRAND[]              = {"GS2100"};
static const char OHOS_MARKET_NAME[]        = {"GS2100"};
static const char OHOS_PRODUCT_SERIES[]     = {"GS21000xx"};
static const char OHOS_PRODUCT_MODEL[]      = {"GS2100xx_ZB205"};
static const char OHOS_SOFTWARE_MODEL[]     = {"1.0.0"};
static const char OHOS_HARDWARE_MODEL[]     = {"2.0.0"};
static const char OHOS_HARDWARE_PROFILE[]   = {"RAM:1MB,ROM:8K,ETH:false"};
static const char OHOS_BOOTLOADER_VERSION[] = {"boot-v2024.05"};
static const char OHOS_ABI_LIST[]           = {"armm3_soft_fpv4-sp-d16-liteos"};
static const char OHOS_SERIAL[]             = {"1234567890"};   // provided by OEM.

const char *HalGetSerial(void)
{
  return OHOS_SERIAL;
}

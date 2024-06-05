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

static const char OHOS_DEVICE_TYPE[]        = {"HPLC Board"};
static const char OHOS_DISPLAY_VERSION[]    = {"OpenHarmony 3.1"};
static const char OHOS_MANUFACTURE[]        = {"Gate-Sea"};
static const char OHOS_BRAND[]              = {"GS2000"};
static const char OHOS_MARKET_NAME[]        = {"GS2000"};
static const char OHOS_PRODUCT_SERIES[]     = {"GS2000xx"};
static const char OHOS_PRODUCT_MODEL[]      = {"GS2000xx_ZB202"};
static const char OHOS_SOFTWARE_MODEL[]     = {"1.0.0"};
static const char OHOS_HARDWARE_MODEL[]     = {"2.0.0"};
static const char OHOS_HARDWARE_PROFILE[]   = {"RAM:768K,ROM:8K,ETH:false"};
static const char OHOS_BOOTLOADER_VERSION[] = {"boot-v2024.05"};
static const char OHOS_ABI_LIST[]           = {"armm3_soft_fpv4-sp-d16-liteos"};
static const char OHOS_SERIAL[]             = {"GS2000xx854567854521"};   // provided by OEM.
static const int OHOS_FIRST_API_VERSION     = 1;

/* 获取设备类型 */
const char *HalGetDeviceType(void)
{
  return OHOS_DEVICE_TYPE;
}

/* 获取厂商信息 */
const char *HalGetManufacture(void)
{
  return OHOS_MANUFACTURE;
}

/* 获取商标名称 */
const char *HalGetBrand(void)
{
  return OHOS_BRAND;
}

const char *HalGetMarketName(void)
{
  return OHOS_MARKET_NAME;
}

/* 获取产品系列名称 */
const char *HalGetProductSeries(void)
{
  return OHOS_PRODUCT_SERIES;
}

/* 获取产品型号 */
const char *HalGetProductModel(void)
{
  return OHOS_PRODUCT_MODEL;
}

const char *HalGetSoftwareModel(void)
{
  return OHOS_SOFTWARE_MODEL;
}

const char *HalGetHardwareModel(void)
{
  return OHOS_HARDWARE_MODEL;
}

const char *HalGetHardwareProfile(void)
{
  return OHOS_HARDWARE_PROFILE;
}

const char *HalGetSerial(void)
{
  return OHOS_SERIAL;
}

const char *HalGetBootloaderVersion(void)
{
  return OHOS_BOOTLOADER_VERSION;
}

const char *HalGetAbiList(void)
{
  return OHOS_ABI_LIST;
}

const char *HalGetDisplayVersion(void)
{
  return OHOS_DISPLAY_VERSION;
}

const char *HalGetIncrementalVersion(void)
{
  return INCREMENTAL_VERSION;
}

const char *HalGetBuildType(void)
{
  return BUILD_TYPE;
}

const char *HalGetBuildUser(void)
{
  return BUILD_USER;
}

const char *HalGetBuildHost(void)
{
  return BUILD_HOST;
}

const char *HalGetBuildTime(void)
{
  return BUILD_TIME;
}

int HalGetFirstApiVersion(void)
{
  return OHOS_FIRST_API_VERSION;
}

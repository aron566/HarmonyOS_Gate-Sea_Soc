/*
 * Copyright (c) 2022 Hihope Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <stdlib.h>
#include <string.h>
#include "hal_dac.h"
#include "hdf_device_desc.h"
#include "device_resource_if.h"
#include "hdf_log.h"

#define DEBUG_DAC_LOG            0

static int32_t GD32DacWrite(struct DacDevice *device, uint32_t channel, uint32_t val)
{
    if (device == NULL || device->priv == NULL) {
        HDF_LOGE("%s: device or priv is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    dac_data_set(DAC0, DAC_ALIGN_12B_L, val);

    return HDF_SUCCESS;
}

static inline int32_t GD32DacStart(struct DacDevice *device)
{
    struct GD32DacDevice *gd32f4xx = NULL;

    if (device == NULL || device->priv == NULL) {
        HDF_LOGE("%s: device or priv is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    gd32f4xx = (struct GD32DacDevice *)device;

#if DEBUG_DAC_LOG
    HDF_LOGI("%s: %d [dacNum = %d] [validChannel = %d]\n", __func__, __LINE__, gd32f4xx->dacNum, gd32f4xx->validChannel);
#endif

    /* rcu config */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_DAC);

    /* gpio config */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, g_map_dac_pin[gd32f4xx->dacNum]);

    /* dac config */
    dac_deinit();
    /* configure the DAC0 */
    dac_trigger_disable(g_map_dac[gd32f4xx->dacNum]);
    dac_wave_mode_config(g_map_dac[gd32f4xx->dacNum], DAC_WAVE_DISABLE);
    dac_output_buffer_enable(g_map_dac[gd32f4xx->dacNum]);
    /* enable DAC0 and set data */
    dac_enable(g_map_dac[gd32f4xx->dacNum]);

    return HDF_SUCCESS;
}

static inline int32_t GD32DacStop(struct DacDevice *device)
{
    if (device == NULL || device->priv == NULL) {
        HDF_LOGE("%s: device or priv is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return HDF_SUCCESS;
}

struct DacMethod g_GD32DACmethod = {
    .write = GD32DacWrite,
    .stop = GD32DacStop,
    .start = GD32DacStart,
};

static int32_t gd32f4xxDacReadDrs(struct GD32DacDevice *gd32f4xx, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid drs ops", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    ret = drsOps->GetUint32(node, "deviceNum", &gd32f4xx->deviceNum, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read deviceNum failed", __func__);
        return ret;
    }
#if DEBUG_DAC_LOG
    HDF_LOGI("%s: gd32f4xx->deviceNum %x", __func__, gd32f4xx->deviceNum);
#endif

    ret = drsOps->GetUint32(node, "dacNum", &gd32f4xx->dacNum, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read dacNum failed", __func__);
        return ret;
    }
#if DEBUG_DAC_LOG
    HDF_LOGI("%s: gd32f4xx->dacNum %x", __func__, gd32f4xx->dacNum);
#endif

    ret = drsOps->GetUint32(node, "validChannel", &gd32f4xx->validChannel, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read validChannel failed", __func__);
        return ret;
    }
#if DEBUG_DAC_LOG
    HDF_LOGI("%s: gd32f4xx->validChannel %x", __func__, gd32f4xx->validChannel);
#endif

    return HDF_SUCCESS;
}

static int32_t GD32DacParseInit(struct HdfDeviceObject *device, struct DeviceResourceNode *node)
{
    int32_t ret;
    struct GD32DacDevice *gd32f4xx = NULL;

    (void)device;
    gd32f4xx = (struct GD32DacDevice *)OsalMemCalloc(sizeof(*gd32f4xx));
    if (gd32f4xx == NULL) {
        HDF_LOGE("%s: alloc gd32f4xx failed", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = gd32f4xxDacReadDrs(gd32f4xx, node);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read drs failed:%d", __func__, ret);
        goto __ERR__;
    }

    gd32f4xx->device.priv = (void *)node;
    gd32f4xx->device.devNum = gd32f4xx->deviceNum;
    gd32f4xx->device.ops = &g_GD32DACmethod;
    ret = DacDeviceAdd(&gd32f4xx->device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: add adc device:%u failed", __func__, gd32f4xx->deviceNum);
        goto __ERR__;
    }
    return HDF_SUCCESS;

__ERR__:
    if (gd32f4xx != NULL) {
        DacDeviceRemove(&gd32f4xx->device);
        OsalMemFree(gd32f4xx);
    }
    return ret;
}

static int32_t DacDriverInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct DeviceResourceNode *childNode = NULL;

    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or property is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = HDF_SUCCESS;
    DEV_RES_NODE_FOR_EACH_CHILD_NODE(device->property, childNode) {
        ret = GD32DacParseInit(device, childNode);
        if (ret != HDF_SUCCESS) {
            break;
        }
    }
    HDF_LOGI("%s: adc init success.", __func__);

    return HDF_SUCCESS;
}

static void GD32DacRemoveByNode(const struct DeviceResourceNode *node)
{
    int32_t ret;
    int32_t deviceNum;
    struct DacDevice *device = NULL;
    struct GD32DacDevice *gd32f4xx = NULL;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid drs ops", __func__);
        return;
    }

    ret = drsOps->GetUint32(node, "deviceNum", (uint32_t *)&deviceNum, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read deviceNum failed", __func__);
        return;
    }

    device = DacDeviceGet(deviceNum);
    if (device != NULL && device->priv == node) {
        DacDevicePut(device);
        DacDeviceRemove(device);
        gd32f4xx = (struct GD32DacDevice *)device;
        OsalMemFree(gd32f4xx);
    }
    return;
}

static void DacDriverRelease(struct HdfDeviceObject *device)
{
    const struct DeviceResourceNode *childNode = NULL;

    HDF_LOGI("%s: enter", __func__);
    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or property is null", __func__);
        return;
    }
    DEV_RES_NODE_FOR_EACH_CHILD_NODE(device->property, childNode) {
        GD32DacRemoveByNode(childNode);
    }
}

struct HdfDriverEntry g_DacDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "GD32F4xx_DAC_DRIVER",
    .Init = DacDriverInit,
    .Release = DacDriverRelease,
};
HDF_INIT(g_DacDriverEntry);

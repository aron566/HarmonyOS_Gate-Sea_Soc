#include "stdio.h"
#include "watchdog_core.h"
#include "hdf_device_desc.h"
#include "device_resource_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "gd32f4xx_fwdgt.h"
#include "gd32_watchdog.h"

#define HDF_LOG_TAG   iwatchdog
#define DEBUG_WDT_LOG            0

Gd32Watchdog *GdWdg = NULL;

static int32_t Gd32WatchdogStart(struct WatchdogCntlr *gdWdt)
{
    (void *)gdWdt;

    fwdgt_enable();

    return HDF_SUCCESS;
}

static int32_t Gd32WatchdogSetTimeout(struct WatchdogCntlr *gdWdt, uint32_t seconds)
{
    (void *)gdWdt;

#if DEBUG_WDT_LOG
    HDF_LOGI("%s: [line = %d] [ReloadMult = %d] [seconds = %d]\n", __func__, __LINE__, GdWdg->ReloadMult, seconds);
#endif

    /* confiure FWDGT counter clock: 32KHz(IRC32K) / 64 = 0.5 KHz */
    fwdgt_config(seconds * (GdWdg->ReloadMult), g_watchdogClockDiv[GdWdg->countPeriod]);

    return HDF_SUCCESS;
}

static int32_t Gd32WatchdogFeed(struct WatchdogCntlr *gdWdt)
{
    (void *)gdWdt;

    fwdgt_counter_reload();

    return HDF_SUCCESS;
}

static struct WatchdogMethod g_method = {
    .getStatus = NULL,
    .start = Gd32WatchdogStart,
    .stop = NULL,
    .setTimeout = Gd32WatchdogSetTimeout,
    .getTimeout = NULL,
    .feed = Gd32WatchdogFeed,
};

static int32_t Gd32WatchdogReadDrs(Gd32Watchdog *GdWdg, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid drs ops!", __func__);
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint32(node, "id", &GdWdg->watchdog_Id, 0);
    if (ret != HDF_SUCCESS ) {
        HDF_LOGE("%s: read watchdog_Id fail!", __func__);
        return ret;
    }
    HDF_LOGI("%s: got valid watchdog_Id %d !", __func__, GdWdg->watchdog_Id);

    ret = drsOps->GetUint32(node, "prescaler", &GdWdg->countPeriod, 0);
    if (ret != HDF_SUCCESS ) {
        HDF_LOGE("%s: read prescaler fail!", __func__);
        return ret;
    }
    HDF_LOGI("%s: got valid prescaler %d !", __func__, GdWdg->countPeriod);

    ret = drsOps->GetUint32(node, "reload_multiple", &GdWdg->ReloadMult, 0);
    if (ret != HDF_SUCCESS ) {
        HDF_LOGE("%s: read reload_multiple fail!", __func__);
        return ret;
    }
    HDF_LOGI("%s: got valid reload_multiple %d !", __func__, GdWdg->ReloadMult);

    return HDF_SUCCESS;
}

static int32_t Gd32WatchdogBind(struct HdfDeviceObject *device)
{
    int32_t ret;

    HDF_LOGI("%s: Enter", __func__);
    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or property is null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    GdWdg = (Gd32Watchdog *)OsalMemCalloc(sizeof(Gd32Watchdog));
    if (GdWdg == NULL) {
        HDF_LOGE("%s: malloc GdWdg fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = Gd32WatchdogReadDrs(GdWdg, device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read drs fail:%d", __func__, ret);
        OsalMemFree(GdWdg);
        return ret;
    }

    GdWdg->wdt.priv = (void *)device->property;
    GdWdg->wdt.ops = &g_method;
    GdWdg->wdt.device = device;
    ret = WatchdogCntlrAdd(&GdWdg->wdt);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: err add watchdog:%d", __func__, ret);
        OsalMemFree(GdWdg);
        return ret;
    }
    HDF_LOGI("%s: dev service %s init success!", __func__, HdfDeviceGetServiceName(device));
    return HDF_SUCCESS;
}

static int32_t Gd32WatchdogInit(struct HdfDeviceObject *device)
{
    (void)device;

    if(device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or device->property is null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return HDF_SUCCESS;
}

static void Gd32WatchdogRelease(struct HdfDeviceObject *device)
{
    struct WatchdogCntlr *wdt = NULL;
    Gd32Watchdog *wdg = NULL;

    HDF_LOGI("%s: enter", __func__);
    if (device == NULL) {
        return;
    }

    wdt = WatchdogCntlrFromDevice(device);
    if (wdt == NULL) {
        return;
    }
    WatchdogCntlrRemove(wdt);

    wdg = (Gd32Watchdog *)wdt;

    OsalMemFree(wdg);
}

struct HdfDriverEntry g_watchdogDriverEntry = {
    .moduleVersion = 1,
    .Bind = Gd32WatchdogBind,
    .Init = Gd32WatchdogInit,
    .Release = Gd32WatchdogRelease,
    .moduleName = "HDF_PLATFORM_WATCHDOG",
};

HDF_INIT(g_watchdogDriverEntry);
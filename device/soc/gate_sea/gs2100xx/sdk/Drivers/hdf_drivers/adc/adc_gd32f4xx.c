#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "adc_gd32f4xx.h"
#include "device_resource_if.h"
#include "adc_core.h"
#include "hal_gpio.h"

#define DEBUG_ADC_LOG            0

uint16_t adc_value[ADC_TYPE_MAX][DMA_BLOCK_SIZE] = {0};

// AdcStart's foundation method
static inline int32_t gd32f4xxAdcStart(struct AdcDevice *device)
{
    struct gd32f4xxAdcDevice *gd32f4xx = (struct gd32f4xxAdcDevice *)device;

    if (device == NULL || device->priv == NULL) {
        HDF_LOGE("%s: device or priv is null", __FUNCTION__);
        return HDF_ERR_INVALID_PARAM;
    }

    return HDF_SUCCESS;
}

// AdcRead's foundation method
static int32_t gd32f4xxAdcRead(struct AdcDevice *device, uint32_t channel, uint32_t *pVal)
{
    struct gd32f4xxAdcDevice *gd32f4xx = (struct gd32f4xxAdcDevice *)device;

    if (device == NULL || device->priv == NULL) {
        HDF_LOGE("%s: invalid device", __FUNCTION__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (channel < 1 || channel > ADC_CHANNEL_MAX) {
        HDF_LOGE("%s: illeage (%s) is out of range", __FUNCTION__, "channel");
        return HDF_ERR_INVALID_PARAM;
    }

    *pVal = adc_value[device->devNum][channel - 1];

    return HDF_SUCCESS;
}

// AdcStop's foundation method
static inline int32_t gd32f4xxAdcStop(struct AdcDevice *device)
{
    struct gd32f4xxAdcDevice *gd32f4xx = (struct gd32f4xxAdcDevice *)device;

    if (device == NULL || device->priv == NULL) {
        HDF_LOGE("%s: device or priv is null", __FUNCTION__);
        return HDF_ERR_INVALID_PARAM;
    }

    //disable adc
    adc_disable(g_adc_type_Maps[device->devNum]);

    return HDF_SUCCESS;
}

static const struct AdcMethod g_method = {
    .read = gd32f4xxAdcRead,
    .stop = gd32f4xxAdcStop,
    .start = gd32f4xxAdcStart,
};

// setup AdcDevice and add to AdcManager
static int32_t gd32f4xxAdcDeviceInit(struct gd32f4xxAdcDevice *gd32f4xx, struct DeviceResourceNode *node)
{
    int32_t ret;

    // device.devNum is the index referred by AdcManager
    gd32f4xx->device.devNum = gd32f4xx->deviceNum;
    // node is like sign to prove it valid
    gd32f4xx->device.priv = node;
    // register the operation api
    gd32f4xx->device.ops = &g_method;

    if ((ret = AdcDeviceAdd(&gd32f4xx->device)) != HDF_SUCCESS) {
        HDF_LOGE("%s: add adc device_num:%u failed", __FUNCTION__, gd32f4xx->deviceNum);
        return ret;
    }

    HDF_LOGD("%s: success{devNum(%d), device(%x), priv(%x)}\n", __FUNCTION__,  gd32f4xx->deviceNum, &gd32f4xx->device, node);
    return ret;
}

// kill and remove the AdcDevice from AdcManager
static void gd32f4xxAdcDeviceDeInit(struct gd32f4xxAdcDevice *gd32f4xx)
{
    AdcDevicePut(&gd32f4xx->device);
    AdcDeviceRemove(&gd32f4xx->device);
}

// wrap all the frees within gd32f4xxAdcDevice
static void freeMemOfAdcDevice(struct gd32f4xxAdcDevice *gd32f4xx)
{
    if (gd32f4xx) {
        // free channel conf
        if (gd32f4xx->AdcConfLst) {
            OsalMemFree(gd32f4xx->AdcConfLst);
        }
        // free itself
        OsalMemFree(gd32f4xx);
    }
}

// parsing the adc config hcs
static int32_t gd32f4xxAdcReadDrs(struct gd32f4xxAdcDevice *gd32f4xx, const struct DeviceResourceNode *node)
{
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL || drsOps->GetUint32ArrayElem == NULL) {
        HDF_LOGE("%s: invalid drs ops", __FUNCTION__);
        return HDF_ERR_NOT_SUPPORT;
    }

    int32_t ret = drsOps->GetUint32(node, "device_num", &gd32f4xx->deviceNum, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read deviceNum failed", __FUNCTION__);
        return;
    }

    ret = drsOps->GetUint16(node, "adc_type", &gd32f4xx->AdcType, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read AdcType failed", __FUNCTION__);
        return;
    }

    ret = drsOps->GetUint16(node, "rcutimer", &gd32f4xx->RcuTimer, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read RcuTimer failed", __FUNCTION__);
        return;
    }

    ret = drsOps->GetUint16(node, "adc_data_align", &gd32f4xx->AdcAlign, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read AdcAlign failed", __FUNCTION__);
        return;
    }

    ret = drsOps->GetUint16(node, "dma_type", &gd32f4xx->DmaType, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read DmaType failed", __FUNCTION__);
        return;
    }

    ret = drsOps->GetUint16(node, "dma_channel", &gd32f4xx->DmaChannel, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read DmaChannel failed", __FUNCTION__);
        return;
    }

    ret = drsOps->GetUint32(node, "adc_ext_trmode", &gd32f4xx->AdcExtTrMode, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read AdcExtTrMode failed", __FUNCTION__);
        return;
    }

    ret = drsOps->GetUint32(node, "adc_ext_trigger", &gd32f4xx->AdcExtTrigger, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read AdcExtTrigger failed", __FUNCTION__);
        return;
    }

    ret = drsOps->GetUint32(node, "adc_ch_sequence", &gd32f4xx->AdcSequence, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read AdcSequence failed", __FUNCTION__);
        return;
    }

    ret = drsOps->GetUint32(node, "mem_increas_mode", &gd32f4xx->MemIncreasMode, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read MemIncreasMode failed", __FUNCTION__);
        return;
    }

    ret = drsOps->GetUint32(node, "rcudma", &gd32f4xx->RcuDma, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read RcuDma failed", __FUNCTION__);
        return;
    }

    ret = drsOps->GetUint32(node, "adc_adcck", &gd32f4xx->AdcAdcck, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read AdcAdcck failed", __FUNCTION__);
        return;
    }

    ret = drsOps->GetUint32(node, "dma_increas_mode", &gd32f4xx->DmaIncreasMode, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read DmaIncreasMode failed", __FUNCTION__);
        return;
    }

    ret = drsOps->GetUint32(node, "dma_trans_width", &gd32f4xx->DmaTransWidth, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read DmaTransWidth failed", __FUNCTION__);
        return;
    }

    ret = drsOps->GetUint32(node, "dma_trans_mode", &gd32f4xx->DmaTransMode, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read DmaTransMode failed", __FUNCTION__);
        return;
    }

    ret = drsOps->GetUint32(node, "dma_ch_priority", &gd32f4xx->DmaChPriority, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read DmaChPriority failed", __FUNCTION__);
        return;
    }

    ret = drsOps->GetUint32(node, "dma_subperi", &gd32f4xx->DmaSubperi, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read DmaSubperi failed", __FUNCTION__);
        return;
    }

    ret = drsOps->GetUint32(node, "adc_sample_time", &gd32f4xx->AdcSampleTime, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read AdcSampleTime failed", __FUNCTION__);
        return;
    }

    ret = drsOps->GetUint16(node, "channel_count", &gd32f4xx->ChannelCount, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read ChannelCount failed", __FUNCTION__);
        return;
    }
    HDF_LOGI("%s: [line = %d] [ChannelCount = %d]\n", __func__, __LINE__, gd32f4xx->ChannelCount);

    uint16_t count = gd32f4xx->ChannelCount;

    if (count < 1 || count > ADC_CHANNEL_MAX) {
        HDF_LOGE("%s: illeage (%s) is out of range", __FUNCTION__, "channel_count");
        return ret;
    }

    // channel is the number to scan convert multi channel
    gd32f4xx->AdcConfLst = (AdcInitRes *)OsalMemCalloc(sizeof(AdcInitRes) * count);

    for (int index = 0; index < count; index++) {
        AdcInitRes *channelConf = gd32f4xx->AdcConfLst + index;
        if (drsOps->GetUint32ArrayElem(node, "adc_channel", index, &channelConf->adchannel, 0) != HDF_SUCCESS) {
            HDF_LOGE("%s: read adc_channel failed", __FUNCTION__);
            return HDF_FAILURE;
        }

        if (drsOps->GetUint32ArrayElem(node, "group", index, &channelConf->group, 0) != HDF_SUCCESS) {
            HDF_LOGE("%s: read group failed", __FUNCTION__);
            return HDF_FAILURE;
        }
        if (drsOps->GetUint32ArrayElem(node, "pin", index, &channelConf->pin, 0) != HDF_SUCCESS) {
            HDF_LOGE("%s: read pin failed", __FUNCTION__);
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}

void rcu_config(struct gd32f4xxAdcDevice *gd32f4xx)
{

#if DEBUG_ADC_LOG
    for (int idx = 0; idx < gd32f4xx->ChannelCount; idx++) {
        HDF_LOGI("%s: [line = %d] [Channel = %d]\n", __func__, __LINE__, (gd32f4xx->AdcConfLst+idx)->adchannel);
        HDF_LOGI("%s: [line = %d] [pin = %d]\n", __func__, __LINE__, (gd32f4xx->AdcConfLst+idx)->pin);
        HDF_LOGI("%s: [line = %d] [group = %d]\n", __func__, __LINE__, (gd32f4xx->AdcConfLst+idx)->group);
    }
#endif

    /* enable GPIOC clock */
    for (int idx = 0; idx < gd32f4xx->ChannelCount; idx++) {
        rcu_periph_clock_enable(g_rcuPeriphMaps[(gd32f4xx->AdcConfLst+idx)->group]);
    }
    /* enable ADC clock */
    rcu_periph_clock_enable(g_rcu_adc_Maps[gd32f4xx->AdcType]);
    /* enable timer1 clock */
    rcu_periph_clock_enable(g_rcu_timer_Maps[gd32f4xx->RcuTimer]);

    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
    /* enable DMA clock */
    rcu_periph_clock_enable(g_rcu_dma_Maps[gd32f4xx->RcuDma]);
    /* config ADC clock */
    adc_clock_config(SYNCCTL_ADCCK(gd32f4xx->AdcAdcck));
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(struct gd32f4xxAdcDevice *gd32f4xx)
{
    for (int idx = 0; idx < gd32f4xx->ChannelCount; idx++) {
        gpio_mode_set(g_gpioxMaps[(gd32f4xx->AdcConfLst+idx)->group], GPIO_MODE_ANALOG, GPIO_PUPD_NONE,\
        g_GdRealPinMaps[(gd32f4xx->AdcConfLst+idx)->pin]);
    }
}

void dma_config(struct gd32f4xxAdcDevice *gd32f4xx)
{

#if DEBUG_ADC_LOG
    HDF_LOGI("%s: [line = %d] [deviceNum = %d]\n", __func__, __LINE__, gd32f4xx->deviceNum);
#endif

    /* ADC_DMA_channel configuration */
    dma_single_data_parameter_struct dma_single_data_parameter;

    /* ADC DMA_channel configuration */
    dma_deinit(g_dma_type_Maps[gd32f4xx->DmaType], gd32f4xx->DmaChannel);

    /* initialize DMA single data mode */
    dma_single_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(g_adc_type_Maps[gd32f4xx->AdcType]));
    dma_single_data_parameter.periph_inc = gd32f4xx->DmaIncreasMode;
    dma_single_data_parameter.memory0_addr = (uint32_t)(adc_value[gd32f4xx->deviceNum]);
    dma_single_data_parameter.memory_inc = gd32f4xx->MemIncreasMode;
    dma_single_data_parameter.periph_memory_width = CHCTL_PWIDTH(gd32f4xx->DmaTransWidth);
    dma_single_data_parameter.direction = CHCTL_TM(gd32f4xx->DmaTransMode);
    dma_single_data_parameter.number = gd32f4xx->ChannelCount;
    dma_single_data_parameter.priority = CHCTL_PRIO(gd32f4xx->DmaChPriority);
    dma_single_data_mode_init(g_dma_type_Maps[gd32f4xx->DmaType], gd32f4xx->DmaChannel, &dma_single_data_parameter);
    dma_channel_subperipheral_select(g_dma_type_Maps[gd32f4xx->DmaType], gd32f4xx->DmaChannel, gd32f4xx->DmaSubperi);

    /* enable DMA circulation mode */
    dma_circulation_enable(g_dma_type_Maps[gd32f4xx->DmaType], gd32f4xx->DmaChannel);

    /* enable DMA channel */
    dma_channel_enable(g_dma_type_Maps[gd32f4xx->DmaType], gd32f4xx->DmaChannel);
}

void adc_config(struct gd32f4xxAdcDevice *gd32f4xx)
{
    /* ADC mode config */
    adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
    /* ADC contineous function set */
    adc_special_function_config(g_adc_type_Maps[gd32f4xx->AdcType], ADC_CONTINUOUS_MODE | ADC_SCAN_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(g_adc_type_Maps[gd32f4xx->AdcType], g_adc_dataalign_Maps[gd32f4xx->AdcAlign]);

    /* ADC channel length config */
    adc_channel_length_config(g_adc_type_Maps[gd32f4xx->AdcType],\
            g_adc_sequence_Maps[gd32f4xx->AdcSequence], gd32f4xx->ChannelCount);

    /* ADC routine channel config */
    for (int idx = 0; idx < gd32f4xx->ChannelCount; idx++) {
        adc_routine_channel_config(g_adc_type_Maps[gd32f4xx->AdcType], idx,\
                g_adc_channel_Maps[(gd32f4xx->AdcConfLst+idx)->adchannel], SAMPTX_SPT(gd32f4xx->AdcSampleTime));
    }

    /* ADC trigger config */
    adc_external_trigger_source_config(g_adc_type_Maps[gd32f4xx->AdcType],\
            g_adc_sequence_Maps[gd32f4xx->AdcSequence], g_adc_ext_trigger_Maps[gd32f4xx->AdcExtTrigger]);

    adc_external_trigger_config(g_adc_type_Maps[gd32f4xx->AdcType], g_adc_sequence_Maps[gd32f4xx->AdcSequence],\
            g_adc_ext_trigger_mode_Maps[gd32f4xx->AdcExtTrMode]);

    /* ADC DMA function enable */
    adc_dma_request_after_last_enable(g_adc_type_Maps[gd32f4xx->AdcType]);
    adc_dma_mode_enable(g_adc_type_Maps[gd32f4xx->AdcType]);

    /* enable ADC interface */
    adc_enable(g_adc_type_Maps[gd32f4xx->AdcType]);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(g_adc_type_Maps[gd32f4xx->AdcType]);

    /* enable ADC software trigger */
    adc_software_trigger_enable(g_adc_type_Maps[gd32f4xx->AdcType], g_adc_sequence_Maps[gd32f4xx->AdcSequence]);
}

static void AdcConfig(struct gd32f4xxAdcDevice *gd32f4xx)
{
    /* system clocks configuration */
    rcu_config(gd32f4xx);
    /* GPIO configuration */
    gpio_config(gd32f4xx);
    /* DMA configuration */
    dma_config(gd32f4xx);
    /* ADC configuration */
    adc_config(gd32f4xx);
}

// initialize a ADC node retrieved from hcs
static int32_t gd32f4xxAdcNodeInit(struct HdfDeviceObject *device, struct DeviceResourceNode *node)
{
    int32_t ret;
    struct gd32f4xxAdcDevice *gd32f4xx = NULL;

    (void)device;
    gd32f4xx = (struct gd32f4xxAdcDevice *)OsalMemCalloc(sizeof(struct gd32f4xxAdcDevice));
    if (gd32f4xx == NULL) {
        HDF_LOGE("%s: alloc gd32f4xx failed", __FUNCTION__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = gd32f4xxAdcReadDrs(gd32f4xx, node);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read drs failed:%d", __FUNCTION__, ret);
        goto __ERR__;
    }

    /* Init adc gpio rcu dma */
    AdcConfig(gd32f4xx);

    ret = gd32f4xxAdcDeviceInit(gd32f4xx, node);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: init adc device_num:%u failed", __FUNCTION__, gd32f4xx->deviceNum);
        goto __ERR__;
    }
    return HDF_SUCCESS;

__ERR__:
    if (gd32f4xx != NULL) {
        freeMemOfAdcDevice(gd32f4xx);
    }
    return ret;
}

// de initialize the ADC node from hcs
static void gd32f4xxAdcRemoveByNode(const struct DeviceResourceNode *node)
{
    int32_t ret;
    uint32_t deviceNum;
    struct AdcDevice *device = NULL;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid drs ops", __FUNCTION__);
        return;
    }

    ret = drsOps->GetUint32(node, "device_num", &deviceNum, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read deviceNum failed", __FUNCTION__);
        return;
    }

    device = AdcDeviceGet(deviceNum);
    if (device != NULL && device->priv == node) {
        gd32f4xxAdcDeviceDeInit((struct gd32f4xxAdcDevice *)device);
        freeMemOfAdcDevice((struct gd32f4xxAdcDevice *)device);
    } else {
        HDF_LOGE("%s: AdcDeviceGet(%d)->NULL", __FUNCTION__, deviceNum);
    }

    return;
}

// common initialization before all the adc device init
static int32_t AdcCommonInit(struct HdfDeviceObject *device) {
    int32_t ret = HDF_SUCCESS;
    uint32_t out;
    struct DeviceResourceIface *drsOps = NULL;

    return ret;
}

// common deinit called by AdcRelease
static int32_t AdcCommonDeInit(struct HdfDeviceObject *device)
{
    int32_t ret = HDF_SUCCESS;

    (void)device;

    return ret;
}

// Adc device init foundation
static int32_t AdcInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct DeviceResourceNode *childNode = NULL;
    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or property is null", __FUNCTION__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = AdcCommonInit(device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: AdcCommonInit failed(%d)", __FUNCTION__, ret);
        return HDF_ERR_NOT_SUPPORT;
    }

    DEV_RES_NODE_FOR_EACH_CHILD_NODE(device->property, childNode) {
        ret = gd32f4xxAdcNodeInit(device, childNode);
        if (ret != HDF_SUCCESS) {
            break;
        }
    }

    HDF_LOGD("%s is over, ret=%d\n\n", __FUNCTION__, ret);
    return ret;
}

// Adc device release foundation
static void AdcRelease(struct HdfDeviceObject *device)
{
    const struct DeviceResourceNode *childNode = NULL;
    HDF_LOGD("%s: enter", __FUNCTION__);

    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or property is null", __FUNCTION__);
        return;
    }
    DEV_RES_NODE_FOR_EACH_CHILD_NODE(device->property, childNode) {
        gd32f4xxAdcRemoveByNode(childNode);
    }

    AdcCommonDeInit(device);
}

// dummy, otherwise init fails
static int32_t AdcBind(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

// register of HDF drivers
static struct HdfDriverEntry g_adcDriverEntry = {
    .moduleVersion = 1,
    .Bind = AdcBind,
    .Init = AdcInit,
    .Release = AdcRelease,
    .moduleName = "HDF_PLATFORM_ADC_DRIVER",
};
HDF_INIT(g_adcDriverEntry);

/*
 * Copyright (c) 2022 Hihope Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#include <stdlib.h>
#include <string.h>
#include "gd32_qspi.h"
#include "osal_mutex.h"
#include "osal_sem.h"
#include "spi_core.h"
#include "device_resource_if.h"
#include "hdf_log.h"
#include "hal_gpio.h"
#include "osal_mem.h"

QspiDevice *qspiDevice = NULL;

uint8_t *qspi_csnum_Buffer = NULL;

uint8_t qspi_flash_send_byte(uint32_t spi_periph, uint8_t byte)
{
    /* loop while data register in not empty */
    while(RESET == spi_i2s_flag_get(g_qspi_BaseMaps[spi_periph],SPI_FLAG_TBE));

    /* send byte through the SPI peripheral */
    spi_i2s_data_transmit(g_qspi_BaseMaps[spi_periph],byte);

    /* wait to receive a byte */
    while(RESET == spi_i2s_flag_get(g_qspi_BaseMaps[spi_periph],SPI_FLAG_RBNE));

    /* return the byte read from the SPI bus */
    return(spi_i2s_data_receive(g_qspi_BaseMaps[spi_periph]));
}

static int32_t HalQspiSend(uint32_t spiCtrl, const uint8_t *data, uint32_t size)
{
    if (spiCtrl == NULL || data == NULL || size == 0) {
        HDF_LOGE("%s: parameters are invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    spi_quad_enable(g_qspi_BaseMaps[spiCtrl]);
    spi_quad_write_enable(g_qspi_BaseMaps[spiCtrl]);

    while (size--) {
        qspi_flash_send_byte(spiCtrl, *data);
        data++;
    }

    spi_quad_disable(g_qspi_BaseMaps[spiCtrl]);

    return HDF_SUCCESS;
}

static int32_t HalQspiRecv(uint32_t spiCtrl, uint8_t *data, uint32_t size)
{
    if (spiCtrl == NULL || data == NULL || size == 0) {
        HDF_LOGE("%s: parameters are invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    uint8_t *cmd = (uint8_t *)OsalMemAlloc(size);

    if (cmd == NULL) {
        HDF_LOGE("%s OsalMemAlloc size(%ld) error", __FUNCTION__, size);
        return HDF_ERR_MALLOC_FAIL;
    }

    (void)memset_s(cmd, size, DUMMY_BYTE, size);

    spi_quad_enable(g_qspi_BaseMaps[spiCtrl]);
    spi_quad_read_enable(g_qspi_BaseMaps[spiCtrl]);

    qspi_flash_send_byte(spiCtrl, DUMMY_BYTE);
    qspi_flash_send_byte(spiCtrl, DUMMY_BYTE);
    qspi_flash_send_byte(spiCtrl, DUMMY_BYTE);
    qspi_flash_send_byte(spiCtrl, DUMMY_BYTE);

    uint8_t *cmd1 = cmd;

    while (size--) {
        *data  = qspi_flash_send_byte(spiCtrl, *cmd);
        data++;
        cmd++;
    }

    OsalMemFree(cmd1);
    spi_quad_disable(g_qspi_BaseMaps[spiCtrl]);

    return HDF_SUCCESS;
}

/* note: the size of rxData and txData should be same as the transmitSize */
static int32_t HalQspiSendRecv(uint32_t spiCtrl, uint8_t *txData,
    uint8_t *rxData, uint32_t transmitSize)
{
    if (spiCtrl == NULL || txData == NULL || rxData == NULL) {
        HDF_LOGE("%s: parameters are invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    while (transmitSize--) {
        *rxData = qspi_flash_send_byte(spiCtrl, *txData);
        rxData++;
        txData++;
    }

    return HDF_SUCCESS;
}

static int32_t GD32QspiTransfer(struct SpiCntlr *spiCntlr, struct SpiMsg *spiMsg, uint32_t count)
{
    if (spiCntlr == NULL || spiCntlr->priv == NULL) {
        HDF_LOGE("%s: spiCntlr is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    struct SpiMsg *msg = NULL;

    QspiDevice *qspiDevice = (QspiDevice *)spiCntlr->priv;
    QspiUartSet *spiUartSet = &qspiDevice->spiUartSet;
    QspiSource *spiSource = &qspiDevice->spiSource;
    if(spiUartSet == NULL || spiSource == NULL) {
        HDF_LOGE("%s: spiUartSet or spiSource is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    /* curCs is specified by app through spiOpen() */
    if (spiCntlr->curCs >= spiCntlr->numCs) {
        HDF_LOGE("%s: current Cs is out of range", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    for (size_t i = 0; i < count; i++) {
        msg = &spiMsg[i];

        /* pull down the csGpio anyway */
        GpioWrite(qspi_csnum_Buffer[spiCntlr->curCs], 0);

        /* only write */
        if ((msg->rbuf == NULL) && (msg->wbuf != NULL)) {
            HalQspiSend(spiCntlr->busNum, msg->wbuf, msg->len);
        }

        /* only read */
        if ((msg->rbuf != NULL) && (msg->wbuf == NULL)) {
            HalQspiRecv(spiCntlr->busNum, msg->rbuf, msg->len);
        }

        /* bi-direction */
        if ((msg->wbuf != NULL) && (msg->rbuf != NULL)) {
            HalQspiSendRecv(spiCntlr->busNum, msg->wbuf, msg->rbuf, msg->len);
        }

        /* if caller want to keep cs, just set csChange=0 */
        if (msg->keepCs == 1) {
            GpioWrite(qspi_csnum_Buffer[spiCntlr->curCs], 1);
        }

        if (msg->delayUs > 0) {
            /* convert us to ms, then delay */
            osDelay(msg->delayUs / 1000 );
        }
    }

    return HDF_SUCCESS;
}

static int32_t GD32QspiOpen(struct SpiCntlr *cntlr)
{
    QspiDevice *qspiDevice = (QspiDevice *)cntlr->priv;
    QspiUartSet *spiUartSet = &qspiDevice->spiUartSet;
    QspiSource *spiSource = &qspiDevice->spiSource;
    if(spiUartSet == NULL || spiSource == NULL) {
        HDF_LOGE("%s: spiUartSet or spiSource is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDF_LOGI("%s: [line = %d] [spiId = %d] [transMode = %d]\n", __func__, __LINE__, qspiDevice->spiId, spiSource->transMode);

    spi_parameter_struct spi_init_struct;

    rcu_periph_clock_enable(g_gpioxMaps[spiUartSet->spiSckPort]);
    rcu_periph_clock_enable(g_gpioxMaps[spiUartSet->spiMosiPort]);
    rcu_periph_clock_enable(g_gpioxMaps[spiUartSet->spiMisoPort]);
    rcu_periph_clock_enable(g_qspi_rcuPeriphMaps[qspiDevice->spiId]);

    /* spi sck set */
    gpio_af_set(g_gpioxMaps[spiUartSet->spiSckPort], GPIO_AF_5, g_GdRealPinMaps[spiUartSet->spiSckPin]);
    gpio_mode_set(g_gpioxMaps[spiUartSet->spiSckPort], GPIO_MODE_AF, GPIO_PUPD_NONE, g_GdRealPinMaps[spiUartSet->spiSckPin]);
    gpio_output_options_set(g_gpioxMaps[spiUartSet->spiSckPort], GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, g_GdRealPinMaps[spiUartSet->spiSckPin]);

    /* spi mosi set */
    gpio_af_set(g_gpioxMaps[spiUartSet->spiMosiPort], GPIO_AF_5, g_GdRealPinMaps[spiUartSet->spiMosiPin]);
    gpio_mode_set(g_gpioxMaps[spiUartSet->spiMosiPort], GPIO_MODE_AF, GPIO_PUPD_NONE, g_GdRealPinMaps[spiUartSet->spiMosiPin]);
    gpio_output_options_set(g_gpioxMaps[spiUartSet->spiMosiPort], GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, g_GdRealPinMaps[spiUartSet->spiMosiPin]);

    /* spi miso set */
    gpio_af_set(g_gpioxMaps[spiUartSet->spiMisoPort], GPIO_AF_5, g_GdRealPinMaps[spiUartSet->spiMisoPin]);
    gpio_mode_set(g_gpioxMaps[spiUartSet->spiMisoPort], GPIO_MODE_AF, GPIO_PUPD_NONE, g_GdRealPinMaps[spiUartSet->spiMisoPin]);
    gpio_output_options_set(g_gpioxMaps[spiUartSet->spiMisoPort], GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, g_GdRealPinMaps[spiUartSet->spiMisoPin]);

    /* spi IO2 set */
    gpio_af_set(g_gpioxMaps[spiUartSet->spiIO2Port], GPIO_AF_5, g_GdRealPinMaps[spiUartSet->spiIO2Pin]);
    gpio_mode_set(g_gpioxMaps[spiUartSet->spiIO2Port], GPIO_MODE_AF, GPIO_PUPD_NONE, g_GdRealPinMaps[spiUartSet->spiIO2Pin]);
    gpio_output_options_set(g_gpioxMaps[spiUartSet->spiIO2Port], GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, g_GdRealPinMaps[spiUartSet->spiIO2Pin]);

    /* spi IO2 set */
    gpio_af_set(g_gpioxMaps[spiUartSet->spiIO3Port], GPIO_AF_5, g_GdRealPinMaps[spiUartSet->spiIO3Pin]);
    gpio_mode_set(g_gpioxMaps[spiUartSet->spiIO3Port], GPIO_MODE_AF, GPIO_PUPD_NONE, g_GdRealPinMaps[spiUartSet->spiIO3Pin]);
    gpio_output_options_set(g_gpioxMaps[spiUartSet->spiIO3Port], GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, g_GdRealPinMaps[spiUartSet->spiIO3Pin]);

    GpioWrite(qspi_csnum_Buffer[cntlr->curCs], 1);

    /* SPI parameter config */
    spi_init_struct.trans_mode           = g_qspitransmode_BaseMaps[spiSource->transMode];
    spi_init_struct.device_mode          = g_qspimode_BaseMaps[spiSource->masterSlave];
    spi_init_struct.frame_size           = g_qspiframesize_BaseMaps[spiSource->framesize];
    spi_init_struct.clock_polarity_phase = g_qspiclkpolarity_BaseMaps[spiSource->clkpolarity];
    spi_init_struct.nss                  = g_qspinssmode_BaseMaps[spiSource->nssmode];
    spi_init_struct.prescale             = g_qspiclkprescale_BaseMaps[spiSource->prescale];
    spi_init_struct.endian               = g_qspiendian_BaseMaps[spiSource->endian];
    spi_init(g_qspi_BaseMaps[qspiDevice->spiId], &spi_init_struct);

    /* quad wire SPI_IO2 and SPI_IO3 pin output enable */
    spi_quad_io23_output_enable(g_qspi_BaseMaps[qspiDevice->spiId]);

    /* enable SPI */
    spi_enable(g_qspi_BaseMaps[qspiDevice->spiId]);

    return HDF_SUCCESS;
}

static int32_t GD32QspiClose(struct SpiCntlr *cntlr)
{
    QspiDevice *qspiDevice = (QspiDevice *)cntlr->priv;

    if(qspiDevice == NULL) {
        HDF_LOGE("%s: qspiDevice is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    OsalMemFree(qspi_csnum_Buffer);
    spi_i2s_deinit(g_qspi_BaseMaps[qspiDevice->spiId]);
    spi_enable(g_qspi_BaseMaps[qspiDevice->spiId]);

    return HDF_SUCCESS;
}

struct SpiCntlrMethod g_QspiCntlrMethod = {
    .Transfer = GD32QspiTransfer,
    .SetCfg = NULL,
    .GetCfg = NULL,
    .Open = GD32QspiOpen,
    .Close = GD32QspiClose,
};

static int32_t GetQspiDeviceResource(QspiDevice *qspiDevice, const struct DeviceResourceNode *resourceNode, struct SpiCntlr *spiCntlr)
{
    uint32_t propVal;
    struct DeviceResourceIface *dri = NULL;

    if (qspiDevice == NULL || resourceNode == NULL) {
        HDF_LOGE("%s: PARAM is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    QspiUartSet *spiUartSet = &qspiDevice->spiUartSet;
    QspiSource *spiSource = &qspiDevice->spiSource;
    if(spiUartSet == NULL || spiSource == NULL) {
        HDF_LOGE("%s: spiUartSet or spiSource is NULL\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    dri = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (dri == NULL || dri->GetUint16 == NULL || dri->GetUint8 == NULL || dri->GetUint32 == NULL) {
        HDF_LOGE("DeviceResourceIface is invalid\r\n");
        return HDF_ERR_INVALID_PARAM;
    }

    if (dri->GetUint32(resourceNode, "busNum", &qspiDevice->spiId, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "busNum");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: qspiDevice->spiId %d", __func__, qspiDevice->spiId);

    if (dri->GetUint32(resourceNode, "TransMode", &spiSource->transMode, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "TransMode");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: spiSource->transMode %d", __func__, spiSource->transMode);

    if (dri->GetUint32(resourceNode, "MasterSlave", &spiSource->masterSlave, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "MasterSlave");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: spiSource->masterSlave %d", __func__, spiSource->masterSlave);

    if (dri->GetUint32(resourceNode, "Framesize", &spiSource->framesize, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "Framesize");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: spiSource->framesize %d", __func__, spiSource->framesize);

    if (dri->GetUint32(resourceNode, "ClkPolarity", &spiSource->clkpolarity, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "ClkPolarity");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: spiSource->clkpolarity %d", __func__, spiSource->clkpolarity);

    if (dri->GetUint32(resourceNode, "Nssmode", &spiSource->nssmode, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "Nssmode");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: spiSource->nssmode %d", __func__, spiSource->nssmode);

    if (dri->GetUint32(resourceNode, "Prescale", &spiSource->prescale, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "Prescale");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: spiSource->prescale %d", __func__, spiSource->prescale);

    if (dri->GetUint32(resourceNode, "Endian", &spiSource->endian, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "Endian");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: spiSource->endian %d", __func__, spiSource->endian);

    if (dri->GetUint8(resourceNode, "SpiCSPort", &spiUartSet->spiCSPort, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "SpiCSPort");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: spiUartSet->spiCSPort %d", __func__, spiUartSet->spiCSPort);

    if (dri->GetUint16(resourceNode, "SpiCSPin", &spiUartSet->spiCSPin, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "SpiCSPin");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: spiUartSet->spiCSPin %d", __func__, spiUartSet->spiCSPin);

    if (dri->GetUint8(resourceNode, "SpiSckPort", &spiUartSet->spiSckPort, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "SpiSckPort");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: spiUartSet->spiSckPort %d", __func__, spiUartSet->spiSckPort);

    if (dri->GetUint16(resourceNode, "SpiSckPin", &spiUartSet->spiSckPin, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "SpiSckPin");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: spiUartSet->spiSckPin %d", __func__, spiUartSet->spiSckPin);

    if (dri->GetUint8(resourceNode, "SpiMosiPort", &spiUartSet->spiMosiPort, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "SpiMosiPort");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: spiUartSet->spiMosiPort %d", __func__, spiUartSet->spiMosiPort);

    if (dri->GetUint16(resourceNode, "SpiMosiPin", &spiUartSet->spiMosiPin, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "SpiMosiPin");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: spiUartSet->spiMosiPin %d", __func__, spiUartSet->spiMosiPin);

    if (dri->GetUint8(resourceNode, "SpiMisoPort", &spiUartSet->spiMisoPort, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "SpiMisoPort");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: spiUartSet->spiMisoPort %d", __func__, spiUartSet->spiMisoPort);

    if (dri->GetUint16(resourceNode, "SpiMisoPin", &spiUartSet->spiMisoPin, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "SpiMisoPin");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: spiUartSet->spiMisoPin %d", __func__, spiUartSet->spiMisoPin);

    if (dri->GetUint8(resourceNode, "SpiIO2Port", &spiUartSet->spiIO2Port, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "SpiIO2Port");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: spiUartSet->spiIO2Port %d", __func__, spiUartSet->spiIO2Port);

    if (dri->GetUint16(resourceNode, "SPiIO2pin", &spiUartSet->spiIO2Pin, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "SPiIO2pin");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: spiUartSet->spiIO2Pin %d", __func__, spiUartSet->spiIO2Pin);

    if (dri->GetUint8(resourceNode, "SpiIO3Port", &spiUartSet->spiIO3Port, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "SpiIO3Port");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: spiUartSet->spiIO3Port %d", __func__, spiUartSet->spiIO3Port);

    if (dri->GetUint16(resourceNode, "SPiIO3pin", &spiUartSet->spiIO3Pin, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "SPiIO3pin");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: spiUartSet->spiIO3Pin %d", __func__, spiUartSet->spiIO3Pin);

    if (dri->GetUint32(resourceNode, "Csnum", &qspiDevice->csnum, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "Csnum");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: qspiDevice->csnum %d", __func__, qspiDevice->csnum);

    qspi_csnum_Buffer = (uint8_t*)OsalMemAlloc(qspiDevice->csnum);

    for (int index = 0; index < qspiDevice->csnum; index ++) {
        if (dri->GetUint32ArrayElem(resourceNode, "cs_gpio_lst", index, &propVal, 0) != HDF_SUCCESS) {
            HDF_LOGE("%s: get cs no(%d) failed", index);
            return HDF_FAILURE;
        }
        GpioSetDir(propVal, 1);
        qspi_csnum_Buffer[index] = propVal;
    }

    return HDF_SUCCESS;
}

static int32_t AttachQspiDevice(struct SpiCntlr *spiCntlr, const struct HdfDeviceObject *device)
{
    int32_t ret;

    if (spiCntlr == NULL || device == NULL || device->property == NULL) {
        HDF_LOGE("%s: property is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    qspiDevice = (QspiDevice *)OsalMemAlloc(sizeof(QspiDevice));
    if (qspiDevice == NULL) {
        HDF_LOGE("%s: OsalMemAlloc qspiDevice error\r\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = GetQspiDeviceResource(qspiDevice, device->property, spiCntlr);
    if (ret != HDF_SUCCESS) {
        (void)OsalMemFree(qspiDevice);
        return HDF_FAILURE;
    }

    spiCntlr->priv = qspiDevice;
    spiCntlr->busNum = qspiDevice->spiId;
    spiCntlr->numCs = qspiDevice->csnum;

    return HDF_SUCCESS;
}

static int32_t HdfQspiDeviceBind(struct HdfDeviceObject *device)
{
    HDF_LOGI("%s: entry", __func__);
    if (device == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return (SpiCntlrCreate(device) == NULL) ? HDF_FAILURE : HDF_SUCCESS;
}

static int32_t HdfQspiDeviceInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct SpiCntlr *spiCntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDF_LOGI("Enter %s:", __func__);
    spiCntlr = SpiCntlrFromDevice(device);
    if (spiCntlr == NULL) {
        HDF_LOGE("%s: spiCntlr is NULL", __func__);
        return HDF_DEV_ERR_NO_DEVICE;
    }

    ret = AttachQspiDevice(spiCntlr, device); // SpiCntlr add TWQspiDevice to priv
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: attach error\r\n", __func__);
        return HDF_DEV_ERR_ATTACHDEV_FAIL;
    }

    spiCntlr->method = &g_QspiCntlrMethod; // register callback

    HDF_LOGI("%s: dev service:%s init success!", __func__, HdfDeviceGetServiceName(device));
    return ret;
}

static void HdfQspiDeviceRelease(struct HdfDeviceObject *device)
{
    struct SpiCntlr *spiCntlr = NULL;
    QspiDevice *qspiDevice = NULL;

    HDF_LOGI("%s: entry", __func__);
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return;
    }
    spiCntlr = SpiCntlrFromDevice(device);
    if (spiCntlr == NULL || spiCntlr->priv == NULL) {
        HDF_LOGE("%s: spiCntlr is NULL\r\n", __func__);
        return;
    }

    qspiDevice = (QspiDevice *)spiCntlr->priv;

    OsalMemFree(qspiDevice);
}

struct HdfDriverEntry g_hdfQspiDevice = {
    .moduleVersion = 1,
    .moduleName = "HDF_PLATFORM_QSPI",
    .Bind = HdfQspiDeviceBind,
    .Init = HdfQspiDeviceInit,
    .Release = HdfQspiDeviceRelease,
};

HDF_INIT(g_hdfQspiDevice);
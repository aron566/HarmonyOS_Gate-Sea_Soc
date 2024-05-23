/*
 * Copyright (c) 2022 Hihope Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <stdlib.h>
#include <string.h>
#include "device_resource_if.h"
#include "hdf_log.h"
#include "los_task.h"
#include "hal_uart.h"
#include "hal_gpio.h"
#include "data_buffer.h"

#define HDF_LOG_TAG uartDev

#define ARRAYNUM(arr_name)     (uint32_t)(sizeof(arr_name)/sizeof(*(arr_name)))

#define UART_DMA_VERIFY(uartDma)          \
    if (uartDma == NULL) {\
        HDF_LOGE("%s: uartDma is NULL", __func__);\
        return HDF_ERR_INVALID_PARAM;\
    }

#define UART_HOST_VERIFY(host)          \
    if (host == NULL || host->priv == NULL) {\
        HDF_LOGE("%s: host invalid parameter", __func__);\
        return HDF_ERR_INVALID_PARAM;\
    }

#define GET_RESOURCE_BY_HOST(host, resource) \
    do { \
        UART_HOST_VERIFY(host); \
        resource = (UartResource *)host->priv; \
        if (resource == NULL) { \
            HDF_LOGE("%s: uart_%d resource is INVALID", __func__, resource->num); \
            return HDF_ERR_INVALID_OBJECT; \
        } \
     } while(0);

#define RESOURCE_ENABLE_VERIFY(resource) \
    do { \
        if (!resource->enable) { \
            HDF_LOGE("%s, whose uart_%d is disabled", __func__, resource->num); \
            return HDF_ERR_INVALID_OBJECT; \
        } \
    } while(0);

#define UART_NUM 8

typedef struct {
    uint8_t enable;
    uint8_t num;
    uint32_t baudval;
    uartPinCfg uartPin;
    uartDmaCfg uartDma;
} UartResource;

typedef struct {
    HWI_PROC_FUNC isr;
    struct buffer_t *data_Buf;
    uint8_t uart_num;
    uartDmaCfg *uartDma;
} UartInst;

void USART0_IRQHandler()
{
    USART_DMA_IRQHandler(GD32_USART0);
}

void USART1_IRQHandler()
{
    USART_DMA_IRQHandler(GD32_USART1);
}

void USART2_IRQHandler()
{
    USART_DMA_IRQHandler(GD32_USART2);
}

void USART3_IRQHandler()
{
    USART_DMA_IRQHandler(GD32_UART3);
}

void USART4_IRQHandler()
{
    USART_DMA_IRQHandler(GD32_UART4);
}

void UART5_IRQHandler()
{
    USART_DMA_IRQHandler(GD32_USART5);
}

void USART6_IRQHandler()
{
    USART_DMA_IRQHandler(GD32_UART6);
}

void USART7_IRQHandler()
{
    USART_DMA_IRQHandler(GD32_UART7);
}

static UartInst g_UartInstTbl[UART_NUM] = {
    {
        .isr = USART0_IRQHandler,
        .data_Buf = NULL,
        .uartDma = NULL,
    },
    {
        .isr = USART1_IRQHandler,
        .data_Buf = NULL,
        .uartDma = NULL,
    },
    {
        .isr = USART2_IRQHandler,
        .data_Buf = NULL,
        .uartDma = NULL,
    },
    {
        .isr = USART3_IRQHandler,
        .data_Buf = NULL,
        .uartDma = NULL,
    },
    {
        .isr = USART4_IRQHandler,
        .data_Buf = NULL,
        .uartDma = NULL,
    },
    {
        .isr = UART5_IRQHandler,
        .data_Buf = NULL,
        .uartDma = NULL,
    },
    {
        .isr = USART6_IRQHandler,
        .data_Buf = NULL,
        .uartDma = NULL,
    },
    {
        .isr = USART7_IRQHandler,
        .data_Buf = NULL,
        .uartDma = NULL,
    }
};

UartResource *resourceInit = NULL;
static uint8_t rx_buffer[DATA_BUF_SIZE];
static uint32_t USART_RX_NUM = 0;

void USART_DMA_IRQHandler(uint8_t num)
{
    dma_single_data_parameter_struct dma_init_struct;
    uartDmaCfg *uartDma = g_UartInstTbl[num].uartDma;
    UART_DMA_VERIFY(uartDma)
    if(RESET != usart_interrupt_flag_get(g_UartPeriph[num], USART_INT_FLAG_IDLE))
    {
        usart_interrupt_flag_clear(g_UartPeriph[num],USART_INT_FLAG_IDLE);
        usart_data_receive(g_UartPeriph[num]);
        dma_channel_disable(g_UartDmaFuncMaps[uartDma->rx_DMA], g_UartDmaChMaps[uartDma->rx_DmaCh]);

        USART_RX_NUM = sizeof(rx_buffer) - dma_transfer_number_get(g_UartDmaFuncMaps[uartDma->rx_DMA],
                                                                   g_UartDmaChMaps[uartDma->rx_DmaCh]);
        HDF_LOGI("RECV %d, dma get num %d, date:%s\r\n", USART_RX_NUM, dma_transfer_number_get(g_UartDmaFuncMaps[uartDma->rx_DMA],g_UartDmaChMaps[uartDma->rx_DmaCh]),
                rx_buffer);
        buff_write(g_UartInstTbl[num].data_Buf, &rx_buffer, USART_RX_NUM);
        memset(&rx_buffer ,'\0',sizeof(rx_buffer));
        usart_dma_rx_config(g_UartPeriph[num], &dma_init_struct, uartDma);
        /* configure DMA mode */
        dma_channel_enable(g_UartDmaFuncMaps[uartDma->rx_DMA], g_UartDmaChMaps[uartDma->rx_DmaCh]);
        usart_dma_receive_config(g_UartPeriph[num], USART_RECEIVE_DMA_ENABLE);
    }
}

void usart_dma_rx_config(uint32_t com, dma_single_data_parameter_struct *dma_rx_init_struct, uartDmaCfg *uartDma)
{
    dma_deinit(g_UartDmaFuncMaps[uartDma->rx_DMA], g_UartDmaChMaps[uartDma->rx_DmaCh]);
    dma_rx_init_struct->direction = DMA_PERIPH_TO_MEMORY;
    dma_rx_init_struct->memory0_addr = (uint32_t)rx_buffer;
    dma_rx_init_struct->memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_rx_init_struct->periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_rx_init_struct->number = ARRAYNUM(rx_buffer);
    dma_rx_init_struct->periph_addr = ((uint32_t)&USART_DATA(com));
    dma_rx_init_struct->periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_rx_init_struct->priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(g_UartDmaFuncMaps[uartDma->rx_DMA], 
                              g_UartDmaChMaps[uartDma->rx_DmaCh], dma_rx_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(g_UartDmaFuncMaps[uartDma->rx_DMA], g_UartDmaChMaps[uartDma->rx_DmaCh]);
    dma_channel_subperipheral_select(g_UartDmaFuncMaps[uartDma->rx_DMA],
                                     g_UartDmaChMaps[uartDma->rx_DmaCh], uartDma->rx_DmaPerSel);

    dma_channel_enable(g_UartDmaFuncMaps[uartDma->rx_DMA], g_UartDmaChMaps[uartDma->rx_DmaCh]);
}

static void usart_dma_tx_config(uint32_t com, dma_single_data_parameter_struct *dma_tx_init_struct, uint8_t *data, 
                                uint32_t number, uartDmaCfg *uartDma)
{
    dma_deinit(g_UartDmaFuncMaps[uartDma->tx_DMA], g_UartDmaChMaps[uartDma->tx_DmaCh]);
    dma_tx_init_struct->direction = DMA_MEMORY_TO_PERIPH;
    dma_tx_init_struct->memory0_addr = (uint32_t)data;
    dma_tx_init_struct->memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_tx_init_struct->periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_tx_init_struct->number = number;
    dma_tx_init_struct->periph_addr = ((uint32_t)&USART_DATA(com));
    dma_tx_init_struct->periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_tx_init_struct->priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(g_UartDmaFuncMaps[uartDma->tx_DMA], 
                              g_UartDmaChMaps[uartDma->tx_DmaCh], dma_tx_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(g_UartDmaFuncMaps[uartDma->tx_DMA], 
                            g_UartDmaChMaps[uartDma->tx_DmaCh]);
    dma_channel_subperipheral_select(g_UartDmaFuncMaps[uartDma->tx_DMA], 
                                     g_UartDmaChMaps[uartDma->tx_DmaCh], uartDma->tx_DmaPerSel);

    dma_channel_enable(g_UartDmaFuncMaps[uartDma->tx_DMA], g_UartDmaChMaps[uartDma->tx_DmaCh]);
}

void com_init_cfg(uint32_t com, UartResource *resource)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(g_rcuPeriphMaps[resource->uartPin.rxPort]);
    rcu_periph_clock_enable(g_rcuPeriphMaps[resource->uartPin.txPort]);

    /* enable USART clock */
    rcu_periph_clock_enable(g_UartPeriphcLOCK[resource->num]);

    /* connect port to USARTx_Tx */
    gpio_af_set(g_gpioxMaps[resource->uartPin.txPort], g_gpioAFMap[resource->uartPin.tx_AF],
                g_GdRealPinMaps[resource->uartPin.txPin]);

    /* connect port to USARTx_Rx */
    gpio_af_set(g_gpioxMaps[resource->uartPin.rxPort], g_gpioAFMap[resource->uartPin.rx_AF],
                g_GdRealPinMaps[resource->uartPin.rxPin]);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(g_gpioxMaps[resource->uartPin.txPort], GPIO_MODE_AF, GPIO_PUPD_PULLUP,
                  g_GdRealPinMaps[resource->uartPin.txPin]);
    gpio_output_options_set(g_gpioxMaps[resource->uartPin.txPort], GPIO_OTYPE_PP,
                            GPIO_OSPEED_50MHZ, g_GdRealPinMaps[resource->uartPin.txPin]);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(g_gpioxMaps[resource->uartPin.rxPort], GPIO_MODE_AF, GPIO_PUPD_PULLUP,
                  g_GdRealPinMaps[resource->uartPin.rxPin]);
    gpio_output_options_set(g_gpioxMaps[resource->uartPin.rxPort], GPIO_OTYPE_PP,
                            GPIO_OSPEED_50MHZ, g_GdRealPinMaps[resource->uartPin.rxPin]);

    /* USART configure */
    usart_deinit(com);
    usart_baudrate_set(com, resource->baudval);
    usart_receive_config(com, USART_RECEIVE_ENABLE);
    usart_transmit_config(com, USART_TRANSMIT_ENABLE);
    usart_enable(com);
}

static void usart_dma_config(uint32_t com, UartResource *resource)
{
    dma_single_data_parameter_struct dma_init_struct;

    rcu_periph_clock_enable(g_DmaPeriphClockMaps[resource->uartDma.rx_DMA]);
    rcu_periph_clock_enable(g_DmaPeriphClockMaps[resource->uartDma.tx_DMA]);
    /* deinitialize DMA channel7(USART0 tx) */
    dma_single_data_para_struct_init(&dma_init_struct);
    usart_dma_tx_config(com, &dma_init_struct, NULL, 0, &resource->uartDma);
    usart_dma_rx_config(com, &dma_init_struct, &resource->uartDma);
}

static void usart_irq_config(uint32_t com, UartResource *resource)
{
    if(USART1 == com) {
        g_UartInstTbl[resource->num].isr = USART1_IRQHandler;
    } else if (USART2 == com) {
        g_UartInstTbl[resource->num].isr = USART2_IRQHandler;
    } else if (UART3 == com) {
        g_UartInstTbl[resource->num].isr = USART3_IRQHandler;
    } else if (UART4 == com) {
        g_UartInstTbl[resource->num].isr = USART4_IRQHandler;
    } else if (USART5 == com) {
        g_UartInstTbl[resource->num].isr = UART5_IRQHandler;
    } else if (UART6 == com) {
        g_UartInstTbl[resource->num].isr = USART6_IRQHandler;
    } else if (UART7 == com) {
        g_UartInstTbl[resource->num].isr = USART7_IRQHandler;
    }
}

static int32_t UartHostDevInit(struct UartHost *host)
{
    GET_RESOURCE_BY_HOST(host, resourceInit)
    RESOURCE_ENABLE_VERIFY(resourceInit)

    struct buffer_t *pDataBuf = OsalMemAlloc(sizeof(struct buffer_t));
    if (!pDataBuf) {
        HDF_LOGE("%s: uart_%d alloc data buffer failed ", __func__, resourceInit->num);
        return HDF_ERR_MALLOC_FAIL;
    }
    g_UartInstTbl[resourceInit->num].data_Buf = pDataBuf;
    buffer_init(g_UartInstTbl[resourceInit->num].data_Buf, DATA_BUF_SIZE);

    /* UART config */
    com_init_cfg(g_UartPeriph[resourceInit->num], resourceInit);
    /* configure USART DMA */
    usart_dma_config(g_UartPeriph[resourceInit->num], resourceInit);

    usart_irq_config(g_UartPeriph[resourceInit->num], resourceInit);

    g_UartInstTbl[resourceInit->num].uartDma = &resourceInit->uartDma;

    nvic_irq_enable(g_UartIrqnMaps[resourceInit->num], 0, 0);
    ArchHwiCreate(g_UartIrqnMaps[resourceInit->num], 0, 0, (HWI_PROC_FUNC)g_UartInstTbl[resourceInit->num].isr, 0);
    usart_interrupt_enable(g_UartPeriph[resourceInit->num], USART_INT_IDLE);

    return HDF_SUCCESS;
}

static int32_t UartHostDevDeinit(struct UartHost *host)
{
    UartResource *resource = NULL;
    GET_RESOURCE_BY_HOST(host, resource)
    RESOURCE_ENABLE_VERIFY(resource)

    dma_deinit(g_UartDmaFuncMaps[resource->uartDma.rx_DMA], g_UartDmaChMaps[resource->uartDma.rx_DmaCh]);
    dma_deinit(g_UartDmaFuncMaps[resource->uartDma.tx_DMA], g_UartDmaChMaps[resource->uartDma.tx_DmaCh]);
    return HDF_SUCCESS;
}

static int32_t UartHostDevRead(struct UartHost *host, uint8_t *data, uint32_t size)
{
    uint32_t read_data_size = 0;
    UartResource *resource = NULL;
    GET_RESOURCE_BY_HOST(host, resource)
    RESOURCE_ENABLE_VERIFY(resource)

    usart_dma_receive_config(g_UartPeriph[resource->num], USART_RECEIVE_DMA_ENABLE);

    return buff_read(g_UartInstTbl[resource->num].data_Buf, data, size, 1);
}

static int32_t UartHostDevWrite(struct UartHost *host, uint8_t *data, uint32_t size)
{
    UartResource *resource = NULL;
    GET_RESOURCE_BY_HOST(host, resource)
    RESOURCE_ENABLE_VERIFY(resource)

    dma_single_data_parameter_struct dma_init_struct;
    usart_dma_tx_config(g_UartPeriph[resource->num], &dma_init_struct, data, size, &resource->uartDma);
    usart_dma_transmit_config(g_UartPeriph[resource->num], USART_TRANSMIT_DMA_ENABLE);

    /* wait until USART0 TX DMA1 channel transfer complete */
    while(RESET == dma_flag_get(g_UartDmaFuncMaps[resource->uartDma.tx_DMA], g_UartDmaChMaps[resource->uartDma.tx_DmaCh], DMA_INTF_FTFIF)){
    }

    return HDF_SUCCESS;
}

static int32_t UartHostDevSetBaud(struct UartHost *host, uint32_t baudRate)
{
    UartResource *resource = NULL;
    GET_RESOURCE_BY_HOST(host, resource)
    RESOURCE_ENABLE_VERIFY(resource)

    usart_deinit(g_UartPeriph[resource->num]);
    usart_baudrate_set(g_UartPeriph[resource->num], baudRate);
    usart_receive_config(g_UartPeriph[resource->num], USART_RECEIVE_ENABLE);
    usart_transmit_config(g_UartPeriph[resource->num], USART_TRANSMIT_ENABLE);
    usart_enable(g_UartPeriph[resource->num]);

    nvic_irq_enable(g_UartIrqnMaps[resource->num], 0, 0);
    //HalHwiCreate(g_UartIrqnMaps[resource->num], 0, 0, (HWI_PROC_FUNC)USART_DMA_IRQHandler, 0);
    ArchHwiCreate(g_UartIrqnMaps[resource->num], 0, 0, (HWI_PROC_FUNC)USART_DMA_IRQHandler, 0);
    usart_interrupt_enable(g_UartPeriph[resource->num], USART_INT_IDLE);
    resource->baudval = baudRate;

    return HDF_SUCCESS;
}

struct UartHostMethod g_uartHostMethod = {
    .Init = UartHostDevInit,
    .Deinit = UartHostDevDeinit,
    .Read = UartHostDevRead,
    .Write = UartHostDevWrite,
    .SetBaud = UartHostDevSetBaud,
    .GetBaud = NULL,
    .SetAttribute = NULL,
    .GetAttribute = NULL,
    .SetTransMode = NULL,
};

static int32_t UartDriverBind(struct HdfDeviceObject *device)
{
    HDF_LOGI("%s: %d\n", __func__, __LINE__);
    if (device == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return (UartHostCreate(device) == NULL) ? HDF_FAILURE : HDF_SUCCESS;
}

static int32_t GetUartHcs(const struct DeviceResourceIface *dri,
    const struct DeviceResourceNode *resourceNode, UartResource *resource)
{
    uartPinCfg *uartConfig = &resource->uartPin;
    uartDmaCfg *uartDmaConfig = &resource->uartDma;
    if (uartConfig == NULL) {
        HDF_LOGE("%s: uartConfig is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (dri->GetUint8(resourceNode, "enable", &resource->enable, 0) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (dri->GetUint8(resourceNode, "num", &resource->num, 0) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "baudval", &resource->baudval, 0) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: num %u, enable %u, baudval %u\n", __func__, resource->num, resource->enable, resource->baudval);

    if (dri->GetUint32(resourceNode, "rxPort", &uartConfig->rxPort, 0) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "rxPin", &uartConfig->rxPin, 0) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "rx_AF", &uartConfig->rx_AF, 0) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "txPort", &uartConfig->txPort, 0) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "txPin", &uartConfig->txPin, 0) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "tx_AF", &uartConfig->tx_AF, 0) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "rx_DMA", &uartDmaConfig->rx_DMA, 0) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "tx_DMA", &uartDmaConfig->tx_DMA, 0) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "rx_DmaCh", &uartDmaConfig->rx_DmaCh, 0) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "tx_DmaCh", &uartDmaConfig->tx_DmaCh, 0) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "rx_DmaPerSel", &uartDmaConfig->rx_DmaPerSel, 0) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "tx_DmaPerSel", &uartDmaConfig->tx_DmaPerSel, 0) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: rxPort %u, rxPin %u, txPort %u, txPin %u, rx_AF %u, tx_AF %u, \
            rx_DMA %u, tx_DMA %u, rx_DmaCh %u, tx_DmaCh %u\n", __func__, 
           uartConfig->rxPort, uartConfig->rxPin, uartConfig->txPort, uartConfig->txPin,
           uartConfig->rx_AF, uartConfig->tx_AF, uartDmaConfig->rx_DMA, uartDmaConfig->tx_DMA,
           uartDmaConfig->rx_DmaCh, uartDmaConfig->tx_DmaCh);

    return HDF_SUCCESS;
}

static int32_t GetUartDeviceResource(UartResource *device, const struct DeviceResourceNode *resourceNode)
{
    UartResource *resource = NULL;
    struct DeviceResourceIface *dri = NULL;
    if (device == NULL || resourceNode == NULL) {
        HDF_LOGE("%s: INVALID PARAM", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    resource = device;
    if (resource == NULL) {
        HDF_LOGE("%s: INVALID OBJECT", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    dri = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (dri == NULL || dri->GetUint32 == NULL || dri->GetUint8 == NULL) {
        HDF_LOGE("DeviceResourceIface is invalid");
        return HDF_ERR_INVALID_PARAM;
    }

    if (GetUartHcs(dri, resourceNode, resource) != HDF_SUCCESS) {
        HDF_LOGE("Get uart hcs failed\n");
        return HDF_ERR_INVALID_PARAM;
    }

    return HDF_SUCCESS;
}

static int32_t AttachUartDevice(struct UartHost *uartHost, struct HdfDeviceObject *device)
{
    int32_t ret;
    UartResource *uartDevice = NULL;

    if (uartHost == NULL || device == NULL || device->property == NULL) {
        HDF_LOGE("%s: property is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    uartDevice = (UartResource *)OsalMemAlloc(sizeof(UartResource));
    if (uartDevice == NULL) {
        HDF_LOGE("%s: OsalMemCalloc uartDevice error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    ret = GetUartDeviceResource(uartDevice, device->property);
    if (ret != HDF_SUCCESS) {
        (void)OsalMemFree(uartDevice);
        return HDF_FAILURE;
    }

    uartHost->priv = uartDevice;
    uartHost->num = uartDevice->num;

    return HDF_SUCCESS;
}

static int32_t UartDriverInit(struct HdfDeviceObject *device)
{
    HDF_LOGI("Enter %s:", __func__);
    int32_t ret;
    struct UartHost *host = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    host = UartHostFromDevice(device);
    if (host == NULL) {
        HDF_LOGE("%s: host is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = AttachUartDevice(host, device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: attach error", __func__);
        return HDF_FAILURE;
    }

    host->method = &g_uartHostMethod;

    HDF_LOGI("%s: dev service:%s init success!", __func__, HdfDeviceGetServiceName(device));
    return HDF_SUCCESS;
}

static void UartDriverRelease(struct HdfDeviceObject *device)
{
    HDF_LOGI("Enter %s:", __func__);
    struct UartHost *host = NULL;
    UartResource *uartDevice = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return;
    }

    host = UartHostFromDevice(device);
    if (host == NULL || host->priv == NULL) {
        HDF_LOGE("%s: host is NULL", __func__);
        return;
    }

    uartDevice = (UartResource *)host->priv;
    // de-init once uart is opend in advance
    UartHostDevDeinit(host);
    host->method = NULL;

    OsalMemFree(uartDevice);
    host->priv = NULL;

    UartHostDestroy(host);
}

struct HdfDriverEntry g_GdUartDmaDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "GD_UART_MODULE_HDF",
    .Bind = UartDriverBind,
    .Init = UartDriverInit,
    .Release = UartDriverRelease,
};
HDF_INIT(g_GdUartDmaDriverEntry);

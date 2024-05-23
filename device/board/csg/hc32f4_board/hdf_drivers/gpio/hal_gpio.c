#include <stdlib.h>
#include "hal_gpio.h"
#include "hc32_ll_interrupts.h"
#include "device_resource_if.h"
#include "gpio_core.h"
#include "hdf_log.h"
#include "osal_irq.h"

#define DDL_IRQ_PRIO_DEFAULT        (DDL_IRQ_PRIO_15)

typedef struct {
    uint16_t pin;
    uint16_t realPin;
    uint32_t group;
    uint16_t pinstate;
    uint16_t pindir;
    uint16_t pinoutputtype;
    uint16_t pindrv;
    uint16_t latch;
    uint16_t pullup;
    uint16_t invert;
    uint16_t extint;
    uint16_t pininputtype;
    uint16_t pinattr;
    uint16_t intsrc;
    uint16_t irqn;
} GpioResource;

typedef struct {
    uint32_t pinNums;
    GpioResource resource;
    HC32_GPIO_GROUP group; /* gpio config */
} GpioDevice;

static struct GpioCntlr g_gpioCntlr;
typedef struct {
    uint32_t group;
    uint32_t realPin;
    uint32_t pin;
    uint16_t intsrc;
    uint16_t irqn;
    uint16_t pindir;
} GpioInflectInfo;

GpioInflectInfo g_gpioPinsMap[HC32_GPIO_PIN_MAX * HC32_GPIO_GROUP_MAX] = {0};

static int32_t HCGpioSetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t dir)
{
    (void)cntlr;
    stc_gpio_init_t stcGpioInit;

    uint16_t realPin = g_gpioPinsMap[gpio].realPin;
    uint32_t pinReg = g_hcRealPinMaps[realPin];

    if (pinReg > GPIO_PIN_15 || pinReg < GPIO_PIN_00) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, realPin);
        return HDF_ERR_NOT_SUPPORT;
    }
    uint8_t gpiox = g_gpioxMaps[g_gpioPinsMap[gpio].group];

    (void)GPIO_StructInit(&stcGpioInit);

    stcGpioInit.u16PinState = PIN_STAT_RST;

    g_gpioPinsMap[gpio].pindir = dir;
    if (dir) {
        stcGpioInit.u16PinDir = PIN_DIR_OUT;
    } else {
        stcGpioInit.u16PinDir = PIN_DIR_IN;
    }

    (void)GPIO_Init(gpiox, pinReg, &stcGpioInit);

    return HDF_SUCCESS;
}

static int32_t HCGpioGetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *dir)
{
    (void)cntlr;
    uint16_t realPin = g_gpioPinsMap[gpio].realPin;
    uint32_t pinReg = g_hcRealPinMaps[realPin];
    uint16_t value = 0;
    if (pinReg > GPIO_PIN_15 || pinReg < GPIO_PIN_00) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, realPin);
        return HDF_ERR_NOT_SUPPORT;
    }
    uint8_t gpiox = g_gpioxMaps[g_gpioPinsMap[gpio].group];

    value = g_gpioPinsMap[gpio].pindir;

    *dir = value;

    return HDF_SUCCESS;
}

static int32_t HCGpioWrite(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t val)
{
    (void)cntlr;

    uint16_t realPin = g_gpioPinsMap[gpio].realPin;
    uint32_t pinReg = g_hcRealPinMaps[realPin];

    if (pinReg > GPIO_PIN_15 || pinReg < GPIO_PIN_00) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, realPin);
        return HDF_ERR_NOT_SUPPORT;
    }

    uint8_t gpiox = g_gpioxMaps[g_gpioPinsMap[gpio].group];

    if (val) {
        GPIO_SetPins(gpiox, pinReg);
    } else {
        GPIO_ResetPins(gpiox, pinReg);
    }

    return HDF_SUCCESS;
}

static int32_t HCGpioRead(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *val)
{
    (void)cntlr;

    uint16_t realPin = g_gpioPinsMap[gpio].realPin;
    uint32_t pinReg = g_hcRealPinMaps[realPin];
    en_pin_state_t value = 0;
    if (pinReg > GPIO_PIN_15 || pinReg < GPIO_PIN_00) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, realPin);
        return HDF_ERR_NOT_SUPPORT;
    }

    uint8_t gpiox = g_gpioxMaps[g_gpioPinsMap[gpio].group];

    value = GPIO_ReadInputPins(gpiox, pinReg);

    *val = value;

    return HDF_SUCCESS;
}

static int32_t HCGpioDisableIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    (void)cntlr;
    stc_irq_signin_config_t stcIrqSignConfig;
    uint16_t realPin = g_gpioPinsMap[gpio].realPin;
    uint32_t pinReg = g_hcRealPinMaps[realPin];

    if (pinReg > GPIO_PIN_15 || pinReg < GPIO_PIN_00) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, realPin);
        return HDF_ERR_NOT_SUPPORT;
    }
    uint8_t gpiox = g_gpioxMaps[g_gpioPinsMap[gpio].group];

    /* IRQ sign-in */
    uint16_t irqn = g_gpioPinsMap[gpio].irqn;

    /* NVIC config */
    NVIC_ClearPendingIRQ(irqn);
    NVIC_SetPriority(irqn, DDL_IRQ_PRIO_DEFAULT);
    NVIC_DisableIRQ(irqn);

    return HDF_SUCCESS;
}

static int32_t HCGpioEnableIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    (void)cntlr;
    stc_irq_signin_config_t stcIrqSignConfig;
    uint16_t realPin = g_gpioPinsMap[gpio].realPin;
    uint32_t pinReg = g_hcRealPinMaps[realPin];

    if (pinReg > GPIO_PIN_15 || pinReg < GPIO_PIN_00) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, realPin);
        return HDF_ERR_NOT_SUPPORT;
    }
    uint8_t gpiox = g_gpioxMaps[g_gpioPinsMap[gpio].group];

    /* IRQ sign-in */
    uint16_t irqn = g_gpioPinsMap[gpio].irqn;

    /* NVIC config */
    NVIC_ClearPendingIRQ(irqn);
    NVIC_SetPriority(irqn, DDL_IRQ_PRIO_DEFAULT);
    NVIC_EnableIRQ(irqn);

    return HDF_SUCCESS;
}

static int32_t HCGpioSetIrq(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t mode, GpioIrqFunc func, void *arg)
{
    (void)cntlr;
    stc_extint_init_t stcExtIntInit;
    stc_irq_signin_config_t stcIrqSignConfig;
    stc_gpio_init_t stcGpioInit;

    uint16_t realPin = g_gpioPinsMap[gpio].realPin;
    uint32_t pinReg = g_hcRealPinMaps[realPin];
    if (pinReg > GPIO_PIN_15 || pinReg < GPIO_PIN_00) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, realPin);
        return HDF_ERR_NOT_SUPPORT;
    }
    uint8_t gpiox = g_gpioxMaps[g_gpioPinsMap[gpio].group];

    /* GPIO config */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16ExtInt = PIN_EXTINT_ON;
    stcGpioInit.u16PullUp = PIN_PU_ON;
    (void)GPIO_Init(gpiox, pinReg, &stcGpioInit);

    /* ExtInt config */
    (void)EXTINT_StructInit(&stcExtIntInit);
    stcExtIntInit.u32Filter      = EXTINT_FILTER_ON;
    stcExtIntInit.u32FilterClock = EXTINT_FCLK_DIV8;
    if (mode == OSAL_IRQF_TRIGGER_RISING) {
        stcExtIntInit.u32Edge = EXTINT_TRIG_RISING;
    }
    else if(mode == OSAL_IRQF_TRIGGER_FALLING) {
        stcExtIntInit.u32Edge = EXTINT_TRIG_FALLING;
    }
    else if(mode == OSAL_IRQF_TRIGGER_LOW) {
        stcExtIntInit.u32Edge = EXTINT_TRIG_LOW;
    }
    else {
        HDF_LOGE("%s %d, error mode:%d", __func__, __LINE__, mode);
        return HDF_ERR_NOT_SUPPORT;
    }
    (void)EXTINT_Init(g_gpioExitLineMap[g_gpioPinsMap[gpio].realPin], &stcExtIntInit);

    /* IRQ sign-in */
    uint16_t intsrc = g_gpioPinsMap[gpio].intsrc;
    uint16_t irqn = g_gpioPinsMap[gpio].irqn;
    stcIrqSignConfig.enIntSrc = intsrc;
    stcIrqSignConfig.enIRQn   = irqn;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    NVIC_SetVector(intsrc, func);

    return HDF_SUCCESS;
}

static int32_t HCGpioUnsetIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    (void)cntlr;
    uint16_t realPin = g_gpioPinsMap[gpio].realPin;
    uint32_t pinReg = g_hcRealPinMaps[realPin];
    if (pinReg > GPIO_PIN_15 || pinReg < GPIO_PIN_00) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, realPin);
        return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static struct GpioMethod g_method = {
    .request = NULL,
    .release = NULL,
    .write = HCGpioWrite,
    .read = HCGpioRead,
    .setDir = HCGpioSetDir,
    .getDir = HCGpioGetDir,
    .toIrq = NULL,
    .setIrq = HCGpioSetIrq,
    .unsetIrq = HCGpioUnsetIrq,
    .enableIrq = HCGpioEnableIrq,
    .disableIrq = HCGpioDisableIrq,
};

static int32_t InitGpioDevice(const GpioDevice* device)
{
    stc_gpio_init_t stcGpioInit;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t halGpio = g_hcRealPinMaps[device->resource.realPin];
    if (halGpio > GPIO_PIN_15 || halGpio < GPIO_PIN_00) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, halGpio);
        return HDF_ERR_NOT_SUPPORT;
    }

    uint8_t gpiox = g_gpioxMaps[device->resource.group];

    /* GPIO config */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = device->resource.pinstate;
    stcGpioInit.u16PinDir = device->resource.pindir;
    if(device->resource.pullup == 0) {
        stcGpioInit.u16PullUp = PIN_PU_OFF;
    }
    else if(device->resource.pullup == 1) {
        stcGpioInit.u16PullUp = PIN_PU_ON;
    }

    if(device->resource.extint == 0) {
        stcGpioInit.u16ExtInt = PIN_EXTINT_OFF;
    }
    else if(device->resource.extint == 1) {
        stcGpioInit.u16ExtInt = PIN_EXTINT_ON;
    }

    if(device->resource.pindir == 0) {
        stcGpioInit.u16PinDir = PIN_DIR_IN;
    }
    else if(device->resource.pindir == 1) {
        stcGpioInit.u16PinDir = PIN_DIR_OUT;
    }

    if(device->resource.pindrv == 0) {
        stcGpioInit.u16PinDrv = PIN_LOW_DRV;
    }
    else if(device->resource.pindrv == 1) {
        stcGpioInit.u16PinDrv = PIN_MID_DRV;
    }
    else if(device->resource.pindrv == 2) {
        stcGpioInit.u16PinDrv = PIN_HIGH_DRV;
    }

    if(device->resource.pinoutputtype == 0) {
        stcGpioInit.u16PinDir = PIN_OUT_TYPE_CMOS;
    }
    else if(device->resource.pinoutputtype == 1) {
        stcGpioInit.u16PinDir = PIN_OUT_TYPE_NMOS;
    }
    (void)GPIO_Init(gpiox, halGpio, &stcGpioInit);

    return HDF_SUCCESS;
}

static int32_t GpioParseHcs(const struct DeviceResourceIface *dri,
    GpioDevice *device, const struct DeviceResourceNode *resourceNode)
{
    GpioResource *resource = NULL;
    resource = &device->resource;
    if (resource == NULL) {
        HDF_LOGE("%s: resource is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (dri->GetUint32(resourceNode, "pinNum", &device->pinNums, 0) != HDF_SUCCESS) {
        HDF_LOGE("gpio config read pinNum fail");
        return HDF_FAILURE;
    }

    //HDF_LOGI("%s %d, pinNum:%d", __func__, __LINE__, device->pinNums);

    for (size_t i = 0; i < device->pinNums; i++) {
        if (dri->GetUint32ArrayElem(resourceNode, "pin", i, &resource->pin, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        if (dri->GetUint32ArrayElem(resourceNode, "realPin", i, &resource->realPin, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        if (dri->GetUint32ArrayElem(resourceNode, "group", i, &resource->group, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        if (dri->GetUint32ArrayElem(resourceNode, "PinState", i, &resource->pinstate, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        if (dri->GetUint32ArrayElem(resourceNode, "PinDir", i, &resource->pindir, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        if (dri->GetUint32ArrayElem(resourceNode, "PinOutputType", i, &resource->pinoutputtype, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        if (dri->GetUint32ArrayElem(resourceNode, "PinDrv", i, &resource->pindrv, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        if (dri->GetUint32ArrayElem(resourceNode, "Latch", i, &resource->latch, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        if (dri->GetUint32ArrayElem(resourceNode, "Invert", i, &resource->invert, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        if (dri->GetUint32ArrayElem(resourceNode, "PinInputType", i, &resource->pininputtype, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        if (dri->GetUint32ArrayElem(resourceNode, "PinAttr", i, &resource->pinattr, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        if (dri->GetUint32ArrayElem(resourceNode, "PullUp", i, &resource->pullup, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        if (dri->GetUint32ArrayElem(resourceNode, "ExtInt", i, &resource->extint, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        if (dri->GetUint32ArrayElem(resourceNode, "IntSrc", i, &resource->intsrc, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        if (dri->GetUint32ArrayElem(resourceNode, "IRQn", i, &resource->irqn, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        g_gpioPinsMap[resource->pin].group = resource->group;
        g_gpioPinsMap[resource->pin].realPin = resource->realPin;
        g_gpioPinsMap[resource->pin].pin = resource->pin;
        g_gpioPinsMap[resource->pin].intsrc = resource->intsrc;
        g_gpioPinsMap[resource->pin].irqn = resource->irqn;
        g_gpioPinsMap[resource->pin].pindir = resource->pindir;

        if (InitGpioDevice(device) != HDF_SUCCESS) {
            HDF_LOGE("InitGpioDevice FAIL\r\n");
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}

static int32_t GetGpioDeviceResource(GpioDevice *device, const struct DeviceResourceNode *resourceNode)
{
    struct DeviceResourceIface *dri = NULL;
    if (device == NULL || resourceNode == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    dri = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (dri == NULL || dri->GetUint32 == NULL) {
        HDF_LOGE("DeviceResourceIface is invalid");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (GpioParseHcs(dri, device, resourceNode) != HDF_SUCCESS) {
        HDF_LOGE("gpio config parse hcs fail");
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t AttachGpioDevice(struct GpioCntlr *gpioCntlr, const struct HdfDeviceObject *device)
{
    int32_t ret;

    GpioDevice *gpioDevice = NULL;
    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: property is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    gpioDevice = (GpioDevice *)OsalMemAlloc(sizeof(GpioDevice));
    if (gpioDevice == NULL) {
        HDF_LOGE("%s: OsalMemAlloc gpioDevice error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = GetGpioDeviceResource(gpioDevice, device->property);
    if (ret != HDF_SUCCESS) {
        (void)OsalMemFree(gpioDevice);
        return HDF_FAILURE;
    }
    gpioCntlr->priv = gpioDevice;
    gpioCntlr->count = gpioDevice->pinNums;

    return HDF_SUCCESS;
}

static int32_t HCGpioInit(struct HdfDeviceObject *device)
{
    int32_t ret;

    //HDF_LOGI("%s :enter!\n", __func__);
    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = PlatformDeviceBind(&g_gpioCntlr.device, device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: bind hdf device failed:%d", __func__, ret);
        return ret;
    }

    struct GpioCntlr *gpioCntlr = NULL;
    gpioCntlr = GpioCntlrFromHdfDev(device);
    if (gpioCntlr == NULL) {
        HDF_LOGE("GpioCntlrFromHdfDev fail\r\n");
        return HDF_DEV_ERR_NO_DEVICE_SERVICE;
    }

    ret = AttachGpioDevice(gpioCntlr, device); /* GpioCntlr add GpioDevice to priv */
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("AttachGpioDevice fail\r\n");
        return HDF_DEV_ERR_ATTACHDEV_FAIL;
    }
    gpioCntlr->ops = &g_method;

    ret = GpioCntlrAdd(gpioCntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: err add controller:%d", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: dev service:%s init success!", __func__, HdfDeviceGetServiceName(device));
    return HDF_SUCCESS;
}

static void HCGpioRelease(struct HdfDeviceObject *device)
{
    //HDF_LOGI("%s :enter!\n", __func__);
    struct GpioCntlr *cntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is null!", __func__);
        return;
    }

    cntlr = GpioCntlrFromHdfDev(device);
    if (cntlr == NULL) {
        HDF_LOGE("%s: no service binded!", __func__);
        return;
    }

    GpioCntlrRemove(cntlr);
}

struct HdfDriverEntry g_gpioDriverEntry = {
    .moduleVersion = 1,
    .Init = HCGpioInit,
    .Release = HCGpioRelease,
    .moduleName = "HC_GPIO_MODULE_HDF",
};
HDF_INIT(g_gpioDriverEntry);

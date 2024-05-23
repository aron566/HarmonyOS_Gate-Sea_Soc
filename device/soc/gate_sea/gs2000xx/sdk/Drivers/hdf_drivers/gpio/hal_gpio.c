#include <stdlib.h>
#include "hal_gpio.h"
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#include "hcs_macro.h"
#include "hdf_config_macro.h"
#else
#include "device_resource_if.h"
#endif
#include "gpio_core.h"
#include "hdf_log.h"
#include "osal_irq.h"

#define DDL_IRQ_PRIO_DEFAULT        (DDL_IRQ_PRIO_15)
#define PIN_EXIT_ZERO 0
#define PIN_EXIT_FIVE 5
#define PIN_EXIT_TEN  10
#define PIN_EXIT_SIXTEEN  16

#define DEBUG_GPIO_LOG            0

Pin_GD g_pinsGroup[GD32_GPIO_GROUP_MAX*GD32_GPIO_PIN_MAX] = {0};

static uint16_t g_exitSetupCounts = 0;

typedef struct {
    uint16_t pin;
    uint16_t realPin;
    uint32_t group;
    uint16_t pinstate;
    uint16_t pindir;
    uint16_t pinoutputtype;
    uint16_t pullup;
    uint16_t subpr;
    uint16_t prepr;
} GpioResource;

typedef struct {
    uint32_t pinNums;
    GpioResource resource;
    GD32_GPIO_GROUP group; /* gpio config */
} GpioDevice;

static struct GpioCntlr g_gpioCntlr;
typedef struct {
    uint32_t group;
    uint32_t realPin;
    uint32_t pin;
    uint16_t prepr;
    uint16_t subpr;
    uint16_t pindir;
    uint16_t pullup;
    uint16_t pinoutputtype;
    uint16_t pinstate;
    uint16_t gpioindex;
    uint16_t gpiosetflag;
} GpioInflectInfo;

GpioInflectInfo g_gpioPinsMap[GD32_GPIO_PIN_MAX * GD32_GPIO_GROUP_MAX] = {0};

static int32_t GDGpioSetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t dir)
{
    (void)cntlr;
    uint32_t gpio_dir;

    uint16_t gpio_pull = g_gpioPinsMap[gpio].pullup;
    uint32_t gpio_type = g_gpioPinsMap[gpio].pinoutputtype;

#if DEBUG_GPIO_LOG
    HDF_LOGI("%s: [line = %d] [gpio_pull = %d] [gpio_type = %d]\n", __func__, __LINE__, gpio_pull, gpio_type);
#endif

    uint16_t realPin = g_gpioPinsMap[gpio].realPin;
    uint32_t pinReg = g_GdRealPinMaps[realPin];

    if (pinReg > GPIO_PIN_15 || pinReg < GPIO_PIN_0) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, realPin);
        return HDF_ERR_NOT_SUPPORT;
    }
    uint32_t gpiox = g_gpioxMaps[g_gpioPinsMap[gpio].group];

    if(dir == 0) {
        gpio_dir =  GPIO_MODE_INPUT;
        g_gpioPinsMap[gpio].pindir = 0;
    }
    else if(dir == 1) {
        gpio_dir =  GPIO_MODE_OUTPUT;
        g_gpioPinsMap[gpio].pindir = 1;
    }

    gpio_mode_set(gpiox, gpio_dir, gpio_pull, pinReg);
    gpio_output_options_set(gpiox, gpio_type, GPIO_OSPEED_50MHZ, pinReg);

    return HDF_SUCCESS;
}

static int32_t GDGpioGetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *dir)
{
    uint8_t value = 0;

    uint16_t realPin = g_gpioPinsMap[gpio].realPin;
    uint32_t pinReg = g_GdRealPinMaps[realPin];

    if (pinReg > GPIO_PIN_15 || pinReg < GPIO_PIN_0) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, realPin);
        return HDF_ERR_NOT_SUPPORT;
    }

    value = g_gpioPinsMap[gpio].pindir;

    *dir = value;

    return HDF_SUCCESS;
}

static int32_t GDGpioWrite(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t val)
{
    (void)cntlr;

    uint16_t realPin = g_gpioPinsMap[gpio].realPin;
    uint32_t pinReg = g_GdRealPinMaps[realPin];

    if (pinReg > GPIO_PIN_15 || pinReg < GPIO_PIN_0) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, realPin);
        return HDF_ERR_NOT_SUPPORT;
    }

#if DEBUG_GPIO_LOG
    HDF_LOGI("%s %d ,write pin num %d", __func__, __LINE__, realPin);
#endif

    uint32_t gpiox = g_gpioxMaps[g_gpioPinsMap[gpio].group];

    if (val) {
        gpio_bit_set(gpiox, pinReg);

    } else {
        gpio_bit_reset(gpiox, pinReg);
    }

    return HDF_SUCCESS;
}

static int32_t GDGpioRead(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *val)
{
    uint8_t value = 0;
    uint16_t realPin = g_gpioPinsMap[gpio].realPin;
    uint32_t pinReg = g_GdRealPinMaps[realPin];

    if (pinReg > GPIO_PIN_15 || pinReg < GPIO_PIN_0) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, realPin);
        return HDF_ERR_NOT_SUPPORT;
    }
    uint32_t gpiox = g_gpioxMaps[g_gpioPinsMap[gpio].group];

    if(g_gpioPinsMap[gpio].pindir == 0) {
        value = gpio_input_bit_get(gpiox, pinReg);
    }
    else if(g_gpioPinsMap[gpio].pindir == 1) {
        value = gpio_output_bit_get(gpiox, pinReg);
    }

    *val = value;

    return HDF_SUCCESS;
}

int32_t Gpio_Exti_Handler(uint16_t gpio, void *data)
{
    for (uint16_t index = 0; index < g_exitSetupCounts; index++) 
    {
        if(g_pinsGroup[index].setup) {
            if(RESET != exti_interrupt_flag_get(BIT(g_pinsGroup[index].pin)))
            {
                GpioCntlrIrqCallback(&g_gpioCntlr, g_pinsGroup[index].localPin);
            }
            exti_interrupt_flag_clear(BIT(g_pinsGroup[index].pin));
        }
    }
}

static int32_t GDGpioDisableIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
#if DEBUG_GPIO_LOG
    HDF_LOGI("%s: [line = %d] [gpioindex = %d]\n", __func__, __LINE__, g_gpioPinsMap[gpio].gpioindex);
#endif

    g_pinsGroup[g_gpioPinsMap[gpio].gpioindex].setup = RESET;

    return HDF_SUCCESS;
}

static int32_t GDGpioEnableIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    (void)cntlr;
    uint16_t realPin = g_gpioPinsMap[gpio].realPin;
    uint32_t pinReg = g_GdRealPinMaps[realPin];
    uint32_t gpiox = g_gpioxMaps[g_gpioPinsMap[gpio].group];

#if DEBUG_GPIO_LOG
    HDF_LOGI("%s: [line = %d] [realPin = %d] [gpio  = %d]\n", __func__, __LINE__, realPin, gpio);
#endif

    if(g_gpioPinsMap[gpio].gpiosetflag == SET) {
        g_pinsGroup[g_gpioPinsMap[gpio].gpioindex].setup = SET;
    }

    if(g_gpioPinsMap[gpio].gpiosetflag != SET) {
        if (realPin < PIN_EXIT_FIVE) {
            OsSetVector(EXTI0_IRQn + realPin, Gpio_Exti_Handler, NULL);
        } else if (realPin >= PIN_EXIT_FIVE && realPin < PIN_EXIT_TEN) {
            OsSetVector(EXTI5_9_IRQn, Gpio_Exti_Handler, NULL);
        } else if (realPin >= PIN_EXIT_TEN && realPin < PIN_EXIT_SIXTEEN) {
            OsSetVector(EXTI10_15_IRQn, Gpio_Exti_Handler, NULL);
        }

        g_pinsGroup[g_exitSetupCounts].setup = SET;
        g_pinsGroup[g_exitSetupCounts].pin = realPin;
        g_pinsGroup[g_exitSetupCounts].localPin = gpio;
        g_pinsGroup[g_exitSetupCounts].setflag = SET;
        g_gpioPinsMap[gpio].gpioindex = g_exitSetupCounts;
        g_gpioPinsMap[gpio].gpiosetflag = SET;

        g_exitSetupCounts++;
    }

    return HDF_SUCCESS;
}

static int32_t GDGpioSetIrq(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t mode, GpioIrqFunc func, void *arg)
{
    (void)cntlr;
    uint32_t gpio_dir;
    uint32_t gpio_pull;
    uint8_t exti_trig;
    uint8_t nvic_irq;

    uint16_t realPin = g_gpioPinsMap[gpio].realPin;
    uint32_t pinReg = g_GdRealPinMaps[realPin];
    if (pinReg > GPIO_PIN_15 || pinReg < GPIO_PIN_0) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, realPin);
        return HDF_ERR_NOT_SUPPORT;
    }
    uint32_t gpiox = g_gpioxMaps[g_gpioPinsMap[gpio].group];

    if(g_gpioPinsMap[gpio].pindir == 0) {
        gpio_dir =  GPIO_MODE_INPUT;
    }
    else if(g_gpioPinsMap[gpio].pindir == 1) {
        gpio_dir =  GPIO_MODE_OUTPUT;
    }

    if(g_gpioPinsMap[gpio].pullup == 0) {
        gpio_pull = GPIO_PUPD_NONE;
    }
    else if(g_gpioPinsMap[gpio].pullup == 1) {
        gpio_pull = GPIO_PUPD_PULLUP;
    }
    else if(g_gpioPinsMap[gpio].pullup == 2) {
        gpio_pull = GPIO_PUPD_PULLDOWN;
    }

    rcu_periph_clock_enable(g_rcuPeriphMaps[g_gpioPinsMap[gpio].group]);
    rcu_periph_clock_enable(RCU_SYSCFG);
    gpio_mode_set(gpiox, gpio_dir, gpio_pull, pinReg);

#if DEBUG_GPIO_LOG
    HDF_LOGI("%s: [line = %d][prepr = %d] [subpr = %d]\n", __func__, __LINE__, \
            g_gpioPinsMap[gpio].prepr, g_gpioPinsMap[gpio].subpr);
#endif

    if(g_gpioPinsMap[gpio].realPin < 5) {
        nvic_irq = EXTI0_IRQn + g_gpioPinsMap[gpio].realPin;
    }
    else if((g_gpioPinsMap[gpio].realPin >= 5)&&(g_gpioPinsMap[gpio].realPin <= 9)) {
        nvic_irq = EXTI5_9_IRQn;
    }
    else if((g_gpioPinsMap[gpio].realPin >= 10)&&(g_gpioPinsMap[gpio].realPin <= 15)) {
        nvic_irq = EXTI10_15_IRQn;
    }
    /* enable EXTI interrupt priority */
    nvic_irq_enable(nvic_irq, g_gpioPinsMap[gpio].prepr, g_gpioPinsMap[gpio].subpr);
    /* connect EXTI line to GPIO pin */
    syscfg_exti_line_config(g_ExtiSrcGpioMaps[g_gpioPinsMap[gpio].group], g_ExtiSrcPinMaps[g_gpioPinsMap[gpio].realPin]);
    /* configure EXTI line */
    if (mode == OSAL_IRQF_TRIGGER_RISING) {
        exti_trig = EXTI_TRIG_RISING;
    }
    else if(mode == OSAL_IRQF_TRIGGER_FALLING) {
        exti_trig = EXTI_TRIG_FALLING;
    }
    else if(mode == OSAL_IRQF_TRIGGER_NONE) {
        exti_trig = EXTI_TRIG_NONE;
    }
    else {
        HDF_LOGE("%s %d, error mode:%d", __func__, __LINE__, mode);
        return HDF_ERR_NOT_SUPPORT;
    }
    exti_init(g_gpioExitLineMap[g_gpioPinsMap[gpio].realPin], EXTI_INTERRUPT, exti_trig);
    exti_interrupt_flag_clear(g_gpioExitLineMap[g_gpioPinsMap[gpio].realPin]);

    return HDF_SUCCESS;
}

static int32_t GDGpioUnsetIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
    uint8_t nvic_irq;

    uint16_t realPin = g_gpioPinsMap[gpio].realPin;
    uint32_t pinReg = g_GdRealPinMaps[realPin];
    if (pinReg > GPIO_PIN_15 || pinReg < GPIO_PIN_0) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, realPin);
        return HDF_ERR_NOT_SUPPORT;
    }
    uint32_t gpiox = g_gpioxMaps[g_gpioPinsMap[gpio].group];

    if(g_gpioPinsMap[gpio].realPin < 5) {
        nvic_irq = EXTI0_IRQn + g_gpioPinsMap[gpio].realPin;
    }
    else if((g_gpioPinsMap[gpio].realPin >= 5)&&(g_gpioPinsMap[gpio].realPin <= 9)) {
        nvic_irq = EXTI5_9_IRQn;
    }
    else if((g_gpioPinsMap[gpio].realPin >= 10)&&(g_gpioPinsMap[gpio].realPin <= 15)) {
        nvic_irq = EXTI10_15_IRQn;
    }

    nvic_irq_disable(nvic_irq);
    return HDF_SUCCESS;
}

static struct GpioMethod g_method = {
    .request = NULL,
    .release = NULL,
    .write = GDGpioWrite,
    .read = GDGpioRead,
    .setDir = GDGpioSetDir,
    .getDir = GDGpioGetDir,
    .toIrq = NULL,
    .setIrq = GDGpioSetIrq,
    .unsetIrq = GDGpioUnsetIrq,
    .enableIrq = GDGpioEnableIrq,
    .disableIrq = GDGpioDisableIrq,
};

static int32_t GDGpioBind(struct HdfDeviceObject *device)
{
    HDF_LOGI("%s: Enter\r\n", __func__);
    (void)device;
    return HDF_SUCCESS;
}

static int32_t InitGpioDevice(const GpioDevice* device)
{
    uint32_t gpio_dir;
    uint32_t gpio_pull;
    uint32_t gpio_type;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    uint32_t halGpio = g_GdRealPinMaps[device->resource.realPin];
    if (halGpio > GPIO_PIN_15 || halGpio < GPIO_PIN_0) {
        HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, halGpio);
        return HDF_ERR_NOT_SUPPORT;
    }

    uint32_t gpiox = g_gpioxMaps[device->resource.group];

#if DEBUG_GPIO_LOG
    HDF_LOGI("%s: [line = %d] [group = %d] [realPin = %d]\n", __func__, __LINE__, \
            device->resource.group, device->resource.realPin);
#endif

    if(device->resource.pindir == 0) {
        gpio_dir =  GPIO_MODE_INPUT;
    }
    else if(device->resource.pindir == 1) {
        gpio_dir =  GPIO_MODE_OUTPUT;
    }

    if(device->resource.pullup == 0) {
        gpio_pull = GPIO_PUPD_NONE;
    }
    else if(device->resource.pullup == 1) {
        gpio_pull = GPIO_PUPD_PULLUP;
    }
    else if(device->resource.pullup == 2) {
        gpio_pull = GPIO_PUPD_PULLDOWN;
    }

    if(device->resource.pinoutputtype == 0) {
        gpio_type = GPIO_OTYPE_PP;
    }
    else if(device->resource.pinoutputtype == 1) {
        gpio_type = GPIO_OTYPE_OD;
    }

    rcu_periph_clock_enable(g_rcuPeriphMaps[device->resource.group]);
    gpio_mode_set(gpiox, gpio_dir, gpio_pull, halGpio);
    gpio_output_options_set(gpiox, gpio_type, GPIO_OSPEED_50MHZ, halGpio);

    if (device->resource.pinstate == 0) {
        gpio_bit_reset(gpiox, halGpio);
    } 
    else if (device->resource.pinstate == 1) {
        gpio_bit_set(gpiox, halGpio);
    }

    return HDF_SUCCESS;
}

#ifndef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
static int32_t GpioParseHcs(const struct DeviceResourceIface *dri,
    GpioDevice *device, const struct DeviceResourceNode *resourceNode)
{
    GpioResource *resource = NULL;
    resource = &device->resource;
    if (resource == NULL) {
        HDF_LOGE("%s: resource is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (dri->GetUint32(resourceNode, "PinNum", &device->pinNums, 0) != HDF_SUCCESS) {
        HDF_LOGE("gpio config read PinNum fail");
        return HDF_FAILURE;
    }

    HDF_LOGI("%s %d, PinNum:%d", __func__, __LINE__, device->pinNums);

    for (size_t i = 0; i < device->pinNums; i++) {
        if (dri->GetUint32ArrayElem(resourceNode, "Pin", i, &resource->pin, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        if (dri->GetUint32ArrayElem(resourceNode, "RealPin", i, &resource->realPin, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        if (dri->GetUint32ArrayElem(resourceNode, "Group", i, &resource->group, 0) != HDF_SUCCESS) {
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

        if (dri->GetUint32ArrayElem(resourceNode, "PullUp", i, &resource->pullup, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        if (dri->GetUint32ArrayElem(resourceNode, "SubPr", i, &resource->subpr, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        if (dri->GetUint32ArrayElem(resourceNode, "PrePr", i, &resource->prepr, 0) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        g_gpioPinsMap[resource->pin].group = resource->group;
        g_gpioPinsMap[resource->pin].realPin = resource->realPin;
        g_gpioPinsMap[resource->pin].pin = resource->pin;
        g_gpioPinsMap[resource->pin].prepr = resource->prepr;
        g_gpioPinsMap[resource->pin].subpr = resource->subpr;
        g_gpioPinsMap[resource->pin].pindir = resource->pindir;
        g_gpioPinsMap[resource->pin].pullup = resource->pullup;
        g_gpioPinsMap[resource->pin].pinoutputtype = resource->pinoutputtype;
        g_gpioPinsMap[resource->pin].pinstate = resource->pinstate;

        if (InitGpioDevice(device) != HDF_SUCCESS) {
            HDF_LOGE("InitGpioDevice FAIL\r\n");
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}
#endif

#ifndef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
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
#else
#define PLATFORM_GPIO_CONFIG HCS_NODE(HCS_NODE(HCS_ROOT, platform), gpio_config)
static uint32_t GetGpioDeviceResource(GpioDevice *device)
{
    GpioResource *resource = NULL;
    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    resource = &device->resource;
    if (resource == NULL) {
        HDF_LOGE("%s: resource is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    device->pinNums = HCS_PROP(PLATFORM_GPIO_CONFIG, PinNum);
    uint32_t pins[] = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, Pin));
    uint32_t realPins[] = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, RealPin));
    uint32_t groups[] = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, Group));
    uint32_t pinstates[] = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, PinState));
    uint32_t pindirs[] = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, PinDir));
    uint32_t pinoutputtypes[] = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, PinOutputType));
    uint32_t pullups[] = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, PullUp));
    uint32_t preprs[] = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, PrePr));
    uint32_t subprs[] = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, SubPr));

    for (size_t i = 0; i < device->pinNums; i++) {
        resource->pin = pins[i];
        resource->realPin = realPins[i];
        resource->group = groups[i];
        resource->prepr = preprs[i];
        resource->subpr = subprs[i];
        resource->pindir = pindirs[i];
        resource->pullup = pullups[i];
        resource->pinoutputtype = pinoutputtypes[i];
        resource->pinstate = pinstates[i];

        g_gpioPinsMap[resource->pin].group = resource->group;
        g_gpioPinsMap[resource->pin].realPin = resource->realPin;
        g_gpioPinsMap[resource->pin].pin = resource->pin;
        g_gpioPinsMap[resource->pin].prepr = resource->prepr;
        g_gpioPinsMap[resource->pin].subpr = resource->subpr;
        g_gpioPinsMap[resource->pin].pindir = resource->pindir;
        g_gpioPinsMap[resource->pin].pullup = resource->pullup;
        g_gpioPinsMap[resource->pin].pinoutputtype = resource->pinoutputtype;
        g_gpioPinsMap[resource->pin].pinstate = resource->pinstate;

        if (InitGpioDevice(device) != HDF_SUCCESS) {
            HDF_LOGE("InitGpioDevice FAIL\r\n");
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}
#endif

static int32_t AttachGpioDevice(struct GpioCntlr *gpioCntlr, const struct HdfDeviceObject *device)
{
    int32_t ret;

    GpioDevice *gpioDevice = NULL;

#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
    if (device == NULL) {
#else
    if (device == NULL || device->property == NULL) {
#endif
        HDF_LOGE("%s: property is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    gpioDevice = (GpioDevice *)OsalMemAlloc(sizeof(GpioDevice));
    if (gpioDevice == NULL) {
        HDF_LOGE("%s: OsalMemAlloc gpioDevice error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

#ifndef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
    ret = GetGpioDeviceResource(gpioDevice, device->property);
#else
    ret = GetGpioDeviceResource(gpioDevice);
#endif
    if (ret != HDF_SUCCESS) {
        (void)OsalMemFree(gpioDevice);
        return HDF_FAILURE;
    }

    gpioCntlr->priv = gpioDevice;
    gpioCntlr->count = gpioDevice->pinNums;

    return HDF_SUCCESS;
}

static int32_t GDGpioInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct GpioCntlr *gpioCntlr = &g_gpioCntlr;

    HDF_LOGI("%s: Enter\r\n", __func__);

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = AttachGpioDevice(gpioCntlr, device); /* GpioCntlr add GpioDevice to priv */
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("AttachGpioDevice fail\r\n");
        return HDF_DEV_ERR_ATTACHDEV_FAIL;
    }
    gpioCntlr->ops = &g_method;
    gpioCntlr->device.hdfDev = device;
    ret = GpioCntlrAdd(gpioCntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: err add controller:%d", __func__, ret);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: dev service:%s init success!", __func__, HdfDeviceGetServiceName(device));

    return HDF_SUCCESS;
}

static void GDGpioRelease(struct HdfDeviceObject *device)
{
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

    cntlr->count = 0;

    GpioCntlrRemove(cntlr);
}

struct HdfDriverEntry g_gpioDriverEntry = {
    .moduleVersion = 1,
    //.Bind = GDGpioBind,
    .Init = GDGpioInit,
    .Release = GDGpioRelease,
    .moduleName = "GD_GPIO_MODULE_HDF",
};
HDF_INIT(g_gpioDriverEntry);

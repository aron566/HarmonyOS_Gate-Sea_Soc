#include <stdio.h>
#include "osal_irq.h"
#include "osal_time.h"
#include "cmsis_os2.h"
#include "hdf_log.h"
#include "gpio_if.h"
#include "main.h"

#define HDF_LOG_TAG         gpio_test

#define HDF_GPIO_STACK_SIZE 0x1000
#define HDF_GPIO_TASK_NAME "hdf_gpio_test_task"
#define HDF_GPIO_TASK_PRIORITY 25

static int32_t TestCaseGpioIrqHandler1(uint16_t gpio, void *data)
{
    HDF_LOGE("%s: irq triggered! on gpio:%u, data=%p", __func__, gpio, data);
    uint16_t val = 0;
    GpioRead(0, &val);
    if (val) {
        GpioWrite(2, 1);
        GpioWrite(1, 1);
        GpioWrite(0, 0); // red
    } else {
        GpioWrite(2, 1);
        GpioWrite(1, 1);
        GpioWrite(0, 1);
    }
    return 0;
}

static int32_t TestCaseGpioIrqHandler2(uint16_t gpio, void *data)
{
    HDF_LOGE("%s: irq triggered! on gpio:%u, data=%p", __func__, gpio, data);
    uint16_t val = 0;
    GpioRead(1, &val);
    if (val) {
        GpioWrite(2, 1);
        GpioWrite(1, 0); // green
        GpioWrite(0, 1);
    } else {
        GpioWrite(2, 1);
        GpioWrite(1, 1);
        GpioWrite(0, 1);
    }
    return 0;
}

static void HdfGpioTestEntry(void* arg)
{
    (void)arg;
    int32_t ret;
    uint16_t mode;
    uint16_t gpio_key0 = 3;
    uint16_t gpio_key1 = 4;

    GpioWrite(0, 1); // close
    GpioWrite(1, 1); // close
    GpioWrite(2, 0); // open blue

    mode = OSAL_IRQF_TRIGGER_FALLING;
    HDF_LOGI("%s: mode:%0x\n", __func__, mode);

    ret = GpioSetIrq(gpio_key0, mode, TestCaseGpioIrqHandler1, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set irq fail! ret:%d\n", __func__, ret);
        return ;
    }

    ret = GpioSetIrq(gpio_key1, mode, TestCaseGpioIrqHandler2, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set irq fail! ret:%d\n", __func__, ret);
        return ;
    }

    ret = GpioEnableIrq(gpio_key0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: enable irq fail! ret:%d\n", __func__, ret);
        return ;
    }

    ret = GpioEnableIrq(gpio_key1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: enable irq fail! ret:%d\n", __func__, ret);
        return ;
    }

}

void StartHdfGpioTest(void)
{
    osThreadAttr_t attr;

    attr.name = HDF_GPIO_TASK_NAME;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = HDF_GPIO_STACK_SIZE;
    attr.priority = HDF_GPIO_TASK_PRIORITY;

    if (osThreadNew((osThreadFunc_t)HdfGpioTestEntry, NULL, &attr) == NULL) {
        printf("Failed to create thread1!\n");
    }
}

OHOS_APP_RUN(StartHdfGpioTest);
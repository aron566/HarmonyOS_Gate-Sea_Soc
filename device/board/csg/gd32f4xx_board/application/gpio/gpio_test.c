#include <stdio.h>
#include "osal_irq.h"
#include "osal_time.h"
#include "cmsis_os2.h"
#include "hdf_log.h"
#include "gpio_if.h"
#include "los_task.h"

#define HDF_GPIO_STACK_SIZE        0x1000
#define HDF_GPIO_TASK_NAME         "hdf_gpio_test_task"
#define HDF_GPIO_TASK_PRIORITY     20

static int32_t TestCaseGpioIrqHandler1(uint16_t gpio, void *data)
{
    HDF_LOGI("%s: Enter! [line = %d]\r\n", __func__, __LINE__);
}

static int32_t TestCaseGpioIrqHandler2(uint16_t gpio, void *data)
{
    HDF_LOGI("%s: Enter! [line = %d]\r\n", __func__, __LINE__);
}

static void* HdfGpioTestEntry(void* arg)
{
    (void)arg;
    int32_t ret;
    uint16_t mode;
    uint16_t val;

    HDF_LOGE("%s: [line = %d]\n", __func__, __LINE__);

    GpioSetDir(0, 1);  // dir: output
    GpioWrite(0, 1);
    GpioSetDir(1, 1);  // dir: output
    GpioWrite(1, 1);

    GpioGetDir(0, &val);
    HDF_LOGI("%s: [line = %d] [dir = %d]\n", __func__, __LINE__, val);

    GpioRead(0, &val); // read output value
    HDF_LOGI("%s: [line = %d] [val = %d]\n", __func__, __LINE__, val);

    GpioSetDir(2, 0);  // dir: input
    GpioRead(2, &val); // read input value
    HDF_LOGI("%s: [line = %d] [val = %d]\n", __func__, __LINE__, val);

    mode = OSAL_IRQF_TRIGGER_RISING;
    HDF_LOGI("%s: mode:%0x\n", __func__, mode);

    ret = GpioSetIrq(3, mode, TestCaseGpioIrqHandler1, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set irq fail! ret:%d\n", __func__, ret);
        return NULL;
    }
    ret = GpioEnableIrq(3);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: enable irq fail! ret:%d\n", __func__, ret);
        return NULL;
    }

    ret = GpioSetIrq(4, mode, TestCaseGpioIrqHandler2, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set irq fail! ret:%d\n", __func__, ret);
        return NULL;
    }
    ret = GpioEnableIrq(4);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: enable irq fail! ret:%d\n", __func__, ret);
        return NULL;
    }

    ret = GpioDisableIrq(3);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: disable irq fail! ret:%d\n", __func__, ret);
        return NULL;
    }

    ret = GpioDisableIrq(4);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: disable irq fail! ret:%d\n", __func__, ret);
        return NULL;
    }

    ret = GpioEnableIrq(3);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: enable irq fail! ret:%d\n", __func__, ret);
        return NULL;
    }

    ret = GpioEnableIrq(4);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: enable irq fail! ret:%d\n", __func__, ret);
        return NULL;
    }

    while (1) {
        (VOID)LOS_TaskDelay(400);
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
        HDF_LOGE("Failed to create thread1!\n");
    }
}


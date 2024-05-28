#include <stdio.h>
#include <hdf_log.h>
#include <dac_if.h>
#include "cmsis_os2.h"
#include "osal_irq.h"
#include "osal_time.h"
#include "los_task.h"

#define HDF_DAC_STACK_SIZE 0x1000
#define HDF_DAC_TASK_NAME "hdf_dac_test_task"
#define HDF_DAC_TASK_PRIORITY 28

//设备号0
#define DAC_DEVICE_NUM 0    //0-DAC0, 1-DAC1
//通道号0
#define DAC_CHANNEL_NUM 0   //GD32F470有DAC0和DAC1,每个DAC有一个通道

static void HdfDactTestEntry(void* arg)
{
    HDF_LOGI("%s enter!\n", __FUNCTION__);
    (void *)arg;
    DevHandle dacHandle = NULL;
    int32_t ret;
    //设置要写入的val值:0~0xFFFF
    //uint32_t val = 32768;  //V = 3.3/2
    uint32_t val = 16384;  //V = 3.3/4

    /* 打开dac设备 */
    dacHandle = DacOpen(DAC_DEVICE_NUM);
    if(dacHandle == NULL) {
        HDF_LOGE("%s: Open DAC%u fail!", __func__, DAC_DEVICE_NUM);
        return;
    }

    ret = DacWrite(dacHandle, DAC_CHANNEL_NUM, val);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: DAC write fail!:%d", __func__, ret);
        DacClose(dacHandle);
        return;
    }

    HDF_LOGI("%s: DAC write successful!", __func__);

    // 访问完毕关闭DAC设备
    DacClose(dacHandle);

    while (1) {
        printf("TaskSample:DAC task create start!\n");
        osDelay(1000);
    }
}

void StartHdfDacTest(void)
{
    osThreadAttr_t attr;

    attr.name = HDF_DAC_TASK_NAME;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = HDF_DAC_STACK_SIZE;
    attr.priority = HDF_DAC_TASK_PRIORITY;

    if (osThreadNew((osThreadFunc_t)HdfDactTestEntry, NULL, &attr) == NULL) {
        HDF_LOGE("Failed to create thread1!\n");
    }
}

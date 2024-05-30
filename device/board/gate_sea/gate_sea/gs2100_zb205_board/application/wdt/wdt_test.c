#include "stdio.h"
#include "los_task.h"
#include "watchdog_if.h"
#include "hdf_log.h"
#include "cmsis_os2.h"
#include "osal_irq.h"
#include "osal_time.h"

#define HDF_WDT_STACK_SIZE 0x1000
#define HDF_WDT_TASK_NAME "hdf_wdt_test_task"
#define HDF_WDT_TASK_PRIORITY 22

/* WDT count period definition */
#define WATCHDOG_TEST_TIMEOUT     2
#define WATCHDOG_TEST_FEED_TIME   6

int32_t HdfWdtTestEntry(void)
{
    int16_t wdtId = 0;
    int32_t ret;

    DevHandle handle;

    ret = WatchdogOpen(wdtId, &handle);
    if(ret != 0) {
        HDF_LOGE("%s: WatchdogOpen fail! ret:%d\n", __func__, ret);
    }

    //设置超时时间为2s
    ret = WatchdogSetTimeout(handle, WATCHDOG_TEST_TIMEOUT);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set timeout fail! ret:%d\n", __func__, ret);
        WatchdogClose(handle);
        return ret;
    }

    //启动开门狗,开始计时
    ret = WatchdogStart(handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: satrt fail! ret:%d\n", __func__, ret);
        WatchdogClose(handle);
        return ret;
    }

    //每隔1s喂狗一次
    for (int i = 0; i < WATCHDOG_TEST_FEED_TIME; i++) {
        HDF_LOGD("%s: feeding watchdog %d times..., tick(%d) \n", __func__, i, LOS_TickCountGet());
        LOS_TaskDelay(1000);
        ret = WatchdogFeed(handle);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: feed dog fail! ret:%d\n", __func__, ret);
            WatchdogClose(handle);
            return ret;
        }
    }

    WatchdogClose(handle);

    //超过2s不进行喂狗，系统复位
    for(int i = 0; i <= WATCHDOG_TEST_FEED_TIME; i++)
    {
        HDF_LOGI("%s: %d wait for watchdog reset, tick(%d) \n", __func__, i, LOS_TickCountGet());
        LOS_TaskDelay(500);
    }

    return ret;
}

void StartHdfWdtTest(void)
{
    osThreadAttr_t attr;

    attr.name = HDF_WDT_TASK_NAME;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = HDF_WDT_STACK_SIZE;
    attr.priority = HDF_WDT_TASK_PRIORITY;

    if (osThreadNew((osThreadFunc_t)HdfWdtTestEntry, NULL, &attr) == NULL) {
        HDF_LOGE("Failed to create thread1!\n");
    }
}


#include <stdio.h>
#include <hdf_log.h>
#include "cmsis_os2.h"
#include "osal_irq.h"
#include "osal_time.h"
#include "rtc_if.h"
#include "rtc_base.h"
#include "los_task.h"

#define HDF_LOG_TAG              rtc_test

#define HDF_RTC_STACK_SIZE 0x1000
#define HDF_RTC_TASK_NAME "hdf_rtc_test_task"
#define HDF_RTC_TASK_PRIORITY 24

static int32_t RtcAlarmXCallback_A(enum RtcAlarmIndex alarmIndex)
{
    if (alarmIndex == RTC_ALARM_INDEX_A) {
        HDF_LOGI("%s: [line = %d]\n", __func__, __LINE__);
    } else {
        HDF_LOGI("%s: [line = %d]\n", __func__, __LINE__);
    }
    return 0;
}

static int32_t RtcAlarmXCallback_B(enum RtcAlarmIndex alarmIndex)
{
    if (alarmIndex == RTC_ALARM_INDEX_A) {
        HDF_LOGI("%s: [line = %d]\n", __func__, __LINE__);
    } else {
        HDF_LOGI("%s: [line = %d]\n", __func__, __LINE__);
    }
    return 0;
}

int32_t HdfRtcTestEntry(void)
{
    //(void)arg;
    DevHandle rtc_handle = NULL;
    struct RtcTime time;
    int32_t ret;

    /* 获取RTC句柄 */
    rtc_handle = RtcOpen();
    if(rtc_handle == NULL) {
        HDF_LOGE("RtcOpen: rtc failed!\n");
        return ;
    }

    /* 设置RTC时间 */
    time.year = 2023;
    time.month = 8;
    time.day = 29;
    time.weekday = 1;
    time.hour = 22;
    time.minute = 22;
    time.second = 22;
    time.millisecond = 0;

    ret = RtcWriteTime(rtc_handle, &time);
    if(ret != 0) {
        HDF_LOGE("RtcWriteTime: rtc failed!\n");
        return ;
    }


#if 0
    /* 复位RTC时间 */
    ret = RtcReset(rtc_handle);//初始化时间为2000 1 1 周六 20点20分20秒
    if(ret != 0) {
        HDF_LOGE("RtcReset: rtc failed!\n");
        return ;
    }
#endif

    /* 读取RTC时间 */
    ret = RtcReadTime(rtc_handle, &time);
    if(ret != 0) {
        HDF_LOGE("RtcReadTime: rtc failed!\n");
        return ;
    }
    HDF_LOGI("[%s] readTime=> year-month-day-weekday-hour-minute-second 20%02d-%02d-%02d-%d-%02d-%02d-%02d!\n", __func__, 
        time.year, time.month, time.day, time.weekday, time.hour, time.minute, time.second);

    /* 设置ALARMA */
    time.year = 2023;
    time.month = 8;
    time.day = 29;
    time.weekday = 1;
    time.hour = 22;
    time.minute = 23;
    time.second = 23;
    time.millisecond = 0;

    ret = RtcWriteAlarm(rtc_handle, RTC_ALARM_INDEX_A, &time);
    if(ret != 0) {
        HDF_LOGE("RtcWriteAlarm: rtc failed!\n");
        return ret;
    }

    ret = RtcRegisterAlarmCallback(rtc_handle, RTC_ALARM_INDEX_A, RtcAlarmXCallback_A);
    if(ret != 0) {
        HDF_LOGE("RtcRegisterAlarmCallback: rtc failed!\n");
        return ;
    }

    /* 使能ALARMA */
    ret = RtcAlarmInterruptEnable(rtc_handle, RTC_ALARM_INDEX_A, 1);
    if(ret != 0) {
        HDF_LOGE("RtcAlarmInterruptEnable: rtc failed!\n");
        return ;
    }

    /* 读ALARMA */
    ret = RtcReadAlarm(rtc_handle, RTC_ALARM_INDEX_A, &time);
    if(ret != 0) {
        HDF_LOGE("RtcReadAlarm: rtc failed!\n");
        return ret;
    }
    HDF_LOGI("[%s] ALARMA: day-hour-minute-second %d-%d-%d-%d!\n", __func__,
            time.day, time.hour, time.minute, time.second);

    /* 设置ALARMB */
    time.year = 2023;
    time.month = 8;
    time.day = 29;
    time.weekday = 1;
    time.hour = 22;
    time.minute = 23;
    time.second = 33;
    time.millisecond = 0;

    ret = RtcWriteAlarm(rtc_handle, RTC_ALARM_INDEX_B, &time);
    if(ret != 0) {
        HDF_LOGE("RtcWriteAlarm: rtc failed!\n");
        return ret;
    }

    ret = RtcRegisterAlarmCallback(rtc_handle, RTC_ALARM_INDEX_B, RtcAlarmXCallback_B);
    if(ret != 0) {
        HDF_LOGE("RtcRegisterAlarmCallback: rtc failed!\n");
        return ;
    }

    /* 使能ALARMB */
    ret = RtcAlarmInterruptEnable(rtc_handle, RTC_ALARM_INDEX_B, 1);
    if(ret != 0) {
        HDF_LOGE("RtcAlarmInterruptEnable: rtc failed!\n");
        return ;
    }
    /* 关闭ALARMB */
    ret = RtcAlarmInterruptEnable(rtc_handle, RTC_ALARM_INDEX_B, 0);
    if(ret != 0) {
        HDF_LOGE("RtcAlarmInterruptEnable: rtc failed!\n");
        return ;
    }
    /* 使能ALARMB */
    ret = RtcAlarmInterruptEnable(rtc_handle, RTC_ALARM_INDEX_B, 1);
    if(ret != 0) {
        HDF_LOGE("RtcAlarmInterruptEnable: rtc failed!\n");
        return ;
    }

    /* 读ALARMB */
    ret = RtcReadAlarm(rtc_handle, RTC_ALARM_INDEX_B, &time);
    if(ret != 0) {
        HDF_LOGE("RtcReadAlarm: rtc failed!\n");
        return ret;
    }
    HDF_LOGI("[%s] ALARMB: day-hour-minute-second %d-%d-%d-%d!\n", __func__,
            time.day, time.hour, time.minute, time.second);

    while (1) {
        ret = RtcReadTime(rtc_handle, &time);
        if(ret != 0) {
            HDF_LOGE("RtcReadTime: rtc failed!\n");
            return;
        }
        HDF_LOGI("[%s] readTime=> year-month-day-weekday-hour-minute-second 20%02d-%02d-%d-%02d-%02d-%02d-%02d!\n", __func__, 
            time.year, time.month, time.day, time.weekday, time.hour, time.minute, time.second);
        osDelay(1000);
    }
}

void StartHdfRtcTest(void)
{
    osThreadAttr_t attr;

    attr.name = HDF_RTC_TASK_NAME;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = HDF_RTC_STACK_SIZE;
    attr.priority = HDF_RTC_TASK_PRIORITY;

    if (osThreadNew((osThreadFunc_t)HdfRtcTestEntry, NULL, &attr) == NULL) {
        HDF_LOGE("Failed to create thread1!\n");
    }
}


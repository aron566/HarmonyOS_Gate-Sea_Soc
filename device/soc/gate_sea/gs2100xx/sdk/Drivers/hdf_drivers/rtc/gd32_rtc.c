/*
 * Copyright (c) 2023 Hihope Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "stdio.h"
#include "hdf_log.h"
#include "device_resource_if.h"
#include "rtc_core.h"
#include "gd32_rtc.h"

#define DEBUG_RTC_LOG            0
#define BKP_VALUE           0x3210

rtc_parameter_struct   rtc_initpara;
rtc_alarm_struct  rtc_alarm;
RTC_GD g_RTCGroup[2] = {0};
uint32_t RTCSRC_FLAG = 0;
uint32_t prescaler_a = 0, prescaler_s = 0;

Gd32RtcConfig *rtcInfo = NULL;

uint8_t BcdToDec(uint8_t temp)
{
    return ((temp / 16) * 10 + (temp & 0xF));
}

uint8_t DecToBcd(uint8_t temp)
{
    return ((temp / 10) * 16 + (temp % 10));
}

int32_t Gd32RtcReadTime(struct RtcHost *host, struct RtcTime *time)
{
    if(host == NULL || host->data == NULL) {
        HDF_LOGE("%s: host or host->data is null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if(time == NULL) {
        HDF_LOGE("%s: time is null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    rtc_current_time_get(&rtc_initpara);

#if DEBUG_RTC_LOG
    HDF_LOGI("%s: [line = %d] [date:20%x-%x-%x, time:%x:%x:%x, week:%x]\n", __func__, __LINE__,\
            rtc_initpara.year, rtc_initpara.month, rtc_initpara.date, rtc_initpara.hour, rtc_initpara.minute,\
            rtc_initpara.second, rtc_initpara.day_of_week);
#endif

    time->year = BcdToDec(rtc_initpara.year);
    time->month = BcdToDec(rtc_initpara.month);
    time->day = BcdToDec(rtc_initpara.date);
    time->weekday = BcdToDec(rtc_initpara.day_of_week);
    time->hour= BcdToDec(rtc_initpara.hour);
    time->minute = BcdToDec(rtc_initpara.minute);
    time->second = BcdToDec(rtc_initpara.second);

    return HDF_SUCCESS;
}

int32_t Gd32RtcWriteTime(struct RtcHost *host, struct RtcTime *time)
{
    if(host == NULL || host->data == NULL) {
        HDF_LOGE("%s: host or host->data is null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if(time == NULL) {
        HDF_LOGE("%s: time is null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    Gd32RtcConfig *rtcInfo = (struct Gd32RtcConfig *)(host->data);
    if(rtcInfo == NULL) {
        HDF_LOGE("%s: rtcInfo is null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    rtc_initpara.factor_asyn = prescaler_a;
    rtc_initpara.factor_syn = prescaler_s;
    rtc_initpara.year = DecToBcd(time->year - 2000);
    rtc_initpara.month = DecToBcd(time->month);
    rtc_initpara.date = DecToBcd(time->day);
    rtc_initpara.hour = DecToBcd(time->hour);
    rtc_initpara.minute = DecToBcd(time->minute);
    rtc_initpara.second = DecToBcd(time->second);
    rtc_initpara.day_of_week = DecToBcd(time->weekday);
    rtc_initpara.display_format = g_DisFormat[rtcInfo->hourFormat];
    rtc_initpara.am_pm = g_AmPm[rtcInfo->AmPm];

#if DEBUG_RTC_LOG
    HDF_LOGI("%s: [line = %d] [date:20%x-%x-%x, time:%x:%x:%x, week:%x]\n", __func__, __LINE__,\
            rtc_initpara.year, rtc_initpara.month, rtc_initpara.date, rtc_initpara.hour, rtc_initpara.minute,\
            rtc_initpara.second, rtc_initpara.day_of_week);
#endif

    /* RTC current time configuration */
    if(ERROR == rtc_init(&rtc_initpara)){
        HDF_LOGI("RTC time configuration failed!\n\r");
    }else{
        HDF_LOGI("RTC time configuration success!\n\r");
        RTC_BKP0 = BKP_VALUE;
    }

    return HDF_SUCCESS;
}

void RTC_Alarm_test(void)
{
    if(RESET != rtc_flag_get(RTC_FLAG_ALRM0)){
        rtc_flag_clear(RTC_FLAG_ALRM0);
        exti_flag_clear(EXTI_17);
        g_RTCGroup[RTC_ALARM_INDEX_A].func(RTC_ALARM_INDEX_A);
    }
    if(RESET != rtc_flag_get(RTC_FLAG_ALRM1)){
        rtc_flag_clear(RTC_FLAG_ALRM1);
        exti_flag_clear(EXTI_17);
        g_RTCGroup[RTC_ALARM_INDEX_B].func(RTC_ALARM_INDEX_B);
    }
}

static int32_t GDReadAlarm(struct RtcHost *host, enum RtcAlarmIndex alarmIndex, struct RtcTime *time)
{
    rtc_alarm_get(g_Rtc_Alarm[alarmIndex], &rtc_alarm);

#if DEBUG_RTC_LOG
    HDF_LOGI("The alarm: %0.2x:%0.2x:%0.2x \n\r", rtc_alarm.alarm_hour, rtc_alarm.alarm_minute,\
           rtc_alarm.alarm_second);
#endif

    time->day = BcdToDec(rtc_alarm.alarm_day);
    time->hour= BcdToDec(rtc_alarm.alarm_hour);
    time->minute = BcdToDec(rtc_alarm.alarm_minute);
    time->second = BcdToDec(rtc_alarm.alarm_second);

    return HDF_SUCCESS;
}

int32_t Gd32WriteAlarm(struct RtcHost *host, enum RtcAlarmIndex alarmIndex, struct RtcTime *time)
{
    int32_t i32Ret;

    if(host == NULL || host->data == NULL) {
        HDF_LOGE("%s: host or host->data is null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    Gd32RtcConfig *rtcInfo = (struct Gd32RtcConfig *)(host->data);
    if(rtcInfo == NULL) {
        HDF_LOGE("%s: rtcInfo is null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

#if DEBUG_RTC_LOG
    HDF_LOGI("%s: [line = %d] [hour = %d] [minute = %d] [second = %d] [alarmIndex = %d]\n", __func__, __LINE__,\
                time->hour, time->minute, time->second, alarmIndex);
#endif

    rtc_alarm_disable(g_Rtc_Alarm[alarmIndex]);
    //rtc_alarm.alarm_mask = RTC_ALARM_DATE_MASK | RTC_ALARM_HOUR_MASK;
    rtc_alarm.weekday_or_date = RTC_ALARM_DATE_SELECTED;
    rtc_alarm.alarm_day = DecToBcd(time->day);
    rtc_alarm.am_pm = g_AmPm[rtcInfo->alarmAmPm];
    rtc_alarm.alarm_hour = DecToBcd(time->hour);
    rtc_alarm.alarm_minute = DecToBcd(time->minute);
    rtc_alarm.alarm_second = DecToBcd(time->second);
    rtc_alarm_config(g_Rtc_Alarm[alarmIndex],&rtc_alarm);

    rtc_alarm_get(g_Rtc_Alarm[alarmIndex],&rtc_alarm);
    HDF_LOGI("The alarm: %0.2x:%0.2x:%0.2x \n\r", rtc_alarm.alarm_hour, rtc_alarm.alarm_minute,\
           rtc_alarm.alarm_second);

    return HDF_SUCCESS;
}

void rtc_reset(void)
{
    /* 初始化时间为2000 1 1 周六 20点20分20秒 */
    rtc_initpara.factor_asyn = prescaler_a;
    rtc_initpara.factor_syn = prescaler_s;
    rtc_initpara.year = 0x0;
    rtc_initpara.month = 0x01;
    rtc_initpara.date = 0x01;
    rtc_initpara.day_of_week = RTC_SATURDAY;
    rtc_initpara.hour = 0x20;
    rtc_initpara.minute = 0x20;
    rtc_initpara.second = 0x20;
    rtc_initpara.display_format = g_DisFormat[rtcInfo->hourFormat];
    rtc_initpara.am_pm = g_AmPm[rtcInfo->AmPm];

    /* RTC current time configuration */
    if(ERROR == rtc_init(&rtc_initpara)){
        HDF_LOGE("RTC time configuration failed!\n\r");
    }else{
        HDF_LOGE("RTC time configuration success!\n\r");
        RTC_BKP0 = BKP_VALUE;
    }
}

static int32_t Gd32Reset(struct RtcHost *host)
{
    if(host == NULL || host->data == NULL) {
        HDF_LOGE("%s: host or host->data is null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    Gd32RtcConfig *rtcInfo = (struct Gd32RtcConfig *)(host->data);
    if(rtcInfo == NULL) {
        HDF_LOGE("%s: rtcInfo is null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    rtc_reset();

    return HDF_SUCCESS;
}

static int32_t GDRegisterAlarmCallback(struct RtcHost *host, enum RtcAlarmIndex alarmIndex, RtcAlarmCallback cb)
{
    g_RTCGroup[alarmIndex].func = cb;

    return HDF_SUCCESS;
}

int32_t Gd32AlarmInterruptEnable(struct RtcHost *host, enum RtcAlarmIndex alarmIndex, uint8_t enable)
{
    int32_t i32Ret;

    if(host == NULL || host->data == NULL) {
        HDF_LOGE("%s: host or host->data is null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    Gd32RtcConfig *rtcInfo = (struct Gd32RtcConfig *)(host->data);
    if(rtcInfo == NULL) {
        HDF_LOGE("%s: rtcInfo is null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if(enable == 1) {
        rtc_interrupt_enable(g_Rtc_Int_Alarm[alarmIndex]);
        rtc_alarm_enable(g_Rtc_Alarm[alarmIndex]);
        OsSetVector(RTC_Alarm_IRQn, RTC_Alarm_test, NULL);
    }
    else if(enable == 0) {
        rtc_interrupt_disable(g_Rtc_Int_Alarm[alarmIndex]);
        rtc_alarm_disable(g_Rtc_Alarm[alarmIndex]);
    }

    return HDF_SUCCESS;
}

static struct RtcMethod g_gd32RtcMethod = {
    .ReadTime = Gd32RtcReadTime,
    .WriteTime = Gd32RtcWriteTime,
    .ReadAlarm = GDReadAlarm,
    .WriteAlarm = Gd32WriteAlarm,
    .RegisterAlarmCallback = GDRegisterAlarmCallback,
    .AlarmInterruptEnable = Gd32AlarmInterruptEnable,
    .GetFreq = NULL,
    .SetFreq = NULL,
    .Reset = Gd32Reset,
    .ReadReg = NULL,
    .WriteReg = NULL,
};

void RtcPreConfig(void)
{
    rcu_osci_on(RCU_LXTAL);
    rcu_osci_stab_wait(RCU_LXTAL);
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
    prescaler_s = 0xFF;
    prescaler_a = 0x7F;
    rcu_periph_clock_enable(RCU_RTC);
    rtc_register_sync_wait();
}

static void RtcConfig(void)
{
    /* enable PMU clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* enable the access of the RTC registers */
    pmu_backup_write_enable();

    RtcPreConfig();
    /* get RTC clock entry selection */
    RTCSRC_FLAG = GET_BITS(RCU_BDCTL, 8, 9);

    /* check if RTC has aready been configured */
    if ((BKP_VALUE != RTC_BKP0) || (0x00 == RTCSRC_FLAG)){
        rtc_reset();
    }else{
        HDF_LOGI("no need to configure RTC!\n\r");
    }

    rcu_all_reset_flag_clear();

    /* alarm irq set */
    rtc_flag_clear(RTC_FLAG_ALRM0);
    rtc_flag_clear(RTC_FLAG_ALRM1);
    exti_flag_clear(EXTI_17);
    /* RTC alarm interrupt configuration */
    exti_init(EXTI_17, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    nvic_irq_enable(RTC_Alarm_IRQn, 0, 0);
}

static int32_t GetRtcDeviceByHcs(Gd32RtcConfig *rtcInfo, const struct DeviceResourceNode *resourceNode)
{
    if (rtcInfo == NULL || resourceNode == NULL) {
        HDF_LOGE("%s: rtcInfo or resourceNode is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    struct DeviceResourceIface *dri = NULL;
    dri = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (dri == NULL || dri->GetUint16 == NULL || dri->GetUint8 == NULL || dri->GetUint32 == NULL) {
        HDF_LOGE("DeviceResourceIface is invalid\r\n");
        return HDF_ERR_INVALID_PARAM;
    }

    if (dri->GetUint32(resourceNode, "RtcId", &rtcInfo->rtcId, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "RtcId");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: rtcInfo->rtcId %d", __func__, rtcInfo->rtcId);

    if (dri->GetUint8(resourceNode, "HourFormat", &rtcInfo->hourFormat, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "HourFormat");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: rtcInfo->hourFormat %d", __func__, rtcInfo->hourFormat);

    if (dri->GetUint8(resourceNode, "AlarmAmPm", &rtcInfo->alarmAmPm, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "AlarmAmPm");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: rtcInfo->alarmAmPm %d", __func__, rtcInfo->alarmAmPm);

    if (dri->GetUint8(resourceNode, "AmPm", &rtcInfo->AmPm, 0) != HDF_SUCCESS) {
        HDF_LOGE("get config %s failed\r\n", "AmPm");
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: rtcInfo->AmPm %d", __func__, rtcInfo->AmPm);

    return HDF_SUCCESS;
}

static int32_t Gd32RtcInit(struct HdfDeviceObject *device)
{
    struct RtcHost *host = NULL;
    uint32_t ret;

    if(device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or device->property is null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    host = RtcHostFromDevice(device);
    if (host == NULL) {
        HDF_LOGE("%s: RtcHostFromDevice fail!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    rtcInfo = OsalMemCalloc(sizeof(Gd32RtcConfig));
    if (rtcInfo == NULL) {
        HDF_LOGE("%s: OsalMemAlloc rtcInfo error\r\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = GetRtcDeviceByHcs(rtcInfo, device->property);
    if (ret != HDF_SUCCESS) {
        (void)OsalMemFree(rtcInfo);
        return HDF_FAILURE;
    }

    RtcConfig();

    host->method = &g_gd32RtcMethod;
    host->data = rtcInfo;

    return HDF_SUCCESS;
}

static int32_t Gd32RtcBind(struct HdfDeviceObject *device)
{
    if(device == NULL) {
        HDF_LOGE("%s: device is null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    struct RtcHost *host = NULL;
    host = RtcHostCreate(device);

    if (host == NULL) {
        HDF_LOGE("%s: create host fail!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    device->service = &host->service;
    return HDF_SUCCESS;
}

static int32_t Gd32RtcRelease(struct HdfDeviceObject *device)
{
    struct RtcHost *host = NULL;
    Gd32RtcConfig *rtcInfo = NULL;

    if(device == NULL) {
        HDF_LOGE("%s: device is null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    host = RtcHostFromDevice(device);
    rtcInfo = (struct RtcConfigInfo *)host->data;
    if (rtcInfo != NULL) {
        OsalMemFree(rtcInfo);
        host->data = NULL;
    }
    RtcHostDestroy(host);
}

struct HdfDriverEntry g_gd32RtcDriverEntry = {
  .moduleVersion = 1,
  .Bind = Gd32RtcBind,
  .Init = Gd32RtcInit,
  .Release = Gd32RtcRelease,
  .moduleName = "GD_HDF_PLATFORM_RTC",
};

HDF_INIT(g_gd32RtcDriverEntry);

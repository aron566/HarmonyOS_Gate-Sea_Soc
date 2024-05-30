#ifndef __GD32_RTC_H
#define __GD32_RTC_H

#include "hal_gpio.h"
#include "gd32f4xx_rtc.h"
#include "gd32f4xx_rcu.h"
#include "gd32f4xx_pmu.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MOUNTH_MAX             12

typedef struct {
    uint32_t rtcId;
    uint8_t hourFormat;
    uint8_t alarmAmPm;
    uint8_t AmPm;
} Gd32RtcConfig;

static const uint8_t g_u8RtcMonth[MOUNTH_MAX] = {
    RTC_JAN,
    RTC_FEB,
    RTC_MAR,
    RTC_APR,
    RTC_MAY_GD,
    RTC_JUN,
    RTC_JUL,
    RTC_AUG_GD,
    RTC_SEP,
    RTC_OCT,
    RTC_NOV,
    RTC_DEC,
};

static const uint32_t g_DisFormat[2] = {
    RTC_24HOUR,
    RTC_12HOUR,
};

static const uint32_t g_AmPm[2] = {
    RTC_AM,
    RTC_PM,
};

static const uint8_t g_Rtc_Alarm[2] = {
    RTC_ALARM0,
    RTC_ALARM1,
};

static const uint32_t g_Rtc_Int_Alarm[2] = {
    RTC_INT_ALARM0,
    RTC_INT_ALARM1,
};

typedef struct {
    uint16_t num;
    RtcAlarmCallback func;
} RTC_GD;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __GD32_RTC_H */

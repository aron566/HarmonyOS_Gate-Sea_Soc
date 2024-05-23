#ifndef GD32_WATCHDOG_H
#define GD32_WATCHDOG_H

#include "stdio.h"
#include "main.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define WATCHDOG_CLOCKDIV_MAX               8

typedef struct {
    int32_t watchdog_Id;
    struct WatchdogCntlr wdt;
    uint32_t countPeriod;
    uint32_t ReloadMult;
} Gd32Watchdog;

static const uint8_t g_watchdogClockDiv[WATCHDOG_CLOCKDIV_MAX] = {
    FWDGT_PSC_DIV4,
    FWDGT_PSC_DIV8,
    FWDGT_PSC_DIV16,
    FWDGT_PSC_DIV32,
    FWDGT_PSC_DIV64,
    FWDGT_PSC_DIV128,
    FWDGT_PSC_DIV256,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* GD32_WATCHDOG_H */



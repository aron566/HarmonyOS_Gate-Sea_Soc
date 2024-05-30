/*
 * Copyright (c) 2021 HiHope Open Source Organization.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _GD32_PWM_H
#define _GD32_PWM_H

#include "stdio.h"
#include "main.h"
#include "gd32f4xx_timer.h"
#include "hal_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    uint8_t ch_num;
    uint8_t *port;
    uint8_t *pin;
    uint16_t *func;
    uint8_t *channel;
} Gd32PwmConfig;

typedef struct {
    struct PwmDev dev;
    uint8_t timer;
    Gd32PwmConfig gd32PwmCfg;
} GD32HrPwm;

const uint32_t g_tmrMapCmd[14] = {
    TIMER0,
    TIMER1,
    TIMER2,
    TIMER3,
    TIMER4,
    TIMER5,
    TIMER6,
    TIMER7,
    TIMER8,
    TIMER9,
    TIMER10,
    TIMER11,
    TIMER12,
    TIMER13,
};

const uint32_t g_rcu_tmrMapCmd[14] = {
    RCU_TIMER0,
    RCU_TIMER1,
    RCU_TIMER2,
    RCU_TIMER3,
    RCU_TIMER4,
    RCU_TIMER5,
    RCU_TIMER6,
    RCU_TIMER7,
    RCU_TIMER8,
    RCU_TIMER9,
    RCU_TIMER10,
    RCU_TIMER11,
    RCU_TIMER12,
    RCU_TIMER13,
};

const uint16_t g_tmrchMapCmd[4] = {
    TIMER_CH_0,
    TIMER_CH_1,
    TIMER_CH_2,
    TIMER_CH_3,
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _GD32_PWM_H */
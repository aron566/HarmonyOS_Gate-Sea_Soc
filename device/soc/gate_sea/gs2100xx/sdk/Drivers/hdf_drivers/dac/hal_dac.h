
#ifndef __HAL_DAC_H
#define __HAL_DAC_H
#include "gd32f4xx_dac.h"

#include "dac_if.h"
#include "dac_core.h"

#ifdef __cplusplus
extern "C" {
#endif

struct GD32DacDevice {
    struct DacDevice device;         // 【必要】是核心层控制对象，必须作为自定义结构体的首个成员，其具体描述见下方。
    uint32_t deviceNum;              // 【必要】设备号
    uint32_t dacNum;                 // 【必要】dac号
    uint32_t validChannel;           // 【必要】有效通道
};

static const uint32_t g_map_dac[2] = {
    DAC0,
    DAC1,
};

static const uint32_t g_map_dac_pin[2] = {
    GPIO_PIN_4,
    GPIO_PIN_5,
};

#ifdef __cplusplus
}
#endif

#endif /* __HAL_DAC_H */
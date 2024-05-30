#ifndef __I2C_GD32F4XX_H__
#define __I2C_GD32F4XX_H__

#include "i2c_core.h"
#include "hal_gpio.h"
#include "gd32f4xx_rcu.h"
#include "gd32f4xx_gpio.h"

#define GD32F4XX_I2C_MAX  3

static const uint32_t g_i2c_BaseMaps[GD32F4XX_I2C_MAX] = {
    I2C0,
    I2C1,
    I2C2,
};

static const uint16_t g_i2c_rcuPeriphMaps[GD32F4XX_I2C_MAX] = {
    RCU_I2C0,
    RCU_I2C1,
    RCU_I2C2,
};

#endif


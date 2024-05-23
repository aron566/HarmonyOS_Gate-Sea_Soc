
#ifndef __HAL_GPIO_H
#define __HAL_GPIO_H

#include "gpio_core.h"
#include "gpio_if.h"
#include "hc32_ll_gpio.h"
#include "hc32_ll_interrupts.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HC32_GPIO_PIN_0 = 0, /* Pin 0 selected    */
    HC32_GPIO_PIN_1,     /* Pin 1 selected    */
    HC32_GPIO_PIN_2,     /* Pin 2 selected    */
    HC32_GPIO_PIN_3,     /* Pin 3 selected    */
    HC32_GPIO_PIN_4,     /* Pin 4 selected    */
    HC32_GPIO_PIN_5,     /* Pin 5 selected    */
    HC32_GPIO_PIN_6,     /* Pin 6 selected    */
    HC32_GPIO_PIN_7,     /* Pin 7 selected    */
    HC32_GPIO_PIN_8,     /* Pin 8 selected    */
    HC32_GPIO_PIN_9,     /* Pin 9 selected    */
    HC32_GPIO_PIN_10,    /* Pin 10 selected   */
    HC32_GPIO_PIN_11,    /* Pin 11 selected   */
    HC32_GPIO_PIN_12,    /* Pin 12 selected   */
    HC32_GPIO_PIN_13,    /* Pin 13 selected   */
    HC32_GPIO_PIN_14,    /* Pin 14 selected   */
    HC32_GPIO_PIN_15,    /* Pin 15 selected   */
    HC32_GPIO_PIN_MAX,   /* Pin Max */
} HC32_GPIO_PIN;

typedef enum {
    HC32_GPIO_GROUP_A = 0,
    HC32_GPIO_GROUP_B,
    HC32_GPIO_GROUP_C,
    HC32_GPIO_GROUP_D,
    HC32_GPIO_GROUP_E,
    HC32_GPIO_GROUP_F,
    HC32_GPIO_GROUP_G,
    HC32_GPIO_GROUP_H,
    HC32_GPIO_GROUP_I,
    HC32_GPIO_GROUP_MAX,
}HC32_GPIO_GROUP;


static const uint16_t g_hcRealPinMaps[HC32_GPIO_PIN_MAX] = {
    GPIO_PIN_00,
    GPIO_PIN_01,
    GPIO_PIN_02,
    GPIO_PIN_03,
    GPIO_PIN_04,
    GPIO_PIN_05,
    GPIO_PIN_06,
    GPIO_PIN_07,
    GPIO_PIN_08,
    GPIO_PIN_09,
    GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15,
};

static const uint32_t g_gpioExitLineMap[HC32_GPIO_PIN_MAX] = {
    EXTINT_CH00,
    EXTINT_CH01,
    EXTINT_CH02,
    EXTINT_CH03,
    EXTINT_CH04,
    EXTINT_CH05,
    EXTINT_CH06,
    EXTINT_CH07,
    EXTINT_CH08,
    EXTINT_CH09,
    EXTINT_CH10,
    EXTINT_CH11,
    EXTINT_CH12,
    EXTINT_CH13,
    EXTINT_CH14,
    EXTINT_CH15,
};

static const uint8_t g_gpioxMaps[HC32_GPIO_GROUP_MAX] = {
    GPIO_PORT_A,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D,
    GPIO_PORT_E,
    GPIO_PORT_F,
    GPIO_PORT_G,
    GPIO_PORT_H,
    GPIO_PORT_I,
};

uint32_t LL_GET_HAL_PIN(HC32_GPIO_PIN pin);

#ifdef __cplusplus
}
#endif

#endif /* __HAL_GPIO_H */

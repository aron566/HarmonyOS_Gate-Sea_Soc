
#ifndef __HAL_GPIO_H
#define __HAL_GPIO_H

#include "gpio_core.h"
#include "gpio_if.h"
#include "gd32f4xx_gpio.h"
#include "gd32f4xx_exti.h"
#include "gd32f4xx_misc.h"
#include "gd32f4xx_syscfg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GD32_GPIO_PIN_0 = 0, /* Pin 0 selected    */
    GD32_GPIO_PIN_1,     /* Pin 1 selected    */
    GD32_GPIO_PIN_2,     /* Pin 2 selected    */
    GD32_GPIO_PIN_3,     /* Pin 3 selected    */
    GD32_GPIO_PIN_4,     /* Pin 4 selected    */
    GD32_GPIO_PIN_5,     /* Pin 5 selected    */
    GD32_GPIO_PIN_6,     /* Pin 6 selected    */
    GD32_GPIO_PIN_7,     /* Pin 7 selected    */
    GD32_GPIO_PIN_8,     /* Pin 8 selected    */
    GD32_GPIO_PIN_9,     /* Pin 9 selected    */
    GD32_GPIO_PIN_10,    /* Pin 10 selected   */
    GD32_GPIO_PIN_11,    /* Pin 11 selected   */
    GD32_GPIO_PIN_12,    /* Pin 12 selected   */
    GD32_GPIO_PIN_13,    /* Pin 13 selected   */
    GD32_GPIO_PIN_14,    /* Pin 14 selected   */
    GD32_GPIO_PIN_15,    /* Pin 15 selected   */
    GD32_GPIO_PIN_MAX,   /* Pin Max */
} GD32_GPIO_PIN;

typedef enum {
    GD32_GPIO_GROUP_A = 0,
    GD32_GPIO_GROUP_B,
    GD32_GPIO_GROUP_C,
    GD32_GPIO_GROUP_D,
    GD32_GPIO_GROUP_E,
    GD32_GPIO_GROUP_F,
    GD32_GPIO_GROUP_G,
    GD32_GPIO_GROUP_H,
    GD32_GPIO_GROUP_I,
    GD32_GPIO_GROUP_MAX,
}GD32_GPIO_GROUP;

static const uint32_t g_GdRealPinMaps[GD32_GPIO_PIN_MAX] = {
    GPIO_PIN_0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7,
    GPIO_PIN_8,
    GPIO_PIN_9,
    GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15,
};

static const uint32_t g_gpioExitLineMap[GD32_GPIO_PIN_MAX] = {
    EXTI_0,
    EXTI_1,
    EXTI_2,
    EXTI_3,
    EXTI_4,
    EXTI_5,
    EXTI_6,
    EXTI_7,
    EXTI_8,
    EXTI_9,
    EXTI_10,
    EXTI_11,
    EXTI_12,
    EXTI_13,
    EXTI_14,
    EXTI_15,
};

static const uint32_t g_gpioxMaps[GD32_GPIO_GROUP_MAX] = {
    GPIOA,
    GPIOB,
    GPIOC,
    GPIOD,
    GPIOE,
    GPIOF,
    GPIOG,
    GPIOH,
    GPIOI,
};

static const uint32_t g_gpioAFMap[GD32_GPIO_PIN_MAX] = {
    GPIO_AF_0,
    GPIO_AF_1,
    GPIO_AF_2,
    GPIO_AF_3,
    GPIO_AF_4,
    GPIO_AF_5,
    GPIO_AF_6,
    GPIO_AF_7,
    GPIO_AF_8,
    GPIO_AF_9,
    GPIO_AF_10,
    GPIO_AF_11,
    GPIO_AF_12,
    GPIO_AF_13,
    GPIO_AF_14,
    GPIO_AF_15,
};

static const uint16_t g_rcuPeriphMaps[GD32_GPIO_PIN_MAX] = {
    RCU_GPIOA,
    RCU_GPIOB,
    RCU_GPIOC,
    RCU_GPIOD,
    RCU_GPIOE,
    RCU_GPIOF,
    RCU_GPIOG,
    RCU_GPIOH,
    RCU_GPIOI,
};

static const uint8_t g_ExtiIrqMaps[7] = {
    EXTI0_IRQn,
    EXTI1_IRQn,
    EXTI2_IRQn,
    EXTI3_IRQn,
    EXTI4_IRQn,
    EXTI5_9_IRQn,
    EXTI10_15_IRQn,
};

static const uint8_t g_ExtiSrcGpioMaps[GD32_GPIO_GROUP_MAX] = {
    EXTI_SOURCE_GPIOA,
    EXTI_SOURCE_GPIOB,
    EXTI_SOURCE_GPIOC,
    EXTI_SOURCE_GPIOD,
    EXTI_SOURCE_GPIOE,
    EXTI_SOURCE_GPIOF,
    EXTI_SOURCE_GPIOG,
    EXTI_SOURCE_GPIOH,
    EXTI_SOURCE_GPIOI,
};

static const uint8_t g_ExtiSrcPinMaps[GD32_GPIO_PIN_MAX] = {
    EXTI_SOURCE_PIN0,
    EXTI_SOURCE_PIN1,
    EXTI_SOURCE_PIN2,
    EXTI_SOURCE_PIN3,
    EXTI_SOURCE_PIN4,
    EXTI_SOURCE_PIN5,
    EXTI_SOURCE_PIN6,
    EXTI_SOURCE_PIN7,
    EXTI_SOURCE_PIN8,
    EXTI_SOURCE_PIN9,
    EXTI_SOURCE_PIN10,
    EXTI_SOURCE_PIN11,
    EXTI_SOURCE_PIN12,
    EXTI_SOURCE_PIN13,
    EXTI_SOURCE_PIN14,
    EXTI_SOURCE_PIN15,
};

typedef void (*HAL_GPIO_PIN_EXIT_HANDLER)(uint16_t pin);

typedef struct {
    uint16_t pin;
    uint16_t localPin;
    uint8_t setup;
	uint8_t setflag;
    HAL_GPIO_PIN_EXIT_HANDLER handler;
} Pin_GD;

#ifdef __cplusplus
}
#endif

#endif /* __HAL_GPIO_H */

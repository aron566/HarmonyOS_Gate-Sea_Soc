/**
 *  @file hal_gpio.c
 *  @brief gpio hdf driver.
 *  @author aron566 (wei.chen@gate-sea.com)
 *  @version v0.0.1 aron566 2024.05.24 09:36 初始版本.
 *  @date 2024-05-24
 *
 *  @details None.
 *  @par 修改日志:
 *  <table>
 *  <tr><th>Date       <th>Version <th>Author  <th>Description
 *  <tr><td>2024-05-24 <td>v0.0.1  <td>aron566 <td>初始版本
 *  </table>
 *  @copyright Copyright (c) 2024  Suzhou Gate-Sea Co.,Ltd.
 */
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< need definition of uint8_t */
#include <stddef.h> /**< need definition of NULL    */
#include <stdbool.h>/**< need definition of BOOL    */
#include <stdio.h>  /**< if need printf             */
#include <stdlib.h>
/* Private includes ----------------------------------------------------------*/
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
  #include "hcs_macro.h"
  #include "hdf_config_macro.h"
#else
  #include "device_resource_if.h"
#endif
#include "gpio_core.h"
#include "gpio_if.h"
#include "hdf_log.h"
#include "osal_irq.h"

#include "gsmcu_hal.h"
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus /**< use C compiler */
extern "C" {
#endif
/** Private macros -----------------------------------------------------------*/

#define PIN_EXIT_FIVE    5
#define PIN_EXIT_TEN     10
#define PIN_EXIT_SIXTEEN 16

#define DEBUG_GPIO_LOG 0

#ifndef GS_GPIO_PIN_MAX
  #define GS_GPIO_PIN_MAX 32U /**< 硬件gpio pin口数目 */
#endif

#ifndef GS_GPIO_GROUP_MAX
  #define GS_GPIO_GROUP_MAX 2U /**< 硬件gpio分组数目 */
#endif
/** Private typedef ----------------------------------------------------------*/

typedef void (*SYS_GPIO_PIN_EXIT_HANDLER)(void);

typedef struct
{
  uint16_t    pin;      /**< 端口所在硬件pin号 */
  uint16_t    localPin; /**< 端口序号 */
  uint8_t     setup;    /**< 是否禁用中断标识，1未禁用 */
  uint8_t     setflag;  /**< 是否已经设置标识，1已设置 */
  GpioIrqFunc handler;  /**< 中断回调 */
  void       *data;     /**< 回调携带参数 */
} GPioIrqInfo;

typedef struct
{
  uint8_t pin;           /**< 端口序号 */
  uint8_t pinstate;      /**< 端口状态 */
  uint8_t realPin;       /**< 端口所在硬件pin号 */
  uint8_t group;         /**< 端口所在硬件gpio组号 */
  uint8_t pindir;        /**< 端口方向：0输入 1输出 */
  uint8_t pinoutputtype; /**< 端口输出类型0推挽 1开漏 */
  uint8_t pullup;        /**< 端口上下拉设置0无 1上拉 2下拉 */
  uint8_t subpr;         /**< 端口子抢占优先级 */
  uint8_t prepr;         /**< 端口抢占优先级 */
} GpioResource;

typedef struct
{
  uint32_t     pinNums;
  uint32_t     group;
  GpioResource resource;
} GpioDevice;

typedef struct
{
  uint8_t group;
  uint8_t realPin;
  uint8_t pin;
  uint8_t prepr;
  uint8_t subpr;
  uint8_t pindir;
  uint8_t pullup;
  uint8_t pinoutputtype;
  uint8_t pinstate;
  uint8_t gpioindex;
} GpioInflectInfo;

/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/
GpioInflectInfo         g_gpioPinsMap[GS_GPIO_PIN_MAX * GS_GPIO_GROUP_MAX] = {0};
static struct GpioCntlr g_gpioCntlr;

/* 硬件端口实际中断回调信息 */
GPioIrqInfo g_pinsGroup[GS_GPIO_GROUP_MAX * GS_GPIO_PIN_MAX] = {0};

/* 硬件gpio组中断号 */
static int32_t g_gpioGrpirq[GS_GPIO_GROUP_MAX] = {
  GPIO0_IRQn,
  GPIO1_IRQn,
};

/* 硬件实际gpio分组 */
static const GPIO_TypeDef *g_gpioxMaps[GS_GPIO_GROUP_MAX] = {
  GPIO0,
  GPIO1,
  // GPIO2,
  // GPIO3,
  // GPIO4,
  // GPIO5,
  // GPIO6,
  // GPIO7,
  // GPIO8,
};

/* gpio组中断回调 */
static void                            GS_GPIO0_Handler(void);
static void                            GS_GPIO1_Handler(void);
static const SYS_GPIO_PIN_EXIT_HANDLER g_gpioxIrqHandle[GS_GPIO_GROUP_MAX] = {
  GS_GPIO0_Handler,
  GS_GPIO1_Handler,
};

/* 硬件实际pin口数值 */
static const uint32_t g_RealPinMaps[GS_GPIO_PIN_MAX] = {
  GPIO_Pin_0,
  GPIO_Pin_1,
  GPIO_Pin_2,
  GPIO_Pin_3,
  GPIO_Pin_4,
  GPIO_Pin_5,
  GPIO_Pin_6,
  GPIO_Pin_7,
  GPIO_Pin_8,
  GPIO_Pin_9,
  GPIO_Pin_10,
  GPIO_Pin_11,
  GPIO_Pin_12,
  GPIO_Pin_13,
  GPIO_Pin_14,
  GPIO_Pin_15,
  GPIO_Pin_16,
  GPIO_Pin_17,
  GPIO_Pin_18,
  GPIO_Pin_19,
  GPIO_Pin_20,
  GPIO_Pin_21,
  GPIO_Pin_22,
  GPIO_Pin_23,
  GPIO_Pin_24,
  GPIO_Pin_25,
  GPIO_Pin_26,
  GPIO_Pin_27,
  GPIO_Pin_28,
  GPIO_Pin_29,
  GPIO_Pin_30,
  GPIO_Pin_31,
};

/** Private function prototypes ----------------------------------------------*/

/** Private user code --------------------------------------------------------*/

/** Private application code -------------------------------------------------*/
/*******************************************************************************
 *
 *       Static code
 *
 ********************************************************************************
 */

/**
 * @brief GPIO0组中断处理接口
 *
 */
static void GS_GPIO0_Handler(void)
{
  /* 获取中断触发源 */
  uint32_t pins = GPIO_GetIntStatus(GPIO0);
  for (uint32_t i = 0U; i < GS_GPIO_PIN_MAX; i++)
  {
    if (pins & (1U << i))
    {
      if (NULL != g_pinsGroup[i].handler)
      {
        g_pinsGroup[i].handler(g_pinsGroup[i].localPin, g_pinsGroup[i].data);
      }
    }
  }
  /* 清除中断标志位 */
  GPIO_ClearPortIntStatus(GPIO0, pins);
}

/**
 * @brief GPIO1组中断处理接口
 *
 */
static void GS_GPIO1_Handler(void)
{
  /* 获取中断触发源 */
  uint32_t pins = GPIO_GetIntStatus(GPIO1);
  for (uint32_t i = 0U; i < GS_GPIO_PIN_MAX; i++)
  {
    if (pins & (1U << i))
    {
      uint32_t gpioIdx = 1U * GS_GPIO_PIN_MAX + i;
      if (NULL != g_pinsGroup[gpioIdx].handler)
      {
        g_pinsGroup[gpioIdx].handler(g_pinsGroup[gpioIdx].localPin, g_pinsGroup[gpioIdx].data);
      }
    }
  }
  /* 清除中断标志位 */
  GPIO_ClearPortIntStatus(GPIO1, pins);
}

/**
 * @brief 设置端口方向
 *
 * @param cntlr 控制句柄
 * @param gpio 端口序号
 * @param dir 方向 0输入 1输出
 * @return int32_t
 */
static int32_t GS_GpioSetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t dir)
{
  (void)cntlr;
  uint16_t gpio_pull = g_gpioPinsMap[gpio].pullup;
  uint32_t gpio_type = g_gpioPinsMap[gpio].pinoutputtype;

#if DEBUG_GPIO_LOG
  HDF_LOGI("%s: [line = %d] [gpio_pull = %d] [gpio_type = %d]\n", __func__, __LINE__, gpio_pull, gpio_type);
#endif

  uint16_t realPin = g_gpioPinsMap[gpio].realPin;
  uint32_t pinReg  = g_RealPinMaps[realPin];

  if (pinReg > GPIO_Pin_31 || pinReg < GPIO_Pin_0)
  {
    HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, realPin);
    return HDF_ERR_NOT_SUPPORT;
  }
  const GPIO_TypeDef *gpiox = g_gpioxMaps[g_gpioPinsMap[gpio].group];

  /* 更新状态 */
  g_gpioPinsMap[gpio].pindir = dir;

  /* bit0 [0:pull down 1:pull up] */
  /* bit1 [1:pllen enable 0:pullen disable] */
  /* bit2 [0:input 1:output] */
  uint32_t mode = (uint32_t)GPIO_Mode_IN_FLOATING;

  /* gpio方向 */
  if (dir == 0)
  {
    /* 输入 */
    mode &= ~(1U << 2U);
  }
  else if (dir == 1)
  {
    /* 输出 */
    mode |= (1U << 2U);
  }

  /* 上下拉设置 */
  if (gpio_pull == 0)
  {
    /* 无上下拉 */
    mode &= ~(1U << 1U);
  }
  else if (gpio_pull == 1)
  {
    /* 上拉 */
    mode |= (1U << 1U);
    mode |= (1U << 0U);
  }
  else if (gpio_pull == 2)
  {
    /* 下拉 */
    mode |= (1U << 1U);
    mode &= ~(1U << 0U);
  }

  /* 输出模式设置 */
  if (gpio_type == 0)
  {
    /* 推挽输出 */
    mode = GPIO_Mode_OPP;
  }
  else if (gpio_type == 1)
  {
    /* 开漏输出 */
    mode = GPIO_Mode_OPD;
  }

  /* 初始化端口 */
  GPIO_GlobalInit();

  /* 使能相应端口 */
  SCU_GPIOInputConfig((GPIO_TypeDef *)gpiox, pinReg);

  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Mode = (GPIOMode_TypeDef)mode;
  GPIO_InitStruct.GPIO_Pin  = pinReg;

  /* 初始化端口 */
  GPIO_Init((GPIO_TypeDef *)gpiox, &GPIO_InitStruct);

  return HDF_SUCCESS;
}

/**
 * @brief 获取端口方向
 *
 * @param cntlr 控制句柄
 * @param gpio 端口序号
 * @param dir 方向存储区
 * @return int32_t
 */
static int32_t GS_GpioGetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *dir)
{
  uint8_t value = 0;

  uint16_t realPin = g_gpioPinsMap[gpio].realPin;
  uint32_t pinReg  = g_RealPinMaps[realPin];

  if (pinReg > GPIO_Pin_31 || pinReg < GPIO_Pin_0)
  {
    HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, realPin);
    return HDF_ERR_NOT_SUPPORT;
  }

  value = g_gpioPinsMap[gpio].pindir;

  *dir = value;

  return HDF_SUCCESS;
}

/**
 * @brief 设置端口输出状态
 *
 * @param cntlr 控制句柄
 * @param gpio 端口序号
 * @param val 数值
 * @return int32_t
 */
static int32_t GS_GpioWrite(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t val)
{
  (void)cntlr;

  uint16_t realPin = g_gpioPinsMap[gpio].realPin;
  uint32_t pinReg  = g_RealPinMaps[realPin];

  if (pinReg > GPIO_Pin_31 || pinReg < GPIO_Pin_0)
  {
    HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, realPin);
    return HDF_ERR_NOT_SUPPORT;
  }

#if DEBUG_GPIO_LOG
  HDF_LOGI("%s %d ,write pin num %d", __func__, __LINE__, realPin);
#endif

  const GPIO_TypeDef *gpiox = g_gpioxMaps[g_gpioPinsMap[gpio].group];

  if (val)
  {
    GPIO_SetBits((GPIO_TypeDef *)gpiox, pinReg);
  }
  else
  {
    GPIO_ResetBits((GPIO_TypeDef *)gpiox, pinReg);
  }

  return HDF_SUCCESS;
}

/**
 * @brief 获取端口状态0 or 1
 *
 * @param cntlr 控制句柄
 * @param gpio 端口序号
 * @param val 端口状态存储区
 * @return int32_t
 */
static int32_t GS_GpioRead(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *val)
{
  uint8_t  value   = 0;
  uint16_t realPin = g_gpioPinsMap[gpio].realPin;
  uint32_t pinReg  = g_RealPinMaps[realPin];

  if (pinReg > GPIO_Pin_31 || pinReg < GPIO_Pin_0)
  {
    HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, realPin);
    return HDF_ERR_NOT_SUPPORT;
  }
  const GPIO_TypeDef *gpiox = g_gpioxMaps[g_gpioPinsMap[gpio].group];

  if (g_gpioPinsMap[gpio].pindir == 0)
  {
    value = GPIO_ReadInputDataBit((GPIO_TypeDef *)gpiox, pinReg);
  }
  else if (g_gpioPinsMap[gpio].pindir == 1)
  {
    value = GPIO_ReadOutputDataBit((GPIO_TypeDef *)gpiox, pinReg);
  }

  *val = value;

  return HDF_SUCCESS;
}

/**
 * @brief 获取端口改组irq号
 *
 * @param gpiox 端口地址
 * @return int32_t irq号
 */
static int32_t GS_GetGpioGroupIrq(const void *gpiox)
{
  for (uint32_t i = 0U; i < GS_GPIO_GROUP_MAX; i++)
  {
    if (gpiox == g_gpioxMaps[i])
    {
      return g_gpioGrpirq[i];
    }
  }
}

/**
 * @brief 获取中断处理函数
 *
 * @param gpiox gpio组地址
 * @return SYS_GPIO_PIN_EXIT_HANDLER 中断处理函数
 */
static SYS_GPIO_PIN_EXIT_HANDLER GS_GetGpioGroupIrqFunc(const void *gpiox)
{
  for (uint32_t i = 0U; i < GS_GPIO_GROUP_MAX; i++)
  {
    if (gpiox == g_gpioxMaps[i])
    {
      return g_gpioxIrqHandle[i];
    }
  }
}

/**
 * @brief 禁用端口中断
 *
 * @param cntlr 控制句柄
 * @param gpio 端口序号
 * @return int32_t
 */
static int32_t GS_GpioDisableIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
  /* 计算gpio所在地址 Idx = pin + pins * groupIdx */
  uint16_t            realPin = g_gpioPinsMap[gpio].realPin;
  uint32_t            pinReg  = g_RealPinMaps[realPin];
  uint32_t            gpioIdx = realPin + GS_GPIO_PIN_MAX * g_gpioPinsMap[gpio].group;
  const GPIO_TypeDef *gpiox   = g_gpioxMaps[g_gpioPinsMap[gpio].group];

#if DEBUG_GPIO_LOG
  HDF_LOGI("%s: [line = %d] [gpioIdx = %d]\n", __func__, __LINE__, gpioIdx);
#endif

  /* 禁用中断标识 */
  g_pinsGroup[gpioIdx].setup = RESET;

  /* 获取irq号 */
  // IRQn_Type irq = (IRQn_Type)GS_GetGpioGroupIrq(gpiox);

  /* 失能gpio组中断 */
  // NVIC_DisableIRQ(irq);

  /* 失能gpio pin口中断 */
  GPIO_SetIRQMode((GPIO_TypeDef *)gpiox, pinReg, 0);

  return HDF_SUCCESS;
}

/**
 * @brief 开启端口中断
 *
 * @param cntlr 控制句柄
 * @param gpio 端口号
 * @return int32_t
 */
static int32_t GS_GpioEnableIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
  (void)cntlr;
  uint16_t            realPin = g_gpioPinsMap[gpio].realPin;
  uint32_t            pinReg  = g_RealPinMaps[realPin];
  const GPIO_TypeDef *gpiox   = g_gpioxMaps[g_gpioPinsMap[gpio].group];

#if DEBUG_GPIO_LOG
  HDF_LOGI("%s: [line = %d] [realPin = %d] [gpio  = %d]\n", __func__, __LINE__, realPin, gpio);
#endif

  /* 计算gpio所在地址 */
  uint32_t gpioIdx = realPin + GS_GPIO_PIN_MAX * g_gpioPinsMap[gpio].group;

  /* 获取irq号 */
  IRQn_Type irq = (IRQn_Type)GS_GetGpioGroupIrq(gpiox);

  /* 设置中断回调 */
  if (g_pinsGroup[gpioIdx].setflag != SET)
  {
    SYS_GPIO_PIN_EXIT_HANDLER func = GS_GetGpioGroupIrqFunc(gpiox);
    NVIC_SetVector(irq, (uint32_t)func);

    g_pinsGroup[gpioIdx].setup    = SET;
    g_pinsGroup[gpioIdx].pin      = realPin;
    g_pinsGroup[gpioIdx].localPin = gpio;
    g_pinsGroup[gpioIdx].setflag  = SET;
  }

  /* 启用中断标识 */
  g_pinsGroup[gpioIdx].setup = SET;

  /* 使能gpio组中断 */
  NVIC_EnableIRQ(irq);

  /* 使能gpio pin口中断 */
  GPIO_SetIRQMode((GPIO_TypeDef *)gpiox, pinReg, 1);
  return HDF_SUCCESS;
}

/**
 * @brief 设置端口中断
 *
 * @param cntlr 控制句柄
 * @param gpio 应用层端口号
 * @param mode 边沿类型
 * @param func 回调函数
 * @param arg 参数
 * @return int32_t
 */
static int32_t GS_GpioSetIrq(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t mode, GpioIrqFunc func, void *arg)
{
  /* 获取对应硬件pin口 */
  uint16_t realPin = g_gpioPinsMap[gpio].realPin;
  uint32_t pinReg  = g_RealPinMaps[realPin];
  if (pinReg > GPIO_Pin_31 || pinReg < GPIO_Pin_0)
  {
    HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, realPin);
    return HDF_ERR_NOT_SUPPORT;
  }
  const GPIO_TypeDef *gpiox = g_gpioxMaps[g_gpioPinsMap[gpio].group];

  /* 设置端口 */
  GS_GpioSetDir(cntlr, gpio, g_gpioPinsMap[gpio].pindir);

#if DEBUG_GPIO_LOG
  HDF_LOGI("%s: [line = %d][prepr = %d] [subpr = %d]\n", __func__, __LINE__, g_gpioPinsMap[gpio].prepr, g_gpioPinsMap[gpio].subpr);
#endif

  /* 获取irq号 */
  IRQn_Type irq = (IRQn_Type)GS_GetGpioGroupIrq(gpiox);

  /* 设置中断优先级 */
  HAL_NVIC_SetPriority(irq, g_gpioPinsMap[gpio].prepr, g_gpioPinsMap[gpio].subpr);

  /* configure EXTI line */
  if (mode == OSAL_IRQF_TRIGGER_RISING)
  {
    /* 置中断触发模式-上升沿 */
    GPIO_SetIRQTriggerMode((GPIO_TypeDef *)gpiox, pinReg, GPIO_Trigger_Rising);
  }
  else if (mode == OSAL_IRQF_TRIGGER_FALLING)
  {
    /* 置中断触发模式-下降沿 */
    GPIO_SetIRQTriggerMode((GPIO_TypeDef *)gpiox, pinReg, GPIO_Trigger_Falling);
  }
  else if (mode == OSAL_IRQF_TRIGGER_NONE)
  {
    /* todo nothing */
  }
  else
  {
    HDF_LOGE("%s %d, error mode:%d", __func__, __LINE__, mode);
    return HDF_ERR_NOT_SUPPORT;
  }

  /* 设置消抖定时器分频系数，消抖定时器时钟 = APBCLOCK / (分频系数 + 1) */
  GPIO_SetBouncePreScale((GPIO_TypeDef *)gpiox, 49);

  /* 开启消抖 */
  GPIO_EnableBounce((GPIO_TypeDef *)gpiox, pinReg);

  // /* 置中断触发模式-上升沿 */
  // GPIO_SetIRQTriggerMode((GPIO_TypeDef *)gpiox, pinReg, GPIO_Trigger_Rising);
  // /* 置中断触发模式-下降沿 */
  // GPIO_SetIRQTriggerMode((GPIO_TypeDef *)gpiox, pinReg, GPIO_Trigger_Falling);
  // /* 设置中断触发模式-双边沿 */
  // GPIO_SetIRQTriggerMode((GPIO_TypeDef *)gpiox, pinReg, GPIO_Trigger_Rising_Falling);
  // /* 设置中断触发模式-高电平 */
  // GPIO_SetIRQTriggerMode((GPIO_TypeDef *)gpiox, pinReg, GPIO_Trigger_High_level);
  // /* 设置中断触发模式-低电平 */
  // GPIO_SetIRQTriggerMode((GPIO_TypeDef *)gpiox, pinReg, GPIO_Trigger_Low_level);

  /* 装载中断回调函数 */
  uint32_t gpioIdx             = realPin + GS_GPIO_PIN_MAX * g_gpioPinsMap[gpio].group;
  g_pinsGroup[gpioIdx].handler = func;
  g_pinsGroup[gpioIdx].data    = arg;

  return HDF_SUCCESS;
}

/**
 * @brief 关闭端口中断
 *
 * @param cntlr 控制句柄
 * @param gpio 应用层端口号
 * @return int32_t
 */
static int32_t GS_GpioUnsetIrq(struct GpioCntlr *cntlr, uint16_t gpio)
{
  uint16_t realPin = g_gpioPinsMap[gpio].realPin;
  uint32_t pinReg  = g_RealPinMaps[realPin];
  if (pinReg > GPIO_Pin_31 || pinReg < GPIO_Pin_0)
  {
    HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, realPin);
    return HDF_ERR_NOT_SUPPORT;
  }
  const GPIO_TypeDef *gpiox = g_gpioxMaps[g_gpioPinsMap[gpio].group];

  /* 中断回调置为空 */
  uint32_t gpioIdx             = realPin + GS_GPIO_PIN_MAX * g_gpioPinsMap[gpio].group;
  g_pinsGroup[gpioIdx].handler = NULL;
  g_pinsGroup[gpioIdx].data    = NULL;

  /* 失能gpio pin口中断 */
  GPIO_SetIRQMode((GPIO_TypeDef *)gpiox, pinReg, 0);

  return HDF_SUCCESS;
}

/**
 * @brief 初始化gpio
 *
 * @param device gpio设备属性（pin 方向 上下拉 推挽 开漏）
 * @return int32_t
 */
static HDF_STATUS InitGpioDevice(const GpioDevice *device)
{
  if (device == NULL)
  {
    HDF_LOGE("%s: device is NULL", __func__);
    return HDF_ERR_INVALID_PARAM;
  }

  uint32_t halGpio = g_RealPinMaps[device->resource.realPin];
  if (halGpio > GPIO_Pin_31 || halGpio < GPIO_Pin_0)
  {
    HDF_LOGE("%s %d, error pin:%d", __func__, __LINE__, halGpio);
    return HDF_ERR_NOT_SUPPORT;
  }

  const GPIO_TypeDef *gpiox = g_gpioxMaps[device->resource.group];

#if DEBUG_GPIO_LOG
  HDF_LOGI("%s: [line = %d] [group = %d] [realPin = %d]\n", __func__, __LINE__, device->resource.group, device->resource.realPin);
#endif

  /* bit0 [0:pull down 1:pull up] */
  /* bit1 [1:pllen enable 0:pullen disable] */
  /* bit2 [0:input 1:output] */
  uint32_t mode = (uint32_t)GPIO_Mode_IN_FLOATING;

  /* gpio方向 */
  if (device->resource.pindir == 0)
  {
    /* 输入 */
    mode &= ~(1U << 2U);
  }
  else if (device->resource.pindir == 1)
  {
    /* 输出 */
    mode |= (1U << 2U);
  }

  /* 上下拉设置 */
  if (device->resource.pullup == 0)
  {
    /* 无上下拉 */
    mode &= ~(1U << 1U);
  }
  else if (device->resource.pullup == 1)
  {
    /* 上拉 */
    mode |= (1U << 1U);
    mode |= (1U << 0U);
  }
  else if (device->resource.pullup == 2)
  {
    /* 下拉 */
    mode |= (1U << 1U);
    mode &= ~(1U << 0U);
  }

  /* 输出模式设置 */
  if (device->resource.pinoutputtype == 0)
  {
    /* 推挽输出 */
    mode = GPIO_Mode_OPP;
  }
  else if (device->resource.pinoutputtype == 1)
  {
    /* 开漏输出 */
    mode = GPIO_Mode_OPD;
  }

  /* 初始化端口 */
  GPIO_GlobalInit();

  /* 使能相应端口 */
  SCU_GPIOInputConfig((GPIO_TypeDef *)gpiox, halGpio);

  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Mode = (GPIOMode_TypeDef)mode;
  GPIO_InitStruct.GPIO_Pin  = halGpio;

  /* 重置该组所有端口 */
  // GPIO_StructInit(&GPIO_InitStruct);

  /* 初始化端口 */
  GPIO_Init((GPIO_TypeDef *)gpiox, &GPIO_InitStruct);

  /* 设置初始状态 */
  if (device->resource.pinstate == 0)
  {
    GPIO_ResetBits((GPIO_TypeDef *)gpiox, halGpio);
  }
  else if (device->resource.pinstate == 1)
  {
    GPIO_SetBits((GPIO_TypeDef *)gpiox, halGpio);
  }

  return HDF_SUCCESS;
}

/* gpio操作资源 */
static struct GpioMethod g_method = {
  .request    = NULL,
  .release    = NULL,
  .write      = GS_GpioWrite,
  .read       = GS_GpioRead,
  .setDir     = GS_GpioSetDir,
  .getDir     = GS_GpioGetDir,
  .toIrq      = NULL,
  .setIrq     = GS_GpioSetIrq,
  .unsetIrq   = GS_GpioUnsetIrq,
  .enableIrq  = GS_GpioEnableIrq,
  .disableIrq = GS_GpioDisableIrq,
};

#ifndef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
/*
gpio_config.hcs
root {
    platform {
        gpio_config {
            match_attr = "gpio_config";
            PinNum = 6;
            Pin =            [ 0, 1,  2,  3,  4,  5,  6, 7]; // GPIO序号
            RealPin =        [ 4, 5, 14, 13,  0,  9,  2, 7]; // GPIO管脚0-31
            Group =          [ 3, 3,  1,  2,  0,  6,  2, 2]; // GPIO组 0:GPIO_GROUP0 1:GPIO_GROUP1 2:GPIO_GROUP2 3:GPIOX
            PinState =       [ 0, 0,  0,  0,  0,  0,  0, 0]; // GPIO状态 0: 置低   1: 置高
            PinDir =         [ 1, 1,  0,  0,  0,  1,  1, 1]; // GPIO方向 0: 输入   1: 输出
            PinOutputType =  [ 0, 0,  0,  0,  0,  0,  0, 0]; // 输出模式 0 :推挽输出 1: 开漏输出
            PullUp =         [ 0, 0,  0,  0,  0,  0,  0, 0]; // 上拉许可 0: 不上拉 1:上拉 2:下拉
            PrePr  =         [ 0, 0,  0,  2,  2,  0,  0, 0]; // GPIO抢占优先级
            SubPr  =         [ 0, 0,  0,  0,  1,  0,  0, 0]; // GPIO子优先级
        }
    }
}
*/
static int32_t GpioParseHcs(const struct DeviceResourceIface *dri,
                            GpioDevice                       *device,
                            const struct DeviceResourceNode  *resourceNode)
{
  GpioResource *resource = NULL;
  resource               = &device->resource;
  if (resource == NULL)
  {
    HDF_LOGE("%s: resource is NULL", __func__);
    return HDF_ERR_INVALID_OBJECT;
  }

  if (dri->GetUint8(resourceNode, "PinNum", &device->pinNums, 0) != HDF_SUCCESS)
  {
    HDF_LOGE("gpio config read PinNum fail");
    return HDF_FAILURE;
  }

  HDF_LOGI("ENTRY FUNC %s line %d, PinNum:%d", __func__, __LINE__, device->pinNums);

  for (size_t i = 0; i < device->pinNums; i++)
  {
    if (dri->GetUint8ArrayElem(resourceNode, "Pin", i, &resource->pin, 0) != HDF_SUCCESS)
    {
      return HDF_FAILURE;
    }

    if (dri->GetUint8ArrayElem(resourceNode, "RealPin", i, &resource->realPin, 0) != HDF_SUCCESS)
    {
      return HDF_FAILURE;
    }

    if (dri->GetUint8ArrayElem(resourceNode, "Group", i, &resource->group, 0) != HDF_SUCCESS)
    {
      return HDF_FAILURE;
    }

    if (dri->GetUint8ArrayElem(resourceNode, "PinState", i, &resource->pinstate, 0) != HDF_SUCCESS)
    {
      return HDF_FAILURE;
    }

    if (dri->GetUint8ArrayElem(resourceNode, "PinDir", i, &resource->pindir, 0) != HDF_SUCCESS)
    {
      return HDF_FAILURE;
    }

    if (dri->GetUint8ArrayElem(resourceNode, "PinOutputType", i, &resource->pinoutputtype, 0) != HDF_SUCCESS)
    {
      return HDF_FAILURE;
    }

    if (dri->GetUint8ArrayElem(resourceNode, "PullUp", i, &resource->pullup, 0) != HDF_SUCCESS)
    {
      return HDF_FAILURE;
    }

    if (dri->GetUint8ArrayElem(resourceNode, "SubPr", i, &resource->subpr, 0) != HDF_SUCCESS)
    {
      return HDF_FAILURE;
    }

    if (dri->GetUint8ArrayElem(resourceNode, "PrePr", i, &resource->prepr, 0) != HDF_SUCCESS)
    {
      return HDF_FAILURE;
    }

    g_gpioPinsMap[resource->pin].group         = resource->group;
    g_gpioPinsMap[resource->pin].realPin       = resource->realPin;
    g_gpioPinsMap[resource->pin].pin           = resource->pin;
    g_gpioPinsMap[resource->pin].prepr         = resource->prepr;
    g_gpioPinsMap[resource->pin].subpr         = resource->subpr;
    g_gpioPinsMap[resource->pin].pindir        = resource->pindir;
    g_gpioPinsMap[resource->pin].pullup        = resource->pullup;
    g_gpioPinsMap[resource->pin].pinoutputtype = resource->pinoutputtype;
    g_gpioPinsMap[resource->pin].pinstate      = resource->pinstate;

    if (InitGpioDevice(device) != HDF_SUCCESS)
    {
      HDF_LOGE("InitGpioDevice FAIL\r\n");
      return HDF_FAILURE;
    }
  }

  return HDF_SUCCESS;
}
#endif

#ifndef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
static int32_t GetGpioDeviceResource(GpioDevice *device, const struct DeviceResourceNode *resourceNode)
{
  struct DeviceResourceIface *dri = NULL;
  if (device == NULL || resourceNode == NULL)
  {
    HDF_LOGE("%s: device is NULL", __func__);
    return HDF_ERR_INVALID_PARAM;
  }

  /* 获取hcs文件 */
  dri = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
  if (dri == NULL || dri->GetUint32 == NULL)
  {
    HDF_LOGE("DeviceResourceIface is invalid");
    return HDF_ERR_INVALID_OBJECT;
  }

  /* 解析hcs配置到设备句柄上，获取信息进行相应初始化 */
  if (GpioParseHcs(dri, device, resourceNode) != HDF_SUCCESS)
  {
    HDF_LOGE("gpio config parse hcs fail");
    return HDF_FAILURE;
  }

  return HDF_SUCCESS;
}
#else
  #define PLATFORM_GPIO_CONFIG HCS_NODE(HCS_NODE(HCS_ROOT, platform), gpio_config)
static uint32_t GetGpioDeviceResource(GpioDevice *device)
{
  GpioResource *resource = NULL;
  if (device == NULL)
  {
    HDF_LOGE("%s: device is NULL", __func__);
    return HDF_ERR_INVALID_PARAM;
  }
  resource = &device->resource;
  if (resource == NULL)
  {
    HDF_LOGE("%s: resource is NULL", __func__);
    return HDF_ERR_INVALID_OBJECT;
  }
  device->pinNums           = HCS_PROP(PLATFORM_GPIO_CONFIG, PinNum);
  uint32_t pins[]           = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, Pin));
  uint32_t realPins[]       = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, RealPin));
  uint32_t groups[]         = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, Group));
  uint32_t pinstates[]      = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, PinState));
  uint32_t pindirs[]        = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, PinDir));
  uint32_t pinoutputtypes[] = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, PinOutputType));
  uint32_t pullups[]        = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, PullUp));
  uint32_t preprs[]         = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, PrePr));
  uint32_t subprs[]         = HCS_ARRAYS(HCS_NODE(PLATFORM_GPIO_CONFIG, SubPr));

  for (size_t i = 0; i < device->pinNums; i++)
  {
    resource->pin           = pins[i];
    resource->realPin       = realPins[i];
    resource->group         = groups[i];
    resource->prepr         = preprs[i];
    resource->subpr         = subprs[i];
    resource->pindir        = pindirs[i];
    resource->pullup        = pullups[i];
    resource->pinoutputtype = pinoutputtypes[i];
    resource->pinstate      = pinstates[i];

    g_gpioPinsMap[resource->pin].group         = resource->group;
    g_gpioPinsMap[resource->pin].realPin       = resource->realPin;
    g_gpioPinsMap[resource->pin].pin           = resource->pin;
    g_gpioPinsMap[resource->pin].prepr         = resource->prepr;
    g_gpioPinsMap[resource->pin].subpr         = resource->subpr;
    g_gpioPinsMap[resource->pin].pindir        = resource->pindir;
    g_gpioPinsMap[resource->pin].pullup        = resource->pullup;
    g_gpioPinsMap[resource->pin].pinoutputtype = resource->pinoutputtype;
    g_gpioPinsMap[resource->pin].pinstate      = resource->pinstate;

    if (InitGpioDevice(device) != HDF_SUCCESS)
    {
      HDF_LOGE("InitGpioDevice FAIL\r\n");
      return HDF_FAILURE;
    }
  }

  return HDF_SUCCESS;
}
#endif

static int32_t AttachGpioDevice(struct GpioCntlr *gpioCntlr, const struct HdfDeviceObject *device)
{
  int32_t ret;

  GpioDevice *gpioDevice = NULL;

#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
  if (device == NULL)
  {
#else
  if (device == NULL || device->property == NULL)
  {
#endif
    HDF_LOGE("%s: property is NULL", __func__);
    return HDF_ERR_INVALID_PARAM;
  }

  /* 分配一个gpio属性句柄 */
  gpioDevice = (GpioDevice *)OsalMemAlloc(sizeof(GpioDevice));
  if (gpioDevice == NULL)
  {
    HDF_LOGE("%s: OsalMemAlloc gpioDevice error", __func__);
    return HDF_ERR_MALLOC_FAIL;
  }

#ifndef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
  ret = GetGpioDeviceResource(gpioDevice, device->property);
#else
  ret = GetGpioDeviceResource(gpioDevice);
#endif
  if (ret != HDF_SUCCESS)
  {
    (void)OsalMemFree(gpioDevice);
    return HDF_FAILURE;
  }

  gpioCntlr->priv  = gpioDevice;
  gpioCntlr->count = gpioDevice->pinNums;

  return HDF_SUCCESS;
}

/**
 * @brief 释放GPIO驱动
 *
 * @param device 设备驱动句柄
 */
static void GpioRelease(struct HdfDeviceObject *device)
{
  struct GpioCntlr *cntlr = NULL;

  if (device == NULL)
  {
    HDF_LOGE("%s: device is null!", __func__);
    return;
  }

  cntlr = GpioCntlrFromHdfDev(device);
  if (cntlr == NULL)
  {
    HDF_LOGE("%s: no service binded!", __func__);
    return;
  }

  cntlr->count = 0;

  GpioCntlrRemove(cntlr);
}

/**
 * @brief HDF驱动适配
 *
 * @param device
 * @return int32_t
 */
static int32_t GpioBind(struct HdfDeviceObject *device)
{
  HDF_LOGI("%s: Enter\r\n", __func__);
  (void)device;
  return HDF_SUCCESS;
}

/**
 * @brief GPIO驱动初始化
 *
 * @param device gpio hdf设备句柄
 * @return int32_t
 */
static int32_t GpioInit(struct HdfDeviceObject *device)
{
  int32_t           ret;
  struct GpioCntlr *gpioCntlr = &g_gpioCntlr;

  HDF_LOGI("%s: Enter\r\n", __func__);

  if (device == NULL)
  {
    HDF_LOGE("%s: device is NULL", __func__);
    return HDF_ERR_INVALID_PARAM;
  }

  ret = PlatformDeviceBind(&g_gpioCntlr.device, device);
  if (ret != HDF_SUCCESS)
  {
    HDF_LOGE("%s: bind hdf device failed:%d", __func__, ret);
    return ret;
  }

  gpioCntlr = GpioCntlrFromHdfDev(device);
  if (gpioCntlr == NULL)
  {
    HDF_LOGE("GpioCntlrFromHdfDev fail\r\n");
    return HDF_DEV_ERR_NO_DEVICE_SERVICE;
  }

  ret = AttachGpioDevice(gpioCntlr, device); /* GpioCntlr add GpioDevice to priv */
  if (ret != HDF_SUCCESS)
  {
    HDF_LOGE("AttachGpioDevice fail\r\n");
    return HDF_DEV_ERR_ATTACHDEV_FAIL;
  }
  gpioCntlr->ops           = &g_method;
  gpioCntlr->device.hdfDev = device;
  ret                      = GpioCntlrAdd(gpioCntlr);
  if (ret != HDF_SUCCESS)
  {
    HDF_LOGE("%s: err add controller:%d", __func__, ret);
    return HDF_FAILURE;
  }

  HDF_LOGI("%s: dev service:%s init success!", __func__, HdfDeviceGetServiceName(device));

  return HDF_SUCCESS;
}

static struct HdfDriverEntry g_gpioDriverEntry = {
  .moduleVersion = 1,
  .Bind          = GpioBind,
  .Init          = GpioInit,
  .Release       = GpioRelease,
  .moduleName    = "GATE_SEA_GPIO_MODULE_HDF",
};
HDF_INIT(g_gpioDriverEntry);
/** Public application code --------------------------------------------------*/
/*******************************************************************************
 *
 *       Public code
 *
 ********************************************************************************
 */
#ifdef __cplusplus /**< end extern c */
}
#endif
/******************************** End of file *********************************/

/**
 ******************************************************************************
 * @file    system_stm32f0xx.h
 * @author  MCD Application Team
 * @version V2.2.3
 * @date    29-January-2016
 * @brief   CMSIS Cortex-M0 Device System Source File for
 ******************************************************************************
 * @attention
 *
 *
 ******************************************************************************
 */

/** @addtogroup CMSIS
 * @{
 */

/** @addtogroup stm32f0xx_system
 * @{
 */

/**
 * @brief Define to prevent recursive inclusion
 */
#ifndef __SYSTEM_STM32F0XX_H
#define __SYSTEM_STM32F0XX_H

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(ZB204_CHIP)
#include "ZB204.h"
#elif defined(ZB205_CHIP)
#include "ZB205.h"
#endif

  typedef enum _CLOCKMOD
  {
    CLKMOD_LOSC,
    CLKMOD_EOSC,
  } CLOCKMOD;
  typedef enum _CLOCKOUT
  {
    CLKOUT_SYSCLK,
    CLKOUT_EOSC,
    CLKOUT_LOSC,
    CLKOUT_DISABLE
  } CLOCKOUT;

  /*********************************************************************************
  **					define device feature
  *********************************************************************************/
  // GPIO

#define EPORT_LIST \
  EPORT0,          \
      EPORT1,      \
      EPORT2,      \
      EPORT3,      \
      EPORT4,      \
      EPORT5,      \
      EPORT6,      \
      EPORT7,      \
      EPORT8,      \
      EPORT9

  extern void SystemCoreClockUpdate(void);
  extern void SystemInit(void);
  extern int SystemClockSet(uint32_t sysDiv, uint32_t ipgDiv);
  void CloseClockGate(void);
  void OpenClockGate(void);
#ifdef DB001
  extern int AdcClockSet(uint32_t adcDiv);
  extern int Set_SWT_Clock_2K_32K(uint32_t clk_sel);
  extern int Set_RTC_Clock_Sel(uint32_t clk_sel);
#endif

  extern uint32_t Get_SWT_Clock_us(void);
  extern void Lock_CPM_Reg(volatile uint32_t *reg);
  extern void Unlock_CPM_Reg(volatile uint32_t *reg);
  extern void SystemUseEOSC(void);
  extern void changeClockOut(CLOCKOUT clock);
  extern void DelayTick(int tick);
  extern void Delayms(uint32_t ms);

  extern uint32_t SystemCoreClock; /*!< System Clock Frequency (Core Clock) */
  extern uint32_t IpgClock;
  extern uint32_t AdcClock;
  extern uint32_t SysTickms;

  extern void SystemInit(void);
  extern void SystemCoreClockUpdate(void);
  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_STM32F0XX_H */

/**
 * @}
 */

/**
 * @}
 */
/************************ (C) COPYRIGHT  *****END OF FILE****/

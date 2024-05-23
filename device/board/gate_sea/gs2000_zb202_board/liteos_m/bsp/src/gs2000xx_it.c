/**
 *  @file gs2000xx_it.c
 *  @brief None.
 *  @author aron566 (wei.chen@gate-sea.com)
 *  @version v0.0.1 aron566 2024.05.23 09:44 初始版本.
 *  @date 2024-05-23
 *
 *  @details None.
 *  @par 修改日志:
 *  <table>
 *  <tr><th>Date       <th>Version <th>Author  <th>Description
 *  <tr><td>2024-05-23 <td>v0.0.1  <td>aron566 <td>初始版本
 *  </table>
 *  @copyright Copyright (c) 2024  Suzhou Gate-Sea Co.,Ltd.
 */
/** Includes -----------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
/* Private includes ----------------------------------------------------------*/
#include "gsmcuxx_hal_def.h"
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus /**< use C compiler */
extern "C" {
#endif
/** Private macros -----------------------------------------------------------*/

/** Private typedef ----------------------------------------------------------*/

/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/

/** Private function prototypes ----------------------------------------------*/

/** Private user code --------------------------------------------------------*/

/** Private application code -------------------------------------------------*/
/*******************************************************************************
 *
 *       Static code
 *
 ********************************************************************************
 */

/** Public application code --------------------------------------------------*/
/*******************************************************************************
 *
 *       Public code
 *
 ********************************************************************************
 */

/**
 * @brief sleep wakeup interrupt handler
 *
 */
void SYSC_Handler(void)
{
  /* clear sleep wakeup interrupt */
  volatile uint32_t reg = SCU_BASE + 0x24U;
  WRITE(reg, 0x48U);
}

/**
 * @brief watch dog interrupt
 *
 */
void WWDG_Handler(void)
{
  /* todo nothing */
  Feed_WDG();
  WDG_Enable();
  WDG_Disable();
}

#ifdef __cplusplus /**< end extern c */
}
#endif
/******************************** End of file *********************************/

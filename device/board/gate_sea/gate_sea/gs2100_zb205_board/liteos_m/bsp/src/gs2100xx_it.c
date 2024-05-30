/**
 *  @file gs2100xx_it.c
 *  @brief 中断处理.
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
/* Private includes ----------------------------------------------------------*/
#include "gs2100xx_it.h"
#include "gsmcu_hal.h"
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus /**< use C compiler */
extern "C" {
#endif
/** Private macros -----------------------------------------------------------*/

/** Private typedef ----------------------------------------------------------*/

/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/

/* DMA中断处理函数 */
static IT_CALLBACK_Typedef_t DMA_Channel_IT_Handler[(int)DMA_Channel7 + 1] = {NULL};


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
 * @brief systick interrupt
 *
 */
void SysTick_Handler(void)
{
  return;
}

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

/**
 * @brief dma interrupt
 *
 */
void DMAC_Handler(void)
{
  uint32_t status = DMA_GetITStatus(DMA);
  uint32_t channelmsk  = 0x01;
  for (int i = 0; i <= (DMA_Channel7); i++)
  {
    if ((status & (channelmsk << i)) == (channelmsk << i))
    {
      if (DMA_Channel_IT_Handler[i] != NULL)
      {
        DMA_Channel_IT_Handler[i]();
      }

      /* clear it flag */
      DMA_ClearChannelTCITStatus(DMA, i);
    }
  }
}

/**
 * @brief 设置dma chanel 中断处理函数
 *
 * @param ch 通道号 @ref dma_channel_t
 * @param func 中断处理函数
 */
void GS2100xx_DMA_IT_Handler_Set(int ch, IT_CALLBACK_Typedef_t func)
{
  DMA_Channel_IT_Handler[ch] = func;
}

#ifdef __cplusplus /**< end extern c */
}
#endif
/******************************** End of file *********************************/

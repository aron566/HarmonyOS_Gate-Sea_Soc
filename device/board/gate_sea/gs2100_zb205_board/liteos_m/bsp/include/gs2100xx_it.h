/**
 *  @file gs2100xx_it.h
 *  @brief None.
 *  @author aron566 (wei.chen@gate-sea.com)
 *  @version v0.0.1 aron566 2024.05.27 17:06 初始版本.
 *  @date 2024-05-29
 *
 *
 *  @par 修改日志:
 *  <table>
 *  <tr><th>Date       <th>Version <th>Author  <th>Description
 *  <tr><td>2024-05-29 <td>v0.0.1  <td>aron566 <td>初始版本
 *  </table>
 *  @copyright Copyright (c) 2024  Suzhou Gate-Sea Co.,Ltd.
 */
#ifndef GS2100XX_IT_H
#define GS2100XX_IT_H
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< need definition of uint8_t */
#include <stddef.h> /**< need definition of NULL    */
#include <stdbool.h>/**< need definition of BOOL    */

/** Private includes ---------------------------------------------------------*/

/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus /**< use C compiler */
extern "C" {
#endif
/** Private defines ----------------------------------------------------------*/
/** Exported typedefines -----------------------------------------------------*/

/* 中断处理函数 */
typedef void (*IT_CALLBACK_Typedef_t)(void);

/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/**
 * @brief 设置dma chanel 中断处理函数
 *
 * @param ch 通道号 @ref dma_channel_t
 * @param func 中断处理函数
 */
void GS2100xx_DMA_IT_Handler_Set(int ch, IT_CALLBACK_Typedef_t func);

#ifdef __cplusplus /**< end extern c */
}
#endif
#endif
/******************************** End of file *********************************/

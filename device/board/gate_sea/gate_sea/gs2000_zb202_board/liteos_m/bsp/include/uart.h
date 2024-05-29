/**
 *  @file uart.h
 *  @brief None.
 *  @author aron566 (wei.chen@gate-sea.com)
 *  @version v0.0.1 aron566 2024.05.22 16:17 初始版本.
 *  @date 2024-05-22
 *
 *
 *  @par 修改日志:
 *  <table>
 *  <tr><th>Date       <th>Version <th>Author  <th>Description
 *  <tr><td>2024-05-22 <td>v0.0.1  <td>aron566 <td>初始版本
 *  </table>
 *  @copyright Copyright (c) 2024  Suzhou Gate-Sea Co.,Ltd.
 */
#ifndef UART_H
#define UART_H
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< need definition of uint8_t */
#include <stddef.h> /**< need definition of NULL    */
#include <stdbool.h>/**< need definition of BOOL    */
#include <stdio.h>  /**< if need printf             */
#include <stdlib.h>
/** Private includes ---------------------------------------------------------*/
#include "gsmcu_hal.h"
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus /**< use C compiler */
extern "C" {
#endif
/** Private defines ----------------------------------------------------------*/
/** Exported typedefines -----------------------------------------------------*/

typedef void (*UartCallBackFunctionType)(void);

typedef struct
{
  uint32_t BaudRate;
  uint32_t Parity;
  uint32_t DataBits;
  uint32_t StopBits;
  //
  UartCallBackFunctionType RxHaveDataFunction;
  UartCallBackFunctionType RxParityErrorFunction;
  UartCallBackFunctionType TxDataCompleteFunction;
} UartParameterType;
/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/
#if 1

/**
 * @brief 串口初始化
 *
 */
void UART_Init(void);

void     UartOpen(UartParameterType *UartParameter);
void     UartWrite(uint8_t *pbuff, uint32_t len);
uint32_t UartRead(uint8_t *pbuff, uint32_t len);
uint32_t GetUartRxDataLen();
void     UartClose();
bool     IsUartTxData();
void     UartWriteWithDma(USART_TypeDef *puart, uint8_t *pbuff, uint32_t len, uint32_t dma_channel);
    #if defined(METER_UART_USE_UART0)
      #define METER_UART              UART0
      #define METER_UART_TX_PIN_REMAP UART0_TX_REMAP
      #define METER_UART_RX_PIN_REMAP UART0_RX_REMAP
      #define METER_UART_INPUT_ENABLE UART0_INPUT_ENABLE
      #define METER_UART_TX_PORT      UART0_TX_PORT
      #define METER_UART_TX_PIN       UART0_TX_PIN
      #define METER_UART_RX_PORT      UART0_RX_PORT
      #define METER_UART_RX_PIN       UART0_RX_PIN
      #define METER_UART_IRQn         UART0_IRQn
      #define METER_UART_DMA_REQ      DMA_UART0_TX
    #elif defined(METER_UART_USE_UART1)
      #define METER_UART              UART1
      #define METER_UART_TX_PIN_REMAP UART1_TX_REMAP
      #define METER_UART_RX_PIN_REMAP UART1_RX_REMAP
      #define METER_UART_INPUT_ENABLE UART1_INPUT_ENABLE
      #define METER_UART_TX_PORT      UART1_TX_PORT
      #define METER_UART_TX_PIN       UART1_TX_PIN
      #define METER_UART_RX_PORT      UART1_RX_PORT
      #define METER_UART_RX_PIN       UART1_RX_PIN
      #define METER_UART_IRQn         UART1_IRQn
      #define METER_UART_DMA_REQ      DMA_UART1_TX
    #elif defined(METER_UART_USE_UART2)
      #define METER_UART              UART2
      #define METER_UART_TX_PIN_REMAP UART2_TX_REMAP
      #define METER_UART_RX_PIN_REMAP UART2_RX_REMAP
      #define METER_UART_INPUT_ENABLE UART2_INPUT_ENABLE
      #define METER_UART_TX_PORT      UART2_TX_PORT
      #define METER_UART_TX_PIN       UART2_TX_PIN
      #define METER_UART_RX_PORT      UART2_RX_PORT
      #define METER_UART_RX_PIN       UART2_RX_PIN
      #define METER_UART_IRQn         UART2_IRQn
      #define METER_UART_DMA_REQ      DMA_UART2_TX
    #elif defined(METER_UART_USE_UART3)
      #define METER_UART              UART3
      #define METER_UART_TX_PIN_REMAP UART3_TX_REMAP
      #define METER_UART_RX_PIN_REMAP UART3_RX_REMAP
      #define METER_UART_INPUT_ENABLE UART3_INPUT_ENABLE
      #define METER_UART_TX_PORT      UART3_TX_PORT
      #define METER_UART_TX_PIN       UART3_TX_PIN
      #define METER_UART_RX_PORT      UART3_RX_PORT
      #define METER_UART_RX_PIN       UART3_RX_PIN
      #define METER_UART_IRQn         UART3_IRQn
      #define METER_UART_DMA_REQ      DMA_UART3_TX
    #endif

void DebugOpen(uint32_t BaudRate);
// void debug_hex(u32 mask, uint8_t *buf, uint16_t len);
// void debug_word_hex(u32 mask, uint32_t *buf, uint16_t len);
// void debug_str(u32 mask, const char *format, ...);
// void DebugSetMask(uint32_t mask);
    #define DEBUG_LOG_PHY    0x00000001
    #define DEBUG_LOG_MAC    0x00000002
    #define DEBUG_LOG_APS    0x00000004
    #define DEBUG_LOG_APP    0x00000008
    #define DEBUG_LOG_UPDATA 0x00000010
    #define DEBUG_LOG_NET    0x00000020
    #define DEBUG_LOG_ZC     0x00000040
    #define DEBUG_LOG_ZONE   0x00000080

    #define DEBUG_LOG_INFO  0x01000000
    #define DEBUG_LOG_ERR   0x02000000
    #define DEBUG_LOG_OTHER 0x80000000

    #if defined(DEBUG_PORT_USE_UART0)
      #define DEBUG_UART              UART0
      #define DEBUG_UART_TX_PIN_REMAP UART0_TX_REMAP
      #define DEBUG_UART_RX_PIN_REMAP UART0_RX_REMAP
      #define DEBUG_UART_INPUT_ENABLE UART0_INPUT_ENABLE
      #define DEBUG_UART_TX_PORT      UART0_TX_PORT
      #define DEBUG_UART_TX_PIN       UART0_TX_PIN
      #define DEBUG_UART_RX_PORT      UART0_RX_PORT
      #define DEBUG_UART_RX_PIN       UART0_RX_PIN
      #define DEBUG_UART_IRQn         UART0_IRQn
    // #define DEBUG_UART_DMA_REQ     DMA_UART0_TX
    #elif defined(DEBUG_PORT_USE_UART1)
      #define DEBUG_UART              UART1
      #define DEBUG_UART_TX_PIN_REMAP UART1_TX_REMAP
      #define DEBUG_UART_RX_PIN_REMAP UART1_RX_REMAP
      #define DEBUG_UART_INPUT_ENABLE UART1_INPUT_ENABLE
      #define DEBUG_UART_TX_PORT      UART1_TX_PORT
      #define DEBUG_UART_TX_PIN       UART1_TX_PIN
      #define DEBUG_UART_RX_PORT      UART1_RX_PORT
      #define DEBUG_UART_RX_PIN       UART1_RX_PIN
      #define DEBUG_UART_IRQn         UART1_IRQn
    // #define DEBUG_UART_DMA_REQ     DMA_UART1_TX
    #elif defined(DEBUG_PORT_USE_UART2)
      #define DEBUG_UART              UART2
      #define DEBUG_UART_TX_PIN_REMAP UART2_TX_REMAP
      #define DEBUG_UART_RX_PIN_REMAP UART2_RX_REMAP
      #define DEBUG_UART_INPUT_ENABLE UART2_INPUT_ENABLE
      #define DEBUG_UART_TX_PORT      UART2_TX_PORT
      #define DEBUG_UART_TX_PIN       UART2_TX_PIN
      #define DEBUG_UART_RX_PORT      UART2_RX_PORT
      #define DEBUG_UART_RX_PIN       UART2_RX_PIN
      #define DEBUG_UART_IRQn         UART2_IRQn
    // #define DEBUG_UART_DMA_REQ				DMA_UART2_TX
    #elif defined(DEBUG_PORT_USE_UART3)
      #define DEBUG_UART              UART3
      #define DEBUG_UART_TX_PIN_REMAP UART3_TX_REMAP
      #define DEBUG_UART_RX_PIN_REMAP UART3_RX_REMAP
      #define DEBUG_UART_INPUT_ENABLE UART3_INPUT_ENABLE
      #define DEBUG_UART_TX_PORT      UART3_TX_PORT
      #define DEBUG_UART_TX_PIN       UART3_TX_PIN
      #define DEBUG_UART_RX_PORT      UART3_RX_PORT
      #define DEBUG_UART_RX_PIN       UART3_RX_PIN
      #define DEBUG_UART_IRQn         UART3_IRQn
    // #define DEBUG_UART_DMA_REQ				DMA_UART3_TX
    #endif

#endif

/**
 * @brief 获取串口数据 for shell
 *
 * @return uint8_t 数据
 */
uint8_t UartGetc(void);

#ifdef __cplusplus /**< end extern c */
}
#endif
#endif
/******************************** End of file *********************************/

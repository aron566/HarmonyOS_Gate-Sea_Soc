#ifndef __HAL_UART_H
#define __HAL_UART_H

#include "uart_if.h"
#include "uart_core.h"

#include "gd32f4xx.h"
#ifdef __cplusplus
extern "C" {
#endif

#define DATA_BUF_SIZE 1024

typedef struct {
    uint32_t rxPort;
    uint32_t rxPin;
    uint32_t rx_AF;
    uint32_t tx_AF;
    uint32_t txPort;
    uint32_t txPin;
} uartPinCfg;

typedef struct {
    uint32_t rx_DMA;
    uint32_t rx_DmaCh;
    uint32_t rx_DmaPerSel;
    uint8_t rx_DmaBuff[DATA_BUF_SIZE];
    uint32_t tx_DMA;
    uint32_t tx_DmaCh;
    uint32_t tx_DmaPerSel;
} uartDmaCfg;

typedef enum 
{
    GD32_USART0 = 0,
    GD32_USART1,
    GD32_USART2,
    GD32_UART3,
    GD32_UART4,
    GD32_USART5,
    GD32_UART6,
    GD32_UART7,
    GD32_UART_MAX,
} GD32_USART;

static const uint32_t g_UartPeriph[GD32_UART_MAX] =
{
    USART0,
    USART1,
    USART2,
    UART3,
    UART4,
    USART5,
    UART6,
    UART7,
};

static const uint32_t g_UartPeriphcLOCK[GD32_UART_MAX] =
{
    RCU_USART0,
    RCU_USART1,
    RCU_USART2,
    RCU_UART3,
    RCU_UART4,
    RCU_USART5,
    RCU_UART6,
    RCU_UART7,
};

static const uint32_t g_UartIrqnMaps[GD32_UART_MAX] =
{
    USART0_IRQn,
    USART1_IRQn,
    USART2_IRQn,
    UART3_IRQn,
    UART4_IRQn,
    USART5_IRQn,
    UART6_IRQn,
    UART7_IRQn,
};


typedef enum 
{
    GD_DMA0 = 0,
    GD_DMA1 = 1,
    GD_DMA_MAX,
} DMA_Func;

static const uint32_t g_UartDmaFuncMaps[GD_DMA_MAX] =
{
    DMA0,
    DMA1,
};

static const uint32_t g_DmaPeriphClockMaps[GD_DMA_MAX] =
{
    RCU_DMA0,
    RCU_DMA1,
};

static const uint32_t g_UartDmaChMaps[8] =
{
    DMA_CH0,
    DMA_CH1,
    DMA_CH2,
    DMA_CH3,
    DMA_CH4,
    DMA_CH5,
    DMA_CH6,
    DMA_CH7,
};

#ifdef __cplusplus
}
#endif

#endif /* __HAL_UART_H */
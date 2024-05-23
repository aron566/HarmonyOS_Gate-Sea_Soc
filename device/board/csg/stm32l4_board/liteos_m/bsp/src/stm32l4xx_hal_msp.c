/**
  ******************************************************************************
  * @file    stm32l4xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.    
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "main.h"

#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

UART_HandleTypeDef UartHandle;

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  //HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);
  while ((__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_TXE) ? SET : RESET) == RESET)
  {
  }
  UartHandle.Instance->TDR = (uint8_t)(ch);
  return ch;
}

void MX_USARTx_UART_Init(void)
{
  /*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = 115200 baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UartHandle.Instance        = USARTx;

  UartHandle.Init.BaudRate     = 115200;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = UART_STOPBITS_1;
  UartHandle.Init.Parity       = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

  if(HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief UART MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    if(huart->Instance == LPUART1) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_LPUART1_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_0;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_1;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    }

    if (huart->Instance == USART1) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USART1_CLK_ENABLE();
      
        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
      
        GPIO_InitStruct.Pin = GPIO_PIN_10;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }

    if (huart->Instance == USART2) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USART2_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_2;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
       
        GPIO_InitStruct.Pin = GPIO_PIN_3;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }

    if (huart->Instance == USART3) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_USART3_CLK_ENABLE();
      
        GPIO_InitStruct.Pin = GPIO_PIN_4;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
      
        GPIO_InitStruct.Pin = GPIO_PIN_5;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    }

    if (huart->Instance == UART4) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_UART4_CLK_ENABLE();
       
        GPIO_InitStruct.Pin = GPIO_PIN_0;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
       
        GPIO_InitStruct.Pin = GPIO_PIN_1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }

    if (huart->Instance == UART5) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();
        __HAL_RCC_UART5_CLK_ENABLE();
       
        GPIO_InitStruct.Pin = GPIO_PIN_12;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
       
        GPIO_InitStruct.Pin = GPIO_PIN_2;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    }
}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
    if(huart->Instance==LPUART1) {
        __HAL_RCC_LPUART1_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0|GPIO_PIN_1);
        HAL_NVIC_DisableIRQ(LPUART1_IRQn);
    }

    if (huart->Instance==USART1) {
        __HAL_RCC_USART1_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);
        HAL_NVIC_DisableIRQ(USART1_IRQn);
    }

    if (huart->Instance==USART2) {
        __HAL_RCC_USART2_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);
        HAL_NVIC_DisableIRQ(USART2_IRQn);
    }

    if (huart->Instance==USART3) {
        __HAL_RCC_USART3_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_4|GPIO_PIN_5);
        HAL_NVIC_DisableIRQ(USART3_IRQn);
    }
  
    if (huart->Instance==UART4) {
        __HAL_RCC_UART4_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1);
        HAL_NVIC_DisableIRQ(UART4_IRQn);
    }
  
    if (huart->Instance==UART5) {
        __HAL_RCC_UART5_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_12);
        HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);
        HAL_NVIC_DisableIRQ(UART5_IRQn);
    }

}

/*
  to override original HAL_GetTick by LOS routine
  Cause:  many routines from STM32 HAL depend on this function,
          but SysTick_Handler is bypassed by OsSetVector(SysTick_IRQn, handler) in OHOS
*/


/**
  * @}
  */
  
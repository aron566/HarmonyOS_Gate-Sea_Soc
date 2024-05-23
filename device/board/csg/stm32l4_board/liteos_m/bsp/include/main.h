/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stdio.h"

/* Definition for USARTx - debug */
#define USARTx                           USART1

#define OHOS_APP_RUN(func) \
void ohos_app_main(void) { \
    LOS_TaskDelay(100); \
    printf("\n\033[1;32m<--------------- OHOS Application Start Here --------------->\033[0m\n"); \
    func(); \
}

#endif /* __MAIN_H */


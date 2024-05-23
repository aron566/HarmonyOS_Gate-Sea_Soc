/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (C) 2022, Xiaohua Semiconductor Co.
  * All rights reserved.
  *
  * This software component is licensed by XHSC under BSD 3-Clause license
  * (the "License"); You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                    opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "los_config.h"
#include "gd32f470z_eval.h"
#include "gd32f4xx.h"
#include "systick.h"
#include "los_task.h"

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    int ret;

    systick_config();
    gd_eval_com_init(EVAL_COM0);

    printf("%s user main!\n", __TIME__);
    ret = LOS_KernelInit();
    if (ret != LOS_OK) {
        printf("Liteos kernel init failed! ERROR: 0x%x\n", ret);
    } else {
        DeviceManagerStart();
        OHOS_SystemInit();
        Lfs_Init();
        //StartHdfGpioTest();
        LOS_Start();
    }

    /* Infinite loop */
    while (1)
    {
    }
}

/**
  * @}
  */

/**
  * @}
  */


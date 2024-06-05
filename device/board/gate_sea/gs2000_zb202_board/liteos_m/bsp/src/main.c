#if 1
/**
 *  @file main.c
 *  @brief None.
 *  @author aron566 (wei.chen@gate-sea.com)
 *  @version v0.0.1 aron566 2024.05.21 11:08 初始版本.
 *  @date 2024-05-21
 *
 *  @details None.
 *  @par 修改日志:
 *  <table>
 *  <tr><th>Date       <th>Version <th>Author  <th>Description
 *  <tr><td>2024-05-21 <td>v0.0.1  <td>aron566 <td>初始版本
 *  </table>
 *  @copyright Copyright (c) 2024  Suzhou Gate-Sea Co.,Ltd.
 */
/** Includes -----------------------------------------------------------------*/
#include "stdio.h"
/* Private includes ----------------------------------------------------------*/
#include "los_config.h"
#include "fs_init_sdk.h"
#include "gsmcu_iwdg.h"
#include "los_task.h"
#include "gsmcu_hal.h"
#include "uart.h"
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
extern void Error_Handler(void);
extern void Hilog_Init(void);

/* 测试代码 */
extern void GS_Gpio_Test(void);
extern void StartHdfSpiLittlefsTest(void);

/** Private user code --------------------------------------------------------*/

/** Private application code -------------------------------------------------*/
/*******************************************************************************
 *
 *       Static code
 *
 ********************************************************************************
 */

void TaskSampleEntry2(void)
{
  while (1)
  {
    printf("TaskSampleEntry2 running...\r\n");
    (void)LOS_TaskDelay(2000); /* 2000 millisecond */
  }
}

void TaskSampleEntry1(void)
{
  while (1)
  {
    printf("TaskSampleEntry1 running...\r\n");
    (void)LOS_TaskDelay(2000); /* 2000 millisecond */
  }
}

void TaskSample(void)
{
  UINT32           uwRet;
  UINT32           taskID1;
  UINT32           taskID2;
  TSK_INIT_PARAM_S stTask = {0};

  stTask.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskSampleEntry1;
  stTask.uwStackSize  = LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE;
  stTask.pcName       = "TaskSampleEntry1";
  stTask.usTaskPrio   = 6; /* Os task priority is 6 */
  uwRet               = LOS_TaskCreate(&taskID1, &stTask);
  if (uwRet != LOS_OK)
  {
    printf("Task1 create failed\r\n");
  }

  stTask.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskSampleEntry2;
  stTask.uwStackSize  = LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE;
  stTask.pcName       = "TaskSampleEntry2";
  stTask.usTaskPrio   = 7; /* Os task priority is 7 */
  uwRet               = LOS_TaskCreate(&taskID2, &stTask);
  if (uwRet != LOS_OK)
  {
    printf("Task2 create failed\r\n");
  }
}

/** Public application code --------------------------------------------------*/
/*******************************************************************************
 *
 *       Public code
 *
 ********************************************************************************
 */

/**
 * @brief 主入口函数
 *
 * @return int
 */
int main(void)
{
  /* 设置默认中断向量表 */
  SCB->VTOR = (UINT32)(UINTPTR)0x10000000;

#if LOSCFG_COMPILE_DEBUG
  WDG_Disable();
#endif

  /* 初始化GPIO */
  SCU_DisableALLGPIOInput();

  /* 初始化串口 */
  UART_Init();

  printf("entry user main!\r\n");

  /* DFX子系统日志输出接口初始化 */
  Hilog_Init();

  /* 内核初始化 */
  uint32_t ret = LOS_OK;
  ret = LOS_KernelInit();
  if (LOS_OK != ret)
  {
    Error_Handler();
  }

#if (LOSCFG_USE_SHELL == 1)
  ret = LosShellInit();
  if (LOS_OK != ret)
  {
    printf("LosShellInit failed! ERROR: 0x%x\n", ret);
  }
  ret = OsShellInit();
  if (LOS_OK != ret)
  {
    printf("OsShellInit failed! ERROR: 0x%x\n", ret);
  }
#endif

  /* 初始化fs */
  FsInit();

  /* 鸿蒙系统初始化 */
  OHOS_SystemInit();

#if LOSCFG_DRIVERS_HDF
  DeviceManagerStart();
#endif

  /* 执行gpio测试 */
  // GS_Gpio_Test();
  /* 执行littlefs测试 */
  // StartHdfSpiLittlefsTest();

  /* 创建任务 */
  // TaskSample();

  /* 使用OS中断处理 */
  NVIC_SetVector(SysTick_IRQn, (uint32_t)OsTickHandler);

  /* 启动内核 */
  LOS_Start();
  while (1)
  {
    printf("Start LiteOS Error\r\n");
  }
}

#ifdef __cplusplus /**< end extern c */
}
#endif
/******************************** End of file *********************************/
#endif
#if 0
/**
 *  @file main.c
 *  @brief None.
 *  @author aron566 (wei.chen@gate-sea.com)
 *  @version v0.0.1 aron566 2024.05.21 11:08 初始版本.
 *  @date 2024-05-21
 *
 *  @details None.
 *  @par 修改日志:
 *  <table>
 *  <tr><th>Date       <th>Version <th>Author  <th>Description
 *  <tr><td>2024-05-21 <td>v0.0.1  <td>aron566 <td>初始版本
 *  </table>
 *  @copyright Copyright (c) 2024  Suzhou Gate-Sea Co.,Ltd.
 */
/** Includes -----------------------------------------------------------------*/
#include "stdio.h"
/* Private includes ----------------------------------------------------------*/
#include "los_config.h"
#include "fs_init_sdk.h"
#include "gsmcu_iwdg.h"
#include "los_task.h"
#include "gsmcu_hal.h"
// #include "kal.h" // Enable KAL CMSIS API
#include "devmgr_service_start.h" //hdf
#include "osal_mem.h"
#include "osal_thread.h"
#include "osal_time.h"
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
extern void Error_Handler(void);
extern void Hilog_Init(void);

/* 测试代码 */
extern void GS_Gpio_Test(void);
extern void StartHdfSpiLittlefsTest(void);

/** Private user code --------------------------------------------------------*/

/** Private application code -------------------------------------------------*/
/*******************************************************************************
 *
 *       Static code
 *
 ********************************************************************************
 */
int TaskSampleEntry2(void *Par)
{
  while (1)
  {
    printf("TaskSampleEntry2 running...\r\n");
    (void)OsalMSleep(2000); /* 2000 millisecond */
  }
  return 0;
}

int TaskSampleEntry1(void *Par)
{
  while (1)
  {
    printf("TaskSampleEntry1 running...\r\n");
    (void)OsalMSleep(2000); /* 2000 millisecond */
  }
  return 0;
}
void TaskSample(void)
{
  int ret = HDF_SUCCESS;
  OSAL_DECLARE_THREAD(Task1Thread);
  OsalThreadCreate(&Task1Thread, TaskSampleEntry1, NULL);
  if (ret != HDF_SUCCESS)
  {
    printf("Task1 create failed\r\n");
    OsalThreadDestroy(&Task1Thread);
  }
  const struct OsalThreadParam Task1Param =
  {
    .name = "Task1",
    .priority = OSAL_THREAD_PRI_DEFAULT,
    .stackSize = LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE,
  };
  OsalThreadStart(&Task1Thread, &Task1Param);

  OSAL_DECLARE_THREAD(Task2Thread);
  ret = OsalThreadCreate(&Task2Thread, TaskSampleEntry2, NULL);
  if (ret != HDF_SUCCESS)
  {
    printf("Task2 create failed\r\n");
    OsalThreadDestroy(&Task2Thread);
  }
  const struct OsalThreadParam Task2Param =
  {
    .name = "Task2",
    .priority = OSAL_THREAD_PRI_DEFAULT,
    .stackSize = LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE,
  };
  OsalThreadStart(&Task2Thread, &Task2Param);
}

/** Public application code --------------------------------------------------*/
/*******************************************************************************
 *
 *       Public code
 *
 ********************************************************************************
 */

/**
 * @brief 主入口函数
 *
 * @return int
 */
int main(void)
{
  /* 设置默认中断向量表 */
  SCB->VTOR = (UINT32)(UINTPTR)0x10000000;

#if LOSCFG_COMPILE_DEBUG
  WDG_Disable();
#endif

  printf("%s user main!\n", __TIME__);

  /* 内核初始化 */
  uint32_t ret = LOS_OK;
  ret = LOS_KernelInit();
  if (LOS_OK != ret)
  {
    Error_Handler();
  }

#if (LOSCFG_USE_SHELL == 1)
  ret = LosShellInit();
  if (LOS_OK != ret)
  {
    printf("LosShellInit failed! ERROR: 0x%x\n", ret);
  }
  ret = OsShellInit();
  if (LOS_OK != ret)
  {
    printf("OsShellInit failed! ERROR: 0x%x\n", ret);
  }
#endif

  /* 鸿蒙系统初始化 */
  OHOS_SystemInit();

#if LOSCFG_DRIVERS_HDF
  DeviceManagerStart();
#endif

  /* 创建任务 */
  TaskSample();

  /* 使用OS中断处理 */
  NVIC_SetVector(SysTick_IRQn, (uint32_t)OsTickHandler);

  /* 初始化fs */
  FsInit();

  /* 执行gpio测试 */
  // GS_Gpio_Test();
  /* 执行littlefs测试 */
  // StartHdfSpiLittlefsTest();

  LOS_Start();

  while (1)
  {
    printf("Error\r\n");
  }
}

#ifdef __cplusplus /**< end extern c */
}
#endif
/******************************** End of file *********************************/
#endif
#if 0
/**
 *  @file main.c
 *  @brief None.
 *  @author aron566 (wei.chen@gate-sea.com)
 *  @version v0.0.1 aron566 2024.05.21 11:08 初始版本.
 *  @date 2024-05-21
 *
 *  @details None.
 *  @par 修改日志:
 *  <table>
 *  <tr><th>Date       <th>Version <th>Author  <th>Description
 *  <tr><td>2024-05-21 <td>v0.0.1  <td>aron566 <td>初始版本
 *  </table>
 *  @copyright Copyright (c) 2024  Suzhou Gate-Sea Co.,Ltd.
 */
/** Includes -----------------------------------------------------------------*/
#include "stdio.h"
/* Private includes ----------------------------------------------------------*/
#include "los_config.h"
#include "fs_init_sdk.h"
#include "gsmcu_iwdg.h"
#include "los_task.h"
#include "gsmcu_hal.h"
#include "kal.h" // Enable KAL CMSIS API
#include "devmgr_service_start.h" //hdf
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
extern void Error_Handler(void);
extern void Hilog_Init(void);

/* 测试代码 */
extern void GS_Gpio_Test(void);
extern void StartHdfSpiLittlefsTest(void);

/** Private user code --------------------------------------------------------*/

/** Private application code -------------------------------------------------*/
/*******************************************************************************
 *
 *       Static code
 *
 ********************************************************************************
 */
void TaskSampleEntry2(void *par)
{
  while (1)
  {
    printf("TaskSampleEntry2 running...\r\n");
    (void)osDelay(2000); /* 2000 millisecond */
  }
}

void TaskSampleEntry1(void *par)
{
  while (1)
  {
    printf("TaskSampleEntry1 running...\r\n");
    (void)osDelay(2000); /* 2000 millisecond */
  }
}
void TaskSample(void)
{
  osThreadId_t Task1_thread;
  osThreadAttr_t Task1_Att = {
    .name = "Task1",
    .priority = osPriorityNormal,
    .stack_size = 1024U
  };

  Task1_thread = osThreadNew(TaskSampleEntry1, NULL, &Task1_Att);
  if (Task1_thread == NULL)
  {
    printf("Task1 create failed\r\n");
  }

  osThreadId_t Task2_thread;
  osThreadAttr_t Task2_Att = {
    .name = "Task1",
    .priority = osPriorityNormal,
    .stack_size = 1024U
  };

  Task2_thread = osThreadNew(TaskSampleEntry2, NULL, &Task2_Att);
  if (Task2_thread == NULL)
  {
    printf("Task2 create failed\r\n");
  }
}

/** Public application code --------------------------------------------------*/
/*******************************************************************************
 *
 *       Public code
 *
 ********************************************************************************
 */

/**
 * @brief 主入口函数
 *
 * @return int
 */
int main(void)
{
  /* 设置默认中断向量表 */
  SCB->VTOR = (UINT32)(UINTPTR)0x10000000;

#if LOSCFG_COMPILE_DEBUG
  WDG_Disable();
#endif

  printf("%s user main!\n", __TIME__);

  /* 内核初始化 */
  uint32_t ret = LOS_OK;
  ret = LOS_KernelInit();
  if (LOS_OK != ret)
  {
    Error_Handler();
  }

#if (LOSCFG_USE_SHELL == 1)
  ret = LosShellInit();
  if (LOS_OK != ret)
  {
    printf("LosShellInit failed! ERROR: 0x%x\n", ret);
  }
  ret = OsShellInit();
  if (LOS_OK != ret)
  {
    printf("OsShellInit failed! ERROR: 0x%x\n", ret);
  }
#endif

  /* 鸿蒙系统初始化 */
  OHOS_SystemInit();

#if LOSCFG_DRIVERS_HDF
  DeviceManagerStart();
#endif

  /* 创建任务 */
  TaskSample();

  /* 使用OS中断处理 */
  NVIC_SetVector(SysTick_IRQn, (uint32_t)OsTickHandler);

  /* 初始化fs */
  FsInit();

  /* 执行gpio测试 */
  // GS_Gpio_Test();
  /* 执行littlefs测试 */
  // StartHdfSpiLittlefsTest();

  osKernelStart();

  while (1)
  {
    printf("Error\r\n");
  }
}

#ifdef __cplusplus /**< end extern c */
}
#endif
/******************************** End of file *********************************/
#endif

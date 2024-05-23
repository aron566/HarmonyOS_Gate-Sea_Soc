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
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus /**< use C compiler */
extern "C" {
#endif
/** Private macros -----------------------------------------------------------*/

#define OHOS_APP_RUN(func)                                                                        \
  void ohos_app_main(void)                                                                        \
  {                                                                                               \
    LOS_TaskDelay(100);                                                                           \
    printf("\n\033[1;32m<--------------- OHOS Application Start Here --------------->\033[0m\n"); \
    func();                                                                                       \
  }

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
  stTask.uwStackSize  = 0x1000;
  stTask.pcName       = "TaskSampleEntry1";
  stTask.usTaskPrio   = 6; /* Os task priority is 6 */
  uwRet               = LOS_TaskCreate(&taskID1, &stTask);
  if (uwRet != LOS_OK)
  {
    printf("Task1 create failed\r\n");
  }

  stTask.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskSampleEntry2;
  stTask.uwStackSize  = 0x1000;
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
#if LOSCFG_COMPILE_DEBUG
  WDG_Disable();
#endif

  printf("%s user main!\n", __TIME__);

  uint32_t ret = LOS_OK;
  ret = LOS_KernelInit();
  if (LOS_OK != ret)
  {
    Error_Handler();
  }

#if LOSCFG_DRIVERS_HDF
  DeviceManagerStart();
#endif

  /* 启动恢复子系统初始化 */
  OHOS_SystemInit();

  // FsInit();
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
#include "osal.h"
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus /**< use C compiler */
extern "C" {
#endif
/** Private macros -----------------------------------------------------------*/

#define OHOS_APP_RUN(func)                                                                        \
  void ohos_app_main(void)                                                                        \
  {                                                                                               \
    LOS_TaskDelay(100);                                                                           \
    printf("\n\033[1;32m<--------------- OHOS Application Start Here --------------->\033[0m\n"); \
    func();                                                                                       \
  }

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
int TaskSampleEntry2(void)
{
  while (1)
  {
    printf("TaskSampleEntry2 running...\r\n");
    (void)OsalMSleep(2000); /* 2000 millisecond */
  }
  return 0;
}

int TaskSampleEntry1(void)
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
  OSAL_DECLARE_THREAD(Task1_thread);
  OsalThreadCreate(&Task1_thread, TaskSampleEntry1, NULL);
  if (ret != HDF_SUCCESS)
  {
    printf("Task1 create failed\r\n");
  }
  const struct OsalThreadParam *Task1Param =
  {
    .name = "Task1",
    .priority = OSAL_THREAD_PRI_DEFAULT,
    .stackSize = 512
  }
  OsalThreadStart(&Task1Thread, &Task1Param);

  OSAL_DECLARE_THREAD(Task2Thread);
  ret = OsalThreadCreate(&Task2Thread, TaskSampleEntry2, NULL);
  if (ret != HDF_SUCCESS)
  {
    printf("Task2 create failed\r\n");
  }
  const struct OsalThreadParam Task2Param =
  {
    .name = "Task2",
    .priority = OSAL_THREAD_PRI_DEFAULT,
    .stackSize = 512
  }
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
#if LOSCFG_COMPILE_DEBUG
  WDG_Disable();
#endif
  int ret;

  printf("%s user main!\n", __TIME__);
  ret = LOS_KernelInit();
  if (ret == LOS_OK)
  {
#if 0   // def LOSCFG_DRIVERS_HDF
        DeviceManagerStart();
#endif
    OHOS_SystemInit();
    FsInit();
    LOS_Start();
  }

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

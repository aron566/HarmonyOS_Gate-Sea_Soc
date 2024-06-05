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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* Private includes ----------------------------------------------------------*/
#include "los_config.h"
#include "ohos_init.h"
#include "ohos_types.h"
#include "fs_init_sdk.h"
#include "gsmcu_iwdg.h"
#include "los_task.h"
#include "gsmcu_hal.h"
// #include "uart.h"
#include "bsp.h"
#include "apdma.h"
#include "gsmcu_trng.h"
#include "system_GSMCU_M3.h"
#include "serial_port.h"
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

/**
 * @brief 测试任务1
 *
 */
void TaskSampleEntry2(void)
{
  while (1)
  {
    printf("TaskSampleEntry2 running...\r\n");
    (void)LOS_TaskDelay(2000); /* 2000 millisecond */
  }
}

/**
 * @brief 测试任务2
 *
 */
void TaskSampleEntry1(void)
{
  while (1)
  {
    printf("TaskSampleEntry1 running...\r\n");
    (void)LOS_TaskDelay(2000); /* 2000 millisecond */
  }
}

/**
 * @brief 测试任务初始化
 *
 */
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

const uint32_t myDevicType = 0xffff;
// 此函数必须在初始化mpu前调用
void GetDeveiceType(void)
{
  // 引脚识别设备类型
  StaModeOpen();
  switch (GetStaModeValue())
  {
  case 0:
    *(uint32_t *)&myDevicType = 3; // meter
    break;
  case 1:
    *(uint32_t *)&myDevicType = 7; // 3 phase meter
    break;
  }
}
__STATIC_INLINE int NVIC_GetIRQ(IRQn_Type IRQn)
{
  return NVIC->ISER[((uint32_t)(IRQn) >> 5)] & (1 << ((uint32_t)(IRQn) & 0x1F)); /* disable interrupt */
}
void SystemClock_Config(void)
{
}
void RegisterMyInterrupt(void)
{

  // 将原有的vector注册到当前的系统中
  void (**vec_fun)(void) = (void (**)(void))(0x10000000);
  UINT32 intSave;
  intSave = LOS_IntLock();
  for (int i = 0; i < 46; i++)
  {
    // 本函数只是注册真实的开启和关闭中断由应用自己掌握
    int flag = NVIC_GetIRQ((IRQn_Type)i);
    LOS_HwiCreate(i, 4, 0, vec_fun[16 + i], NULL);
    if (flag)
    {
      NVIC_EnableIRQ((IRQn_Type)i);
    }
    else
    {
      NVIC_DisableIRQ((IRQn_Type)i);
    }
  }
  LOS_IntRestore(intSave);
  debug_ready();
}
UINT32 CreateTaskID1;
void App_CreateTask(void)
{

  while (1)
  {
    FeedWdg();
    LOS_TaskDelay(20); // 20ms延时
  }
}
static void App_CreateTaskInit(void)
{
  TSK_INIT_PARAM_S stTask = {0};
  int ret;
  stTask.pfnTaskEntry = (TSK_ENTRY_FUNC)App_CreateTask;
  stTask.uwStackSize = 4096;
  stTask.pcName = "HelloWorld";
  stTask.usTaskPrio = 6;
  ret = LOS_TaskCreate(&CreateTaskID1, &stTask);
  if (ret != LOS_OK)
  {
    __BKPT(1);
  }
}
SYS_RUN(App_CreateTaskInit);
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

  QSPI_Enable();
  FLASH_CMD_CONFIG();
  GetDeveiceType();
  FLASH_Init();
  FLASH_Reset();
  FLASH_Close();
  GetDevFlashID();
  Protection1MFlashNoVolatile();
  DataFlashInit();
  Protection1MFlash();

  // FLASH_WriteQeFlag();

  DataFlashClose();
  HAL_Init();

  // srand(getTrngRand());
  CloseClockGate();

  /* 初始化DMA GPIO等 */
  BSP_Init();

  /* 串口 */
  DebugOpen(115200);
  debug_ready();

  /* 初始化串口 3 */
  Serial_Port_Init();

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

  /* 中断导入os */
  RegisterMyInterrupt();

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

#if LOSCFG_USE_SYSTEM_DEFINED_INTERRUPT == 0
  /* 使用OS中断处理 */
  NVIC_SetVector(SysTick_IRQn, (uint32_t)OsTickHandler);
#endif

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

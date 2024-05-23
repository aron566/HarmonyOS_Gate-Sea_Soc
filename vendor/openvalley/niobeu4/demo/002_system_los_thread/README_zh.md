# NiobeU4开发板OpenHarmony内核编程开发——Thread多线程
本示例将演示如何在Niobe u4开发板上使用liteos-m接口进行多线程开发

## Thread struct分析

### TSK_INIT_PARAM_S

```
typedef struct tagTskInitParam {
    TSK_ENTRY_FUNC       pfnTaskEntry;              /< Task entrance function                 */
    UINT16               usTaskPrio;                /< Task priority                          */
    UINT32               uwArg;                     /< Task parameters                        */
    UINTPTR              stackAddr;                 /< Task satck memory                      */
    UINT32               uwStackSize;               /< Task stack size                        */
    CHAR                 *pcName;                   /< Task name                              */
    UINT32               uwResved;                  /**< Reserved                               */
} TSK_INIT_PARAM_S;
```

**描述：**
|名字|描述|
|:--|:------|
| pfnTaskEntry | 新建线程的函数入口  |
| usTaskPrio |线程优先级，0-31|
| uwArg | 线程函数传参 |
| stackAddr | 堆栈起始地址 |
| uwStackSize | 堆栈大小，当传入值为0时，使用默认值LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE |
| pcName | 线程名称 |

## Thread API分析

### LOS_TaskCreate()

```c
UINT32 LOS_TaskCreate(UINT32 *taskID, TSK_INIT_PARAM_S *taskInitParam);
```
**描述：**

函数LOS_TaskCreate通过将线程添加到活动线程列表并将其设置为就绪状态来启动线程函数。当创建的thread函数的优先级高于当前运行的线程且线程调度未被锁定时，创建的thread函数立即启动并成为新的运行线程。
可以在RTOS启动(调用 osKernelStart)之前安全地调用该函数，但不能在内核初始化 (调用 osKernelInitialize)之前调用该函数。
> **注意** :不能在中断服务调用该函数


**参数：**

|名字|描述|
|:--|:------|
| taskID | 指针，指向线程id  |
| taskInitParam |新建线程的相关属性以及线程的函数入口和参数等|

### LOS_TaskSuspend()

```c
UINT32 LOS_TaskSuspend(UINT32 taskID);
```
**描述：**

根据参数taskID，挂起相应任务。此时任务taskID如果处于运行态，会由运行态切换到阻塞态，并插入到阻塞队列中，而cpu会转去执行就绪列表中最高优先级的任务。
> **注意** :不能在任务调度处于关闭的状态下调用（即不能在调用osKernelLock之后调用）


**参数：**

|名字|描述|
|:--|:------|
| taskID | 线程id  |

### LOS_TaskResume()

```c
UINT32 LOS_TaskResume(UINT32 taskID)
```
**描述：**

根据参数taskID，恢复相应任务。此时任务taskID如果处于阻塞态，会由阻塞态切换到就绪态，并插入到就绪队列中，如果此时就绪队列中没有比该任务taskID更高优先级的任务，则任务taskID会由就绪态切换到运行态。


**参数：**

|名字|描述|
|:--|:------|
| taskID | 线程id  |

### osThreadJoin()

```c
UINT32 LOS_TaskJoin(UINT32 taskID, UINTPTR *retval);
```
**描述：**

挂起当前任务，等待指定任务taskID运行结束并回收其任务控制块资源。


**参数：**

|名字|描述|
|:--|:------|
| taskID | 线程id  |
| retval | 线程taskID结束后的返回值 |

### LOS_TaskDelete()

```c
UINT32 LOS_TaskDelete(UINT32 taskID);
```
**描述：**

删除指定线程/任务，释放其任务堆栈和任务控制块的资源

**参数：**

|名字|描述|
|:--|:------|
| taskID | 线程id  |

### LOS_TaskLock()
```c
VOID LOS_TaskLock(VOID);
```
**描述：**

锁任务调度。在任务调度被锁的状态下，不能调用LOS_TaskSuspend，LOS_TaskDelay


### LOS_TaskUnlock()
```c
UINT32 LOS_TaskUnlock(VOID);
```
**描述：**

解锁任务调度。


## 软件设计

**主要代码分析**

在OS_Thread_example函数中，通过osThreadNew()函数创建了threadHi和threadLo两个不同优先级的任务，包括任务延迟、挂起、恢复等操作，，threadHi和threadLo启动后会输出打印日志，通过打印日志可熟悉任务优先级调度机制。

```c
UINT32 g_taskHiId;
UINT32 g_taskLoId;
#define TSK_PRIOR_HI 4
#define TSK_PRIOR_LO 5

UINT32 Example_TaskHi(VOID)
{
    UINT32 ret;

    printf("Enter TaskHi Handler.\n");

    /* 延时100个Ticks，延时后该任务会挂起，执行剩余任务中最高优先级的任务(TaskLo任务) */
    ret = LOS_TaskDelay(100);
    if (ret != LOS_OK) {
        printf("Delay TaskHi Failed.\n");
        return LOS_NOK;
    }

    /* 100个Ticks时间到了后，该任务恢复，继续执行 */
    printf("TaskHi LOS_TaskDelay Done.\n");

    /* 挂起自身任务 */
    ret = LOS_TaskSuspend(g_taskHiId);
    if (ret != LOS_OK) {
        printf("Suspend TaskHi Failed.\n");
        return LOS_NOK;
    }
    printf("TaskHi LOS_TaskResume Success.\n");
    return ret;
}

/* 低优先级任务入口函数 */
UINT32 Example_TaskLo(VOID)
{
    UINT32 ret;

    printf("Enter TaskLo Handler.\n");

    /* 延时100个Ticks，延时后该任务会挂起，执行剩余任务中最高优先级的任务 */
    ret = LOS_TaskDelay(100);
    if (ret != LOS_OK) {
        printf("Delay TaskLo Failed.\n");
        return LOS_NOK;
    }

    printf("TaskHi LOS_TaskSuspend Success.\n");

    /* 恢复被挂起的任务g_taskHiId */
    ret = LOS_TaskResume(g_taskHiId);
    if (ret != LOS_OK) {
        printf("Resume TaskHi Failed.\n");
        return LOS_NOK;
    }
    return ret;
}

/* 任务测试入口函数，创建两个不同优先级的任务 */
UINT32 Example_TskCaseEntry(VOID)
{
    UINT32 ret;
    TSK_INIT_PARAM_S initParam;

    /* 锁任务调度，防止新创建的任务比本任务高而发生调度 */
    LOS_TaskLock();

    printf("LOS_TaskLock() Success!\n");

    UINT32 curTaskID = LOS_CurTaskIDGet();
    TSK_INFO_S taskInfo;
    ret = LOS_TaskInfoGet(curTaskID,&taskInfo);
    if(ret == LOS_OK)
    {
        printf("curTask pro = %d\n",taskInfo.usTaskPrio);
    }


    initParam.pfnTaskEntry = (TSK_ENTRY_FUNC)Example_TaskHi;
    initParam.usTaskPrio = TSK_PRIOR_HI;
    initParam.pcName = "TaskHi";
    initParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    initParam.uwResved = 0; /* detach 属性 */

    /* 创建高优先级任务，由于锁任务调度，任务创建成功后不会马上执行 */
    ret = LOS_TaskCreate(&g_taskHiId, &initParam);
    if (ret != LOS_OK) {
        LOS_TaskUnlock();

        printf("Example_TaskHi create Failed!\n");
        return LOS_NOK;
    }

    printf("Example_TaskHi create Success!\n");

    initParam.pfnTaskEntry = (TSK_ENTRY_FUNC)Example_TaskLo;
    initParam.usTaskPrio = TSK_PRIOR_LO;
    initParam.pcName = "TaskLo";
    initParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;

    /* 创建低优先级任务，由于锁任务调度，任务创建成功后不会马上执行 */
    ret = LOS_TaskCreate(&g_taskLoId, &initParam);
    if (ret != LOS_OK) {
        LOS_TaskUnlock();
        printf("Example_TaskLo create Failed!\n");
        return LOS_NOK;
    }

    printf("Example_TaskLo create Success!\n");

    /* 解锁任务调度，此时会发生任务调度，执行就绪队列中最高优先级任务 */
    LOS_TaskUnlock();
    ret = LOS_TaskJoin(g_taskHiId, NULL);
    if (ret != LOS_OK) {
        printf("Join Example_TaskHi Failed!\n");
    } else {
        printf("Join Example_TaskHi Success!\n");
    }
    return LOS_OK;
}

OHOS_APP_RUN(Example_TskCaseEntry);
```

## 编译调试

### 修改 Kconfig.liteos_m.board 文件

- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:
     `(Top) → Platform → Board Selection → select board niobeu4 → use openvalley niobeu4 application → niobeu4 application choose`
- 选择 `002_system_los_thread`
- 回到sdk根目录，执行`hb build`脚本进行编译。

### 运行结果<a name="section_os_thread_example"></a>

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志:
```c
LOS_TaskLock() Success!
curTask pro = 15
Example_TaskHi create Success!
Example_TaskLo create Success!
Enter TaskHi Handler.
Enter TaskLo Handler.
TaskHi LOS_TaskDelay Done.
TaskHi LOS_TaskSuspend Success.
TaskHi LOS_TaskResume Success.
Join Example_TaskHi Success!

```

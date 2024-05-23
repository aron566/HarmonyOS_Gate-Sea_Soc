# NiobeU4开发板OpenHarmony内核编程开发——定时器
本示例将演示如何在Niobe u4开发板上使用liteos-m 接口进行定时器开发

## Timer API分析


### LOS_SwtmrCreate()

```c
   UINT32 LOS_SwtmrCreate(UINT32 interval,
                              UINT8 mode,
                              SWTMR_PROC_FUNC handler,
                              UINT32 *swtmrID,
                              UINT32 arg
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
                              ,
                              UINT8 rouses,
                              UINT8 sensitive
#endif
                              );
```
**描述：**

创建一个一次性或周期性计时器，并将其与一个带参数的回调函数相关联。计时器在LOS_SwtmrStart启动之前一直处于停止状态。可以在RTOS启动(调用 LOS_Start)之前安全地调用该函数，但不能在内核初始化 (调用 LOS_KernelInit)之前调用该函数。
> **注意** :不能在中断服务调用该函数

**参数：**

|名字|描述|
|:--|:------|
| interval | 要创建的软件计时器的定时时长，单位ms |
| mode | 定时器类型,课参考EnSwTmrType，包括单次定时器、周期性定时器以及单次定时器但不删除定时器三种类型  |
| handler | 函数指针指向回调函数  |
| swtmrID | 定时器ID |
| argument |定时器回调函数的参数|
| arg |回调函数handler的入参|

### LOS_SwtmrStart()

```c
 UINT32 LOS_SwtmrStart(UINT32 swtmrID);
```
**描述：**

启动或重新启动指定参数swtmrID的计时器。

> **注意** :不能在中断服务调用该函数


**参数：**

|名字|描述|
|:--|:------|
| swtmrID | 由osTimerNew获得的计时器ID  |

### LOS_SwtmrStop()

```c
 UINT32 LOS_SwtmrStop(UINT32 swtmrID);
```
**描述：**

暂停指定参数swtmrID的计时器。

**参数：**

|名字|描述|
|:--|:------|
| swtmrID | 由LOS_SwtmrCreate获得的计时器ID  |

### LOS_SwtmrDelete()

```c
 UINT32 LOS_SwtmrDelete(UINT32 swtmrID);
```
**描述：**

删除指定参数swtmrID的计时器。

**参数：**

|名字|描述|
|:--|:------|
| swtmrID | 由LOS_SwtmrCreate获得的计时器ID  |

## 软件设计

### 主要代码分析

创建一个单次定时器timerId1,设置定时时长为1000，启动timerId1，记录当前系统tickCount，而后暂停定时器timerId1，
延期1000ticks后，可见timerId1并为被触发。再启动timerId1，在过去timerId1所剩的ticks数后，timerId1被触发。
创建一个周期性定时器timerId2，设置定时时长为100ticks，启动timerId2，可通过串口日志每隔100ticks触发timerId2。

```c
UINT32 g_timerCount1 = 0;   
UINT32 g_timerCount2 = 0;

/* 任务ID */
UINT32 g_testTaskId01;

void Timer1_Callback(UINT32 arg) // 回调函数1 
{
    UINT32 tick_last1;
    g_timerCount1++;
    tick_last1 = (UINT32)LOS_TickCountGet(); // 获取当前Tick数
    printf("g_timerCount1=%d, tick_last1=%d\n", g_timerCount1, tick_last1);
}  

void Timer2_Callback(UINT32 arg) // 回调函数2 
{
    UINT32 tick_last2;
    tick_last2 = (UINT32)LOS_TickCountGet();
    g_timerCount2++;
    printf("g_timerCount2=%d tick_last2=%d\n", g_timerCount2, tick_last2);
}  

void Timer_example(void)  
{
    UINT32 ret;
    UINT32 id1; // timer id1
    UINT32 id2; // timer id2
    UINT32 tickCount;

    /*创建单次软件定时器，Tick数为1000，启动到1000Tick数时执行回调函数1 */ 
    LOS_SwtmrCreate(1000, LOS_SWTMR_MODE_ONCE, Timer1_Callback, &id1, 1
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
        , OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_INSENSITIVE
#endif
    );

    /*创建周期性软件定时器，每100Tick数执行回调函数2 */
    LOS_SwtmrCreate(100, LOS_SWTMR_MODE_PERIOD, Timer2_Callback, &id2, 1
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
        , OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_INSENSITIVE
#endif
    );
    printf("create Timer1 success\n");

    LOS_SwtmrStart(id1); //启动单次软件定时器 
    printf("start Timer1 success\n");

    LOS_TaskDelay(200); //延时200Tick数
    LOS_SwtmrTimeGet(id1, &tickCount); // 获得单次软件定时器剩余Tick数
    printf("tickCount=%d\n", tickCount);

    LOS_SwtmrStop(timerId1); // 停止软件定时器
    printf("stop Timer1 success\n");
    LOS_TaskDelay(1000);

    printf("start Timer2\n");
    LOS_SwtmrStart(timerId1);
    LOS_SwtmrTimeGet(timerId1, &tickCount); // 获得单次软件定时器剩余Tick数
    printf("tickCount=%d\n", tickCount);
    LOS_TaskDelay(1000);

    LOS_SwtmrStart(timerId2); // 启动周期性软件定时器
    printf("start Timer2\n");

    LOS_TaskDelay(1000);
    LOS_SwtmrStop(id2);
    ret = LOS_SwtmrDelete(id2);  // 删除软件定时器
    if (ret == LOS_OK) {
        printf("delete Timer2 success\n");
    }
}

UINT32 Example_TaskEntry(VOID)
{
    UINT32 ret;
    TSK_INIT_PARAM_S task1;

    /* 锁任务调度 */
    LOS_TaskLock();

    /* 创建任务1 */
    (VOID)memset(&task1, 0, sizeof(TSK_INIT_PARAM_S));
    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)Timer_example;
    task1.pcName       = "TimerTsk";
    task1.uwStackSize  = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    task1.usTaskPrio   = 5;
    ret = LOS_TaskCreate(&g_testTaskId01, &task1);
    if (ret != LOS_OK) {
        printf("TimerTsk create failed.\n");
        return LOS_NOK;
    }

    /* 解锁任务调度 */
    LOS_TaskUnlock();

    return LOS_OK;
}
```

## 编译调试

### 修改 Kconfig.liteos_m.board 文件

- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:
     `(Top) → Platform → Board Selection → select board niobeu4 → use openvalley niobeu4 application → niobeu4 application choose`
- 选择 `003_system_los_timer`
- 回到sdk根目录，执行`hb build`脚本进行编译。

### 运行结果<a name="sec_os_timer_example"></a>

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志
```
create Timer1 success
start Timer1 success
tickCount=800
stop Timer1 success
start Timer2
tickCount=1000
g_timerCount1=1, tick_last1=2304
start Timer2
g_timerCount2=1 tick_last2=2404
g_timerCount2=2 tick_last2=2504
g_timerCount2=3 tick_last2=2604
g_timerCount2=4 tick_last2=2704
g_timerCount2=5 tick_last2=2804
g_timerCount2=6 tick_last2=2904
g_timerCount2=7 tick_last2=3004
g_timerCount2=8 tick_last2=3104
g_timerCount2=9 tick_last2=3204
g_timerCount2=10 tick_last2=3304
delete Timer2 success

```

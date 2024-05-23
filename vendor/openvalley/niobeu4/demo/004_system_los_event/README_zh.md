# NiobeU4开发板OpenHarmony内核编程开发——事件标志
本示例将演示如何在Niobe u4开发板上使用liteos-m 接口使用事件标志同步线程

## EventFlags API分析


### LOS_EventInit()

```c
UINT32 LOS_EventInit(PEVENT_CB_S eventCB);
```
**描述：**

创建了一个新的事件标志对象，用于跨线程发送事件，eventCB是指向要初始化的事件控制块的指针。可以在RTOS启动(调用 LOS_Start)之前安全地调用该函数，但不能在内核初始化 (调用 LOS_KernelInit)之前调用该函数。
> **注意** :不能在中断服务调用该函数

**参数：**

|名字|描述|
|:--|:------|
| eventCB | 输入/输出，指向要初始化的事件控制块的指针  |

### LOS_EventWrite()

```c
UINT32 LOS_EventWrite(PEVENT_CB_S eventCB, UINT32 events);
```
**描述：**
将传入的事件掩码指定的事件events写入eventCB指向的事件控制块。

> **注意** :不能在中断服务调用该函数


**参数：**

|名字|描述|
|:--|:------|
| eventCB | 事件控制块， 由调用LOS_EventInit初始化得到  |
| events | 事件掩码指定的事件  |

### LOS_EventRead()

```c
UINT32 LOS_EventRead(PEVENT_CB_S eventCB, UINT32 eventMask, UINT32 mode, UINT32 timeOut);
```
**描述：**
读事件（等待事件），任务会根据timeOut（单位：tick）进行阻塞等待；
未读取到事件时，返回值为0；
正常读取到事件时，返回正值（事件发生的集合）；
其他情况返回特定错误码。
> **注意** :如果参数timeout设置为0，可以从中断服务例程调用


**参数：**

|名字|描述|
|:--|:------|
| eventCB | 事件控制块， 由调用LOS_EventInit初始化得到 |
| eventMask | 事件类型掩码  |
| mode | 指定标记选项LOS_WAITMODE_AND、逻辑与  LOS_WAITMODE_OR、逻辑或  LOS_WAITMODE_CLR，这是一种附加读取模式，需要与所有事件模式或任一事件模式结合使用 |
| timeout | 超时时间，0表示不超时  |


## 软件设计

### 主要代码分析

创建三个线程OS_Thread_EventSender，OS_Thread_EventReceiverOR，OS_Thread_EventReceiverAND；线程OS_Thread_EventSender通过对g_event_flags_id写入不同的事件，分别实现其于线程OS_Thread_EventReceiverOR和OS_Thread_EventReceiverAND之间的同步：
线程OS_Thread_EventReceiverOR调用读事件进入阻塞状态，直到线程OS_Thread_EventSender写入事件0x00000001U；
线程OS_Thread_EventReceiverAND调用读事件进入阻塞状态，直到线程OS_Thread_EventSender写入事件0x00000001U和0x00000002U；

```c
#define FLAGS_MSK1 0x00000001U
#define FLAGS_MSK2 0x00000002U
EVENT_CB_S g_event;

//发送事件
void OS_Thread_EventSender(void *argument)
{
    (void)argument;
    while (1)
    {
        if(LOS_EventWrite(&g_event, FLAGS_MSK1) == LOS_OK)
        {
            printf("write eventFlags %d success.\n",FLAGS_MSK1);
        }
        LOS_TaskDelay(100);

        if(LOS_EventWrite(&g_event, FLAGS_MSK1|FLAGS_MSK2) == LOS_OK)
        {
            printf("write eventFlags %d success.\n",FLAGS_MSK1|FLAGS_MSK2);
        }
       
        LOS_TaskDelay(100);
    }
}

//接收事件
void OS_Thread_EventReceiverOR(void *argument)
{
    (void)argument;
    UINT32 flags;

    while (1)
    {
        flags = LOS_EventRead(&g_event, FLAGS_MSK1|FLAGS_MSK2, LOS_WAITMODE_OR|LOS_WAITMODE_CLR, 0xFFFFFFFFU);
        printf("EventReceiverOR wait event! Receive Flags is %d\n", flags);
    }
}

//接收事件
void OS_Thread_EventReceiverAND(void *argument)
{
    (void)argument;
    UINT32 flags;

    while (1)
    {
        flags = osEventFlagsWait(&g_event, FLAGS_MSK1|FLAGS_MSK2, LOS_WAITMODE_AND|LOS_WAITMODE_CLR, 0xFFFFFFFFU);
        printf("EventReceiverAND wait event! Receive Flags is %d\n", flags);
    }
}


//事件测试Example
static void OS_Event_example(void)
{
    if(LOS_EventInit(&g_event) != LOS_OK)
    {
        printf("Failed to create EventFlags!\n");
        return ;
    }

    LOS_TaskLock();
    TSK_INIT_PARAM_S attr={0};
    attr.pfnTaskEntry = (TSK_ENTRY_FUNC)OS_Thread_EventSender;
    attr.uwStackSize = 1024 * 4;
    attr.usTaskPrio = 5;

    attr.pcName = "Thread_EventSender";
    UINT32 taskIDEventSender;
    if (LOS_TaskCreate(&taskIDEventSender,&attr)!=LOS_OK)
    {
        printf("Failed to create Thread_EventSender!\n");
        return;
    }
    
    attr.pcName = "Thread_EventReceiverOR";
    attr.pfnTaskEntry = (TSK_ENTRY_FUNC)OS_Thread_EventReceiverOR;
    UINT32 taskIDEventRecvOR;
    if (LOS_TaskCreate(&taskIDEventRecvOR,&attr) != LOS_OK)
    {
        printf("Failed to create Thread_EventReceiverOR!\n");
        return;
    }

    attr.pcName = "Thread_EventReceiverAND";
    attr.pfnTaskEntry = (TSK_ENTRY_FUNC)OS_Thread_EventReceiverAND;
    UINT32 taskIDEventRecvAND;
    if (osThreadNew(&taskIDEventRecvAND, &attr) != LOS_OK)
    {
        printf("Failed to create Thread_EventReceiverAND!\n");
        return;
    }

    LOS_TaskUnlock();
}
```

## 编译调试

- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:
     `(Top) → Platform → Board Selection → select board niobeu4 → use openvalley niobeu4 application → niobeu4 application choose`
- 选择 `004_system_los_event`
- 回到sdk根目录，执行`hb build`脚本进行编译。


### 运行结果<a name="section18115713118"></a>

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志，会每隔1S输出一次日志。
```
write eventFlags 1 success.
EventReceiverOR wait event! Receive Flags is 1
write eventFlags 3 success.
EventReceiverAND wait event! Receive Flags is 3
EventReceiverOR wait event! Receive Flags is 0
write eventFlags 1 success.
EventReceiverOR wait event! Receive Flags is 1
write eventFlags 3 success.
EventReceiverOR wait event! Receive Flags is 3
EventReceiverAND wait event! Receive Flags is 0
write eventFlags 1 success.
EventReceiverOR wait event! Receive Flags is 1
write eventFlags 3 success.
EventReceiverAND wait event! Receive Flags is 3
EventReceiverOR wait event! Receive Flags is 0
```

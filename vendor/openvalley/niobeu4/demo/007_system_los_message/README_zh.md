# NiobeU4开发板OpenHarmony内核编程开发——message
本示例将演示如何在Niobe u4开发板上使用cmsis 2.0 接口进行消息队列开发


## message API分析

### LOS_QueueCreate()

```c
UINT32 LOS_QueueCreate(const CHAR *queueName,
                              UINT16 len,
                              UINT32 *queueID,
                              UINT32 flags,
                              UINT16 maxMsgSize);
```
**描述：**

创建消息队列
> **注意** :不能在中断服务调用该函数


**参数：**

|名字|描述|
|:--|:------|
| queueName | 消息队列名称，暂时未使用  |
| len |消息队列长度，该值的范围：[1,0xffff]|
| queueID |消息队列ID，属输出|
| flags | 队列模式，暂未用，可传0 |
| maxMsgSize | 消息的大小，该值范围：[1,0xffff-4] |

### LOS_QueueReadCopy()

```c
UINT32 LOS_QueueReadCopy(UINT32 queueID, VOID *bufferAddr, UINT32 *bufferSize, UINT32 timeOut);
```
**描述：**

读取消息队列，用于读取指定队列中的数据，并将获取到的数据存储到bufferAddr指定的地址中。 要读取的数据的地址和大小由用户定义。  
> **注意** :不能在中断服务调用该函数


**参数：**

|名字|描述|
|:--|:------|
| queueID | 队列ID |
| bufferAddr | 指针，存放读取到的消息 |
| bufferSize | 在读取之前维护缓冲区的期望大小，和读取后消息的实际大小 |
| timeOut | 超时值，范围：[0,LOS_WAIT_FOREVER]，单位tick |

### LOS_QueueWriteCopy()

```c
UINT32 LOS_QueueWriteCopy(UINT32 queueID, VOID *bufferAddr, UINT32 bufferSize, UINT32 timeOut);

```
**描述：**

发送消息，用于将bufferSize指定的大小和存储在bufferAddr指定地址的数据写入队列中。
> **注意** :如果参数timeout设置为0，可以从中断服务例程调用

**参数：**

|名字|描述|
|:--|:------|
| queueID |消息队列ID|
| bufferAddr |要发送的消息|
| bufferSize |消息大小|
| timeout |超时值|


### LOS_QueueRead()

```c
UINT32 LOS_QueueRead(UINT32 queueID, VOID *bufferAddr, UINT32 bufferSize, UINT32 timeOut);
```
**描述：**

获取消息，用于读取指定队列中的数据地址，并将其存储到由bufferAddr指定的地址中。
> **注意** :不能在中断服务调用该函数

**参数：**

|名字|描述|
|:--|:------|
| queueID |消息队列ID|
| bufferAddr | 存储获取数据的起始地址。起始地址不能为空。 |
| bufferSize | 传入的缓冲区大小，不能为0，范围:[1,0xffffffff]|
| timeout |超时值，范围：[0,LOS_WAIT_FOREVER]，单位tick|
返回	0 - 成功，非0 - 失败


### LOS_QueueWrite()

```c
UINT32 LOS_QueueWrite(UINT32 queueID, VOID *bufferAddr, UINT32 bufferSize, UINT32 timeOut);
```
**描述：**

发送消息

**参数：**

|名字|描述|
|:--|:------|
| queueID |消息队列ID|
| bufferAddr |要发送的消息|
| bufferSize |消息大小|
| timeout |超时值|


## 软件设计

### 主要代码分析

创建一个队列，两个任务。任务1调用写队列接口发送消息，任务2通过读队列接口接收消息。

```c
VOID SendEntry(VOID)
{
    UINT32 ret = 0;
    CHAR abuf[] = "test message";
    UINT32 len = sizeof(abuf);

    ret = LOS_QueueWriteCopy(g_queue, abuf, len, 0);
    if(ret != LOS_OK) {
        printf("send message failure, error: %x\n", ret);
    }
}

VOID RecvEntry(VOID)
{
    UINT32 ret = 0;
    CHAR readBuf[BUFFER_LEN] = {0};
    UINT32 readLen = BUFFER_LEN;

    //休眠1s
    usleep(1000000);
    ret = LOS_QueueReadCopy(g_queue, readBuf, &readLen, 0);
    if(ret != LOS_OK) {
        printf("recv message failure, error: %x\n", ret);
    }

    printf("recv message: %s\n", readBuf);

    ret = LOS_QueueDelete(g_queue);
    if(ret != LOS_OK) {
        printf("delete the queue failure, error: %x\n", ret);
    }

    printf("delete the queue success.\n");
}

UINT32 ExampleQueue(VOID)
{
    printf("start queue example.\n");
    UINT32 ret = 0;
    UINT32 task1, task2;
    TSK_INIT_PARAM_S initParam = {0};

    initParam.pfnTaskEntry = (TSK_ENTRY_FUNC)SendEntry;
    initParam.usTaskPrio = 9;
    initParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    initParam.pcName = "SendQueue";

    LOS_TaskLock();
    ret = LOS_TaskCreate(&task1, &initParam);
    if(ret != LOS_OK) {
        printf("create task1 failed, error: %x\n", ret);
        return ret;
    }

    initParam.pcName = "RecvQueue";
    initParam.pfnTaskEntry = (TSK_ENTRY_FUNC)RecvEntry;
    initParam.usTaskPrio = 10;
    ret = LOS_TaskCreate(&task2, &initParam);
    if(ret != LOS_OK) {
        printf("create task2 failed, error: %x\n", ret);
        return ret;
    }

    ret = LOS_QueueCreate("queue", 5, &g_queue, 0, 50);
    if(ret != LOS_OK) {
        printf("create queue failure, error: %x\n", ret);
    }

    printf("create the queue success.\n");
    LOS_TaskUnlock();
    return ret;
}

```

## 编译调试

- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:
     `(Top) → Platform → Board Selection → select board niobeu4 → use openvalley niobeu4 application → niobeu4 application choose`
- 选择 `007_system_los_message`
- 回到sdk根目录，执行`hb build`脚本进行编译。


### 运行结果<a name="section_os_message_example"></a>

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志
```c
 start queue example.
 create the queue success.
 recv message: test message
 delete the queue success.
```




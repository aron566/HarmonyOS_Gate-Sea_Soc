# NiobeU4开发板OpenHarmony内核编程开发——Semaphore
本示例将演示如何在Niobe u4开发板上使用liteos-m 接口进行信号量开发


## Semaphore API分析

### LOS_SemCreate()

```c
UINT32 LOS_SemCreate(UINT16 count, UINT32 *semHandle);
```
**描述：**

信号量创建，从未使用的信号量链表中获取一个信号量，并设定初值count，并将新建的信号量ID赋值给semHandle所指向的空间
> **注意** :不能在中断服务调用该函数


**参数：**

|名字|描述|
|:--|:------|
| count |信号量计数值的初始值，其取值范围：[0, OS_SEM_COUNTING_MAX_COUNT)|
| semHandle |指针，指向存放信号量ID的对象|

### LOS_BinarySemCreate()

```c
UINT32 LOS_BinarySemCreate(UINT16 count, UINT32 *semHandle);
```
**描述：**

创建二值信号量，从未使用的信号量链表中获取一个二值信号量，并设定初值count，并将新建的信号量ID赋值给semHandle所指向的空间
> **注意** :不能在中断服务调用该函数


**参数：**

|名字|描述|
|:--|:------|
| count |信号量计数值的初始值，其取值范围:[0,1]|
| semHandle |指针，指向存放信号量ID的对象|

### LOS_SemPost()

```c
UINT32 LOS_SemPost(UINT32 semHandle);
```
**描述：**

释放信号量
> **注意** :该函数可以在中断服务例程调用

**参数：**

|名字|描述|
|:--|:------|
| semHandle |信号量ID|
返回	0 - 成功，非0 - 失败


### LOS_SemPend()

```c
UINT32 LOS_SemPend(UINT32 semHandle, UINT32 timeout);
```
**描述：**

获取信号量


**参数：**

|名字|描述|
|:--|:------|
| semHandle |信号量ID|
| timeout |超时值|
返回	0 - 成功，非0 - 失败


### LOS_SemGetValue()

```c
UINT32 LOS_SemGetValue(UINT32 semHandle, INT32 *currVal);
```

**描述：**

获取当前可被使用的共享资源数


**参数：**

|名字|描述|
|:--|:------|
| semHandle |信号量ID|
| currVal | 指针，输出参数，存放当前可被使用的共享资源数 |
返回	0 - 成功，非0 - 失败

## 软件设计

### 主要代码分析

在semp_example函数中，创建了两个信号量semMutex和semSync，通过semSync来实现线程Thread_semp1和Thread_semp2之间的同步；通过semMutex来实现线程Thread_sempA、Thread_sempB、Thread_sempC、Thread_sempD对全局变量g_str的互斥访问。

```c
osSemaphoreId_t semMutex;
osSemaphoreId_t semSync;

static char g_str[7]="hello!" ;

void Thread_SempA(void)
{
    while(1)
    {
        osSemaphoreAcquire(semMutex,osWaitForever);
        uint32_t count = osSemaphoreGetCount(semMutex);
        printf("Thread_SempA read g_str = %s    semMutex available-count = %d\n",g_str,count);
        osDelay(100);
        osSemaphoreRelease(semMutex);
    }
}

void Thread_SempB(void)
{
    while(1)
    {
        osSemaphoreAcquire(semMutex,osWaitForever);
        uint32_t count = osSemaphoreGetCount(semMutex);
        printf("Thread_SempB read g_str = %s    semMutex available-count = %d\n",g_str,count);
        osDelay(100);
        osSemaphoreRelease(semMutex);
    }
}

void Thread_SempC(void)
{
    while(1)
    {
        osSemaphoreAcquire(semMutex,osWaitForever);
        uint32_t count = osSemaphoreGetCount(semMutex);
        printf("Thread_SempC read g_str = %s    semMutex available-count = %d\n",g_str,count);
        osDelay(100);
        osSemaphoreRelease(semMutex);
    }
}

void Thread_SempD(void)
{
    while(1)
    {
        osSemaphoreAcquire(semMutex,osWaitForever);
        uint32_t count = osSemaphoreGetCount(semMutex);
        printf("Thread_SempD read g_str = %s    semMutex available-count = %d\n",g_str,count);
        osDelay(100);
        osSemaphoreRelease(semMutex);
    }
}

void Thread_Semp1(void)
{
    osDelay(300U);
    osSemaphoreRelease(semSync);
    printf("Thread_Semp1 Release semSync.\n");
    osDelay(100U);
}

void Thread_Semp2(void)
{
    printf("entry Thread_Semp2 : try get semSync, timeout 100 ticks!\n");
    osStatus_t ret = osSemaphoreAcquire(semSync, 100);
    if(ret == osOK)
    {
        printf("Thread_Semp2 : got semSync.\n");
    }
    else if(ret == osErrorTimeout)
    {
        printf("Thread_Semp2: failed to get semSync! and try get semSync wait forever.\n");
    }

    osSemaphoreAcquire(semSync, osWaitForever);
    printf("Thread_Semp2: wait_forever and get semSync.\n");
    osDelay(100U);
}

void semp_example(void)
{
    //创建信号量，用户线程间同步
    semSync = osSemaphoreNew(2, 0, NULL);
    if (semSync == NULL)
    {
        printf("semSync create failed!\n");
    }

    //创建信号量，用户线程间互斥
    semMutex = osSemaphoreNew(3, 3, NULL);
    if (semMutex == NULL)
    {
        printf("semMutex create failed!\n");
    }

    osThreadAttr_t attr;

    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 4;
    attr.priority = 10;

    //创建两个线程，通过信号量实现线程间同步
    osKernelLock();
    attr.name = "Thread_Semp1";
    if (osThreadNew((osThreadFunc_t)Thread_Semp1, NULL, &attr) == NULL)
    {
        printf("create Thread_Semp1 failed!\n");
    }

    attr.name = "Thread_Semp2";
    if (osThreadNew((osThreadFunc_t)Thread_Semp2, NULL, &attr) == NULL)
    {
        printf("create Thread_Semp2 failed!\n");
    }
    osKernelUnlock();

    osDelay(400);
    osSemaphoreDelete(semSync);

    //创建四个线程，通过信号量实现线程间互斥
    osKernelLock();
    attr.name = "Thread_SempA";
    if (osThreadNew((osThreadFunc_t)Thread_SempA, NULL, &attr) == NULL)
    {
        printf("create Thread_SempA failed!\n");
    }
    attr.name = "Thread_SempB";
    if (osThreadNew((osThreadFunc_t)Thread_SempB, NULL, &attr) == NULL)
    {
        printf("create Thread_SempB failed!\n");
    }
    attr.name = "Thread_SempC";
    if (osThreadNew((osThreadFunc_t)Thread_SempC, NULL, &attr) == NULL)
    {
        printf("create Thread_SempC failed!\n");
    }
    attr.name = "Thread_SempD";
    if (osThreadNew((osThreadFunc_t)Thread_SempD, NULL, &attr) == NULL)
    {
        printf("create Thread_SempD failed!\n");
    }
    osKernelUnlock();

}

OHOS_APP_RUN(semp_example);

```

## 编译调试

- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:
     `(Top) → Platform → Board Selection → select board niobeu4 → use openvalley niobeu4 application → niobeu4 application choose`
- 选择 `006_system_los_semp`
- 回到sdk根目录，执行`hb build`脚本进行编译。


### 运行结果<a name="section_os_semp_example"></a>

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志
```c
entry Thread_Semp2 : try get semSync, timeout 100 ticks!
Thread_Semp2: failed to get semSync! and try get semSync wait forever.
Thread_Semp1 Release semSync.
Thread_Semp2: wait_forever and get semSync.
Thread_SempA read g_str = hello!
Thread_SempB read g_str = hello!
Thread_SempC read g_str = hello!
Thread_SempD read g_str = hello!
Thread_SempC read g_str = hello!
Thread_SempB read g_str = hello!
Thread_SempA read g_str = hello!
Thread_SempB read g_str = hello!
Thread_SempC read g_str = hello!
Thread_SempD read g_str = hello!
Thread_SempC read g_str = hello!
Thread_SempB read g_str = hello!
Thread_SempA read g_str = hello!
Thread_SempB read g_str = hello!
Thread_SempC read g_str = hello!
Thread_SempD read g_str = hello!
Thread_SempC read g_str = hello!
Thread_SempB read g_str = hello!
Thread_SempA read g_str = hello!
Thread_SempB read g_str = hello!
Thread_SempC read g_str = hello!
Thread_SempD read g_str = hello!
Thread_SempC read g_str = hello!
Thread_SempB read g_str = hello!
Thread_SempA read g_str = hello!
Thread_SempB read g_str = hello!
Thread_SempC read g_str = hello!
Thread_SempD read g_str = hello!
```





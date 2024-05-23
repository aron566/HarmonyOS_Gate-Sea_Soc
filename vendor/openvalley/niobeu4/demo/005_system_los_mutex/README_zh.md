# NiobeU4开发板OpenHarmony内核编程开发——mutex
本示例将演示如何在Niobe u4开发板上使用liteos-m 接口进行互斥锁开发

## mutex API分析

### LOS_MuxCreate()

```c
UINT32 LOS_MuxCreate(UINT32 *muxHandle);
```
**描述：**

创建互斥锁


**参数：**

|名字|描述|
|:--|:------|
| muxHandle | 互斥锁id |

### LOS_MuxPend()

```c
UINT32 LOS_MuxPend(UINT32 muxHandle, UINT32 timeout);
```
**描述：**

获取互斥锁


**参数：**

|名字|描述|
|:--|:------|
| muxHandle | 互斥锁id，由LOS_MuxCreate创建得到 |
| timeout | delay时间 |


### LOS_MuxPost()

```c
UINT32 LOS_MuxPost(UINT32 muxHandle);
```
**描述：**

释放互斥锁


**参数：**

|名字|描述|
|:--|:------|
| muxHandle | 互斥锁id，由LOS_MuxCreate创建得到 |

### LOS_MuxDelete()

```c
UINT32 LOS_MuxDelete(UINT32 muxHandle);
```
**描述：**

删除互斥锁


**参数：**

|名字|描述|
|:--|:------|
| muxHandle | 互斥锁id，由LOS_MuxCreate创建得到 |


## 软件设计

### 主要代码分析

创建四个线程，同时访问全局变量g_str，通过互斥锁控制实现 g_str中的值按照 "firstThread" -->  "firstThread+twoThread"  -->  "firstThread+twoThread+threeThread" --> 清空   上述步骤。

```c
UINT32 mutex1_id;
UINT32 mutex2_id;

EVENT_CB_S g_event_clean;
#define EVENT_MASK_CLEAN 0x00000001

static char g_str[256];

void firstThread(void)
{
	//osDelay(100U);
	while(1)
	{
		LOS_MuxPend(mutex1_id, osWaitForever);
		sprintf(g_str,"firstThread");
    	printf("firstThread is Acquire. g_str=%s\r\n",g_str);    	
		LOS_MuxPost(mutex1_id);
		LOS_TaskDelay(1000U);
	}
}

void twoThread(void)
{
	//osDelay(100U);
	while(1) 
	{
		LOS_MuxPend(mutex2_id, osWaitForever);
		char tmp[256]={0};
		sprintf(tmp,"%s+twoThread",g_str);
		memcpy(g_str,tmp,sizeof(tmp));
		printf("twoThread is Acquire. g_str=%s\r\n",g_str);
		LOS_MuxPost(mutex2_id);
    	LOS_TaskDelay(1000U);
	}
}

void threeThread(void)
{
    while(1)
	{
		LOS_MuxPend(mutex1_id, osWaitForever);
		LOS_MuxPend(mutex2_id, osWaitForever);
		char tmp[256]={0};
		sprintf(tmp,"%s+threeThread",g_str);
		memcpy(g_str,tmp,sizeof(tmp));
    	printf("threeThread is Acquire. g_str=%s\r\n",g_str);
		UINT32 flags = LOS_EventWrite(&g_event_clean, EVENT_MASK_CLEAN);
		printf("write event %x\n",flags);
		LOS_TaskDelay(100U);
		LOS_MuxPost(mutex1_id);
		LOS_MuxPost(mutex2_id);
		LOS_TaskDelay(1000U);
	}
}

void fourThread(void)
{
    while(1)
	{
		LOS_EventRead(&g_event_clean,EVENT_MASK_CLEAN,LOS_WAITMODE_OR,osWaitForever);
		memset(g_str,0,sizeof(g_str));
    	printf("threeThread:   clean g_str.\r\n");
	}
}

void os_mutex_example(void)
{
	memset(g_str,0,sizeof(g_str));
    TSK_INIT_PARAM_S attr={0};
    attr.uwStackSize = 1024 * 4;
    attr.usTaskPrio = 5;

	//创建互斥锁
	mutex1_id = LOS_MuxCreate(NULL);
    if (mutex1_id == NULL)
    {
      	printf("create Mutex1 failed!\n");
		  return ;
    }
	mutex2_id = osMutexNew(NULL);
    if (mutex2_id == NULL)
    {
      	printf("create Mutex2 failed!\n");
		LOS_MuxDelete(mutex1_id);
		return ;
    }

	//创建事件标志位
    if(LOS_EventInit(&g_event_clean) != LOS_OK)
    {
        printf("Failed to create EventFlags!\n");
		LOS_MuxDelete(mutex1_id);
		LOS_MuxDelete(mutex2_id);
        return ;
    }

	//锁任务调度
	LOS_TaskLock();
    attr.pcName = "firstThread";
	attr.pfnTaskEntry = (TSK_ENTRY_FUNC)firstThread;
	UINT32 taskIDFirst;
    if (osThreadNew(&taskIDFirst, &attr) != LOS_OK)
	{
    	printf("create firstThread failed!\n");
		LOS_MuxDelete(mutex1_id);
		LOS_MuxDelete(mutex2_id);
		LOS_EventDestroy(g_event_clean);
		return ;
    }

	attr.pcName = "twoThread";
	attr.pfnTaskEntry = (TSK_ENTRY_FUNC)twoThread;
	UINT32 taskIDTwo;
    if (osThreadNew(&taskIDTwo, &attr) != LOS_OK)
    {
      	printf("create twoThread failed!\n");
    }

    attr.pcName = "threeThread";
	attr.pfnTaskEntry = (TSK_ENTRY_FUNC)threeThread;
	UINT32 taskIDThree;
    if (osThreadNew(&taskIDThree, &attr) != LOS_OK)
    {
      	printf("create threeThread failed!\n");
    }

    attr.pcName = "fourThread";
	attr.pfnTaskEntry = (TSK_ENTRY_FUNC)fourThread;
	UINT32 taskIDFour;
    if (osThreadNew(&taskIDFour, &attr) != LOS_OK)
    {
      	printf("create fourThread failed!\n");
    }

	//解锁任务调度
	LOS_TaskUnlock();

	//osThreadYield()
}

```

## 编译调试

- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:
     `(Top) → Platform → Board Selection → select board niobeu4 → use openvalley niobeu4 application → niobeu4 application choose`
- 选择 `005_system_los_mutex`
- 回到sdk根目录，执行`hb build`脚本进行编译。


### 运行结果<a name="section_os_mutex_example"></a>

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志
```c
firstThread is Acquire. g_str = firstThread
twoThread is Acquire. g_str = firstThread+twoThread
threeThread is Acquire. g_str = firstThread+twoThread+threeThread
write event 1
threeThread:   clean g_str.

firstThread is Acquire. g_str = firstThread
twoThread is Acquire. g_str = firstThread+twoThread
threeThread is Acquire. g_str = firstThread+twoThread+threeThread
write event 1
threeThread:   clean g_str.

firstThread is Acquire. g_str = firstThread
twoThread is Acquire. g_str = firstThread+twoThread
threeThread is Acquire. g_str = firstThread+twoThread+threeThread
write event 1
threeThread:   clean g_str.
```




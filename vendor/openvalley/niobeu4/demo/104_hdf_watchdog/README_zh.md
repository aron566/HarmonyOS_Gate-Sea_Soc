# NiobeU4开发板WATCHDOG使用说明文档
本示例将演示如何在Niobeu4开发板上使用WATCHDOG接口进行任务看门狗开发

## 接口说明

### WatchdogOpen()

```
int32_t WatchdogOpen(int16_t wdtId,  DevHandle *handle);
```

**描述：**

watchdog Open打开看门狗函数。

**参数：**

| 名字   | 描述     |
| :----- | :------- |
| wdtId  | 看门狗ID |
| handle | 设备句柄 |

### WatchdogClose()

```
void WatchdogClose(DevHandle handle);
```

**描述：**

watchdog close关闭看门狗函数。

**参数：**

| 名字   | 描述     |
| :----- | :------- |
| handle | 设备句柄 |

### WatchdogFeed()

```
int32_t WatchdogFeed(DevHandle handle);
```

**描述：**

watchdog feed喂狗函数。

**参数：**

| 名字   | 描述     |
| :----- | :------- |
| handle | 设备句柄 |

### WatchdogGetStatus()

```
int32_t WatchdogGetStatus(DevHandle handle, int32_t *status);
```

**描述：**

watchdog getStatus获取看门狗状态函数。

**参数：**

| 名字   | 描述           |
| :----- | :------------- |
| handle | 设备句柄       |
| status | 看门狗状态指针 |

### WatchdogStart()

```
int32_t WatchdogStart(DevHandle handle);
```

**描述：**

watchdog start启动看门狗函数。

**参数：**

| 名字   | 描述     |
| :----- | :------- |
| handle | 设备句柄 |

### watchdogStop()

```
int32_t WatchdogStop(DevHandle handle);
```

**描述：**

watchdog stop停止看门狗函数。

**参数：**

| 名字   | 描述     |
| :----- | :------- |
| handle | 设备句柄 |

### WatchdogSetTimeout()

```
int32_t WatchdogSetTimeout(DevHandle handle, uint32_t seconds);
```

**描述：**

watchdog setTimeout设置看门狗超时时间函数。

**参数：**

| 名字    | 描述     |
| :------ | :------- |
| handle  | 设备句柄 |
| seconds | 超时时间 |

### WatchdogGetTimeout()

```
int32_t WatchdogGetTimeout(DevHandle handle, uint32_t *seconds);
```

**描述：**

watchdog getTimeout获取看门狗超时时间函数。

**参数：**

| 名字    | 描述                   |
| :------ | :--------------------- |
| handle  | 设备句柄               |
| seconds | 超时时间指针，单位为秒 |

## 编译调试

- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:
  `(Top) → Platform → Board Selection → select board niobeu4 → use openvalley niobeu4 application → niobeu4 application choose`
- 选择 `104_hdf_watchdog`
- 回到sdk根目录，执行`hb build`脚本进行编译。

## hcs配置

hcs配置文件在device/board/openvalley/niobeu4/liteos_m/hdf_config/hdf.hcs目录下，需要进行如下设置：

```
watchdog_config {
    watchdog0_config {
        match_attr = "watchdog0";
        id = 0;         // 看门狗ID
        timeout = 1;    // 看门狗超时时间，单位s
    }
  }
```

# NiobeU4开发板ADC使用说明文档

## 简介
本案例程序将演示ADC采集相关程序。

## 接口说明

### AdcOpen()

```
int32_t AdcOpen(uint32_t number)
```

**描述：**

开启ADC函数，使用ADC的时候调用。

**参数：**

| 名字   | 描述                       |
| :----- | :------------------------- |
| number | 设置 1：ADC序号, 范围[1,2] |

### AdcClose()

```
void AdcClose(DevHandle handle)
```

**描述：**

关闭ADC函数，关闭ADC的时候调用。

**参数：**

| 名字   | 描述     |
| :----- | :------- |
| handle | 设备句柄 |

### AdcRead()

```
int32_t  AdcRead(DevHandle handle, uint32_t channel, uint32_t *val)
```

**描述：**

关闭ADC函数，关闭ADC的时候调用。

**参数：**

| 名字    | 描述                                                         |
| :------ | :----------------------------------------------------------- |
| handle  | 设备句柄                                                     |
| channel | ADC通道,默认为6，6:GPIO34,范围 ADC1_CHANNEL:0-7, ADC2_CHANNEL:0-9 |
| val     | ADC采集结果指针                                              |

## 约束

ADC程序使用C语言编写，目前支持NiobeU4开发板。

## 案例程序
```
#define ADC_DEVICE_NUM 1
#define ADC_CHANNEL_NUM 6

osThreadId_t g_taskID = NULL;

static void adc_test(void)
{
    DevHandle adcHandle;
    uint32_t readVal = 0;

    adcHandle = AdcOpen(ADC_DEVICE_NUM);
    if (adcHandle == NULL) {
        HDF_LOGE("AdcOpen fail!\r\n");
        return;
    }

    for(;;){
        if(AdcRead(adcHandle, ADC_CHANNEL_NUM, &readVal) != HDF_SUCCESS){
            HDF_LOGE("%s: read adc fail!\r\n", __func__);
            AdcClose(adcHandle);
            osThreadTerminate(g_taskID);
            return;
        }

        HDF_LOGE("Read adc value: %d\r\n",readVal);
        osDelay(1000);
    }
}

static void adc_task()
{
    HDF_LOGE("into adc hdf example!\n");
    osThreadAttr_t attr;
    attr.name = "adc_test";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 4;
    attr.priority = 25;
    g_taskID = osThreadNew((osThreadFunc_t)adc_test, NULL, &attr);
    if (g_taskID == NULL) {
        HDF_LOGE("Failed to create Test ADC thread!\n");
    }
}
```

## 编译调试

- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:
     `(Top) → Platform → Board Selection → select board niobeu4 → use openvalley niobeu4 application → niobeu4 application choose`
- 选择 `101_hdf_adc`
- 回到sdk根目录，执行`hb build`脚本进行编译。

## 运行结果
```
ADC voltage val: 144
ADC voltage val: 142
ADC voltage val: 142
ADC voltage val: 142
ADC voltage val: 142
ADC voltage val: 142
ADC voltage val: 142
ADC voltage val: 142
ADC voltage val: 142
ADC voltage val: 142
ADC voltage val: 142
ADC voltage val: 144
ADC voltage val: 142
```
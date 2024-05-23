# NiobeU4开发板SPI使用说明文档

## 简介

SPI（Serial Peripheral Interface） 协议是由摩托罗拉公司提出的通讯协议，即串行外围设备接口，是一种高速全双工的通信总线。它被广泛地使用在 ADC、LCD 等设备与 MCU 间，要求通讯速率较高的场合。

芯片的管脚上只占用四根线。
MISO： 主器件数据输出，从器件数据输入。
MOSI：主器件数据输入，从器件数据输出。
SCK： 时钟信号，由主设备控制发出。
NSS（CS）： 从设备选择信号，由主设备控制。当NSS为低电平则选中从器件。

## 接口说明

### SpiOpen()

```
DevHandle SpiOpen(const struct SpiDevInfo *info);
```

**描述：**

spi open获取SPI设备的句柄。

**参数：**

| 名字 | 描述                  |
| :--- | :-------------------- |
| info | 指向SPI设备信息的指针 |

### SpiTransfer()

```
int32_t SpiTransfer(DevHandle handle, struct SpiMsg *msgs, uint32_t count);
```

**描述：**

 uart transfer SPI设备的自定义传输函数。

**参数：**

| 名字   | 描述                  |
| :----- | :-------------------- |
| handle | 指向SPI设备句柄的指针 |
| msgs   | 要传输的数据的指针    |
| count  | 消息结构数组的长度    |

### SpiClose()

```
void SpiClose(DevHandle handle);
```

**描述：**

spi close关闭SPI设备的句柄。

**参数：**

| 名字   | 描述                  |
| :----- | :-------------------- |
| handle | 指向SPI设备句柄的指针 |

## 编译调试

- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:
  `(Top) → Platform → Board Selection → select board niobeu4 → use openvalley niobeu4 application → niobeu4 application choose`
- 选择 `107_hdf_spi`
- 回到sdk根目录，执行`hb build`脚本进行编译。

## HCS配置

hcs配置文件在device/board/openvalley/niobeu4/liteos_m/hdf_config/hdf.hcs目录下，需要进行如下设置：

    spi_config {
        template default_config  {
            match_attr = "spi0_config";
            spi_num = 0;    // SPI端口号
            /* spi引脚配置，分别对应 miso_io_num、mosi_io_num、sclk_io_num、spics_io_num */
            spi_pin = ["GPIO_NUM_2","GPIO_NUM_7","GPIO_NUM_6","GPIO_NUM_10"];
            clock_speed_hz = 10000000;  // 时钟信号频率、单位(hz)
            spi_mode = 0;               // SPI模式,表示CPOL与CPHA配置(0-3)
            max_transfer_size = 12168;  // 最大传输字节数
            queue_size = 7;             // 队列长度
        }
        spi0 :: default_config {
            spi_num = 0;
        }
    }   

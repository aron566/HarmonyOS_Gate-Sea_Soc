# NiobeU4开发板UART使用说明文档

## 简介

UART是通用异步收发传输器（Universal Asynchronous Receiver/Transmitter）的缩写，是通用串行数据总线，用于异步通信。该总线双向通信，可以实现全双工传输。UART通信之前，收发双方需要约定好一些参数：波特率、数据格式（起始位、数据位、校验位、停止位）等。通信过程中，UART通过TX发送给对端数据，通过RX接收对端发送的数据。

## 接口说明

### UartOpen()

```
DevHandle UartOpen(uint32_t port);
```

**描述：**

uart open初始化函数。

**参数：**

| 名字 | 描述                                               |
| :--- | :------------------------------------------------- |
| port | 对应的uart号，ESP32U4芯片有三组uart，分别为0、1、2 |

### UartRead()

```
int32_t UartRead(DevHandle handle, uint8_t *data, uint32_t size);
```

**描述：**

uart read读数据函数。

**参数：**

| 名字   | 描述                       |
| :----- | :------------------------- |
| handle | 指向uart设备句柄的指针     |
| data   | uart读取数据后的缓存地址   |
| size   | uart本次期望读取的数据长度 |

### UartWrite()

```
int32_t UartWrite(DevHandle handle, uint8_t *data, uint32_t size);
```

**描述：**

uartwrite写数据函数。

**参数：**

| 名字   | 描述                       |
| :----- | :------------------------- |
| handle | 指向uart设备句柄的指针     |
| data   | uart待发送的数据           |
| size   | uart本次期望发送的数据长度 |

### UartGetBaud()

```
int32_t UartGetBaud(DevHandle handle, uint32_t *baudRate);
```

**描述：**

uart getBaud获取波特率函数。

**参数：**

| 名字     | 描述                   |
| :------- | :--------------------- |
| handle   | 指向uart设备句柄的指针 |
| baudRate | 获取到的波特率         |

### UartSetAttribute()

```
int32_t UartSetAttribute(DevHandle handle, struct UartAttribute *attribute);
```

**描述：**

uart setTransMode设置UART属性函数。

**参数：**

| 名字      | 描述                       |
| :-------- | :------------------------- |
| handle    | 指向uart设备句柄的指针     |
| attribute | 指向要设置的UART属性的指针 |

### UartSetTransMode()

```
int32_t UartSetTransMode(DevHandle handle, enum UartTransMode mode);
```

**描述：**

uart setTransMode设置UART传输模式函数。

**参数：**

| 名字   | 描述                   |
| :----- | :--------------------- |
| handle | 指向uart设备句柄的指针 |
| mode   | 传输模式枚举           |


## HCS配置

hcs配置文件在`device/board/openvalley/niobeu4/liteos_m/hdf_config/hdf.hcs`目录下，需要进行如下设置：

    uart_config {
       template default_config {
           match_attr = "config_uart0";// 匹配字符串，与device_info.hcs中对应
           uart_port = 0;              // 串口端口: UART0
           baudrate = 115200;          // 波特率
           /* Tx引脚GPIO号、Rx引脚GPIO号、rts引脚GPIO号、cts引脚GPIO号*/
           uart_gpio_pin = [1, 3, 255, 255];     // 串口引脚GPIO号, 无流控则引脚号置为255
           /* uart_word_length_t   uart_parity_t   uart_stop_bits_t  uart_hw_flowcontrol_t */
           /*               数据位 、         校验位、                 停止位、         流控 */
           uart_attr = ["UART_DATA_8_BITS", "UART_PARITY_DISABLE", "UART_STOP_BITS_1", "UART_HW_FLOWCTRL_DISABLE"];
    }

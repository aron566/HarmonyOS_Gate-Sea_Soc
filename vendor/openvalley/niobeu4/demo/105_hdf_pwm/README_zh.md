# NiobeU4开发板PWM使用说明文档
本示例将演示如何在Niobeu4开发板上使用PWM接口进行PWM开发

## 接口说明

### PwmOpen()

```
DevHandle PwmOpen(uint32_t num);
```

**描述：**

pwm Open打开pwm函数。

**参数：**

| 名字 | 描述      |
| :--- | :-------- |
| num  | PWM设备号 |

### PwmSetConfig()

```
int32_t PwmSetConfig(DevHandle handle, struct PwmConfig *config);
```

**描述：**

pwm setConfig设置PWM设备的配置参数函数。

**参数：**

| 名字   | 描述                    |
| :----- | :---------------------- |
| handle | 设备句柄                |
| config | 指向PwmConfig结构体指针 |

### PwmSetDuty()

```
int32_t PwmSetDuty(DevHandle handle, uint32_t duty);
```

**描述：**

pwm setDuty设置PWM占空比函数。

**参数：**

| 名字   | 描述                       |
| :----- | :------------------------- |
| handle | 设备句柄                   |
| config | 要设置的占空比，单位为纳秒 |

## 编译调试

- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:
  `(Top) → Platform → Board Selection → select board niobeu4 → use openvalley niobeu4 application → niobeu4 application choose`
- 选择 `105_hdf_pwm`
- 回到sdk根目录，执行`hb build`脚本进行编译。

## hcs配置

hcs配置文件在device/board/openvalley/niobeu4/liteos_m/hdf_config/hdf.hcs目录下，需要进行如下设置：

```
pwm_config {
    template default_config {
        match_attr = "config_pwm0";
        channel = "LEDC_CHANNEL_0";         // PWM通道, 对应 ledc_channel_t 枚举
        timer = "LEDC_TIMER_0";             // 定时器单元，对应 ledc_timer_t 枚举
        freq_hz = 2000;                      // PWM周期频率(单位Hz)
        gpio_pin = "GPIO_NUM_21";           // 外部PWM输出的GPIO Pin脚，对应 gpio_num_t 枚举
    }
    pwm0 :: default_config {
        match_attr = "config_pwm0";
    }
    pwm1 :: default_config {
        match_attr = "config_pwm1";
        channel = "LEDC_CHANNEL_1";
        timer = "LEDC_TIMER_1";
        pwm_pin = "GPIO_NUM_25";
    }
}
```

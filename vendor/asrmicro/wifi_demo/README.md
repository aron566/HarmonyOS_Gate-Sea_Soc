# ASR Wi-Fi Demo说明

## tests list

编辑config.json，在tests_list中的test_modules增减项即可
```
"tests_list": [
    {
    "enable": "true",
    "test_modules": [
        "example",
        "wifi_test",
        "hdf_test"
    ]
    }
]
```

## wifi命令

### wifi_open sta [SSID] [KEY]
连接路由指令

### wifi_close
断开指令

## HDF测试
hdf_test模块会测试gpio和uart0：
1) 每分钟闪一次开发板上的红色LED2，按下KEY1按钮，串口会打印“user key 1 pressed”。
2) 短接GPIO1排针的pin1和pin4，uart0会进行收发测试，串口打印Hello OpenHarmony。

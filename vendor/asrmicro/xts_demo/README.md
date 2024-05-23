# ASR XTS测试说明

## xts list

编辑config.json，在xts_list中的xts_modules增减项即可
```
"xts_list": [
    {
    "enable": "true",
    "xts_modules": [
        "ActsKvStoreTest",
        "ActsDfxFuncTest",
        "ActsHieventLiteTest",
        "ActsSamgrTest",
        "ActsParameterTest",
        "ActsWifiServiceTest",
        "ActsWifiIotTest",
        "ActsBootstrapTest"
    ]
    }
]
```

## xts命令

### kv_clear
清除KV区域

### at_harmony_xts
执行XTS



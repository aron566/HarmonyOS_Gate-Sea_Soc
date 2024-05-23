/*
 * Copyright (c) 2022 Hunan OpenValley Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __DEVICE_INIT_H__
#define __DEVICE_INIT_H__

#include "los_task.h"
#include "fm17622_reg.h"
#include "reader_api.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// #define NFC_DEBUG       //NFC调试宏

typedef enum {
    NFC_SUCCESS,
    NFC_ERROR,
    NFC_CRC_ERROR,
} NfcError;

enum {RESET = 0, SET = !RESET};

typedef enum {
    ENABLE = 0,
    DISABLE = !ENABLE
} FunctionalState;


typedef void (*NfcTypeACallback)(struct picc_a_struct *picc_typea_info);   // NFC读卡回调函数
typedef void (*NfcTypeBCallback)(struct picc_b_struct *picc_typeb_info);   // NFC读卡回调函数
typedef void (*NfcTouchCallback)(VOID* arg);       // 回调接口

#define BIT0               0x01
#define BIT1               0x02
#define BIT2               0x04
#define BIT3               0x08
#define BIT4               0x10
#define BIT5               0x20
#define BIT6               0x40
#define BIT7               0x80

#define NUM2               2
#define NUM3               3
#define NUM4               4
#define NUM5               5
#define NUM6               6
#define NUM7               7
#define NUM8               8
#define NUM9               9
#define NUM10              10
#define NUM11              11
#define NUM12              12
#define NUM13              13
#define NUM14              14
#define NUM15              15

#define	LPCD_THRSH_H         0x00
#define	LPCD_THRSH_L         0x10
#define LPCD_CWP                4       // LPCD PMOS输出驱动 0~63
#define	LPCD_CWN                4       // LPCD NMOS输出驱动 0~15
#define	LPCD_SLEEPTIME         16       // LPCD 唤醒间隔时间，每一档为32ms，休眠时间：(16+1)*32=544ms

#define USLEEP_2MS  (2*1000)
#define USLEEP_5MS  (5*1000)
#define USLEEP_10MS  (10*1000)
/**
 * @brief 对应hcs文件中gpio的配置序号
 *
 */
#define LED_RED 0                // 红色LED
#define LED_BLUE  1              // 蓝色LED
#define PWR_SW_PIN_INDEX    2    // VCC_3.3 电源使能
#define NFC_RESET_PIN_INDEX 7    // NFC复位引脚号
#define NFC_INT_PIN_INDEX 8      // NFC中断引脚号
#define I2C_BUS 0                // I2C总线号

NfcError NfcInit(void);
void SetNfcTypeACallback(NfcTypeACallback pfunc);
void SetNfcTypeBCallback(NfcTypeBCallback pfunc);
VOID *GetCallbackArg(void);
NfcTypeACallback GetNfcTypeACallback(void);
NfcTypeBCallback GetNfcTypeBCallback(void);
NfcTouchCallback GetNfcTouchCallback(void);
NfcError NfcSetTouchCallback(NfcTouchCallback pfunc, VOID* arg);
void NfcDeinit(void);
void FM17622_HardReset(void);
unsigned char FM17622_SoftReset(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __DEVICE_INIT_H__ */
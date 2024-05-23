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
#include <stdio.h>
#include <hdf_log.h>
#include <i2c_if.h>
#include <gpio_if.h>
#include "gpio_types.h"
#include "cmsis_os2.h"
#include "ohos_run.h"
#include "device_init.h"

#define NFC_SERVICE_STACK_SIZE  (1024 * 4)
#define NFC_SERVICE_TASK_PRI    25
#define I2C_WRITE_BUF_SIZE  2

osEventFlagsId_t gNfcIrqEvent;
NfcTypeACallback gNfcTypeACallback = NULL;
NfcTypeBCallback gNfcTypeBCallback = NULL;
VOID *g_callback_arg = NULL;
NfcTouchCallback gNfcTouchCallback = NULL;
static DevHandle g_i2c_handle = NULL;

VOID *GetCallbackArg(void)
{
    return g_callback_arg;
}

NfcTypeACallback GetNfcTypeACallback(void)
{
    return gNfcTypeACallback;
}

NfcTypeBCallback GetNfcTypeBCallback(void)
{
    return gNfcTypeBCallback;
}

NfcTouchCallback GetNfcTouchCallback(void)
{
    return gNfcTouchCallback;
}

/**
 * @brief NFC中断函数
 *
 */
static int32_t NfcIrqFunc(uint16_t gpio, VOID *data)
{
    (void)gpio;
    (void)data;
    GpioWrite(LED_RED, GPIO_VAL_HIGH);
    GpioWrite(LED_BLUE, GPIO_VAL_HIGH);
    osEventFlagsSet(gNfcIrqEvent, 1); // 产生中断,通知读取NFC数据
}

/**
 * @brief GPIO初始化
 *
 * @return int
 */
int GpioConfig(void)
{
    GpioSetDir(PWR_SW_PIN_INDEX, GPIO_DIR_OUT);     // 设置VCC_3.3电源使能
    GpioSetDir(NFC_RESET_PIN_INDEX, GPIO_DIR_OUT);  // 设置NFC复位引脚为输出
    GpioWrite(PWR_SW_PIN_INDEX, GPIO_VAL_HIGH);
    GpioSetDir(NFC_INT_PIN_INDEX, GPIO_DIR_IN);  // NFC中断引脚配置为输入
    GpioSetIrq(NFC_INT_PIN_INDEX, GPIO_IRQ_TRIGGER_FALLING, (GpioIrqFunc)NfcIrqFunc, NULL); // 设置中断下降沿触发
    GpioEnableIrq(NFC_INT_PIN_INDEX);            // 使能中断
    return NFC_SUCCESS;
}


/**
 * @brief 读取I2C寄存器数据
 *
 * @param regAddr
 * @param data
 * @param datalen
 * @return int
 */
int NfcI2cRead(unsigned char regAddr, unsigned char *data, int datalen)
{
    int ret = NFC_ERROR;
    struct I2cMsg writeMsg = {
        .addr = FM17622_DEV_ADDR,
        .buf = &regAddr,
        .len = 1,
        .flags = 0,
    };
    ret = I2cTransfer(g_i2c_handle, &writeMsg, 1);
    if (ret != 0) {
        printf("NfcI2cRead writeMsg failed\n");
        return NFC_ERROR;
    }

    struct I2cMsg readMsg = {
        .addr = FM17622_DEV_ADDR,
        .buf = data,
        .len = datalen,
        .flags = I2C_FLAG_READ,
    };
    ret = I2cTransfer(g_i2c_handle, &readMsg, 1);
    if (ret != 0) {
        printf("NfcI2cRead readMsg failed\n");
        return NFC_ERROR;
    }
    return NFC_SUCCESS;
}

/**
 * @brief 向I2C寄存器写数据
 *
 * @param regAddr  寄存器地址
 * @param data     写入的数据
 * @return int
 */
int NfcI2cWrite(unsigned char regAddr, unsigned char data)
{
    int ret = NFC_ERROR;
    unsigned char buf[I2C_WRITE_BUF_SIZE] = {regAddr, data};
    struct I2cMsg msg = {
        .addr = FM17622_DEV_ADDR,
        .buf = buf,
        .len = I2C_WRITE_BUF_SIZE,
        .flags = 0,
    };
    ret = I2cTransfer(g_i2c_handle, &msg, 1);
    if (ret != 0) {
        printf("NfcI2cWrite msg failed\n");
        return NFC_ERROR;
    }
    return NFC_SUCCESS;
}

/**
 * @brief I2C总线初始化
 *
 * @return int
 */
int I2cInit(void)
{
    g_i2c_handle = I2cOpen(I2C_BUS);
    if (g_i2c_handle == NULL) {
        printf("I2c Open fail!!\n");
        return NFC_ERROR;
    }
    return NFC_SUCCESS;
}

/**
 * @brief 关闭I2C
 *
 * @return int
 */
void I2cDeInit(void)
{
    I2cClose(g_i2c_handle);
}

/**
 * @brief 切换HPD模式
 *
 * @param mode  mode = 1 退出HPD模式, mode = 0 进入HPD模式
 */
void FM17622_HPD(unsigned char mode)
{
    if (mode == 0) {
        GpioWrite(NFC_RESET_PIN_INDEX, GPIO_VAL_LOW);
    } else {
        GpioWrite(NFC_RESET_PIN_INDEX, GPIO_VAL_HIGH);
    }
}
/**
 * @brief 复位FM17622
 *
 */
void FM17622_HardReset(void)
{
    FM17622_HPD(0); // NPD=0
    usleep(USLEEP_10MS);
    FM17622_HPD(1); // NPD=1
    usleep(USLEEP_10MS);
}

/**
 * @brief 读寄存器
 *
 * @param addr  目标寄存器地址
 * @param regdata 读取的值
 */
void GetReg(unsigned char addr, unsigned char *regdata)
{
    NfcI2cRead(addr & 0x3F, regdata, 1);
}

/**
 * @brief 读取扩展寄存器值
 *
 * @param ExtRegAddr 扩展寄存器地址
 * @param ExtRegData 读取的值
 */
void GetReg_Ext(unsigned char ExtRegAddr, unsigned char *ExtRegData)
{
    SetReg(JREG_EXT_REG_ENTRANCE, JBIT_EXT_REG_RD_ADDR + ExtRegAddr);
    GetReg(JREG_EXT_REG_ENTRANCE, ExtRegData);
}

/**
 * @brief 写寄存器
 *
 * @param addr  目标寄存器地址
 * @param regdata 要写入的值
 */
void SetReg(unsigned char addr, unsigned char regdata)
{
    NfcI2cWrite(addr & 0x3F, regdata);
}

/**
 * @brief 写扩展寄存器
 *
 * @param ExtRegAddr 扩展寄存器地址
 * @param ExtRegData 要写入的值
 */
void SetReg_Ext(unsigned char ExtRegAddr, unsigned char ExtRegData)
{
    SetReg(JREG_EXT_REG_ENTRANCE, JBIT_EXT_REG_WR_ADDR + ExtRegAddr);
    SetReg(JREG_EXT_REG_ENTRANCE, JBIT_EXT_REG_WR_DATA + ExtRegData);
}

/**
 * @brief 寄存器位操作
 *
 * @param ExtRegAddr 目标寄存器地址
 * @param mask 要改变的位
 * @param set 0:标志的位清零   其它:标志的位置位
 */
void ModifyReg(unsigned char addr, unsigned char mask, unsigned char set)
{
    unsigned char regdata;
    GetReg(addr, &regdata);
    if (set) {
        regdata |= mask;
    } else {
        regdata &= ~mask;
    }
    SetReg(addr, regdata);
}

/**
 * @brief 扩展寄存器位操作
 *
 * @param ExtRegAddr 目标寄存器地址
 * @param mask 要改变的位
 * @param set 0:标志的位清零   其它:标志的位置位
 */
void ModifyReg_Ext(unsigned char ExtRegAddr, unsigned char mask, unsigned char set)
{
    unsigned char regdata;
    GetReg_Ext(ExtRegAddr, &regdata);
    if (set) {
        regdata |= mask;
    } else {
        regdata &= ~(mask);
    }
    SetReg_Ext(ExtRegAddr, regdata);
    return;
}

/**
 * @brief 设置NFC载波
 *
 * @param cw_mode
 */
void SetCW(unsigned char cw_mode)
{
    unsigned char reg;
    SetReg(JREG_TXCONTROL, 0x80);
    if (cw_mode == TX1_TX2_CW_DISABLE) {
        ModifyReg(JREG_TXCONTROL, JBIT_TX1RFEN | JBIT_TX2RFEN, RESET);
        GetReg(JREG_TXCONTROL, &reg);
    }
    if (cw_mode == TX1_CW_ENABLE) {
        ModifyReg(JREG_TXCONTROL, JBIT_TX1RFEN, SET);
        ModifyReg(JREG_TXCONTROL, JBIT_TX2RFEN, RESET);
    }
    if (cw_mode == TX2_CW_ENABLE) {
        ModifyReg(JREG_TXCONTROL, JBIT_TX1RFEN, RESET);
        ModifyReg(JREG_TXCONTROL, JBIT_TX2RFEN, SET);
    }
    if (cw_mode == TX1_TX2_CW_ENABLE) {
        ModifyReg(JREG_TXCONTROL, JBIT_TX1RFEN | JBIT_TX2RFEN, SET);
    }
    usleep(USLEEP_5MS);
}

/**
 * @brief FM17622软件复位
 *
 * @return unsigned char
 */
unsigned char FM17622_SoftReset(void)
{
    unsigned char reg_data;
    SetReg(JREG_COMMAND, CMD_SOFT_RESET);
    usleep(USLEEP_2MS); // FM17622芯片复位需要2ms
    GetReg(JREG_COMMAND, &reg_data);
    if (reg_data == 0x20)
        return NFC_SUCCESS;
    else
        return NFC_ERROR;
}

/**
 * @brief 检查芯片版本
 *
 * @return int
 */
int CheckChipVersion(void)
{
    unsigned char reg_data;
    GetReg(JREG_VERSION, &reg_data);
    if (reg_data == 0xA2) { // 确认芯片版本
#ifdef NFC_DEBUG
        printf("--->NFC IC Version = FM17622 or FM17610\r\n");
#endif
        return 0;
    } else {
        printf("---> CheckChipVersion fail!\r\n");
    }
    return -1;
}

void SetNfcTypeACallback(NfcTypeACallback pfunc)
{
    if (gNfcTypeACallback == NULL) {
        gNfcTypeACallback = pfunc;
    }
}

void SetNfcTypeBCallback(NfcTypeBCallback pfunc)
{
    if (gNfcTypeBCallback == NULL) {
        gNfcTypeBCallback = pfunc;
    }
}

NfcError NfcSetTouchCallback(NfcTouchCallback pfunc, VOID *arg)
{
    if (pfunc != NULL) {
        gNfcTouchCallback = pfunc;
        g_callback_arg = arg;
        printf("NfcSetTouchCallback set success!!\n");
    }
    return NFC_SUCCESS;
}

void nfc_service(void)
{
    gNfcIrqEvent = osEventFlagsNew(NULL); // 创建NFC中断事件
    if (gNfcIrqEvent == NULL) {
        printf("Failed to create gNfcIrqEvent!\n");
    }

    while (1) {
#ifdef NFC_DEBUG
        printf("Wait nfc interrupt event!!\n");
#endif
        osEventFlagsWait(gNfcIrqEvent, 1, osFlagsWaitAny, osWaitForever); // 等待中断产生触发NFC读取事件
        Lpcd_IRQ_Event();   // 中断处理
        GpioWrite(LED_RED, GPIO_VAL_LOW);
        GpioWrite(LED_BLUE, GPIO_VAL_LOW);
    }
}

static void CreatTwNfcService(void)
{
    osThreadAttr_t attr;
    attr.name = "twnfc_service";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = NFC_SERVICE_STACK_SIZE;
    attr.priority = NFC_SERVICE_TASK_PRI;
    if (osThreadNew((osThreadFunc_t)nfc_service, NULL, &attr) == NULL) {
        printf("Failed to create nfc_detect_task!\n");
    }
}

/**
 * @brief NFC设备初始化
 *
 * @return int
 */
NfcError NfcInit(void)
{
    GpioConfig(); // 初始化GPIO配置

    I2cInit(); // 初始化I2C

    FM17622_HardReset(); // 复位FM17622

    CheckChipVersion(); // 检查芯片版本

    Lpcd_Init_Register(); // LPCD初始化

    Lpcd_Set_Mode(ENABLE); // 进入LPCD模式

    CreatTwNfcService(); // 创建NFC服务

    printf("---> NfcDeviceInit success!\n");

    return NFC_SUCCESS;
}

void NfcDeinit(void)
{
    I2cDeInit();
    GpioDisableIrq(NFC_INT_PIN_INDEX);  // 失能GPIO中断
    printf("---> NfcDeviceDeInit.\n");
}
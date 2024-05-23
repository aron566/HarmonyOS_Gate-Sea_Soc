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

#include "device_init.h"
#include "fm17622_reg.h"
#include "reader_api.h"

static void Uart_Send_Hex(unsigned char* data, int len)
{
    for (int i = 0; i < len; i++) {
        printf(" 0x%02X", data[i]);
    }
}

/**
 * @brief LPCD寄存器初始化配置
 *
 * @return unsigned char
 */
unsigned char Lpcd_Init_Register(void)
{
    if (FM17622_SoftReset() == NFC_SUCCESS) {
        SetReg_Ext(0x24, 0x14); // 请勿修改
        SetReg_Ext(0x25, 0x3A); // 请勿修改
        SetReg_Ext(JREG_LPCDCTRLMODE, (RF_DET_ENABLE | RF_DET_SEN_00 | LPCD_ENABLE));
        // SetReg_Ext(JREG_LPCDDETECTMODE, LPCD_TXSCALE_4|LPCD_RX_CHANGE_MODE);
        SetReg_Ext(JREG_LPCDSLEEPTIMER, LPCD_SLEEPTIME);
        SetReg_Ext(JREG_LPCDRFTIMER, LPCD_IRQINV_ENABLE | LPCD_IRQ_PUSHPULL | LPCD_RFTIME_5us); // 探测时间配置使用5us
        SetReg_Ext(JREG_LPCDTHRESH_L, LPCD_THRSH_L);
        SetReg_Ext(JREG_LPCDTXCTRL2, LPCD_CWP);
        SetReg_Ext(JREG_LPCDTXCTRL3, LPCD_CWN);
        SetReg_Ext(JREG_LPCDREQATIMER, LPCD_REQA_TIME_5ms); // REQA操作时间,5ms兼容手机方案
        SetReg_Ext(JREG_LPCDREQAANA, LPCD_RXGAIN_33DB | LPCD_MINLEVEL_9 | LPCD_MODWIDTH_38);
        SetReg_Ext(JREG_LPCDDETECTMODE, LPCD_TXSCALE_4 | LPCD_COMBINE_MODE); // 配置REQA检测功能
        return NFC_SUCCESS;
    }
    return NFC_ERROR;
}

/**
 * @brief LCPD工作模式设置
 *
 * @param mode ENABLE:开启, DISABLE:关闭
 */
void Lpcd_Set_Mode(unsigned char mode)
{
    if (mode == ENABLE) {
        FM17622_HPD(0); // NPD = 0 进入LPCD模式
    } else {
        FM17622_HPD(1); // NPD = 1 退出LPCD模式
    }
    return;
}

/**
 * @brief   Lpcd_Get_ADC_Value读取LPCD的ADC数据
 *
 */
void Lpcd_Get_ADC_Value(void)
{
    unsigned char reg, reg1;
    unsigned char lpcd_delta1, lpcd_delta2;
    GetReg_Ext(JREG_LPCDDELTA_HI, &reg);
    GetReg_Ext(JREG_LPCDDELTA_LO, &reg1);
    lpcd_delta1 = (reg << NUM6) + reg1;
    lpcd_delta2 = ((reg << NUM6) >> NUM8);
    printf("-> LPCD Delta is:");
    Uart_Send_Hex(&lpcd_delta2, 1);
    Uart_Send_Hex(&lpcd_delta1, 1);
    printf("\r\n");
}

/**
 * @brief TypeA卡处理
 *
 */
void TypeACardProc(void)
{
    unsigned char result;
    FM17622_HardReset();
    FM17622_Initial_ReaderA();
    SetCW(TX1_TX2_CW_ENABLE);
    result = ReaderA_CardActivate(); // 寻卡操作
    if (result == FM17622_SUCCESS) {
#ifdef NFC_DEBUG
        printf("ReaderA_CardActivate success!!\n");
#endif
        NfcTypeACallback NfcTypeACallbackPtr = GetNfcTypeACallback();
        NfcTouchCallback NfcTouchCallbackPtr = GetNfcTouchCallback();
        struct picc_a_struct PICC_A = GetPICC_A();
        VOID* CallbackArgPrt = GetCallbackArg();
        if (NfcTypeACallbackPtr) {
            NfcTypeACallbackPtr(&PICC_A);
        }
        if (NfcTouchCallbackPtr) {
            NfcTouchCallbackPtr(CallbackArgPrt);
        }
    } else {
        printf("-> CardActivate Fail!\r\n");
    }
    SetCW(TX1_TX2_CW_DISABLE); // 关闭载波输出
}

/**
 * @brief TypeB卡处理
 *
 */
void TypeBCardProc(void)
{
    unsigned char result;
    printf("->NFC TYPE B CARD!\r\n");
    FM17622_Initial_ReaderB();
    SetCW(TX1_TX2_CW_ENABLE);   // 开载波
    result = ReaderB_Request();
    if (result != FM17622_SUCCESS) {
        return FM17622_COMM_ERR;
    }
    struct picc_b_struct PICC_B = GetPICC_B();
    printf("NFC ATQB:");
    Uart_Send_Hex(PICC_B.ATQB, sizeof(PICC_B.ATQB));
    result = ReaderB_Attrib();
    if (result != FM17622_SUCCESS) {
        return FM17622_COMM_ERR;
    } else {
        printf("NFC ATTRIB:");
        Uart_Send_Hex(PICC_B.ATTRIB, (int)PICC_B.LEN_ATTRIB);
        printf("\n");
        NfcTypeBCallback NfcTypeBCallbackPtr = GetNfcTypeBCallback();
        if (NfcTypeBCallbackPtr) {
            NfcTypeBCallbackPtr(&PICC_B);
        }
    }
    SetCW(TX1_TX2_CW_DISABLE); // 关闭载波输出
    return NFC_SUCCESS;
}

unsigned char Lpcd_Card_Event(void)
{
    unsigned char polling_card;
    unsigned char result = FM17622_Polling(&polling_card);
    if (result == NFC_SUCCESS) {
        if (polling_card & BIT0) {
            TypeACardProc(); // TYPE A卡处理
        }
        if (polling_card & BIT1) {
            TypeBCardProc(); // TYPE B卡处理
        }
    } else {
        printf("->NFC No Card Detected!\n");
    }
    return result;
}

/**
 * @brief LPCD中断处理
 *
 */
void Lpcd_IRQ_Event(void)
{
    unsigned char reg;
    Lpcd_Set_Mode(DISABLE); // NPD = 1,FM17622退出休眠模式
#ifdef NFC_DEBUG
    printf("-> LPCD Detceted!\r\n");
#endif
    GetReg_Ext(JREG_LPCDIRQ, &reg); // 读取LPCD中断标志
    SetReg_Ext(JREG_LPCDIRQ, reg);  // CLEAR LPCD IRQ
#ifdef NFC_DEBUG
    printf("IRQ = ");
    Uart_Send_Hex(&reg, 1);
    printf("\r\n");
#endif
    if (reg & BIT3) {
#ifdef NFC_DEBUG
        printf("-> LPCD IRQ RFDET SET!\r\n");
#endif
    }
    if (reg & BIT2) {
#ifdef NFC_DEBUG
        printf("-> LPCD IRQ RXCHANGE SET!\r\n"); // LPCD中断标志
        Lpcd_Get_ADC_Value();                    // 用于LPCD功能调试
#endif
    }
    if (reg & BIT1) {
#ifdef NFC_DEBUG
        printf("-> LPCD IRQ ATQAREC SET!\r\n");
#endif
    }
    if ((reg & BIT3) || (reg & BIT2) || (reg & BIT1)) {
        Lpcd_Card_Event();     // 卡片操作处理流程
        Lpcd_Init_Register();  // LPCD初始化
        Lpcd_Set_Mode(ENABLE); // 返回LPCD模式
    }
    return;
}

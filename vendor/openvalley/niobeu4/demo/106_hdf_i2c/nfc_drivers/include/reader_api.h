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

#ifndef _READERAPI_H_
#define _READERAPI_H_

#define SIZE_3 3

static const unsigned char RF_CMD_REQA = 0x26;
static const unsigned char RF_CMD_WUPA = 0x52;
static const unsigned char RF_CMD_ANTICOL[SIZE_3] = {0x93, 0x95, 0x97};
static const unsigned char RF_CMD_SELECT[SIZE_3] = {0x93, 0x95, 0x97};

// 发射参数设置
#define MODWIDTH_106 0x26 // 106Kbps为0x26
#define MODWIDTH_212 0x13 // 212kbps为0x13
#define MODWIDTH_424 0x09 // 424kbps为0x09
#define MODWIDTH_848 0x04 // 848kbps为0x04
// 接收参数配置
// TYPE A
#define RXGAIN_A 6    // 设置范围0~7
#define GSNON_A 15    // 设置范围0~15
#define GSP_A 63      // 设置范围0~63
#define COLLLEVEL_A 4 // 设置范围0~7
#define MINLEVEL_A 8  // 设置范围0~15
// TYPE B
#define RXGAIN_B 6   // 设置范围0~7
#define GSNON_B 15   // 设置范围0~15
#define MODGSNON_B 6 // 设置范围0~15
#define GSP_B 31     // 设置范围0~63
#define MODGSP_B 10  // 设置范围0~63
#define MINLEVEL_B 4 // 设置范围0~15

#define RXWAIT 4  // 设置范围0~63
#define UARTSEL 2 // 默认设置为2  设置范围0~3 0:固定低电平 1:TIN包络信号 2:内部接收信号 3:TIN调制信号

struct picc_b_struct {
    unsigned char ATQB[12];
    unsigned char PUPI[4];
    unsigned char APPLICATION_DATA[4];
    unsigned char PROTOCOL_INF[3];
    unsigned char ATTRIB[10];
    unsigned char LEN_ATTRIB;
    unsigned char UID[8];
};

struct picc_a_struct {
    unsigned char ATQA[2];  // 对A型卡请求的应答
    unsigned char UID[12];   // 卡片ID
    unsigned char BCC[3];   // 校验值
    unsigned char SAK[3];
    unsigned char CASCADE_LEVEL;
};

#define FM17622_SUCCESS 0x00
#define FM17622_RESET_ERR 0xF1
#define FM17622_PARAM_ERR 0xF2 // 输入参数错误
#define FM17622_TIMER_ERR 0xF3 // 接收超时
#define FM17622_COMM_ERR 0xF4  // 通信错误
#define FM17622_COLL_ERR 0xF5  // 冲突错误
#define FM17622_FIFO_ERR 0xF6  // FIFO错误
#define FM17622_CRC_ERR 0xF7
#define FM17622_PARITY_ERR 0xF8
#define FM17622_PROTOCOL_ERR 0xF9
#define FM17622_AUTH_ERR 0xE1
#define FM17622_RATS_ERR 0xD1
#define FM17622_PPS_ERR 0xD2
#define FM17622_PCB_ERR 0xD3

typedef struct {
    unsigned char Cmd; // 命令代码
    unsigned char SendCRCEnable;
    unsigned char ReceiveCRCEnable;
    unsigned char nBitsToSend;     // 准备发送的位数
    unsigned char nBytesToSend;    // 准备发送的字节数
    unsigned char nBitsToReceive;  // 准备接收的位数
    unsigned char nBytesToReceive; // 准备接收的字节数
    unsigned char nBytesReceived;  // 已接收的字节数
    unsigned char nBitsReceived;   // 已接收的位数
    unsigned char *pSendBuf;       // 发送数据缓冲区
    unsigned char *pReceiveBuf;    // 接收数据缓冲区
    unsigned char CollPos;         // 碰撞位置
    unsigned char Error;           // 错误状态
    unsigned char Timeout;         // 超时时间
} command_struct;

extern struct picc_b_struct GetPICC_B(void);
extern struct picc_a_struct GetPICC_A(void);

extern void FM17622_Initial_ReaderA(void);
extern void FM17622_Initial_ReaderB(void);
extern unsigned char ReaderA_Halt(void);
extern unsigned char ReaderA_Request(void);
extern unsigned char ReaderA_Wakeup(void);

extern unsigned char ReaderA_AntiColl(unsigned char cascade_level);
extern unsigned char ReaderA_Select(unsigned char cascade_level);
extern unsigned char ReaderA_CardActivate(void);

extern unsigned char ReaderB_Request(void);
extern unsigned char ReaderB_Wakeup(void);
extern unsigned char ReaderB_Attrib(void);
extern unsigned char ReaderB_GetUID(void);
extern unsigned char FM17622_Polling(unsigned char *polling_card);
#endif

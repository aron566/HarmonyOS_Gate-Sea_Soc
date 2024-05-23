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

#ifndef __NFC_API_H__
#define __NFC_API_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define KEY_A_AUTH 0
#define KEY_B_AUTH 1

#define FM17622_SUCCESS 0x00
#define FM17622_COMM_ERR 0xF4 // 通信错误
#define FM17622_FIFO_ERR 0xF6 // FIFO错误
#define FM17622_AUTH_ERR 0xE1 // 认证失败

#define NUM2               2
#define NUM3               3
#define NUM4               4
#define NUM5               5
#define NUM6               6
#define NUM7               7
#define NUM8               8
#define NUM9               9
#define NUM10              10

struct picc_a_struct {
    unsigned char ATQA[2]; // 对A型卡请求的应答
    unsigned char UID[12]; // 卡片ID
    unsigned char BCC[3];  // 校验值
    unsigned char SAK[3];
    unsigned char CASCADE_LEVEL;
};

struct picc_b_struct {
    unsigned char ATQB[12];
    unsigned char PUPI[4];
    unsigned char APPLICATION_DATA[4];
    unsigned char PROTOCOL_INF[3];
    unsigned char ATTRIB[10];
    unsigned char LEN_ATTRIB;
    unsigned char UID[8];
};

typedef void (*NfcTypeACallback)(struct picc_a_struct *picc_typea_info); // NFC读TypeA卡回调函数
typedef void (*NfcTypeBCallback)(struct picc_b_struct *picc_typeb_info); // NFC读TypeB卡回调函数

/* NFC回调接口 */
extern void SetNfcTypeACallback(NfcTypeACallback pfunc);
extern void SetNfcTypeBCallback(NfcTypeBCallback pfunc);

/* M1卡原始数据读写接口 */
extern unsigned char Mifare_Blockwrite(unsigned char block_num, unsigned char *data_buff);
extern unsigned char Mifare_Blockread(unsigned char block_num, unsigned char *data_buff);
extern unsigned char Mifare_Auth(unsigned char mode, unsigned char sector, unsigned char *mifare_key,
                                 unsigned char *card_uid);

/* NTAG卡原始数据读写接口 */
extern unsigned char ReadNtagPage(unsigned char page_num, unsigned char *page_data);
extern unsigned char WriteNtagPage(unsigned char block_num, unsigned char *block_data);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __NFC_API_H__ */
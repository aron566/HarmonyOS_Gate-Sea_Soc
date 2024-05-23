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

#ifndef __MIFARE_H__
#define __MIFARE_H__

/**
 * Tips:
 * 以常用的M1卡为例：M1卡分为16个扇区、每个扇区4块(0~3 Block)、共64块，每块16个字节。
 * 按块号编址为0~63，第0扇区的块0用于存放厂商代码，已经固化不可更改。
 * 其它扇区的块0、块1、块2为数据块，用于存储数据，块3为控制块，分别存放:
 * 密码A(6bytes)、存取控制(4bytes)、密码B(6bytes)
 * 每个扇区的密码和存取控制是独立的，可以根据实际需要设定各自的密码及存取控制。
 * 在存取控制中，每个块都有相应的三个控制位:
 * |0|块0|C10|C20|C30|
 * |1|块1|C11|C21|C31|
 * |2|块2|C12|C22|C32|
 * |3|块3|C13|C23|C33|
 * 三个控制位以正和反两种形式存在于存取控制字节中，决定了该块的访问权限。
 * 如果进行减值操作，必须验证KEYA, 如果进行加值操作，必须验证KEYB
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define KEY_A_AUTH 0
#define KEY_B_AUTH 1

extern void Mifare_Clear_Crypto(void);
extern unsigned char Mifare_Transfer(unsigned char block_num);
extern unsigned char Mifare_Restore(unsigned char block_num);
extern unsigned char Mifare_Blockset(unsigned char block_num, unsigned char *data_buff);
extern unsigned char Mifare_Blockinc(unsigned char block_num, unsigned char *data_buff);
extern unsigned char Mifare_Blockdec(unsigned char block_num, unsigned char *data_buff);
extern unsigned char Mifare_Blockwrite(unsigned char block_num, unsigned char *data_buff);
extern unsigned char Mifare_Blockread(unsigned char block_num, unsigned char *data_buff);
extern unsigned char Mifare_Auth(unsigned char mode, unsigned char sector,
                                 unsigned char *mifare_key, unsigned char *card_uid);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __MIFARE_H__ */
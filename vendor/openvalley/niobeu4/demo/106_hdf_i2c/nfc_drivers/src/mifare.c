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
#include "device_init.h"
#include "mifare.h"

#define BLOCK_SIZE  16

/**
 * @brief Mifare_Clear_Crypto清除认证标志
 *
 */
void Mifare_Clear_Crypto(void)
{
    ModifyReg(JREG_STATUS2, BIT3, RESET);
    return;
}

/**
 * @brief Mifare_Auth卡片认证
 *
 * @param mode      认证模式（0：key A认证，1：key B认证）
 * @param sector    认证的扇区号（0~15）
 * @param mifare_key    6字节认证密钥数组
 * @param card_uid      4字节卡片UID数组
 * @return FM17622_SUCCESS :认证成功  FM17622_AUTH_ERR: 认证失败
 */
unsigned char Mifare_Auth(unsigned char mode, unsigned char sector, unsigned char *mifare_key, unsigned char *card_uid)
{
    unsigned char reg_data;
    struct picc_a_struct PICC_A = GetPICC_A();
    SetReg(JREG_TXMODE, 0x80);              // Enable TxCRC
    SetReg(JREG_RXMODE, 0x80);              // Enable RxCRC
    SetReg(JREG_COMMAND, CMD_IDLE);         // command = Idel
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    if (mode == KEY_A_AUTH) {
        SetReg(JREG_FIFODATA, 0x60);
    }
    if (mode == KEY_B_AUTH) {
        SetReg(JREG_FIFODATA, 0x61);
    }
    SetReg(JREG_FIFODATA, sector * NUM4);
    SetReg(JREG_FIFODATA, mifare_key[0]);
    SetReg(JREG_FIFODATA, mifare_key[1]);
    SetReg(JREG_FIFODATA, mifare_key[NUM2]);
    SetReg(JREG_FIFODATA, mifare_key[NUM3]);
    SetReg(JREG_FIFODATA, mifare_key[NUM4]);
    SetReg(JREG_FIFODATA, mifare_key[NUM5]);
    SetReg(JREG_FIFODATA, PICC_A.UID[0]);
    SetReg(JREG_FIFODATA, PICC_A.UID[1]);
    SetReg(JREG_FIFODATA, PICC_A.UID[NUM2]);
    SetReg(JREG_FIFODATA, PICC_A.UID[NUM3]);
    SetReg(JREG_COMMAND, CMD_AUTHENT); // command = Transceive
    SetReg(JREG_BITFRAMING, 0x80);     // Start Send
    usleep(USLEEP_10MS);                        // Wait 10ms
    GetReg(JREG_STATUS2, &reg_data);
    if (reg_data & 0x08) {
        return FM17622_SUCCESS;
    }
    return FM17622_AUTH_ERR;
}

/**
 * @brief Mifare_Blockset卡片数值设置
 *
 * @param block_num     块号
 * @param data_buff     需要设置的4字节数值数组
 * @return FM17622_SUCCESS :成功  FM17622_COMM_ERR: 失败
 */
unsigned char Mifare_Blockset(unsigned char block_num, unsigned char *data_buff)
{
    unsigned char block_data[BLOCK_SIZE], result;
    block_data[0] = data_buff[0];
    block_data[1] = data_buff[1];
    block_data[NUM2] = data_buff[NUM2];
    block_data[NUM3] = data_buff[NUM3];
    block_data[NUM4] = ~data_buff[0];
    block_data[NUM5] = ~data_buff[1];
    block_data[NUM6] = ~data_buff[NUM2];
    block_data[NUM7] = ~data_buff[NUM3];
    block_data[NUM8] = data_buff[0];
    block_data[NUM9] = data_buff[1];
    block_data[NUM10] = data_buff[NUM2];
    block_data[NUM11] = data_buff[NUM3];
    block_data[NUM12] = block_num;
    block_data[NUM13] = ~block_num;
    block_data[NUM14] = block_num;
    block_data[NUM15] = ~block_num;
    result = Mifare_Blockwrite(block_num, block_data);
    return result;
}

/**
 * @brief Mifare_Blockread卡片读块操作
 *
 * @param block_num 块号（0x00~0x3F）
 * @param data_buff 16字节读块数据数组
 * @return FM17622_SUCCESS :成功  FM17622_COMM_ERR: 失败
 */
unsigned char Mifare_Blockread(unsigned char block_num, unsigned char *data_buff)
{
    unsigned char reg_data, i;
    SetReg(JREG_TXMODE, 0x80);              // Enable TxCRC
    SetReg(JREG_RXMODE, 0x80);              // Enable RxCRC
    SetReg(JREG_COMMAND, CMD_IDLE);         // command = Idel
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    SetReg(JREG_FIFODATA, 0x30);
    SetReg(JREG_FIFODATA, block_num);

    SetReg(JREG_COMMAND, CMD_TRANSCEIVE); // command = Transceive
    SetReg(JREG_BITFRAMING, 0x80);        // Start Send
    usleep(USLEEP_10MS);                           // Wait 10ms
    GetReg(JREG_FIFOLEVEL, &reg_data);
    if (reg_data == BLOCK_SIZE) {
        for (i = 0; i < BLOCK_SIZE; i++) {
            GetReg(JREG_FIFODATA, &data_buff[i]);
        }
        return FM17622_SUCCESS;
    }
    return FM17622_COMM_ERR;
}

/**
 * @brief Mifare卡片写块操作
 *
 * @param block_num 块号（0x00~0x3F）
 * @param data_buff 16字节写块数据数组
 * @return FM17622_SUCCESS :成功  FM17622_COMM_ERR: 失败
 */
unsigned char Mifare_Blockwrite(unsigned char block_num, unsigned char *data_buff)
{
    unsigned char reg_data, i;
    SetReg(JREG_TXMODE, 0x80);              // Enable TxCRC
    SetReg(JREG_RXMODE, 0x00);              // Disable RxCRC
    SetReg(JREG_COMMAND, CMD_IDLE);         // command = Idel
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    SetReg(JREG_FIFODATA, 0xA0);
    SetReg(JREG_FIFODATA, block_num);
    SetReg(JREG_COMMAND, CMD_TRANSCEIVE); // command = Transceive
    SetReg(JREG_BITFRAMING, 0x80);        // Start Send
    usleep(USLEEP_10MS);                           // Wait 10ms
    GetReg(JREG_FIFOLEVEL, &reg_data);
    if (reg_data == 1) {
        GetReg(JREG_FIFODATA, &reg_data);
        if (reg_data != 0x0A) {
            return FM17622_COMM_ERR;
        }
    }
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    for (i = 0; i < BLOCK_SIZE; i++) {
        SetReg(JREG_FIFODATA, data_buff[i]);
    }
    SetReg(JREG_COMMAND, CMD_TRANSCEIVE); // command = Transceive
    SetReg(JREG_BITFRAMING, 0x80);        // Start Send
    usleep(USLEEP_10MS);                           // Wait 10ms
    GetReg(JREG_FIFOLEVEL, &reg_data);
    if (reg_data == 1) {
        GetReg(JREG_FIFODATA, &reg_data);
        if (reg_data == 0x0A) {
            return FM17622_SUCCESS;
        }
    }
    return FM17622_COMM_ERR;
}

/**
 * @brief Mifare 卡片增值操作
 *
 * @param block_num 块号（0x00~0x3F）
 * @param data_buff 4字节增值数据数组
 * @return FM17622_SUCCESS :成功  FM17622_COMM_ERR: 失败
 */
unsigned char Mifare_Blockinc(unsigned char block_num, unsigned char *data_buff)
{
    unsigned char reg_data, i;
    SetReg(JREG_TXMODE, 0x80);              // Enable TxCRC
    SetReg(JREG_RXMODE, 0x00);              // Disable RxCRC
    SetReg(JREG_COMMAND, CMD_IDLE);         // command = Idel
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    SetReg(JREG_FIFODATA, 0xC1);
    SetReg(JREG_FIFODATA, block_num);
    SetReg(JREG_COMMAND, CMD_TRANSCEIVE); // command = Transceive
    SetReg(JREG_BITFRAMING, 0x80);        // Start Send
    usleep(USLEEP_10MS);                           // Wait 10ms
    GetReg(JREG_FIFOLEVEL, &reg_data);
    if (reg_data == 1) {
        GetReg(JREG_FIFODATA, &reg_data);
        if (reg_data != 0x0A) {
            return FM17622_COMM_ERR;
        }
    }
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    for (i = 0; i < NUM4; i++) {
        SetReg(JREG_FIFODATA, data_buff[i]);
    }
    SetReg(JREG_COMMAND, CMD_TRANSMIT); // command = Transmit
    SetReg(JREG_BITFRAMING, 0x80);      // Start Send
    usleep(USLEEP_10MS);                         // Wait 10ms
    return FM17622_SUCCESS;
}

/**
 * @brief Mifare 卡片减值操作
 *
 * @param block_num 块号（0x00~0x3F）
 * @param data_buff 4字节减值数据数组
 * @return FM17622_SUCCESS :成功  FM17622_COMM_ERR: 失败
 */
unsigned char Mifare_Blockdec(unsigned char block_num, unsigned char *data_buff)
{
    unsigned char reg_data, i;
    SetReg(JREG_TXMODE, 0x80);              // Enable TxCRC
    SetReg(JREG_RXMODE, 0x00);              // Disable RxCRC
    SetReg(JREG_COMMAND, CMD_IDLE);         // command = Idel
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    SetReg(JREG_FIFODATA, 0xC0);
    SetReg(JREG_FIFODATA, block_num);
    SetReg(JREG_COMMAND, CMD_TRANSCEIVE); // command = Transceive
    SetReg(JREG_BITFRAMING, 0x80);        // Start Send
    usleep(USLEEP_10MS);                           // Wait 10ms
    GetReg(JREG_FIFOLEVEL, &reg_data);
    if (reg_data == 1) {
        GetReg(JREG_FIFODATA, &reg_data);
        if (reg_data != 0x0A) {
            return FM17622_COMM_ERR;
        }
    }
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    for (i = 0; i < NUM4; i++) {
        SetReg(JREG_FIFODATA, data_buff[i]);
    }
    SetReg(JREG_COMMAND, CMD_TRANSMIT); // command = Transmit
    SetReg(JREG_BITFRAMING, 0x80);      // Start Send
    usleep(USLEEP_10MS);                         // Wait 10ms
    return FM17622_SUCCESS;
}

/**
 * @brief Mifare 卡片transfer操作
 *
 * @param block_num 块号（0x00~0x3F）
 * @return FM17622_SUCCESS :成功  FM17622_COMM_ERR: 失败
 */
unsigned char Mifare_Transfer(unsigned char block_num)
{
    unsigned char reg_data;
    SetReg(JREG_TXMODE, 0x80);              // Enable TxCRC
    SetReg(JREG_RXMODE, 0x00);              // Disable RxCRC
    SetReg(JREG_COMMAND, CMD_IDLE);         // command = Idel
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    SetReg(JREG_FIFODATA, 0xB0);
    SetReg(JREG_FIFODATA, block_num);
    SetReg(JREG_COMMAND, CMD_TRANSCEIVE); // command = Transceive
    SetReg(JREG_BITFRAMING, 0x80);        // Start Send
    usleep(USLEEP_10MS);                           // Wait 10ms
    GetReg(JREG_FIFOLEVEL, &reg_data);
    if (reg_data == 1) {
        GetReg(JREG_FIFODATA, &reg_data);
        if (reg_data == 0x0A) {
            return FM17622_SUCCESS;
        }
    }
    return FM17622_COMM_ERR;
}

/**
 * @brief Mifare卡片restore操作
 *
 * @param block_num 块号（0x00~0x3F）
 * @return  FM17622_SUCCESS: 成功  FM17622_COMM_ERR: 失败
 */
unsigned char Mifare_Restore(unsigned char block_num)
{
    unsigned char reg_data, i;
    SetReg(JREG_TXMODE, 0x80);              // Enable TxCRC
    SetReg(JREG_RXMODE, 0x00);              // Disable RxCRC
    SetReg(JREG_COMMAND, CMD_IDLE);         // command = Idel
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    SetReg(JREG_FIFODATA, 0xC2);
    SetReg(JREG_FIFODATA, block_num);
    SetReg(JREG_COMMAND, CMD_TRANSCEIVE); // command = Transceive
    SetReg(JREG_BITFRAMING, 0x80);        // Start Send
    usleep(USLEEP_10MS);                           // Wait 10ms
    GetReg(JREG_FIFOLEVEL, &reg_data);
    if (reg_data == 1) {
        GetReg(JREG_FIFODATA, &reg_data);
        if (reg_data != 0x0A) {
            return FM17622_COMM_ERR;
        }
    }
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    for (i = 0; i < NUM4; i++) {
        SetReg(JREG_FIFODATA, 0x00);
    }
    SetReg(JREG_COMMAND, CMD_TRANSMIT); // command = Transmit
    SetReg(JREG_BITFRAMING, 0x80);      // Start Send
    usleep(USLEEP_10MS);                         // Wait 10ms
    GetReg(JREG_FIFOLEVEL, &reg_data);
    if (reg_data != 0) {
        return FM17622_COMM_ERR;
    }
    return FM17622_SUCCESS;
}

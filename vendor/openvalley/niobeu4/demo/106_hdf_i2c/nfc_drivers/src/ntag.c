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
#include "ntag.h"

/**
 * @brief 读取page序号的ntag卡数据
 * @param page_num  页号
 * @param page_data 读取的数据（长度为16字节）
 * @return FM17622_SUCCESS :成功  FM17622_COMM_ERR: 失败
 */
unsigned char ReadNtagPage(unsigned char page_num, unsigned char *page_data)
{
    unsigned char reg_data, i;
    SetReg(JREG_TXMODE, 0x80);              // Enable TxCRC
    SetReg(JREG_RXMODE, 0x80);              // Enable RxCRC
    SetReg(JREG_COMMAND, CMD_IDLE);         // command = Idel
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    SetReg(JREG_FIFODATA, 0x30);
    SetReg(JREG_FIFODATA, page_num);
    SetReg(JREG_COMMAND, CMD_TRANSCEIVE); // command = Transceive
    SetReg(JREG_BITFRAMING, 0x80);        // Start Send
    usleep(USLEEP_10MS);                  // Wait 10ms
    GetReg(JREG_FIFOLEVEL, &reg_data);
    if (reg_data == PAGE_DATA_LEN) {
        for (i = 0; i < PAGE_DATA_LEN; i++) {
            GetReg(JREG_FIFODATA, &page_data[i]);
        }
        return FM17622_SUCCESS;
    }
    return FM17622_COMM_ERR;
}

/**
 * @brief 按块号向ntag卡写数据
 * @param block_num  块号
 * @param page_data 写入该块的数据(长度为4字节)
 * @return FM17622_SUCCESS :成功  FM17622_COMM_ERR: 失败
 */
unsigned char WriteNtagPage(unsigned char block_num, unsigned char *block_data)
{
    unsigned char reg_data;
    SetReg(JREG_TXMODE, 0x80);              // Enable TxCRC
    SetReg(JREG_RXMODE, 0x00);              // Disable RxCRC
    SetReg(JREG_COMMAND, CMD_IDLE);         // command = Idel
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    SetReg(JREG_FIFODATA, 0xA2);
    SetReg(JREG_FIFODATA, block_num);
    SetReg(JREG_FIFODATA, block_data[0]);
    SetReg(JREG_FIFODATA, block_data[1]);
    SetReg(JREG_FIFODATA, block_data[NUM2]);
    SetReg(JREG_FIFODATA, block_data[NUM3]);
    SetReg(JREG_COMMAND, CMD_TRANSCEIVE); // command = Transceive
    SetReg(JREG_BITFRAMING, 0x80);        // Start Send
    usleep(USLEEP_10MS);                  // Wait 10ms
    GetReg(JREG_FIFOLEVEL, &reg_data);
    if (reg_data == 1) {
        GetReg(JREG_FIFODATA, &reg_data);
        if (reg_data == 0x0A) {
            return FM17622_SUCCESS;
        }
    }
    return FM17622_COMM_ERR;
}
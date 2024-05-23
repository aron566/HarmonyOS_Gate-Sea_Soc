/*
 * Copyright (c) 2022 Hunan OpenValley Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http:// www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <unistd.h>
#include "device_init.h"
#include "fm17622_reg.h"
#include "reader_api.h"

static struct picc_a_struct PICC_A;
static struct picc_b_struct PICC_B;

struct picc_a_struct GetPICC_A(void)
{
    return PICC_A;
}

struct picc_b_struct GetPICC_B(void)
{
    return PICC_B;
}

void FM17622_Initial_ReaderA(void)
{
    SetReg(JREG_MODWIDTH, MODWIDTH_106);                      // MODWIDTH = 106kbps
    ModifyReg(JREG_TXAUTO, BIT6, SET);                        // Force 100ASK = 1
    SetReg(JREG_GSN, (GSNON_A << NUM4));                         // Config GSN; Config ModGSN
    SetReg(JREG_CWGSP, GSP_A);                                // Config GSP
    SetReg(JREG_CONTROL, BIT4);                               // Initiator = 1
    SetReg(JREG_RFCFG, RXGAIN_A << NUM4);                        // Config RxGain
    SetReg(JREG_RXTRESHOLD, (MINLEVEL_A << NUM4) | COLLLEVEL_A); // Config MinLevel; Config CollLevel
    return;
}

void FM17622_Initial_ReaderB(void)
{
    ModifyReg(JREG_STATUS2, BIT3, RESET);
    SetReg(JREG_MODWIDTH, MODWIDTH_106);           // MODWIDTH = 106kbps
    SetReg(JREG_TXAUTO, 0);                        // Force 100ASK = 0
    SetReg(JREG_GSN, (GSNON_B << NUM4) | MODGSNON_B); // Config GSN; Config ModGSN
    SetReg(JREG_CWGSP, GSP_B);                     // Config GSP
    SetReg(JREG_MODGSP, MODGSP_B);                 // Config ModGSP
    SetReg(JREG_CONTROL, BIT4);                    // Initiator = 1
    SetReg(JREG_RFCFG, RXGAIN_B << NUM4);             // Config RxGain
    SetReg(JREG_RXTRESHOLD, MINLEVEL_B << NUM4);      // Config MinLevel;
    return;
}

unsigned char ReaderA_Halt(void)
{
    unsigned char reg_data;
    SetReg(JREG_TXMODE, 0x80);              // Enable TxCRC
    SetReg(JREG_RXMODE, 0x80);              // Enable RxCRC
    SetReg(JREG_COMMAND, CMD_IDLE);         // command = Idel
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    SetReg(JREG_FIFODATA, 0x50);
    SetReg(JREG_FIFODATA, 0x00);
    SetReg(JREG_COMMAND, CMD_TRANSMIT); // command = Transceive
    SetReg(JREG_BITFRAMING, 0x80);      // Start Send
    usleep(USLEEP_2MS);                   // Wait 2ms
    GetReg(JREG_FIFOLEVEL, &reg_data);
    if (reg_data == 0) {
        return FM17622_SUCCESS;
    }
    return FM17622_COMM_ERR;
}

unsigned char ReaderA_Wakeup(void)
{
    unsigned char reg_data;
    SetReg(JREG_TXMODE, 0);                 // Disable TxCRC
    SetReg(JREG_RXMODE, 0);                 // Disable RxCRC
    SetReg(JREG_COMMAND, CMD_IDLE);         // command = Idel
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    SetReg(JREG_FIFODATA, RF_CMD_WUPA);
    SetReg(JREG_COMMAND, CMD_TRANSCEIVE); // command = Transceive
    SetReg(JREG_BITFRAMING, 0x87);        // Start Send
    usleep(USLEEP_2MS);                     // Wait 2ms
    GetReg(JREG_FIFOLEVEL, &reg_data);
    if (reg_data == NUM2) {
        GetReg(JREG_FIFODATA, PICC_A.ATQA);
        GetReg(JREG_FIFODATA, PICC_A.ATQA + 1);
        return FM17622_SUCCESS;
    }
    return FM17622_COMM_ERR;
}

unsigned char ReaderA_Request(void)
{
    unsigned char reg_data;
    SetReg(JREG_TXMODE, 0);                 // Disable TxCRC
    SetReg(JREG_RXMODE, 0);                 // Disable RxCRC
    SetReg(JREG_COMMAND, CMD_IDLE);         // command = Idel
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    SetReg(JREG_FIFODATA, RF_CMD_REQA);
    SetReg(JREG_COMMAND, CMD_TRANSCEIVE); // command = Transceive
    SetReg(JREG_BITFRAMING, 0x87);        // Start Send
    usleep(USLEEP_2MS);                     // Wait 2ms
    GetReg(JREG_FIFOLEVEL, &reg_data);
    if (reg_data == NUM2) {
        GetReg(JREG_FIFODATA, PICC_A.ATQA);
        GetReg(JREG_FIFODATA, PICC_A.ATQA + 1);
        return FM17622_SUCCESS;
    }
    return FM17622_COMM_ERR;
}

unsigned char ReaderA_AntiColl(unsigned char cascade_level)
{
    unsigned char reg_data;
    SetReg(JREG_TXMODE, 0);                 // Disable TxCRC
    SetReg(JREG_RXMODE, 0);                 // Disable RxCRC
    SetReg(JREG_COMMAND, CMD_IDLE);         // command = Idel
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    SetReg(JREG_FIFODATA, RF_CMD_ANTICOL[cascade_level]);
    SetReg(JREG_FIFODATA, 0x20);
    SetReg(JREG_COMMAND, CMD_TRANSCEIVE); // command = Transceive
    SetReg(JREG_BITFRAMING, 0x80);        // Start Send
    usleep(USLEEP_2MS);                     // Wait 2ms
    GetReg(JREG_FIFOLEVEL, &reg_data);
    if (reg_data == NUM5) {
        GetReg(JREG_FIFODATA, &PICC_A.UID[cascade_level * NUM4]);
        GetReg(JREG_FIFODATA, &PICC_A.UID[cascade_level * NUM4 + 1]);
        GetReg(JREG_FIFODATA, &PICC_A.UID[cascade_level * NUM4 + NUM2]);
        GetReg(JREG_FIFODATA, &PICC_A.UID[cascade_level * NUM4 + NUM3]);
        GetReg(JREG_FIFODATA, &PICC_A.BCC[cascade_level]);
#ifdef NFC_DEBUG
        printf("PICC_A.UID[%d] = 0x%02X\n", cascade_level * NUM4, PICC_A.UID[cascade_level * NUM4]);
        printf("PICC_A.UID[%d] = 0x%02X\n", cascade_level * NUM4 + 1, PICC_A.UID[cascade_level * NUM4 + 1]);
        printf("PICC_A.UID[%d] = 0x%02X\n", cascade_level * NUM4 + NUM2, PICC_A.UID[cascade_level * NUM4 + NUM2]);
        printf("PICC_A.UID[%d] = 0x%02X\n", cascade_level * NUM4 + NUM3, PICC_A.UID[cascade_level * NUM4 + NUM3]);
        printf("PICC_A.BCC[%d] = 0x%02X\n", cascade_level, PICC_A.BCC[cascade_level]);
#endif
        if ((PICC_A.UID[cascade_level * NUM4] ^
            PICC_A.UID[cascade_level * NUM4 + 1] ^
            PICC_A.UID[cascade_level * NUM4 + NUM2] ^
            PICC_A.UID[cascade_level * NUM4 + NUM3]) == PICC_A.BCC[cascade_level]) {
#ifdef NFC_DEBUG
            printf("ReaderA_AntiColl[%d] success!\n", cascade_level);
#endif
            return FM17622_SUCCESS;
        }
    }
    return FM17622_COMM_ERR;
}

unsigned char ReaderA_Select(unsigned char cascade_level)
{
    unsigned char reg_data;
    SetReg(JREG_TXMODE, 0x80);              // Enable TxCRC
    SetReg(JREG_RXMODE, 0x80);              // Enable RxCRC
    SetReg(JREG_COMMAND, CMD_IDLE);         // command = Idel
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    SetReg(JREG_FIFODATA, RF_CMD_SELECT[cascade_level]);
    SetReg(JREG_FIFODATA, 0x70);
    SetReg(JREG_FIFODATA, PICC_A.UID[cascade_level * NUM4]);
    SetReg(JREG_FIFODATA, PICC_A.UID[cascade_level * NUM4 + 1]);
    SetReg(JREG_FIFODATA, PICC_A.UID[cascade_level * NUM4 + NUM2]);
    SetReg(JREG_FIFODATA, PICC_A.UID[cascade_level * NUM4 + NUM3]);
    SetReg(JREG_FIFODATA, PICC_A.BCC[cascade_level]);
    SetReg(JREG_COMMAND, CMD_TRANSCEIVE); // command = Transceive
    SetReg(JREG_BITFRAMING, 0x80);        // Start Send
    usleep(USLEEP_2MS);                     // Wait 2ms
    GetReg(JREG_FIFOLEVEL, &reg_data);
    if (reg_data == 1) {
        GetReg(JREG_FIFODATA, &PICC_A.SAK[cascade_level]);
#ifdef NFC_DEBUG
        printf("ReaderA_Select[%d] success!\n", cascade_level);
#endif
        return FM17622_SUCCESS;
    }
    return FM17622_COMM_ERR;
}

unsigned char ReaderA_CardActivate(void)
{
    unsigned char result = ReaderA_Wakeup();
    if (result != FM17622_SUCCESS) {
        printf("NFC ReaderA Wakeup fail\n");
        return FM17622_COMM_ERR;
    }

    if ((PICC_A.ATQA[0] & 0xC0) == 0x00) {          // 一重UID
        PICC_A.CASCADE_LEVEL = 1;
    } else if ((PICC_A.ATQA[0] & 0xC0) == 0x40) {   // 二重UID
        PICC_A.CASCADE_LEVEL = NUM2;
    } else if ((PICC_A.ATQA[0] & 0xC0) == 0x80) {   // 三重UID
        PICC_A.CASCADE_LEVEL = NUM3;
    }
#ifdef NFC_DEBUG
    printf("CASCADE_LEVEL = %d\n", PICC_A.CASCADE_LEVEL);
#endif
    for (int i = 0;i < PICC_A.CASCADE_LEVEL; i++) {
        result = ReaderA_AntiColl(i);
        if (result != FM17622_SUCCESS) {
            printf("NFC ReaderA_AntiColl[%d] fail.\n", i);
            return FM17622_COMM_ERR;
        }
        result = ReaderA_Select(i);
        if (result != FM17622_SUCCESS) {
            printf("NFC ReaderA_Select[%d] fail.\n", i);
            return FM17622_COMM_ERR;
        }
    }
    return result;
}

unsigned char ReaderB_Wakeup(void)
{
    unsigned char reg_data, i;
    SetReg(JREG_TXMODE, 0x83);              // Enable TxCRC
    SetReg(JREG_RXMODE, 0x83);              // Enable RxCRC
    SetReg(JREG_COMMAND, CMD_IDLE);         // command = Idel
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    SetReg(JREG_FIFODATA, 0x05);
    SetReg(JREG_FIFODATA, 0x00);
    SetReg(JREG_FIFODATA, 0x08);

    SetReg(JREG_COMMAND, CMD_TRANSCEIVE); // command = Transceive
    SetReg(JREG_BITFRAMING, 0x80);        // Start Send
    usleep(USLEEP_10MS);                    // Wait 10ms
    GetReg(JREG_FIFOLEVEL, &reg_data);
    if (reg_data == sizeof(PICC_B.ATQB)) {
        for (i = 0; i < sizeof(PICC_B.ATQB); i++)
            GetReg(JREG_FIFODATA, &PICC_B.ATQB[i]);
        memcpy(PICC_B.PUPI, PICC_B.ATQB + 1, NUM4);
        memcpy(PICC_B.APPLICATION_DATA, PICC_B.ATQB + NUM6, NUM4);
        memcpy(PICC_B.PROTOCOL_INF, PICC_B.ATQB + NUM10, NUM3);
        return FM17622_SUCCESS;
    }
    return FM17622_COMM_ERR;
}

unsigned char ReaderB_Request(void)
{
    unsigned char reg_data, i;
    SetReg(JREG_TXMODE, 0x83);              // Enable TxCRC
    SetReg(JREG_RXMODE, 0x83);              // Enable RxCRC
    SetReg(JREG_COMMAND, CMD_IDLE);         // command = Idel
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    SetReg(JREG_FIFODATA, 0x05);
    SetReg(JREG_FIFODATA, 0x00);
    SetReg(JREG_FIFODATA, 0x00);

    SetReg(JREG_COMMAND, CMD_TRANSCEIVE); // command = Transceive
    SetReg(JREG_BITFRAMING, 0x80);        // Start Send
    usleep(USLEEP_10MS);                    // Wait 10ms
    GetReg(JREG_FIFOLEVEL, &reg_data);
    if (reg_data == sizeof(PICC_B.ATQB)) {
        for (i = 0; i < sizeof(PICC_B.ATQB); i++) {
            GetReg(JREG_FIFODATA, &PICC_B.ATQB[i]);
        }
        memcpy(PICC_B.PUPI, PICC_B.ATQB + 1, NUM4);
        memcpy(PICC_B.APPLICATION_DATA, PICC_B.ATQB + NUM6, NUM4);
        memcpy(PICC_B.PROTOCOL_INF, PICC_B.ATQB + NUM10, NUM3);
        return FM17622_SUCCESS;
    }
    return FM17622_COMM_ERR;
}

unsigned char ReaderB_Attrib(void)
{
    unsigned char reg_data;
    SetReg(JREG_TXMODE, 0x83);              // Enable TxCRC
    SetReg(JREG_RXMODE, 0x83);              // Enable RxCRC
    SetReg(JREG_COMMAND, CMD_IDLE);         // command = Idel
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    SetReg(JREG_FIFODATA, 0x1D);
    SetReg(JREG_FIFODATA, PICC_B.PUPI[0]);
    SetReg(JREG_FIFODATA, PICC_B.PUPI[1]);
    SetReg(JREG_FIFODATA, PICC_B.PUPI[NUM2]);
    SetReg(JREG_FIFODATA, PICC_B.PUPI[NUM3]);
    SetReg(JREG_FIFODATA, 0x00);
    SetReg(JREG_FIFODATA, 0x08);
    SetReg(JREG_FIFODATA, 0x01);
    SetReg(JREG_FIFODATA, 0x01);
    SetReg(JREG_COMMAND, CMD_TRANSCEIVE); // command = Transceive
    SetReg(JREG_BITFRAMING, 0x80);        // Start Send
    usleep(USLEEP_10MS);                    // Wait 10ms
    GetReg(JREG_FIFOLEVEL, &reg_data);
    if (reg_data == 1) {
        GetReg(JREG_FIFODATA, PICC_B.ATTRIB);
        return FM17622_SUCCESS;
    }
    return FM17622_COMM_ERR;
}

unsigned char ReaderB_GetUID(void)
{
    unsigned char reg_data, i;
    SetReg(JREG_TXMODE, 0x83);              // Enable TxCRC
    SetReg(JREG_RXMODE, 0x83);              // Enable RxCRC
    SetReg(JREG_COMMAND, CMD_IDLE);         // command = Idel
    SetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO); // Clear FIFO
    SetReg(JREG_FIFODATA, 0x00);
    SetReg(JREG_FIFODATA, 0x36);
    SetReg(JREG_FIFODATA, 0x00);
    SetReg(JREG_FIFODATA, 0x00);
    SetReg(JREG_FIFODATA, 0x08);
    SetReg(JREG_COMMAND, CMD_TRANSCEIVE); // command = Transceive
    SetReg(JREG_BITFRAMING, 0x80);        // Start Send
    usleep(USLEEP_10MS);                    // Wait 10ms
    GetReg(JREG_FIFOLEVEL, &reg_data);
    if (reg_data == NUM10) {
        for (i = 0; i < sizeof(PICC_B.UID); i++) {
            GetReg(JREG_FIFODATA, &PICC_B.UID[i]);
        }
        return FM17622_SUCCESS;
    }
    return FM17622_COMM_ERR;
}

unsigned char FM17622_Polling(unsigned char* polling_card)
{
    unsigned char result;
    *polling_card = 0;
    FM17622_SoftReset();
    FM17622_Initial_ReaderA();
    SetCW(TX1_TX2_CW_ENABLE);
    result = ReaderA_Wakeup();
    if (result == FM17622_SUCCESS) {
        *polling_card |= BIT0;
    }
    FM17622_Initial_ReaderB();
    result = ReaderB_Wakeup();
    if (result == FM17622_SUCCESS) {
        *polling_card |= BIT1;
    }
    SetCW(TX1_TX2_CW_DISABLE);
    if (*polling_card != 0) {
        return FM17622_SUCCESS;
    } else {
        return FM17622_COMM_ERR;
    }
}

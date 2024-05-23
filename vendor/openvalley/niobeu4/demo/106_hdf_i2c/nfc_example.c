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
#include "cmsis_os2.h"
#include "ohos_run.h"
#include "nfc_api.h"

char KEY_DEFAULT[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; // 出厂默认KEY
char KEY_TEST1[] = { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5 };
char KEY_NFC_ANDROID[] = { 0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7 }; // 安卓数据扇区默认KEY

#define NFC_M1_KEY KEY_DEFAULT

#define BUFF_SIZE 16
#define PAGE_SIZE 4
#define BLOCK_SIZE 4
#define MAX_PAGE_NUM 48
#define MAX_SECTOR_NUM 16

/**
 * @brief 打印hex数组
 *
 * @param size
 * @param buff
 */
static void print_hex_buff(int size, unsigned char* buff)
{
    for (int k = 0; k < size; k++) {
        printf(" 0x%02X", buff[k]);
    }
    printf("\n");
}

/**
 * @brief 向TypeA卡中读数据
 *
 */
void ReadNtagCardAllData(struct picc_a_struct* TypeAInfo)
{
    int result = 0;
    unsigned char recv_buff[BUFF_SIZE] = { 0 }; // 读取的数据
    for (int i = 0; i < MAX_PAGE_NUM; i += PAGE_SIZE) { // 按PAGE读取, 共读取48页
        result = ReadNtagPage(i, recv_buff);
        if (result == 0) {
            printf("--->Read NTAG Card Page[%03d~%03d]:", i, i + NUM3);
            print_hex_buff(BUFF_SIZE, recv_buff);
        } else {
            printf("--->Read NTAG Card Page[%03d~%03d] Fail!\r\n", i, i + NUM3);
        }
    }
}
/**
 * @brief 向TypeA卡中写数据(请根据卡片存储结构定义，谨慎操作)
 *
 */
void WriteNtagCardAllData(struct picc_a_struct* TypeAInfo)
{
    int result = 0;
    unsigned char write_buff[PAGE_SIZE] = { 0x00, 0x00, 0x00, 0x00 }; // 写入的数据
    for (int i = PAGE_SIZE; i < (PAGE_SIZE * MAX_PAGE_NUM); i++) { // 用户数据区域为 4-225 块, 222个Block，共888字节
        result = WriteNtagPage(i, write_buff);
        if (result == 0) {
            printf("--->Write NTAG Card Block[%d]:", i);
            print_hex_buff(PAGE_SIZE, write_buff);
        } else {
            printf("--->Write NTAG Card Block[%d] Fail!\r\n", i);
            break;
        }
    }
}

/**
 * @brief 读写NTAG卡示例
 *
 * @param TypeAInfo
 */
void ReadWriteNtagCardExample(struct picc_a_struct* TypeAInfo)
{
    int result = 0;
    const int page_index = NUM4;                                // 操作第4页
    unsigned char write_buff[BUFF_SIZE] = { 0x00, 0x01, 0x02, 0x03 }; // 要写入卡片的真实数据
    unsigned char read_buff[BUFF_SIZE] = { 0 };

    result = WriteNtagPage(page_index, write_buff); // 将加密后的数据写至第1页，Ntag卡每页存放4字节
    if (result == 0) {
        printf("Write Page[%d]:", page_index);
        print_hex_buff(PAGE_SIZE, write_buff);
    }

    result = ReadNtagPage(page_index, read_buff); // 读取刚刚写入的数据, Ntag读卡时，每次会返回16个字节
    if (result != 0) {
        printf("Read Page[%d]:", page_index);
        print_hex_buff(PAGE_SIZE, read_buff); // 只取前4个字节
        printf("\n");
    }

    printf("Final Read Data:"); // 得到的数据应该与加密前的数据一致
    print_hex_buff(BUFF_SIZE, read_buff);
}

/**
 * @brief 读M1卡中保存的数据
 */
void ReadM1CardAllData(struct picc_a_struct* TypeAInfo)
{
    int result = 0;
    unsigned char recv_buff[BUFF_SIZE];
    for (int sector_index = 0; sector_index < MAX_SECTOR_NUM; sector_index++) {
        printf("\n--->Read M1Card SECTOR[%d] Data:\n", sector_index);
        result = Mifare_Auth(KEY_A_AUTH, sector_index, NFC_M1_KEY, TypeAInfo->UID); // 读M1卡前需要认证扇区
        if (result != 0) {
            printf("->NFC SECTOR[%d] AUTH fail! Please change the Mifare Key and retry.\r\n", sector_index);
            return FM17622_AUTH_ERR;
        }

        for (int j = 0; j < BLOCK_SIZE; j++) { // 读取该扇区第0、1、2、3块数据
            memset_s(recv_buff, sizeof(recv_buff), 0, sizeof(recv_buff));
            result = Mifare_Blockread(sector_index * BLOCK_SIZE + j, recv_buff);
            if (result != 0) {
                printf("->NFC read BLOCK Fail!\r\n");
                return FM17622_FIFO_ERR;
            } else {
                printf("SECTOR[%d] BLOCK[%d]:", sector_index, j);
                print_hex_buff(BLOCK_SIZE, recv_buff);
            }
        }
    }
}

/**
 * @brief 向M1卡中写数据
 *
 */
void WriteM1CardAllData(struct picc_a_struct* TypeAInfo)
{
    int result = 0;
    static int sector_index = 0;
    unsigned char write_buff[] = { 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
                                   0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66 }; // 写入的数据
    printf("\n--->Write M1Card SECTOR[%d] Data:\n", sector_index);
    result = Mifare_Auth(KEY_A_AUTH, sector_index, NFC_M1_KEY, TypeAInfo->UID); // 认证扇区
    if (result != 0) {
        printf("->NFC SECTOR[%d] AUTH fail! Please change the Mifare Key and retry.\r\n", sector_index);
        return FM17622_AUTH_ERR;
    }

    for (int j = 0; j < (BLOCK_SIZE - 1); j++) { // 写该扇区第0、1、2块数据，第0-2块是数据块，第3块是控制块
        if (sector_index == 0 && j == 0) { // M1卡第0扇区第0块为厂商固化信息，无法擦除
            continue;
        }
        result = Mifare_Blockwrite(sector_index * BLOCK_SIZE + j, write_buff); // 向数据块中写入数据
        if (result != 0) {
            printf("->NFC write BLOCK Fail!\r\n");
            return FM17622_FIFO_ERR;
        } else {
            printf("Write SECTOR[%d] BLOCK[%d]:", sector_index, j);
            print_hex_buff(BUFF_SIZE, write_buff);
        }
    }
    sector_index++; // 下一次读取下一个扇区
    if (sector_index >= MAX_SECTOR_NUM) {
        sector_index = 0;
    }
}

/**
 * @brief 读写M1卡示例
 *
 * @param TypeAInfo
 */
void ReadWriteM1CardExample(struct picc_a_struct* TypeAInfo)
{
    int result = 0;
    const int sector_index = 0; // 操作扇区0
    const int block_index = 1;  // 操作块1
    unsigned char write_buff[BUFF_SIZE] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F }; // 要写入卡片的真实数据
    unsigned char read_buff[BUFF_SIZE] = { 0 };
    printf("\n--->Auth M1Card SECTOR[%d]\n", sector_index);
    result = Mifare_Auth(KEY_A_AUTH, sector_index, NFC_M1_KEY, TypeAInfo->UID); // 读M1卡前需要认证扇区
    if (result != 0) {
        printf("->NFC SECTOR[%d] AUTH fail! Please change the Mifare Key and retry.\r\n", sector_index);
        return FM17622_AUTH_ERR;
    }

    result = Mifare_Blockwrite(sector_index * NUM4 + block_index, write_buff); // 将加密后的数据写至扇区0的第1块
    if (result == 0) {
        printf("Write SECTOR[%d] BLOCK[%d]:", sector_index, block_index);
        print_hex_buff(BUFF_SIZE, write_buff);
    }

    result = Mifare_Blockread(sector_index * NUM4 + block_index, read_buff); // 读取刚刚写入的数据
    if (result != 0) {
        printf("Read SECTOR[%d] BLOCK[%d]:", sector_index, block_index);
        print_hex_buff(BUFF_SIZE, read_buff);
    }

    printf("Final Read Data:");
    print_hex_buff(BUFF_SIZE, read_buff);
}

// 通过如下宏控制运行哪一个示例程序
#define READ_ALL_TEST // 读取卡片所有数据
// #define WIRTE_ALL_TEST       // 将卡片所有可写区域进行写入

/**
 * @brief 打印TypeA卡片信息
 *
 * @param TypeAInfo
 */
static void print_picc_a_info(struct picc_a_struct* TypeAInfo)
{
    printf("=========================================\n");
    printf("ATQA = 0x%02X 0x%02X\n", TypeAInfo->ATQA[0], TypeAInfo->ATQA[1]);
    printf("UID =");
    for (int i = 0; i < TypeAInfo->CASCADE_LEVEL; i++) {
        printf(" 0x%02X", TypeAInfo->UID[i * NUM4]);
        printf(" 0x%02X", TypeAInfo->UID[i * NUM4 + 1]);
        printf(" 0x%02X", TypeAInfo->UID[i * NUM4 + NUM2]);
        printf(" 0x%02X", TypeAInfo->UID[i * NUM4 + NUM3]);
    }
    printf("\n");
    printf("BCC =");
    print_hex_buff(TypeAInfo->CASCADE_LEVEL, TypeAInfo->BCC);

    printf("SAK =");
    print_hex_buff(TypeAInfo->CASCADE_LEVEL, TypeAInfo->SAK);
    printf("=========================================\n");
}

/**
 * @brief TypeA卡检测回调函数
 * 当检测到卡片后，该函数被调用
 * @param picc_typea_info TypeA卡信息
 */
void nfc_detected_typeA_callbak(struct picc_a_struct* picc_typea_info)
{
    printf("\n---> Nfc detect TypeA Card Callback <---\n");
    // To do: 检测到卡片，在此添加读写操作

    // 根据ATQA确定卡片类型
    if (picc_typea_info->ATQA[0] == 0x04 && picc_typea_info->ATQA[1] == 0x00) { // M1卡处理
#ifdef READ_ALL_TEST
        ReadM1CardAllData(picc_typea_info); // 读取M1卡片数据
#elif defined WIRTE_ALL_TEST
        WriteM1CardAllData(picc_typea_info);   // 向M1卡片写数据
#else
        ReadWriteM1CardExample(picc_typea_info);   // M1卡读写示例
#endif
    } else if (picc_typea_info->ATQA[0] == 0x44 && picc_typea_info->ATQA[1] == 0x00) { // NTAG卡处理
#ifdef READ_ALL_TEST
        ReadNtagCardAllData(picc_typea_info); // 读取Ntag卡片数据
#elif defined WIRTE_ALL_TEST
        WriteNtagCardAllData(picc_typea_info); // 向Ntag卡片写数据
#else
        ReadWriteNtagCardExample(picc_typea_info); // Ntag卡读写示例
#endif
    } else {
        printf("---> Unkown Card Type <---\n");
        print_picc_a_info(picc_typea_info);
    }
    return;
}

/**
 * @brief NFC扩展示例
 *
 */
void NfcExtExample(void)
{
    NfcInit(); // 初始化NFC组件
    SetNfcTypeACallback(nfc_detected_typeA_callbak); // 设置TypeA卡检测回调,获取卡片原始数据
}

/* 声明测试函数入口 */
OHOS_APP_RUN(NfcExtExample);
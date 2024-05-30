/*
 *
 */
#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__
#include "gsmcu_flash.h"
// flash boot
#define FLASH_START_ADDR (0x13000000UL)

#define FLASH_INFO_ADDRESS   FLASH_START_ADDR   // FLASH信息字段
#define FLASH_INTERRUPT_SIZE 0x200

#define FLASH_USER_FEATURE1_ADDRESS (FLASH_START_ADDR + 0x01000)                               // FLASH用户特征字1
#define FLASH_USER_FEATURE1_SIZE    0x1000                                                     // FLASH用户特征字1占用空间

#define FLASH_USER_FEATURE2_ADDRESS (FLASH_USER_FEATURE1_ADDRESS + FLASH_USER_FEATURE1_SIZE)   // FLASH用户特征字2
#define FLASH_USER_FEATURE2_SIZE    0x1000

#if defined(ZB205_CHIP)
  #define FLASH_USER_FEATURE3_ADDRESS (FLASH_USER_FEATURE2_ADDRESS + FLASH_USER_FEATURE2_SIZE)   // FLASH用户特征字2
  #define FLASH_USER_FEATURE3_SIZE    0x1000
#endif

#if defined(ZB204_CHIP)
  #define FLASH_CODE1_ADDRESS (FLASH_USER_FEATURE2_ADDRESS + FLASH_USER_FEATURE2_SIZE)   // 代码1
  #define FLASH_CODE1_SIZE    0x7d000                                                    // 500K
#elif defined(ZB205_CHIP)
  #define FLASH_CODE1_ADDRESS (FLASH_USER_FEATURE3_ADDRESS + FLASH_USER_FEATURE3_SIZE)   // 代码1
  #define FLASH_CODE1_SIZE    0x54000                                                    // 336K
#endif

#define FLASH_CODE2_ADDRESS (FLASH_CODE1_ADDRESS + FLASH_CODE1_SIZE)   // 代码2
#define FLASH_CODE2_SIZE    FLASH_CODE1_SIZE                           // 500K

#if defined(ZB205_CHIP)
  #define FLASH_CODE3_ADDRESS (FLASH_CODE2_ADDRESS + FLASH_CODE2_SIZE)   // 代码3
  #define FLASH_CODE3_SIZE    FLASH_CODE1_SIZE
#endif

// 506+506+4+4+4=1024 (1M)
// 到此代码段为+flash信息+code信息未为 516K 此段修改时需要解锁 需要改变code段大小时必须同步更改 flash保护字段大小!!!!!
#define FLASH_PARAM_ADDRESS (0x13100000UL)   // 参数
// #define FLASH_PARAM_SIZE 0x6000

#define ENABLE_PROTOCOL_FLASH
#define DATA_STA_ATTR_STORE_ADDR     FLASH_PARAM_ADDRESS
#define DATA_STA_FIX_PARA_STORE_ADDR (DATA_STA_ATTR_STORE_ADDR + DATA_FLASH_SECTION_SIZE)
#define DATA_STA_ID_INFO_STORE_ADDR  (DATA_STA_FIX_PARA_STORE_ADDR + DATA_FLASH_SECTION_SIZE * 2)
#define DATA_PLL_TRIM_STORE_ADDR     (DATA_STA_ID_INFO_STORE_ADDR + DATA_FLASH_SECTION_SIZE * 2)

#define DATA_VER_INFO_STORE_ADDR     (DATA_PLL_TRIM_STORE_ADDR + DATA_FLASH_SECTION_SIZE * 2)
#define DATA_STA_ROUTE_STORE_ADDR    (DATA_VER_INFO_STORE_ADDR + DATA_FLASH_SECTION_SIZE * 2)
#define STA_REAL_TIME_ADDRESS        (DATA_STA_ROUTE_STORE_ADDR + DATA_FLASH_SECTION_SIZE)
#define DATA_STA_PARA_STORE_ADDR     (STA_REAL_TIME_ADDRESS + DATA_FLASH_SECTION_SIZE)
#define DATA_RESET_CNT_STORE_ADDR    (DATA_STA_PARA_STORE_ADDR + DATA_FLASH_SECTION_SIZE)
#define DATA_POWER_OFF_STORE_ADDR    (DATA_RESET_CNT_STORE_ADDR + DATA_FLASH_SECTION_SIZE)
#define TRY_RUN_TIME_ADDRESS         (DATA_POWER_OFF_STORE_ADDR + DATA_FLASH_SECTION_SIZE)   // 试运行参数存放
#define DATA_EXT_VER_INFO_STORE_ADDR (TRY_RUN_TIME_ADDRESS + DATA_FLASH_SECTION_SIZE)

#define STA_READ_GRAPH_PARA      (DATA_EXT_VER_INFO_STORE_ADDR + DATA_FLASH_SECTION_SIZE * 2)
#define STA_READ_GRAPH_DATA      (STA_READ_GRAPH_PARA + DATA_FLASH_SECTION_SIZE)   // 0x12008A000
#define STA_READ_GRAPH_DATA_SIZE 0x32000                                           // 200K

#define DATA_FLASH_PAGE_SIZE    0x100
#define DATA_FLASH_SECTION_SIZE 0x1000
#define MAX_DATA_FLASH_SIZE     0xFFFFFF

#define DataFlashClose() \
  FLASH_Close();         \
  LOS_IntRestore(intSave)
#define DataFlashInit()    \
  UINT32 intSave;          \
  intSave = LOS_IntLock(); \
  FLASH_Init()

void EraseDataFlash(uint32_t start_addr, uint32_t dlen);
void WriteDataFlash(uint32_t start_addr, uint8_t *pbuf, uint32_t dlen);
void ReadDataFlash(uint32_t start_addr, uint8_t *pbuf, uint32_t dlen);

void EraseFlash(uint32_t start_addr, uint32_t dlen);
void WriteFlash(uint32_t start_addr, uint8_t *pbuf, uint32_t dlen);
void ReadFlash(uint32_t start_addr, uint8_t *pbuf, uint32_t dlen);

void Protection1MFlash(void);
void Unprotection1MFlash(void);
void Protection1MFlashNoVolatile(void);

#endif

/**
 *  @file littlefs_port.c
 *  @brief None.
 *  @author aron566 (wei.chen@gate-sea.com)
 *  @version v0.0.1 aron566 2024.05.27 13:29 初始版本.
 *  @date 2024-05-27
 *
 *  @details None.
 *  @par 修改日志:
 *  <table>
 *  <tr><th>Date       <th>Version <th>Author  <th>Description
 *  <tr><td>2024-05-27 <td>v0.0.1  <td>aron566 <td>初始版本
 *  </table>
 *  @copyright Copyright (c) 2024  Suzhou Gate-Sea Co.,Ltd.
 */
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "littlefs_port.h"
#include "los_memory.h"

#include <gsmcu_hal.h>
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus /**< use C compiler */
extern "C" {
#endif
/** Private macros -----------------------------------------------------------*/
// flash boot
#define FLASH_START_ADDR (0x12000000UL)

#define FLASH_INFO_ADDRESS   FLASH_START_ADDR   // FLASH信息字段
#define FLASH_INTERRUPT_SIZE 0x200

#define FLASH_USER_FEATURE1_ADDRESS (FLASH_START_ADDR + 0x01000)                               // FLASH用户特征字1
#define FLASH_USER_FEATURE1_SIZE    0x1000                                                     // FLASH用户特征字1占用空间

#define FLASH_USER_FEATURE2_ADDRESS (FLASH_USER_FEATURE1_ADDRESS + FLASH_USER_FEATURE1_SIZE)   // FLASH用户特征字2
#define FLASH_USER_FEATURE2_SIZE    0x1000

#define FLASH_CODE1_ADDRESS (FLASH_USER_FEATURE2_ADDRESS + FLASH_USER_FEATURE2_SIZE)   // 代码1
#define FLASH_CODE1_SIZE    0x3F000                                                    // 212K

#define FLASH_CODE2_ADDRESS (FLASH_CODE1_ADDRESS + FLASH_CODE1_SIZE)                   // 代码2
#define FLASH_CODE2_SIZE    0x3F000                                                    // 212K

// 到此代码段为+flash信息+code信息未为 512K 此段修改时需要解锁 需要改变code段大小时必须同步更改 flash保护字段大小!!!!!

#define FLASH_PARAM_ADDRESS (FLASH_CODE2_ADDRESS + FLASH_CODE2_SIZE)                   // 参数
// #define FLASH_PARAM_SIZE 0x6000

#define DATA_FLASH_PAGE_SIZE    0x100U
#define DATA_FLASH_SECTION_SIZE 0x1000U
#define MAX_DATA_FLASH_SIZE     0xFFFFFFU

#define ENABLE_PROTOCOL_FLASH
#define DATA_STA_ATTR_STORE_ADDR     FLASH_PARAM_ADDRESS
#define DATA_STA_FIX_PARA_STORE_ADDR (DATA_STA_ATTR_STORE_ADDR + DATA_FLASH_SECTION_SIZE)

#define DATA_STA_ID_INFO_STORE_ADDR (DATA_STA_FIX_PARA_STORE_ADDR + DATA_FLASH_SECTION_SIZE * 2)

#define DATA_PLL_TRIM_STORE_ADDR (DATA_STA_ID_INFO_STORE_ADDR + DATA_FLASH_SECTION_SIZE * 2)

#define TRY_RUN_TIME_ADDRESS     (DATA_PLL_TRIM_STORE_ADDR + DATA_FLASH_SECTION_SIZE)   // 试运行参数存放
#define DATA_VER_INFO_STORE_ADDR (TRY_RUN_TIME_ADDRESS + DATA_FLASH_SECTION_SIZE)

#define DATA_STA_PARA_STORE_ADDR  (DATA_VER_INFO_STORE_ADDR + DATA_FLASH_SECTION_SIZE * 2)   //
#define DATA_STA_ROUTE_STORE_ADDR (DATA_STA_PARA_STORE_ADDR + DATA_FLASH_SECTION_SIZE)       //
#define DATA_POWER_OFF_STORE_ADDR (DATA_STA_ROUTE_STORE_ADDR + DATA_FLASH_SECTION_SIZE)      //

#define STA_REAL_TIME_ADDRESS (DATA_POWER_OFF_STORE_ADDR + DATA_FLASH_SECTION_SIZE)          //
#define STA_READ_GRAPH_PARA   (STA_REAL_TIME_ADDRESS + DATA_FLASH_SECTION_SIZE)              //

#define STA_READ_GRAPH_DATA      (STA_READ_GRAPH_PARA + DATA_FLASH_SECTION_SIZE)             // 0x1208B000
#define STA_READ_GRAPH_DATA_SIZE 0x32000U                                                     // 200K


#define DataFlashClose() \
  FLASH_Close();         \
  LOS_IntRestore(intSave)

#define DataFlashInit() \
  uint32_t intSave; \
  intSave = LOS_IntLock(); \
  FLASH_Init()

static void EraseDataFlash(uint32_t start_addr, uint32_t dlen);
static void WriteDataFlash(uint32_t start_addr, uint8_t *pbuf, uint32_t dlen);
static void ReadDataFlash(uint32_t start_addr, uint8_t *pbuf, uint32_t dlen);

static void EraseFlash(uint32_t start_addr, uint32_t dlen);
static void WriteFlash(uint32_t start_addr, uint8_t *pbuf, uint32_t dlen);
static void ReadFlash(uint32_t start_addr, uint8_t *pbuf, uint32_t dlen);

void Protection512KFlash(void);
void Unprotection512KFlash(void);
void Protection512KFlashNoVolatile(void);
/** Private typedef ----------------------------------------------------------*/

/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/

/** Private function prototypes ----------------------------------------------*/

/** Private user code --------------------------------------------------------*/

/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/


static bool DiagnosticDataFlashAddr(uint32_t start_addr, uint32_t dlen)
{
  if ((start_addr < FLASH_START_ADDR) ||
      ((start_addr + dlen) > FLASH_START_ADDR + MAX_DATA_FLASH_SIZE))
  {
    return false;
  }
  return true;
}

static void EraseDataFlash(uint32_t start_addr, uint32_t dlen)
{
  uint32_t head_section, tail_section;
  uint32_t section_num;
  uint32_t i;
  uint32_t addr_idx;
  head_section = start_addr % DATA_FLASH_SECTION_SIZE;
  if (dlen > head_section)
  {
    if (0 != head_section)
    {
      FLASH_EarseBlock(start_addr);
    }
    addr_idx = start_addr + head_section;
    dlen -= head_section;
    section_num  = dlen / DATA_FLASH_SECTION_SIZE;
    tail_section = dlen % DATA_FLASH_SECTION_SIZE;
    for (i = 0; i < section_num; i++)
    {
      FLASH_EarseBlock(addr_idx);
      addr_idx += DATA_FLASH_SECTION_SIZE;
    }
    if (0 != tail_section)
    {
      FLASH_EarseBlock(addr_idx);
    }
  }
  else
  {
    FLASH_EarseBlock(start_addr);
  }
}

static void WriteDataFlash(uint32_t start_addr, uint8_t *pbuf, uint32_t dlen)
{
  uint32_t head_data_len, tail_data_len;
  uint32_t page_num;
  uint32_t data_idx = 0;
  uint32_t i;
  head_data_len = start_addr % DATA_FLASH_PAGE_SIZE;
  if (dlen > head_data_len)
  {
    FLASH_ProgramPackageBytes(start_addr, pbuf, head_data_len);
    data_idx += head_data_len;
    dlen -= head_data_len;
    page_num      = dlen / DATA_FLASH_PAGE_SIZE;
    tail_data_len = dlen % DATA_FLASH_PAGE_SIZE;
    for (i = 0; i < page_num; i++)
    {
      FLASH_ProgramPackageBytes(start_addr + data_idx, &pbuf[data_idx], DATA_FLASH_PAGE_SIZE);
      data_idx += DATA_FLASH_PAGE_SIZE;
    }
    FLASH_ProgramPackageBytes(start_addr + data_idx, &pbuf[data_idx], tail_data_len);
  }
  else
  {
    FLASH_ProgramPackageBytes(start_addr, pbuf, dlen);
  }
}

static void ReadDataFlash(uint32_t start_addr, uint8_t *pbuf, uint32_t dlen)
{
  if (!DiagnosticDataFlashAddr(start_addr, dlen))
  {
    return;
  }
  FLASH_Close();
  memcpy(pbuf, (uint32_t *)start_addr, dlen);
}

static void EraseFlash(uint32_t start_addr, uint32_t dlen)
{
  if (start_addr == 0 || start_addr == 0x12000000)
  {
    return;
  }
  // 擦除只能以DATA_FLASH_SECTION_SIZE为单位进行擦除，传入的参数横跨多少个BLOCK就擦多少个
  // 计算擦除的起始地址
  uint32_t realStartAddr = 0;
  uint32_t blockNum      = 0;
  uint32_t i             = 0;
  uint32_t headLen       = start_addr % DATA_FLASH_SECTION_SIZE;
  CPU_SR_ALLOC();
  if (headLen)
    realStartAddr = start_addr - headLen;
  else
    realStartAddr = start_addr;

  // 计算需要擦除多少个block
  if (dlen > DATA_FLASH_SECTION_SIZE - headLen)
  {
    uint32_t leftByte = dlen - (DATA_FLASH_SECTION_SIZE - headLen);
    if (leftByte % DATA_FLASH_SECTION_SIZE)
      blockNum = leftByte / DATA_FLASH_SECTION_SIZE + 1 + 1;
    else
      blockNum = leftByte / DATA_FLASH_SECTION_SIZE + 1;
  }
  else
    blockNum = 1;

  // 擦除
  for (; i < blockNum; i++)
  {
    OS_CRITICAL_ENTER();
    FLASH_EarseBlock(realStartAddr + i * DATA_FLASH_SECTION_SIZE);
    OS_CRITICAL_EXIT();
  }
}
static void WriteFlash(uint32_t start_addr, uint8_t *pbuf, uint32_t dlen)
{
  if (start_addr == 0 || start_addr == 0x12000000)
  {
    return;
  }
  uint32_t headLen          = start_addr % DATA_FLASH_PAGE_SIZE;
  uint32_t currentStartAddr = start_addr;
  uint32_t nextStartAddr    = start_addr - headLen + DATA_FLASH_PAGE_SIZE;
  CPU_SR_ALLOC();
  for (; dlen;)
  {
    uint32_t currentWriteNum = 0;
    if (nextStartAddr - currentStartAddr < dlen)
    {
      currentWriteNum = nextStartAddr - currentStartAddr;
    }
    else
    {
      currentWriteNum = dlen;
    }
    OS_CRITICAL_ENTER();
    FLASH_ProgramPackageBytes(currentStartAddr, pbuf, currentWriteNum);
    OS_CRITICAL_EXIT();
    currentStartAddr += currentWriteNum;
    nextStartAddr += DATA_FLASH_PAGE_SIZE;
    dlen -= currentWriteNum;
    pbuf += currentWriteNum;
  }
}

static void ReadFlash(uint32_t start_addr, uint8_t *pbuf, uint32_t dlen)
{
  FLASH_Close();
  memcpy(pbuf, (uint32_t *)start_addr, dlen);
}

// 保护头512K数据
void Protection512KFlash(void)
{
  if (FlashStatusRegNum == 3)
  {
    FLASH_WriteFlashStatus(1, ((1 << 4 | 1 << 5) << 16) | (0x2 << 8) | (1 << 6));
  }
  else if (FlashStatusRegNum == 2)
  {
    FLASH_WriteFlashStatus(1, ((1 << 4 | 1 << 5) << 8) | (0x2 << 0));
  }
  else
  {
    __BKPT(1);
  }
}
void Unprotection512KFlash(void)
{
  if (FlashStatusRegNum == 3)
  {
    FLASH_WriteFlashStatus(1, (0x2 << 8) | (1 << 6));
  }
  else if (FlashStatusRegNum == 2)
  {
    FLASH_WriteFlashStatus(1, 0x2);
  }
  else
  {
    __BKPT(1);
  }
}

void Protection512KFlashNoVolatile(void)
{
  DataFlashInit();
  uint8_t reg = FLASH_GetFalshStatus(1);
  if ((reg & (1 << 4 | 1 << 5)) == (1 << 4 | 1 << 5))
  {
    DataFlashClose();
    return;
  }
  if (FlashStatusRegNum == 3)
  {
    FLASH_WriteFlashStatusNoVolatile(1, ((1 << 4 | 1 << 5) << 16) | (0x3e << 8) | (1 << 6));
  }
  else if (FlashStatusRegNum == 2)
  {
    FLASH_WriteFlashStatusNoVolatile(1, ((1 << 4 | 1 << 5) << 8) | (0x6 << 0));
  }
  else
  {
    __BKPT(1);
  }
  DataFlashClose();
}

/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/

/**
 * @brief 获取作为littlsfs flash起始地址
 *
 * @return uint32_t 起始地址
 */
uint32_t Littlefs_Port_Get_Start_Addr(void)
{
  uint32_t Start_Addr = STA_READ_GRAPH_DATA + STA_READ_GRAPH_DATA_SIZE;
  Start_Addr += ((DATA_FLASH_SECTION_SIZE - (Start_Addr % DATA_FLASH_SECTION_SIZE)) % DATA_FLASH_SECTION_SIZE);
  return Start_Addr;
}

/**
 * @brief 获取littlefs 空间大小
 *
 * @return uint32_t
 */
uint32_t Littlefs_Port_Get_Littlefs_Size(void)
{
  uint32_t Start_Addr = Littlefs_Port_Get_Start_Addr();
  uint32_t Remain = ((FLASH_START_ADDR + MAX_DATA_FLASH_SIZE + 1U) - Start_Addr);
  return Remain;
}

/**
 * @brief 获取littlefs 块数量
 *
 * @return uint32_t
 */
uint32_t Littlefs_Port_Get_Littlefs_Blocks(void)
{
  uint32_t Blocks = Littlefs_Port_Get_Littlefs_Size() / DATA_FLASH_SECTION_SIZE;
  return Blocks;
}

int LittlefsRead(int partition, uint32_t *offset, void *buf, uint32_t size)
{
  // printf("%s Partition %d Addr 0x%08X, Size %u Bytes\r\n", __func__, partition, *offset, size);
  ReadDataFlash(*offset, (uint8_t *)buf, size);
  FLASH_Close();
  return LFS_ERR_OK;
}

int LittlefsProg(int partition, uint32_t *offset, const void *buf, uint32_t size)
{
  // printf("%s Partition %d Addr 0x%08X, Size %u Bytes\r\n", __func__, partition, *offset, size);
  FLASH_Init();
  WriteDataFlash(*offset, (uint8_t *)buf, size);
  FLASH_ResetAtFault();
  FLASH_Close();
  return LFS_ERR_OK;
}

int LittlefsErase(int partition, uint32_t offset, uint32_t size)
{
  // printf("%s Partition %d Addr 0x%08X, Size %u Bytes\r\n", __func__, partition, offset, size);
  FLASH_Init();
  EraseDataFlash(offset, size);
  FLASH_ResetAtFault();
  FLASH_Close();
  return LFS_ERR_OK;
}

#ifdef __cplusplus /**< end extern c */
}
#endif
/******************************** End of file *********************************/

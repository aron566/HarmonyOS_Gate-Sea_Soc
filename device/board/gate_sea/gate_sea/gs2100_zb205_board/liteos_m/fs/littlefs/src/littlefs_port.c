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
#include "bps_flash.h"
#include <gsmcu_hal.h>
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus /**< use C compiler */
extern "C" {
#endif
/** Private macros -----------------------------------------------------------*/
#define FLASH_FILE_ADDR   0x13180000U/**< 用于文件系统的起始存储地址 */
#define FLASH_FILE_SIZE   0x8000U    /**< 用于文件系统的大小 */
#define FLASH_BLOCK_SIZE  0x1000U    /**< flash块大小 */
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
  uint32_t Start_Addr = FLASH_FILE_ADDR;
  Start_Addr += ((FLASH_BLOCK_SIZE - (Start_Addr % FLASH_BLOCK_SIZE)) % FLASH_BLOCK_SIZE);
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
  uint32_t Remain = ((FLASH_FILE_ADDR + FLASH_FILE_SIZE) - Start_Addr);
  return Remain;
}

/**
 * @brief 获取littlefs 块数量
 *
 * @return uint32_t
 */
uint32_t Littlefs_Port_Get_Littlefs_Blocks(void)
{
  uint32_t Blocks = Littlefs_Port_Get_Littlefs_Size() / FLASH_BLOCK_SIZE;
  return Blocks;
}

int LittlefsRead(int partition, uint32_t *offset, void *buf, uint32_t size)
{
  // printf("%s Partition %d Addr 0x%08X, Size %u Bytes\r\n", __func__, partition, *offset, size);
  uint32_t Addr = *offset;
  memcpy(buf, (void *)Addr, size);
  (*offset) += size;
  return LFS_ERR_OK;
}

int LittlefsProg(int partition, uint32_t *offset, const void *buf, uint32_t size)
{
  // printf("%s Partition %d Addr 0x%08X, Size %u Bytes\r\n", __func__, partition, *offset, size);
  uint32_t Addr = *offset;
  WriteFlash(Addr, buf, size);
  (*offset) += size;
  return LFS_ERR_OK;
}

int LittlefsErase(int partition, uint32_t offset, uint32_t size)
{
  // printf("%s Partition %d Addr 0x%08X, Size %u Bytes\r\n", __func__, partition, offset, size);
  EraseFlash(offset, size);
  return LFS_ERR_OK;
}

#ifdef __cplusplus /**< end extern c */
}
#endif
/******************************** End of file *********************************/

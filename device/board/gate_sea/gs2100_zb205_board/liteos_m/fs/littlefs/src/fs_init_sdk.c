/**
 *  @file fs_init_sdk.c
 *  @brief None.
 *  @author aron566 (wei.chen@gate-sea.com)
 *  @version v0.0.1 aron566 2024.05.27 10:59 初始版本.
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
#include "fs_init_sdk.h"
#include <sys/mount.h>
#include "littlefs_port.h"
#include "los_config.h"
#include <sys/stat.h>
#include <dirent.h>
#include "los_config.h"
#include "los_fs.h"
#include "los_interrupt.h"

/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus /**< use C compiler */
extern "C" {
#endif
/** Private macros -----------------------------------------------------------*/

#define USE_EXTERN_FLASH_FOR_LITEFS 1 /**< 是否使用真实flash作为litefs */

/** Private typedef ----------------------------------------------------------*/

typedef enum
{
  FLASH_PARTITION_DATA0 = 0,
  FLASH_PARTITION_DATA1 = 1,
  FLASH_PARTITION_MAX,
} HalPartition;

typedef struct
{
  const CHAR *partitionDescription;
  uint32_t    partitionStartAddr;
  uint32_t    partitionLength;
  uint32_t    partitionOptions;
} HalLogicPartition;

struct fs_cfg
{
  char               *mount_point;
  struct PartitionCfg lfs_cfg;
};

/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/

/* litefs句柄 */
static struct fs_cfg fs;

#if USE_EXTERN_FLASH_FOR_LITEFS == 0

/* ram flash信息 */
static HalLogicPartition g_halPartitions[] = {
  [FLASH_PARTITION_DATA0] = {
                             .partitionDescription = "littlefs",
                             .partitionStartAddr   = 0x10000000 + 0x1C000,
                             .partitionLength      = 0x4000,               /* 16K */
  },
  [FLASH_PARTITION_DATA1] = {
                             .partitionDescription = "vfat",
                             .partitionStartAddr   = 0x10000000 + 0x1C000, // 112K start
    .partitionLength      = 0x4000,                 // 16K
  },
};

/** Private function prototypes ----------------------------------------------*/

/** Private user code --------------------------------------------------------*/

/** Private application code -------------------------------------------------*/
/*******************************************************************************
 *
 *       Static code
 *
 ********************************************************************************
 */

/**
 * @brief ram flash 擦除
 *
 * @param pdrv
 * @param offSet
 * @param size
 * @return int32_t
 */
static int32_t lfs_flash_erase(HalPartition pdrv, uint32_t offSet, uint32_t size)
{
  uint32_t startAddr = offSet;
  uint32_t partitionEnd;

  partitionEnd = g_halPartitions[pdrv].partitionStartAddr + g_halPartitions[pdrv].partitionLength;
  if (startAddr >= partitionEnd)
  {
    printf("flash over erase, 0x%lx, 0x%lx\r\n", startAddr, partitionEnd);
    printf("flash over write\r\n");
    return -1;
  }
  if ((startAddr + size) > partitionEnd)
  {
    size = partitionEnd - startAddr;
    printf("flash over write, new len is %ld\r\n", size);
  }

  uint32_t intSave = LOS_IntLock();
  memset((VOID *)startAddr, 0, size);
  LOS_IntRestore(intSave);
  return 0;
}

/**
 * @brief ram flash 写入
 *
 * @param pdrv
 * @param offSet
 * @param buf
 * @param bufLen
 * @return int32_t
 */
static int32_t lfs_flash_write(HalPartition pdrv, uint32_t *offSet, const VOID *buf, uint32_t bufLen)
{
  uint32_t startAddr = *offSet;
  uint32_t partitionEnd;

  partitionEnd = g_halPartitions[pdrv].partitionStartAddr + g_halPartitions[pdrv].partitionLength;
  if (startAddr >= partitionEnd)
  {
    printf("flash over write, 0x%lx, 0x%lx\r\n", startAddr, partitionEnd);
    return -1;
  }
  if ((startAddr + bufLen) > partitionEnd)
  {
    bufLen = partitionEnd - startAddr;
    printf("flash over write, new len is %ld\r\n", bufLen);
  }

  uint32_t intSave = LOS_IntLock();
  memcpy((VOID *)startAddr, buf, bufLen);
  LOS_IntRestore(intSave);

  return 0;
}

/**
 * @brief ram flash 擦写
 *
 * @param pdrv
 * @param offSet
 * @param buf
 * @param bufLen
 * @return int32_t
 */
static int32_t lfs_flash_erase_write(HalPartition pdrv, uint32_t *offSet, const VOID *buf, uint32_t bufLen)
{
  uint32_t startAddr = *offSet;
  uint32_t partitionEnd;

  partitionEnd = g_halPartitions[pdrv].partitionStartAddr + g_halPartitions[pdrv].partitionLength;
  if (startAddr >= partitionEnd)
  {
    printf("flash over e&w, 0x%lx, 0x%lx\r\n", startAddr, partitionEnd);
    return -1;
  }
  if ((startAddr + bufLen) > partitionEnd)
  {
    bufLen = partitionEnd - startAddr;
    printf("flash over erase&write, new len is %ld\r\n", bufLen);
  }

  uint32_t intSave = LOS_IntLock();
  memset((VOID *)startAddr, 0, bufLen);
  memcpy((VOID *)startAddr, buf, bufLen);
  LOS_IntRestore(intSave);
  return 0;
}

/**
 * @brief ram flash 读
 *
 * @param pdrv
 * @param offSet
 * @param buf
 * @param bufLen
 * @return int32_t
 */
static int32_t lfs_flash_read(HalPartition pdrv, uint32_t *offSet, VOID *buf, uint32_t bufLen)
{
  uint32_t startAddr = *offSet;
  uint32_t partitionEnd;

  partitionEnd = g_halPartitions[pdrv].partitionStartAddr + g_halPartitions[pdrv].partitionLength;
  if (startAddr >= partitionEnd)
  {
    printf("flash over read, 0x%lx, 0x%lx\r\n", startAddr, partitionEnd);
    return -1;
  }
  if ((startAddr + bufLen) > partitionEnd)
  {
    bufLen = partitionEnd - startAddr;
    printf("flash over read, new len is %ld\r\n", bufLen);
  }

  uint32_t intSave = LOS_IntLock();
  memcpy(buf, (VOID *)startAddr, bufLen);
  LOS_IntRestore(intSave);
  return 0;
}

/** Public application code --------------------------------------------------*/
/*******************************************************************************
 *
 *       Public code
 *
 ********************************************************************************
 */

/**
 * @brief Get the Partition Info object
 *
 * @return HalLogicPartition*
 */
HalLogicPartition *getPartitionInfo(VOID)
{
  return g_halPartitions;
}

/**
 * @brief ram flash 初始化
 *
 * @return int32_t
 */
int32_t FsInit(void)
{
  int32_t ret;
  DIR    *dir;

  struct fs_cfg      fs[LOSCFG_LFS_MAX_MOUNT_SIZE] = {0};
  HalLogicPartition *halPartitionsInfo             = getPartitionInfo();

  int32_t lengthArray[2];
  lengthArray[0] = halPartitionsInfo[FLASH_PARTITION_DATA0].partitionLength;

  int32_t addrArray[2];
  addrArray[0] = halPartitionsInfo[FLASH_PARTITION_DATA0].partitionStartAddr;

  ret = LOS_DiskPartition("flash0", "littlefs", lengthArray, addrArray, 2);
  printf("%s: DiskPartition %s\n", __func__, (ret == 0) ? "succeed" : "failed");
  if (ret != 0)
  {
    return -1;
  }
  fs[0].mount_point        = "/littlefs";
  fs[0].lfs_cfg.partNo     = 0;
  fs[0].lfs_cfg.blockSize  = 4096; /* 4096, lfs block size */
  fs[0].lfs_cfg.blockCount = 4;    /* 4, lfs block count */

  fs[0].lfs_cfg.readFunc  = lfs_flash_read;
  fs[0].lfs_cfg.writeFunc = lfs_flash_write;
  fs[0].lfs_cfg.eraseFunc = lfs_flash_erase;

  fs[0].lfs_cfg.readSize      = 256; /* 256, lfs read size */
  fs[0].lfs_cfg.writeSize     = 256; /* 256, lfs prog size */
  fs[0].lfs_cfg.cacheSize     = 256; /* 256, lfs cache size */
  fs[0].lfs_cfg.lookaheadSize = 16;  /* 16, lfs lookahead size */
  fs[0].lfs_cfg.blockCycles   = 500; /* 500, lfs block cycles */

  ret = LOS_PartitionFormat("flash0", "littlefs", &fs[0].lfs_cfg);
  printf("%s: PartitionFormat %s\n", __func__, (ret == 0) ? "succeed" : "failed");
  if (ret != 0)
  {
    return -1;
  }

  ret = mount(NULL, fs[0].mount_point, "littlefs", 0, &fs[0].lfs_cfg);
  printf("%s: mount fs on '%s' %s\n", __func__, fs[0].mount_point, (ret == 0) ? "succeed" : "failed");
  if (ret != 0)
  {
    return -1;
  }

  if (!ret)
  {
    ret = mkdir(fs[0].mount_point, S_IRUSR | S_IWUSR | S_IXUSR);
    printf("%s: mkdir %s %s\n", __func__, fs[0].mount_point, (ret == 0) ? "succeed" : "failed");
  }

  return 0;
}
#endif                                 /**< USE_EXTERN_FLASH_FOR_LITEFS == 0 */
/**
 * @brief 拼出路径
 *
 * @param base_path 基础路径
 * @param filename 文件名
 */
static void print_file_path(const char *base_path, const char *filename)
{
  char path[1024];
  strcpy(path, base_path);
  strcat(path, "/");
  strcat(path, filename);
  printf("File path: %s\n", path);
}

/**
 * @brief 列出文件夹文件信息
 *
 * @param path 文件夹绝对路径
 */
static void list_dir(const char *path)
{
  struct dirent *dirent;
  DIR           *dir = opendir(path);

  if (!dir)
  {
    fprintf(stderr, "Cannot open directory: %s\n", path);
    return;
  }

  while ((dirent = readdir(dir)))
  {
    if (strcmp(dirent->d_name, ".") != 0 && strcmp(dirent->d_name, "..") != 0)
    {
      print_file_path(path, dirent->d_name);
    }
  }

  closedir(dir);
}

#if USE_EXTERN_FLASH_FOR_LITEFS == 1   // use extern flash

/**
 * @brief 外部flash 初始化
 *
 * @return int32_t
 */
int32_t FsInit(void)
{
  int ret;

  int32_t lengthArray = Littlefs_Port_Get_Littlefs_Size();
  int32_t addrArray   = Littlefs_Port_Get_Start_Addr();

  /* 磁盘分区 */
  ret = LOS_DiskPartition("flash0", "littlefs", &lengthArray, &addrArray, 1);
  printf("%s: DiskPartition %s\n", __func__, (ret == 0) ? "succeed" : "failed");
  if (ret != 0)
  {
    return -1;
  }

  /* flash 256Bytes/page 4KB/block rw */
  fs.mount_point           = "/littlefs";
  fs.lfs_cfg.readFunc      = LittlefsRead;
  fs.lfs_cfg.writeFunc     = LittlefsProg;
  fs.lfs_cfg.eraseFunc     = LittlefsErase;
  fs.lfs_cfg.readSize      = 256U;
  fs.lfs_cfg.writeSize     = 256U;
  fs.lfs_cfg.blockSize     = 4096U;
  fs.lfs_cfg.blockCount    = Littlefs_Port_Get_Littlefs_Blocks();
  fs.lfs_cfg.cacheSize     = 256U; /* must be set */
  fs.lfs_cfg.partNo        = 0U;
  fs.lfs_cfg.lookaheadSize = 2U; /* must be set */
  fs.lfs_cfg.blockCycles   = 500;

  do
  {
    ret = mount(NULL, fs.mount_point, "littlefs", 0, &fs.lfs_cfg);
    printf("mount fs on '%s' %s\n", fs.mount_point, (ret == 0) ? "success" : "failed");

    /* 尝试打开文件夹 */
    DIR           *dir;
    struct dirent *ent;

    dir = opendir("/littlefs");
    if (dir != NULL)
    {
      list_dir("/littlefs");
      closedir(dir);
      /* 存在目录 */
      break;
    }
    else
    {
      printf("Couldn't open directory\r\n");
    }

    /* 创建文件夹 */
    if (ret == 0)
    {
      ret = mkdir(fs.mount_point, 0777);
      if (ret != LOS_OK)
      {
        printf("mkdir failed %d\n", ret);
      }
      else
      {
        printf("mkdir successed %d\n", ret);
      }
      /* 挂载成功跳出 */
      break;
    }
    /* 挂载失败 */
    else
    {
      /* 格式化 */
      ret = LOS_PartitionFormat("flash0", "littlefs", &fs.lfs_cfg);
      printf("%s: PartitionFormat %s\n", __func__, (ret == 0) ? "succeed" : "failed");
      if (ret != 0)
      {
        return -1;
      }
    }
  }
  while (1);

  return 0;
}
#endif             /**< USE_EXTERN_FLASH_FOR_LITEFS == 1 */

#ifdef __cplusplus /**< end extern c */
}
#endif
/******************************** End of file *********************************/

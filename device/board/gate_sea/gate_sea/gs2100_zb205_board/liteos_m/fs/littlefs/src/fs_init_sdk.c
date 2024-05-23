/*
 * Copyright (c) 2022 Hihope Co., Ltd.
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

#include "fs_init_sdk.h"
#include <sys/mount.h>
#include "littlefs.h"
#include "los_config.h"
#include <sys/stat.h>
#include <dirent.h>
#include "los_config.h"
#include "los_fs.h"
#include "los_interrupt.h"

typedef enum {
    FLASH_PARTITION_DATA0 = 0,
    FLASH_PARTITION_DATA1 = 1,
    FLASH_PARTITION_MAX,
} HalPartition;

typedef struct {
    const CHAR *partitionDescription;
    uint32_t partitionStartAddr;
    uint32_t partitionLength;
    uint32_t partitionOptions;
} HalLogicPartition;

struct fs_cfg {
    char *mount_point;
    struct PartitionCfg lfs_cfg;
};

HalLogicPartition g_halPartitions[] = {
    [FLASH_PARTITION_DATA0] = {
        .partitionDescription = "littlefs",
        .partitionStartAddr = 0x20000000 + 0x1C000, //112K start
        .partitionLength = 0x4000, //16K
    },
    [FLASH_PARTITION_DATA1] = {
        .partitionDescription = "vfat",
        .partitionStartAddr = 0x20000000 + 0x1C000, //112K start
        .partitionLength = 0x4000, //16K
    },
};

static struct fs_cfg fs;

int32_t lfs_flash_erase(HalPartition pdrv, uint32_t offSet, uint32_t size)
{
    uint32_t startAddr = offSet;
    uint32_t partitionEnd;

    partitionEnd = g_halPartitions[pdrv].partitionStartAddr + g_halPartitions[pdrv].partitionLength;
    if (startAddr >= partitionEnd) {
        printf("flash over erase, 0x%x, 0x%x\r\n", startAddr, partitionEnd);
        printf("flash over write\r\n");
        return -1;
    }
    if ((startAddr + size) > partitionEnd) {
        size = partitionEnd - startAddr;
        printf("flash over write, new len is %d\r\n", size);
    }

    uint32_t intSave = LOS_IntLock();
    memset((VOID *)startAddr, 0, size);
    LOS_IntRestore(intSave);
    return 0;
}

int32_t lfs_flash_write(HalPartition pdrv, uint32_t *offSet, const VOID *buf, uint32_t bufLen)
{
    uint32_t startAddr = *offSet;
    uint32_t partitionEnd;

    partitionEnd = g_halPartitions[pdrv].partitionStartAddr + g_halPartitions[pdrv].partitionLength;
    if (startAddr >= partitionEnd) {
        printf("flash over write, 0x%x, 0x%x\r\n", startAddr, partitionEnd);
        return -1;
    }
    if ((startAddr + bufLen) > partitionEnd) {
        bufLen = partitionEnd - startAddr;
        printf("flash over write, new len is %d\r\n", bufLen);
    }

    uint32_t intSave = LOS_IntLock();
    memcpy((VOID *)startAddr, buf, bufLen);
    LOS_IntRestore(intSave);

    return 0;
}

int32_t lfs_flash_erase_write(HalPartition pdrv, uint32_t *offSet, const VOID *buf, uint32_t bufLen)
{
    uint32_t startAddr = *offSet;
    uint32_t partitionEnd;

    partitionEnd = g_halPartitions[pdrv].partitionStartAddr + g_halPartitions[pdrv].partitionLength;
    if (startAddr >= partitionEnd) {
        printf("flash over e&w, 0x%x, 0x%x\r\n", startAddr, partitionEnd);
        return -1;
    }
    if ((startAddr + bufLen) > partitionEnd) {
        bufLen = partitionEnd - startAddr;
        printf("flash over erase&write, new len is %d\r\n", bufLen);
    }

    uint32_t intSave = LOS_IntLock();
    memset((VOID *)startAddr, 0, bufLen);
    memcpy((VOID *)startAddr, buf, bufLen);
    LOS_IntRestore(intSave);
    return 0;
}

int32_t lfs_flash_read(HalPartition pdrv, uint32_t *offSet, VOID *buf, uint32_t bufLen)
{
    uint32_t startAddr = *offSet;
    uint32_t partitionEnd;

    partitionEnd = g_halPartitions[pdrv].partitionStartAddr + g_halPartitions[pdrv].partitionLength;
    if (startAddr >= partitionEnd) {
        printf("flash over read, 0x%x, 0x%x\r\n", startAddr, partitionEnd);
        return -1;
    }
    if ((startAddr + bufLen) > partitionEnd) {
        bufLen = partitionEnd - startAddr;
        printf("flash over read, new len is %d\r\n", bufLen);
    }

    uint32_t intSave = LOS_IntLock();
    memcpy(buf, (VOID *)startAddr, bufLen);
    LOS_IntRestore(intSave);
    return 0;
}

HalLogicPartition *getPartitionInfo(VOID)
{
    return g_halPartitions;
}

int32_t FsInit()
{
    int32_t ret;
    DIR *dir;

    struct fs_cfg fs[LOSCFG_LFS_MAX_MOUNT_SIZE] = {0};
    HalLogicPartition *halPartitionsInfo = getPartitionInfo();

    int32_t lengthArray[2];
    lengthArray[0]= halPartitionsInfo[FLASH_PARTITION_DATA0].partitionLength;

    int32_t addrArray[2];
    addrArray[0] = halPartitionsInfo[FLASH_PARTITION_DATA0].partitionStartAddr;

    ret = LOS_DiskPartition("flash0", "littlefs", lengthArray, addrArray, 2);
    printf("%s: DiskPartition %s\n", __func__, (ret == 0) ? "succeed" : "failed");
    if (ret != 0) {
        return -1;
    }
    fs[0].mount_point = "/littlefs";
    fs[0].lfs_cfg.partNo = 0;
    fs[0].lfs_cfg.blockSize = 4096; /* 4096, lfs block size */
    fs[0].lfs_cfg.blockCount = 4; /* 4, lfs block count */

    fs[0].lfs_cfg.readFunc = lfs_flash_read;
    fs[0].lfs_cfg.writeFunc = lfs_flash_write;
    fs[0].lfs_cfg.eraseFunc = lfs_flash_erase;

    fs[0].lfs_cfg.readSize = 256; /* 256, lfs read size */
    fs[0].lfs_cfg.writeSize = 256; /* 256, lfs prog size */
    fs[0].lfs_cfg.cacheSize = 256; /* 256, lfs cache size */
    fs[0].lfs_cfg.lookaheadSize = 16; /* 16, lfs lookahead size */
    fs[0].lfs_cfg.blockCycles = 500; /* 500, lfs block cycles */

    ret = LOS_PartitionFormat("flash0", "littlefs", &fs[0].lfs_cfg);
    printf("%s: PartitionFormat %s\n", __func__, (ret == 0) ? "succeed" : "failed");
    if (ret != 0) {
        return -1;
    }

    ret = mount(NULL, fs[0].mount_point, "littlefs", 0, &fs[0].lfs_cfg);
    printf("%s: mount fs on '%s' %s\n", __func__, fs[0].mount_point, (ret == 0) ? "succeed" : "failed");
    if (ret != 0) {
        return -1;
    }

    if (!ret) {
        ret = mkdir(fs[0].mount_point, S_IRUSR | S_IWUSR | S_IXUSR);
        printf("%s: mkdir %s %s\n", __func__, fs[0].mount_point, (ret == 0) ? "succeed" : "failed");
    }

    return 0;
}

#if 0 //use w25qxx
int32_t FsInit(void)
{
    int ret;

    int32_t lengthArray = 0x00800000;
    int32_t addrArray = 4096;
    W25QXX_Init();
    printf("[%d]\r\n",W25QXX_ReadID());

    ret = LOS_DiskPartition("flash0", "littlefs", &lengthArray, &addrArray, 1);
    printf("%s: DiskPartition %s\n", __func__, (ret == 0) ? "succeed" : "failed");
    if (ret != 0) {
        return -1;
    }

    fs.mount_point = "/littlefs";
    fs.lfs_cfg.readFunc = LittlefsRead;
    fs.lfs_cfg.writeFunc = LittlefsProg;
    fs.lfs_cfg.eraseFunc = LittlefsErase;
    fs.lfs_cfg.readSize = 1024;
    fs.lfs_cfg.writeSize = 1024;
    fs.lfs_cfg.blockSize = 4096;
    fs.lfs_cfg.blockCount = 4096;
    fs.lfs_cfg.cacheSize = 2048;
    fs.lfs_cfg.partNo = 0;
    fs.lfs_cfg.lookaheadSize = 2048;
    fs.lfs_cfg.blockCycles = 500;

    ret = LOS_PartitionFormat("flash0", "littlefs", &fs.lfs_cfg);
    printf("%s: PartitionFormat %s\n", __func__, (ret == 0) ? "succeed" : "failed");
    if (ret != 0) {
        return -1;
    }
    ret = mount(NULL, fs.mount_point, "littlefs", 0, &fs.lfs_cfg);
    printf("mount fs on '%s' %s\n", fs.mount_point, (ret == 0) ? "success" : "failed");
    if(ret == 0)
    {
        ret = mkdir(fs.mount_point, 0777);
        if(ret != LOS_OK) {
            printf("mkdir failed %d\n", ret);
        } else {
            printf("mkdir successed %d\n", ret);
        }
    }

    return 0;
}
#endif


#include "stdio.h"
#include "littlefs.h"
#include "errno.h"
#include "device_resource_if.h"
#include "hdf_log.h"
#include "hdf_device_desc.h"
#include "los_config.h"
#include "los_fs.h"
#include "los_interrupt.h"

#define QSPI_W25Q16_LFS 1
#define W25Q16_ID  0xC84015

typedef enum {
    FLASH_PARTITION_DATA0 = 0,
    FLASH_PARTITION_DATA1 = 1,
    FLASH_PARTITION_MAX,
} HalPartition;

typedef struct {
    const CHAR *partitionDescription;
    UINT32 partitionStartAddr;
    UINT32 partitionLength;
    UINT32 partitionOptions;
} HalLogicPartition;

struct fs_cfg {
    char *mount_point;
    struct lfs_config lfs_cfg;
};

static struct fs_cfg fs;

int W25Qxx_readLittlefs(const struct lfs_config *lfsCfg, lfs_block_t block,
                lfs_off_t off, void *buffer, lfs_size_t size)
{
    uint16_t sector_size = fs.lfs_cfg.block_size;
    uint16_t sector_count = fs.lfs_cfg.block_count;

    if(block >= sector_count) {
        HDF_LOGE("%s: block size error![block = %d] [sector_count = %d]\n", __FUNCTION__, block, sector_count);
        return LFS_ERR_IO;
    }

#if QSPI_W25Q16_LFS
    W25Qxx_QSPI_Read((uint8_t*)buffer, (uint32_t)block * sector_size + off, size);
#else
    W25Qxx_Read((uint8_t*)buffer, (uint32_t)block * sector_size + off, size);
#endif
    return 0;
}

int W25Qxx_writeLittlefs(const struct lfs_config *c, lfs_block_t block,
                lfs_off_t off, void *buffer, lfs_size_t size)
{
    uint16_t sector_size = fs.lfs_cfg.block_size;
    uint16_t sector_count = fs.lfs_cfg.block_count;

    if(block >= sector_count) {
        HDF_LOGE("%s: block size error!\n", __FUNCTION__);
        return LFS_ERR_IO;
    }

#if QSPI_W25Q16_LFS
    W25Qxx_QSPI_Write_NoCheck((uint8_t*)buffer, (uint32_t)block * sector_size + off, size);
#else
    W25Qxx_Write_NoCheck((uint8_t*)buffer, (uint32_t)block * sector_size + off, size);
#endif
    return 0;
}

int W25Qxx_eraseLittlefs(const struct lfs_config *c, lfs_block_t block,
                lfs_off_t off, void *buffer, lfs_size_t size)
{
    uint16_t sector_size = fs.lfs_cfg.block_size;
    uint16_t sector_count = fs.lfs_cfg.block_count;

    if(block >= sector_count) {
        HDF_LOGE("%s: block size error!\n", __FUNCTION__);
        return LFS_ERR_IO;
    }

#if QSPI_W25Q16_LFS
    W25Qxx_QSPI_EraseSector((block) * sector_size);
#else
    W25Qxx_EraseSector((block) * sector_size);
#endif

    return 0;
}

int W25Qxx_syncLittlefs(const struct lfs_config *c, lfs_block_t block,
                lfs_off_t off, void *buffer, lfs_size_t size)
{
    return 0;
}

static uint32_t FsGetResource(struct fs_cfg *fs, const struct DeviceResourceNode *resourceNode)
{
    if (fs == NULL || resourceNode == NULL) {
        HDF_LOGE("%s: fs is NULL or resourceNode is NULL\n", __func__);
        return HDF_FAILURE;
    }

    struct DeviceResourceIface *resource = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);

    if (resource == NULL) {
        HDF_LOGE("Invalid DeviceResourceIface");
        return HDF_FAILURE;
    }

    if (resource->GetString(resourceNode, "mount_points",
        &fs->mount_point, NULL) != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to get mount_points", __func__);
        return HDF_FAILURE;
    }

    if (resource->GetUint32(resourceNode, "block_size",
        &fs->lfs_cfg.block_size, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to get block_size", __func__);
        return HDF_FAILURE;
    }

    if (resource->GetUint32(resourceNode, "block_count",
        &fs->lfs_cfg.block_count, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to get block_count", __func__);
        return HDF_FAILURE;
    }

    if (resource->GetUint32(resourceNode, "read_size",
        &fs->lfs_cfg.read_size, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to get read_size", __func__);
        return HDF_FAILURE;
    }

    if (resource->GetUint32(resourceNode, "prog_size",
        &fs->lfs_cfg.prog_size, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to get prog_size", __func__);
        return HDF_FAILURE;
    }

    if (resource->GetUint32(resourceNode, "cache_size",
        &fs->lfs_cfg.cache_size, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to get cache_size", __func__);
        return HDF_FAILURE;
    }

    if (resource->GetUint32(resourceNode, "lookahead_size",
        &fs->lfs_cfg.lookahead_size, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to get lookahead_size", __func__);
        return HDF_FAILURE;
    }

    if (resource->GetUint32(resourceNode, "block_cycles",
        &fs->lfs_cfg.block_cycles, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to get block_cycles", __func__);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: fs mount_point=%s, block_size=%u, block_count=%u, read_size=%u,\
prog_size=%u, cache_size=%u, lookahead_size=%u, block_cycles=%u\n",\
            __func__, fs->mount_point, fs->lfs_cfg.block_size,\
            fs->lfs_cfg.block_count, fs->lfs_cfg.read_size, fs->lfs_cfg.prog_size,\
            fs->lfs_cfg.cache_size, fs->lfs_cfg.lookahead_size, fs->lfs_cfg.block_cycles);

    return HDF_SUCCESS;
}

static int32_t FsDriverCheck(struct HdfDeviceObject *device)
{
    HDF_LOGI("Fs Driver Init\n");
    if (device == NULL) {
        return HDF_FAILURE;
    }

    if (device->property) {
        if (FsGetResource(&fs, device->property) != HDF_SUCCESS) {
            HDF_LOGE("%s: FsGetResource failed", __func__);
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}

static int32_t HdfLfsDeviceInit(struct HdfDeviceObject *device)
{
    int ret;
    uint32_t device_id;
    DIR *dir;

#if QSPI_W25Q16_LFS
    W25Qxx_InitQspiFlash(1, 0);
    device_id = W25Qxx_QSPI_ReadJedecID();
#else
    W25Qxx_InitSpiFlash(0, 0);
    device_id = W25Qxx_ReadJedecID();
#endif

    HDF_LOGI("%s: [line = %d] [device_id = 0x%x]\r\n", __func__, __LINE__, device_id);

    if (HDF_SUCCESS != FsDriverCheck(device)) {
        return HDF_FAILURE;
    }

    if(device_id != W25Q16_ID) {
        HDF_LOGE("W25Q16 Check Failed!\r\n");
        return HDF_FAILURE;
    }

    fs.lfs_cfg.read = W25Qxx_readLittlefs;
    fs.lfs_cfg.prog = W25Qxx_writeLittlefs;
    fs.lfs_cfg.erase = W25Qxx_eraseLittlefs;
    fs.lfs_cfg.sync = W25Qxx_syncLittlefs;

    ret = mount(NULL, fs.mount_point, "littlefs", 0, &fs.lfs_cfg);
    HDF_LOGI("littlefs_test mount %s\n", (ret == 0) ? "success" : "failed");

    if ((dir = opendir(fs.mount_point)) == NULL) {
        ret = mkdir(fs.mount_point, S_IRWXU | S_IRWXG | S_IRWXO);
        HDF_LOGI("%s:  return(%d) err (%d)\n", __FUNCTION__, ret, errno);
    }

    return HDF_SUCCESS;
}

static int32_t HdfLfsDeviceBind(struct HdfDeviceObject *device)
{
    (void *)device;
    return HDF_SUCCESS;
}

static void HdfLfsDeviceRelease(struct HdfDeviceObject *device)
{
    (void *)device;
    return HDF_SUCCESS;
}

struct fs_cfg_l {
    CHAR *mount_point;
    struct PartitionCfg partCfg;
};

HalLogicPartition g_halPartitions[] = {
    [FLASH_PARTITION_DATA0] = {
        .partitionDescription = "littlefs",
		.partitionStartAddr = 0x20000000 + 0x2C000, //176K start
		.partitionLength = 0x4000, //16K
    },
    [FLASH_PARTITION_DATA1] = {
        .partitionDescription = "vfat",
		.partitionStartAddr = 0x20000000 + 0x2C000, //176K start
		.partitionLength = 0x4000, //16K
    },
};

INT32 lfs_flash_erase(HalPartition pdrv, UINT32 offSet, UINT32 size)
{
    UINT32 startAddr = offSet;
    UINT32 partitionEnd;

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

    UINT32 intSave = LOS_IntLock();
    (VOID)memset_s((VOID *)startAddr, size, 0, size);
    LOS_IntRestore(intSave);
    return 0;
}

INT32 lfs_flash_write(HalPartition pdrv, UINT32 *offSet, const VOID *buf, UINT32 bufLen)
{
    UINT32 startAddr = *offSet;
    UINT32 partitionEnd;

    partitionEnd = g_halPartitions[pdrv].partitionStartAddr + g_halPartitions[pdrv].partitionLength;
    if (startAddr >= partitionEnd) {
        printf("flash over write, 0x%x, 0x%x\r\n", startAddr, partitionEnd);
        return -1;
    }
    if ((startAddr + bufLen) > partitionEnd) {
        bufLen = partitionEnd - startAddr;
        printf("flash over write, new len is %d\r\n", bufLen);
    }

    UINT32 intSave = LOS_IntLock();
    (VOID)memcpy_s((VOID *)startAddr, bufLen, buf, bufLen);
    LOS_IntRestore(intSave);

    return 0;
}

INT32 lfs_flash_erase_write(HalPartition pdrv, UINT32 *offSet, const VOID *buf, UINT32 bufLen)
{
    UINT32 startAddr = *offSet;
    UINT32 partitionEnd;

    partitionEnd = g_halPartitions[pdrv].partitionStartAddr + g_halPartitions[pdrv].partitionLength;
    if (startAddr >= partitionEnd) {
        printf("flash over e&w, 0x%x, 0x%x\r\n", startAddr, partitionEnd);
        return -1;
    }
    if ((startAddr + bufLen) > partitionEnd) {
        bufLen = partitionEnd - startAddr;
        printf("flash over erase&write, new len is %d\r\n", bufLen);
    }

    UINT32 intSave = LOS_IntLock();
    (VOID)memset_s((VOID *)startAddr, bufLen, 0, bufLen);
    (VOID)memcpy_s((VOID *)startAddr, bufLen, buf, bufLen);
    LOS_IntRestore(intSave);
    return 0;
}

INT32 lfs_flash_read(HalPartition pdrv, UINT32 *offSet, VOID *buf, UINT32 bufLen)
{
    UINT32 startAddr = *offSet;
    UINT32 partitionEnd;

    partitionEnd = g_halPartitions[pdrv].partitionStartAddr + g_halPartitions[pdrv].partitionLength;
    if (startAddr >= partitionEnd) {
        printf("flash over read, 0x%x, 0x%x\r\n", startAddr, partitionEnd);
        return -1;
    }
    if ((startAddr + bufLen) > partitionEnd) {
        bufLen = partitionEnd - startAddr;
        printf("flash over read, new len is %d\r\n", bufLen);
    }

    UINT32 intSave = LOS_IntLock();
    (VOID)memcpy_s(buf, bufLen, (VOID *)startAddr, bufLen);
    LOS_IntRestore(intSave);
    return 0;
}

HalLogicPartition *getPartitionInfo(VOID)
{
    return g_halPartitions;
}

INT32 Lfs_Init()
{
    INT32 ret;
    DIR *dir;

    struct fs_cfg_l fs_m[LOSCFG_LFS_MAX_MOUNT_SIZE] = {0};
    HalLogicPartition *halPartitionsInfo = getPartitionInfo();

    INT32 lengthArray[2];
    lengthArray[0]= halPartitionsInfo[FLASH_PARTITION_DATA0].partitionLength;

    INT32 addrArray[2];
    addrArray[0] = halPartitionsInfo[FLASH_PARTITION_DATA0].partitionStartAddr;

    ret = LOS_DiskPartition("flash0", "littlefs", lengthArray, addrArray, 2);
    printf("%s: DiskPartition %s\n", __func__, (ret == 0) ? "succeed" : "failed");
    if (ret != 0) {
        return -1;
    }
    fs_m[0].mount_point = "/littlefs";
    fs_m[0].partCfg.partNo = 0;
    fs_m[0].partCfg.blockSize = 4096; /* 4096, lfs block size */
    fs_m[0].partCfg.blockCount = 4; /* 4, lfs block count */

    fs_m[0].partCfg.readFunc = lfs_flash_read;
    fs_m[0].partCfg.writeFunc = lfs_flash_write;
    fs_m[0].partCfg.eraseFunc = lfs_flash_erase;

    fs_m[0].partCfg.readSize = 256; /* 256, lfs read size */
    fs_m[0].partCfg.writeSize = 256; /* 256, lfs prog size */
    fs_m[0].partCfg.cacheSize = 256; /* 256, lfs cache size */
    fs_m[0].partCfg.lookaheadSize = 16; /* 16, lfs lookahead size */
    fs_m[0].partCfg.blockCycles = 500; /* 500, lfs block cycles */

    ret = LOS_PartitionFormat("flash0", "littlefs", &fs_m[0].partCfg);
    printf("%s: PartitionFormat %s\n", __func__, (ret == 0) ? "succeed" : "failed");
    if (ret != 0) {
        return -1;
    }

    ret = mount(NULL, fs_m[0].mount_point, "littlefs", 0, &fs_m[0].partCfg);
    printf("%s: mount fs on '%s' %s\n", __func__, fs_m[0].mount_point, (ret == 0) ? "succeed" : "failed");
    if (ret != 0) {
        return -1;
    }

    if (!ret) {
        ret = mkdir(fs_m[0].mount_point, S_IRUSR | S_IWUSR | S_IXUSR);
        printf("%s: mkdir %s %s\n", __func__, fs_m[0].mount_point, (ret == 0) ? "succeed" : "failed");
    }

    return 0;
}

struct HdfDriverEntry g_hdfLfsDevice = {
    .moduleVersion = 1,
    .moduleName = "GD32_FS_LITTLEFS",
    .Bind = HdfLfsDeviceBind,
    .Init = HdfLfsDeviceInit,
    .Release = HdfLfsDeviceRelease,
};

HDF_INIT(g_hdfLfsDevice);

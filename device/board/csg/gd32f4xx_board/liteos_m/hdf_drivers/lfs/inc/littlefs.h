#ifndef __LITTLEFS_H
#define __LITTLEFS_H

#include "stdio.h"
#include "lfs.h"
#include "w25qxx_qspi.h"
#include "w25qxx.h"
#include "fcntl.h"
#include <dirent.h>

struct LfsPartition {
  const char *mount_point;      // alloc by DeviceResourceNode, can't be released
  uint32_t start_addr;          // the start address of this mount point（i.e partition startaddr）
  struct lfs_config detail;     // detail configuration of lfs
};


int littlefs_test();
int W25Qxx_readLittlefs(const struct lfs_config *c, lfs_block_t block,
                lfs_off_t off, void *buffer, lfs_size_t size);

int W25Qxx_writeLittlefs(const struct lfs_config *c, lfs_block_t block,
                lfs_off_t off, void *buffer, lfs_size_t size);

int W25Qxx_eraseLittlefs(const struct lfs_config *c, lfs_block_t block,
                lfs_off_t off, void *buffer, lfs_size_t size);

int W25Qxx_syncLittlefs(const struct lfs_config *c, lfs_block_t block,
                lfs_off_t off, void *buffer, lfs_size_t size);

#endif


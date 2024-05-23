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

struct fs_cfg {
    char *mount_point;
    struct PartitionCfg lfs_cfg;
};

static struct fs_cfg fs;

int32_t FsInit(void)
{
    int ret;

    INT32 lengthArray = 0x00800000;
    INT32 addrArray = 4096;
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


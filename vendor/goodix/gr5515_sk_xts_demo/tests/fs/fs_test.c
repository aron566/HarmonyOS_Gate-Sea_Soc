/*
 * Copyright (c) 2021 GOODIX.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define LOG_TAG "FS"
#include "log.h"
#include "ohos_init.h"
#include "cmsis_os2.h"

#define FS_TASK_STACK_SIZE   4096
#define FS_TASK_PRIO         25
#define MS_1000              1000
#define PRINTF_ERR           (-1)

#define LOG_E(fmt, ...) HILOG_ERROR(HILOG_MODULE_APP, fmt, ##__VA_ARGS__)
#define LOG_I(fmt, ...) HILOG_INFO(HILOG_MODULE_APP, fmt, ##__VA_ARGS__)

static void dir_test(const char *path)
{
    DIR *dir;
    struct dirent *dp;
    int ret;
    struct stat st_buf = {0};
    char realpath[128];

    ret = 0;
    if ((dir = opendir(path)) == NULL) {
        LOG_E("opendir %s failed, %s\n", path, strerror(errno));
        return;
    }
    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
            continue;
        }

        ret = snprintf_s(realpath, sizeof(realpath), sizeof(realpath), "%s/%s", path, dp->d_name);
        if (ret == PRINTF_ERR) {
            printf("snprintf_s faiked, err = %d", ret);
            return; // 返回失败
        }

        if (stat(realpath, &st_buf) != 0) {
            LOG_E("can not access %s\n", realpath);
            closedir(dir);
            return;
        }
        if ((st_buf.st_mode & S_IFMT) == S_IFDIR) {
            LOG_E("DIR %s\n", realpath);
        } else {
            LOG_I("FILE %s, %ld bytes\n", realpath, st_buf.st_size);
        }
    }
    closedir(dir);
}

static void read_file(const char *file, bool print_str)
{
    int bytes = 0;
    char buf[513];
    int ret = 0;

    int fd = open(file, O_RDONLY);
    if (fd < 0) {
        LOG_E("open file '%s' failed, %s\r\n", file, strerror(errno));
        return;
    }

    while (1) {
        ret = memset_s(buf, sizeof(buf), 0, sizeof(buf));
        if (ret < 0) {
            return;
        }
        int rc = read(fd, buf, sizeof(buf) - 1);
        if (rc > 0) {
            bytes += rc;
        }

        if (print_str) {
            buf[rc] = '\0';
            LOG_I("%s", buf);
        }

        if (rc < sizeof(buf) - 1) {
            break;
        }
    }
    close(fd);
    LOG_I("read file '%s' total bytes: %d\r\n", file, bytes);
}

static void write_file(const char *file, const char *data)
{
    int fd = open(file, O_RDWR | O_CREAT);
    if (fd < 0) {
        LOG_E("fopen file '%s' failed, %s\r\n", file, strerror(errno));
        return;
    }

    int bytes = write(fd, data, strlen(data));
    close(fd);
    LOG_I("fwrite file '%s' total bytes: %d, %s\r\n", file, bytes, data);
}

void fs_test(void)
{
    dir_test("/data");
    write_file("/data/test.txt", "fs write data test.\n");
    read_file("/data/test.txt", true);
}

static void *FsTestTask(const char *arg)
{
    (void)arg;
    
    printf("FS task.\r\n");

    while (1) {
        fs_test();
        osDelay(MS_1000);
    }
}

void FsTaskEntry(void)
{
    osThreadAttr_t attr;

    attr.name = "FsTestTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = FS_TASK_STACK_SIZE;
    attr.priority = FS_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)FsTestTask, NULL, &attr) == NULL) {
        printf("Failed to create FsTestTask!\n");
    }
}

SYS_RUN(FsTaskEntry);


/******************************************************************************
 * Copyright (c) 2022 Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 * All rights reserved.
 *
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
 *
 *****************************************************************************/

#include <securec.h>

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <hiview_log.h>

#include <hal_file.h>
#include <utils_file.h>

#define RD_WR_FIELD_MASK      0x000f
#define CREAT_EXCL_FIELD_MASK 0x00f0
#define TRUNC_FILED_MASK      0x0f00

#define ADDITIONAL_LEN    2
#define MAX_PATH_LEN      40
#define MAX_OPEN_FILE_NUM 32
#define ROOT_PATH         "/littlefs"
#define DIR_SEPARATOR     "/"

#define SLOT_AVAILABLE -1

#define HAL_ERROR -1

static int FileHandlerArray[MAX_OPEN_FILE_NUM] = {
    SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE,
    SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE,
    SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE,
    SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE,
    SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE, SLOT_AVAILABLE,
};

static int GetAvailableFileHandlerIndex(void)
{
    int i = MAX_OPEN_FILE_NUM;

    for (; i > 0; i--) {
        if (FileHandlerArray[i - 1] == SLOT_AVAILABLE) {
            break;
        }
    }

    return i;
}
static int ConvertFlags(int oflag)
{
    int ret = 0;
    int buffer = 0;

    buffer = (oflag & RD_WR_FIELD_MASK);
    if (buffer == O_RDONLY_FS) {
        ret = O_RDONLY;
    } else if (buffer == O_WRONLY_FS) {
        ret = O_WRONLY;
    } else if (buffer == O_RDWR_FS) {
        ret = O_RDWR;
    }

    buffer = (oflag & CREAT_EXCL_FIELD_MASK);
    if ((buffer & O_CREAT_FS) != 0) {
        ret |= O_CREAT;
    }

    if ((buffer & O_EXCL_FS) != 0) {
        ret |= O_EXCL;
    }

    buffer = (oflag & TRUNC_FILED_MASK);
    if ((buffer & O_TRUNC_FS) != 0) {
        ret |= O_TRUNC;
    }

    if ((buffer & O_APPEND_FS) != 0) {
        ret |= O_APPEND;
    }

    return ret;
}

static char *GetActualFilePath(const char *path)
{
    int len;
    char *file_path = NULL;

    len = strnlen(path, MAX_PATH_LEN);
    if (len >= MAX_PATH_LEN) {
        printf("path is too long!\r\n");
        return NULL;
    }

    len += (strlen(ROOT_PATH) + ADDITIONAL_LEN);
    file_path = (char *)malloc(len);
    if (file_path == NULL) {
        printf("malloc failed!\r\n");
        return NULL;
    }

    strcpy_s(file_path, len, ROOT_PATH);
    strcat_s(file_path, len, DIR_SEPARATOR);
    strcat_s(file_path, len, path);

    return file_path;
}

int HalFileOpen(const char *path, int oflag, int mode)
{
    int index;
    int fd;
    char *file_path;

    index = GetAvailableFileHandlerIndex();
    if (index == 0) {
        HILOG_ERROR(HILOG_MODULE_HIVIEW, "no space available!");
        return HAL_ERROR;
    }

    file_path = GetActualFilePath(path);
    if (file_path == NULL) {
        return HAL_ERROR;
    }

    fd = open(file_path, ConvertFlags(oflag));
    if (fd < 0) {
        HILOG_ERROR(HILOG_MODULE_HIVIEW, "failed to open file : %d", errno);
        free(file_path);
        return HAL_ERROR;
    }

    FileHandlerArray[index - 1] = fd;
    free(file_path);

    return index;
}

int HalFileClose(int fd)
{
    int ret;

    /* make sure fd is within the allowed range, which is 1 to MAX_OPEN_FILE_NUM */
    if ((fd > MAX_OPEN_FILE_NUM) || (fd <= 0)) {
        return HAL_ERROR;
    }

    ret = close(FileHandlerArray[fd - 1]);
    if (ret != 0) {
        return HAL_ERROR;
    }

    FileHandlerArray[fd - 1] = SLOT_AVAILABLE;

    return ret;
}

int HalFileRead(int fd, char *buf, unsigned int len)
{
    /* make sure fd is within the allowed range, which is 1 to MAX_OPEN_FILE_NUM */
    if ((fd > MAX_OPEN_FILE_NUM) || (fd <= 0)) {
        return HAL_ERROR;
    }

    return read(FileHandlerArray[fd - 1], buf, len);
}

int HalFileWrite(int fd, const char *buf, unsigned int len)
{
    /* make sure fd is within the allowed range, which is 1 to MAX_OPEN_FILE_NUM */
    if ((fd > MAX_OPEN_FILE_NUM) || (fd <= 0)) {
        return HAL_ERROR;
    }

    return write(FileHandlerArray[fd - 1], buf, len);
}

int HalFileDelete(const char *path)
{
    char *file_path;
    int ret;

    file_path = GetActualFilePath(path);
    if (file_path == NULL) {
        return HAL_ERROR;
    }

    ret = unlink(file_path);
    free(file_path);

    return ret;
}

int HalFileStat(const char *path, unsigned int *fileSize)
{
    char *file_path;
    struct stat f_info;
    int ret;

    file_path = GetActualFilePath(path);
    if (file_path == NULL) {
        return HAL_ERROR;
    }

    ret = stat(file_path, &f_info);
    *fileSize = f_info.st_size;
    free(file_path);

    return ret;
}

int HalFileSeek(int fd, int offset, unsigned int whence)
{
    int ret = 0;
    struct stat f_info;

    /* make sure fd is within the allowed range, which is 1 to MAX_OPEN_FILE_NUM */
    if ((fd > MAX_OPEN_FILE_NUM) || (fd <= 0)) {
        return HAL_ERROR;
    }

    ret = fstat(FileHandlerArray[fd - 1], &f_info);
    if (ret != 0) {
        return HAL_ERROR;
    }

    if (whence == SEEK_SET_FS) {
        if (offset > f_info.st_size) {
            ret = HAL_ERROR;
        }
    }

    ret = lseek(FileHandlerArray[fd - 1], offset, whence);
    if ((ret > f_info.st_size) || (ret < 0)) {
        return HAL_ERROR;
    }

    return ret;
}

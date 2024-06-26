/**
 *  @file littlefs_test.c
 *  @brief 文件系统测试.
 *  @author aron566 (wei.chen@gate-sea.com)
 *  @version v0.0.1 aron566 2024.05.27 20:53 初始版本.
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
#include <stdio.h>
/* Private includes ----------------------------------------------------------*/
#include "hdf_log.h"
// #include "spi_if.h"
#include "cmsis_os2.h"
#include "los_fs.h"
#include "fcntl.h"
// #include "littlefs_port.h"
#include <sys/stat.h>
#include "los_task.h"
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus /**< use C compiler */
extern "C" {
#endif
/** Private macros -----------------------------------------------------------*/

#define HDF_LFS_STACK_SIZE    0x1000
#define HDF_LFS_TASK_NAME     "hdf_littlefs_test_task"
#define HDF_LFS_TASK_PRIORITY 29

#define TEST_FILE_DIR "/littlefs/test.cfg"
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

static void read_test(const char *file, char *buf, int size)
{
  int fd = open(file, O_RDWR);
  if (fd < 0)
  {
    HDF_LOGI("%s open file '%s' failed, \r\n", __func__, file);
    return;
  }

  int rc = read(fd, buf, size);
  close(fd);
}

static void write_test(const char *file, const char *data, int size)
{
  int fd = open(file, O_RDWR | O_CREAT);
  if (fd < 0)
  {
    printf("open file '%s' failed \r\n", file);
    return;
  }

  int bytes = write(fd, data, size);
  close(fd);
  printf("write file '%s' total bytes: %d\r\n", file, bytes);
  printf("%s ok \n", __func__);
}

static void ConstructTestBuffer(char *buffer, int size)
{
  HDF_LOGI("Contruct (%d) >>>>>>>>", size);
  for (int idx = 0; idx < size; idx++)
  {
    buffer[idx] = '0' + idx % 10;
    printf("%c", buffer[idx]);
  }
  printf("\r\n");
  HDF_LOGI("%s buffer constructed\n");
}

static uint8_t BufferCmp(uint8_t *pBuffer1, uint8_t *pBuffer2, int BufferLength)
{
  int count = 0;

  HDF_LOGI("%s: start to buffer compare length: %d\n", __func__, BufferLength);

  while (BufferLength--)
  {
    printf("[%c/%c]", *pBuffer1, *pBuffer2);

    if (*pBuffer1 != *pBuffer2)
    {
      HDF_LOGI("%s: compare FOUND the different @ %d\n", __func__, count);
      return 1;
    }
    count++;
    pBuffer1++;
    pBuffer2++;
  }
  printf("\r\n");
  HDF_LOGI("%s: compare ok\n", __func__);
  return 0;
}

static void *HdfSpiTestEntry(void *arg)
{
  (void)arg;

  uint8_t *txBuffer = NULL;
  uint8_t *rxBuffer = NULL;

  // test case1: data size is 520, if the file is exist, positive, or negative
  txBuffer            = (uint8_t *)OsalMemCalloc(520);
  rxBuffer            = (uint8_t *)OsalMemCalloc(520);
  uint8_t *txBuffer_f = txBuffer;
  uint8_t *rxBuffer_f = rxBuffer;
  ConstructTestBuffer(txBuffer, 520);
  read_test(TEST_FILE_DIR, rxBuffer, 520);
  BufferCmp(txBuffer, rxBuffer, 520);
  OsalMemFree(txBuffer_f);
  OsalMemFree(rxBuffer_f);

  // test case2: data size is 1045, result is positive
  txBuffer   = (uint8_t *)OsalMemCalloc(1045);
  rxBuffer   = (uint8_t *)OsalMemCalloc(1045);
  txBuffer_f = txBuffer;
  rxBuffer_f = rxBuffer;
  ConstructTestBuffer(txBuffer, 1045);
  write_test(TEST_FILE_DIR, txBuffer, 1045);
  read_test(TEST_FILE_DIR, rxBuffer, 1045);
  BufferCmp(txBuffer, rxBuffer, 1045);
  OsalMemFree(txBuffer_f);
  OsalMemFree(rxBuffer_f);

  // test case3: data size is 4356, result is positive
  txBuffer   = (uint8_t *)OsalMemCalloc(4356);
  rxBuffer   = (uint8_t *)OsalMemCalloc(4356);
  txBuffer_f = txBuffer;
  rxBuffer_f = rxBuffer;
  ConstructTestBuffer(txBuffer, 4356);
  write_test(TEST_FILE_DIR, txBuffer, 4356);
  read_test(TEST_FILE_DIR, rxBuffer, 4356);
  BufferCmp(txBuffer, rxBuffer, 4356);
  OsalMemFree(txBuffer_f);
  OsalMemFree(rxBuffer_f);

  return NULL;
}

/** Public application code --------------------------------------------------*/
/*******************************************************************************
 *
 *       Public code
 *
 ********************************************************************************
 */

void StartHdfSpiLittlefsTest(void)
{
  osThreadAttr_t attr;

  attr.name       = HDF_LFS_TASK_NAME;
  attr.attr_bits  = 0U;
  attr.cb_mem     = NULL;
  attr.cb_size    = 0U;
  attr.stack_mem  = NULL;
  attr.stack_size = HDF_LFS_STACK_SIZE;
  attr.priority   = HDF_LFS_TASK_PRIORITY;
  HDF_LOGI("%s: Start ...\r\n", __func__);
  if (osThreadNew((osThreadFunc_t)HdfSpiTestEntry, NULL, &attr) == NULL)
  {
    HDF_LOGE("Failed to create thread1!\n");
  }
}

#ifdef __cplusplus /**< end extern c */
}
#endif
/******************************** End of file *********************************/

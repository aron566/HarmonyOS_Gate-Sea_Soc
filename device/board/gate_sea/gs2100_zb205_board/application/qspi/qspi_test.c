#include "cmsis_os2.h"
#include "main.h"
#include "w25qxx_qspi.h"
#include "los_task.h"

#define HDF_LOG_TAG   qspi_test

#define HDF_QSPI_STACK_SIZE 0x1000
#define HDF_QSPI_TASK_NAME "hdf_qspi_test_task"
#define HDF_QSPI_TASK_PRIORITY 25

static uint8_t BufferCmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
    int count = 0;
    while (BufferLength--) {
        if (*pBuffer1 != *pBuffer2) {
            HDF_LOGI("%s: the different position @ %d\n", __func__, count);
            return 0;
        }
        count++;
        pBuffer1++;
        pBuffer2++;
    }
    return 1;
}

uint8_t *qspi_txBuffer = NULL;
uint8_t *qspi_rxBuffer = NULL;

static void constructTestCase(char *tag, int size)
{
    HDF_LOGI("%s with size(%d) >>>>>>>>", tag, size);
    qspi_txBuffer = (uint8_t*)OsalMemCalloc(size);
    qspi_rxBuffer = (uint8_t*)OsalMemCalloc(size);
    for (int idx = 0;idx < size; idx++) {
        qspi_txBuffer[idx] = '0' + idx % 10;
        printf("%c", qspi_txBuffer[idx]);
    }
    printf("\n");
    HDF_LOGI("tick(%d), %s start\n", LOS_TickCountGet(), tag);
}

static void deConstructTestCase(char *tag)
{
    OsalMemFree(qspi_txBuffer);
    qspi_txBuffer = NULL;
    OsalMemFree(qspi_rxBuffer);
    qspi_rxBuffer = NULL;
    HDF_LOGI("tick(%d), %s end\n", LOS_TickCountGet(), tag);
}

static void TestWriteThenRead(uint8_t *tx,uint8_t *rx, uint32_t addr, uint32_t bufferSize)
{
    W25Qxx_QSPI_Write_NoCheck(tx, addr, bufferSize);
    W25Qxx_QSPI_Read(rx, addr, bufferSize);

    if (BufferCmp(tx, rx, bufferSize)) {
        HDF_LOGI("hdf qspi write read flash @ %d success\n", addr);
        for (int idx = 0;idx < bufferSize; idx++) {
            printf("%c", rx[idx]);
        }
    } else {
        HDF_LOGE("hdf qspi write read flash @ %d failed\n rx ===>>>", addr);
        for (int idx = 0;idx < bufferSize; idx++) {
            printf("%c", rx[idx]);
        }
        printf("\n<<<===\n");
    }
}

/*
NOTE: this demo shows how to access the w25qxx without File system
    BUT run it either or run littleFs test, to avoid QSPI-flash conflict
*/
static void HdfQspiTestEntry(void* arg)
{
    (void)arg;

    W25Qxx_InitQspiFlash(1, 0);

    HDF_LOGI("tick(%d), read id is 0x%02x\n", LOS_TickCountGet(), W25Qxx_QSPI_ReadJedecID());

    // erace the 2nd sector, then try to write at around
    W25Qxx_QSPI_EraseSector(4096);
    HDF_LOGI("tick(%d), W25Qxx_QSPI_EraseSector( \n", LOS_TickCountGet());
    // case0: smoking test within one sector
    constructTestCase("Test case 0:  write addr(4096) ", 100);
    TestWriteThenRead(qspi_txBuffer, qspi_rxBuffer, 4096, 100);
    // negative case0: write overlay, should fail log
    deConstructTestCase("Test case 0");
    constructTestCase("Test case 0:  write addr(4096) ", 100);
    TestWriteThenRead(qspi_txBuffer, qspi_rxBuffer, 4000, 100);
    deConstructTestCase("Test case 1");

    // clean the whole
    W25Qxx_QSPI_EraseWholeChip();
    // case1: within one sector
    constructTestCase("Test case 1: in one page write addr(10) ", 100);
    TestWriteThenRead(qspi_txBuffer, qspi_rxBuffer, 10, 100);
    deConstructTestCase("Test case 2");

    // case2: from sector 2~3
    constructTestCase("Test case 2: cross 2 pages write addr(510) ", 100);
    TestWriteThenRead(qspi_txBuffer, qspi_rxBuffer, 510, 100);
    deConstructTestCase("Test case 3");

    // case3: from sector 4~7
    constructTestCase("Test case 3: cross 4 pages write addr(1000) ", 1000);
    TestWriteThenRead(qspi_txBuffer, qspi_rxBuffer, 1000, 1000);
    deConstructTestCase("Test case 4");

    // clean dirty data in qspi-flash
    W25Qxx_QSPI_EraseWholeChip();

    W25Qxx_DeInitQspiFlash();
}

void StartHdfQspiTest(void)
{
    osThreadAttr_t attr;

    attr.name = HDF_QSPI_TASK_NAME;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = HDF_QSPI_STACK_SIZE;
    attr.priority = HDF_QSPI_TASK_PRIORITY;

    if (osThreadNew((osThreadFunc_t)HdfQspiTestEntry, NULL, &attr) == NULL) {
        HDF_LOGE("Failed to create thread1!\n");
    }
}

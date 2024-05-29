#include "cmsis_os2.h"
#include "main.h"
#include "w25qxx.h"
#include "los_task.h"

#define HDF_LOG_TAG   spi_test

#define HDF_SPI_STACK_SIZE 0x1000
#define HDF_SPI_TASK_NAME "hdf_spi_test_task"
#define HDF_SPI_TASK_PRIORITY 23

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

uint8_t *spi_txBuffer = NULL;
uint8_t *spi_rxBuffer = NULL;

static void constructTestCase(char *tag, int size)
{
    HDF_LOGI("%s with size(%d) >>>>>>>>", tag, size);
    spi_txBuffer = (uint8_t*)OsalMemCalloc(size);
    spi_rxBuffer = (uint8_t*)OsalMemCalloc(size);
    for (int idx = 0;idx < size; idx++) {
        spi_txBuffer[idx] = '0' + idx % 10;
        printf("%c", spi_txBuffer[idx]);
    }
    printf("\n");
    HDF_LOGI("tick(%d), %s start\n", LOS_TickCountGet(), tag);
}

static void deConstructTestCase(char *tag)
{
    OsalMemFree(spi_txBuffer);
    spi_txBuffer = NULL;
    OsalMemFree(spi_rxBuffer);
    spi_rxBuffer = NULL;
    HDF_LOGI("tick(%d), %s end\n", LOS_TickCountGet(), tag);
}

static void TestWriteThenRead(uint8_t *tx,uint8_t *rx, uint32_t addr, uint32_t bufferSize)
{
    W25Qxx_Write_NoCheck(tx, addr, bufferSize);
    W25Qxx_Read(rx, addr, bufferSize);

    if (BufferCmp(tx, rx, bufferSize)) {
        HDF_LOGI("hdf spi write read flash @ %x success\n", addr);
        for (int idx = 0;idx < bufferSize; idx++) {
            printf("%c", rx[idx]);
        }
    } else {
        HDF_LOGE("hdf spi write read flash @ %x failed\n rx ===>>>", addr);
        for (int idx = 0;idx < bufferSize; idx++) {
            printf("%c", rx[idx]);
        }
        printf("\n<<<===\n");
    }
}


/*
NOTE: this demo shows how to access the w25qxx without File system
    BUT run it either or run littleFs test, to avoid SPI-flash conflict
*/
static void HdfSpiTestEntry(void* arg)
{
    (void)arg;
    HDF_LOGI("%s: %d", __func__, __LINE__);

    W25Qxx_InitSpiFlash(0, 0);

    HDF_LOGI("tick(%d), read id is 0x%02x\n", LOS_TickCountGet(), W25Qxx_ReadJedecID());

    // erace the 2nd sector, then try to write at around
    W25Qxx_EraseSector(4096);
    HDF_LOGI("tick(%d), W25Qxx_EraseSector \n", LOS_TickCountGet());
    // case0: smoking test within one sector
    constructTestCase("Test case 0:  write addr(4096) ", 100);
    TestWriteThenRead(spi_txBuffer, spi_rxBuffer, 4096, 100);
    // negative case0: write overlay, should fail log
    TestWriteThenRead(spi_txBuffer, spi_rxBuffer, 4000, 96);
    deConstructTestCase("Test case 0");

    // clean the whole
    W25Qxx_EraseWholeChip();
    // case1: within one sector
    constructTestCase("Test case 1: in one page write addr(10) ", 100);
    TestWriteThenRead(spi_txBuffer, spi_rxBuffer, 10, 100);
    deConstructTestCase("Test case 1");

    // case1: from sector 2~3
    constructTestCase("Test case 2: cross 2 pages write addr(510) ", 100);
    TestWriteThenRead(spi_txBuffer, spi_rxBuffer, 510, 100);
    deConstructTestCase("Test case 2");

    // case1: from sector 4~7
    constructTestCase("Test case 3: cross 4 pages write addr(1000) ", 600);
    TestWriteThenRead(spi_txBuffer, spi_rxBuffer, 1000, 600);
    deConstructTestCase("Test case 3");

    // clean dirty data in spi-flash
    W25Qxx_EraseWholeChip();

    W25Qxx_DeInitSpiFlash();
}

void StartHdfSpiTest(void)
{
    osThreadAttr_t attr;

    attr.name = HDF_SPI_TASK_NAME;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = HDF_SPI_STACK_SIZE;
    attr.priority = HDF_SPI_TASK_PRIORITY;

    if (osThreadNew((osThreadFunc_t)HdfSpiTestEntry, NULL, &attr) == NULL) {
        HDF_LOGE("Failed to create thread1!\n");
    }
}

#include "w25qxx.h"

#define HDF_LOG_TAG      w25qxx_log

#define W25Qxx_PageSize            256
#define BUSY_ST                    0x01
#define BUSY_DETECT_DELAY          1000
#define Dummy_Byte                 0xFF

// cmd of w25qxx
#define W25Q_WriteEnable           0x06
#define W25Q_WriteDisable          0x04
#define W25Q_ReadStatusReg         0x05
#define W25Q_WriteStatusReg        0x01

#define W25Q_ReadData              0x03
#define W25Q_PageProgram           0x02

#define W25Q_FastReadData          0x0B
#define W25Q_FastReadDual          0x3B

#define W25Q_BlockErase            0xD8
#define W25Q_SectorErase           0x20
#define W25Q_ChipErase             0xC7

#define W25Q_PowerDown             0xB9
#define W25Q_ReleasePowerDown      0xAB
#define W25Q_DeviceID              0xAB
#define W25Q_ManufactDeviceID      0x90
#define W25Q_JedecDeviceID         0x9F

#define DEBUG_SPI_FLASH            0
// singlton spiHandle, so defined a global var
static DevHandle spiHandle = NULL;

// to simplify the common judgement for all API
#define CHECK_SPI_HANDLE(spi) \
    do {\
        if (spi == NULL) { \
            HDF_LOGE("%s: spi handler hasn't been inited", __func__); \
            return; \
        } \
    }while(0);\

// must enable before write or erase
static void W25QxxWriteEnable(void)
{
    uint8_t wbuf[1] = {W25Q_WriteEnable};
    uint8_t rbuf[1] = {0};
    struct SpiMsg msg = {0};
    msg.wbuf = wbuf;
    msg.rbuf = rbuf;
    msg.len = sizeof(wbuf);
    msg.keepCs = 1;
    msg.delayUs = 0;
    int32_t ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("%s: SpiTransfer: failed, ret(%d)", __func__, ret);
    }

    return;
}

// wait for readStatus BUSY flag = 0
static uint8_t W25QxxReadStatusReg1(uint16_t delay) {
    uint8_t wbuf[2] = {W25Q_ReadStatusReg, Dummy_Byte};
    uint8_t rbuf[2] = {0};
    struct SpiMsg msg = {0};
    msg.wbuf = wbuf;
    msg.rbuf = rbuf;
    msg.len = sizeof(wbuf);
    msg.keepCs = 1;
    msg.delayUs = delay;

    int32_t ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("%s: SpiTransfer: failed, ret(%d)", __func__, ret);
    }
    return rbuf[1];
}

static void W25QxxWaitForWriteEnd(void)
{
    uint8_t status = 0;
    uint32_t count = 0;
    uint32_t countLimit = (1000 * 1000) / BUSY_DETECT_DELAY; // count max is 1 second
    uint32_t tickStart = LOS_TickCountGet();

    do {
        status = W25QxxReadStatusReg1(BUSY_DETECT_DELAY);
#if DEBUG_SPI_FLASH
        if ((count++ % countLimit) == 0) {
            HDF_LOGD("%s: elapse tick(%d)", __func__, LOS_TickCountGet() - tickStart);
        }
#endif
    } while ((status & BUSY_ST) == 1);
#if DEBUG_SPI_FLASH
    HDF_LOGD("%s: ==> status(%d)", __func__, status);
#endif
}

/*
 * Write one page(256B)
 * @Note: MUST wrap within writeEnable / waitForWriteEnd
 *        or cross page writing will fail
 * @param pBuffer to to write
 * @param WriteAddr address within 24bit
 * @param NumByteToWrite number in scope [1,W25Qxx_PageSize]
 */
static void W25QxxWritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
    W25QxxWriteEnable();

    uint8_t wbuf[4] = {
        W25Q_PageProgram,
        (WriteAddr & 0xff0000) >> 16,
        (WriteAddr & 0xff00) >> 8,
        (WriteAddr & 0xff)
    };
    uint8_t rbuf[4] = {0};
    struct SpiMsg msg = {0};
    int32_t ret = 0;

    msg.wbuf = wbuf;
    msg.rbuf = rbuf;
    msg.len = sizeof(wbuf);
    msg.keepCs = 0;
    msg.delayUs = 0;
    ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("%s: SpiTransfer: failed, ret(%d)", __func__, ret);
    }

    if (NumByteToWrite > W25Qxx_PageSize) {
        NumByteToWrite = W25Qxx_PageSize;
        HDF_LOGE("%s: numByteToWrite is too large! force to %d", __func__, W25Qxx_PageSize);
    }

    uint8_t *rbuf1 = (uint8_t*)OsalMemCalloc(NumByteToWrite * sizeof(uint8_t));
    if (rbuf1 == NULL) {
        HDF_LOGE("%s: malloc failed", __func__);
        return;
    }

    memset_s(rbuf1, NumByteToWrite, 0, NumByteToWrite);
	
	uint8_t *rbuf2 = rbuf1;
	
    msg.wbuf = pBuffer;
    msg.rbuf = rbuf1;
    msg.len = NumByteToWrite;
    // The CS pin must be driven high after the eighth bit of the last byte has been latched.
    msg.keepCs = 1;
    msg.delayUs = 0;
    ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("%s: SpiTransfer: failed, ret(%d)", __func__, ret);
    }
    //OsalMemFree(rbuf1);
	OsalMemFree(rbuf2);

    W25QxxWaitForWriteEnd();
    return;
}

/**
 * init the spi
 * @param busNum  the bus num specified in spi_config.hcs
 * @param csNum   the cs num specified in spi_config.hcs
 * @note  only one spi node is supported in liteos_m
 */
HDF_STATUS W25Qxx_InitSpiFlash(uint32_t busNum, uint32_t csNum)
{
    struct SpiDevInfo spiDevinfo;
    spiDevinfo.busNum = busNum;
    spiDevinfo.csNum = csNum;
    spiHandle = SpiOpen(&spiDevinfo);
    if (spiHandle == NULL) {
        HDF_LOGE("%s: SpiOpen failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: success(%x)", __func__, spiHandle);
    return HDF_SUCCESS;
}

/**
 * deinit spi
 */
HDF_STATUS W25Qxx_DeInitSpiFlash(void)
{
    if (spiHandle != NULL) {
        SpiClose(spiHandle);
        HDF_LOGI("%s: success(%x)", __func__, spiHandle);
        spiHandle = NULL;
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s: SpiClose failed", __func__);
    return HDF_FAILURE;
}

/**
 * erase sector(4KB)
 * @param SectorAddr Sector Address （multiple of 4K）
 */
void W25Qxx_EraseSector(uint32_t SectorAddr)
{
    CHECK_SPI_HANDLE(spiHandle);

    W25QxxWriteEnable();
    uint8_t wbuf[4] = {
        W25Q_SectorErase,
        (SectorAddr & 0xff0000) >> 16,
        (SectorAddr & 0xff00) >> 8,
        (SectorAddr & 0xff)
    };
    uint8_t rbuf[4] = {0};
    struct SpiMsg msg = {0};

    msg.wbuf = wbuf;
    msg.rbuf = rbuf;
    msg.len = sizeof(wbuf);
    msg.keepCs = 1;
    msg.delayUs = 0;
    int32_t ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("%s: SpiTransfer: failed, ret(%d)", __func__, ret);
        return;
    }

    W25QxxWaitForWriteEnd();
}

/**
 * erase the whole chip
*/
void W25Qxx_EraseWholeChip(void)
{
    CHECK_SPI_HANDLE(spiHandle);

    HDF_LOGI(" ======= CAUTION: it may costs a long time to erase whole chip, be patient =======");

    W25QxxWriteEnable();
    uint8_t wbuf[1] = {W25Q_ChipErase};
    uint8_t rbuf[1] = {0};
    struct SpiMsg msg = {0};
    msg.wbuf = wbuf;
    msg.rbuf = rbuf;
    msg.len = sizeof(wbuf);
    msg.keepCs = 1;
    msg.delayUs = 0;
    int32_t ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("%s: SpiTransfer: failed, ret(%d)", __func__, ret);
        return;
    }

    W25QxxWaitForWriteEnd();
    return;
}

/**
 * split the buffer into slices in unit of page
 * then write them to flash one by one
 * @param pBuffer source buffer to be written
 * @param WriteAddr the destine address of the spi flash
 * @param NumByteToWrite size of the pBuffer
 * @note enure erased before write
 */
void W25Qxx_Write_NoCheck(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
    CHECK_SPI_HANDLE(spiHandle);

    uint32_t pageRemain;

    pageRemain = W25Qxx_PageSize - WriteAddr % W25Qxx_PageSize; // remain size to be written in a page

    if (NumByteToWrite < pageRemain) {
        pageRemain = NumByteToWrite; // maybe written bufer is smaller than one page
    }

    do {
#if DEBUG_SPI_FLASH
        HDF_LOGD("%s: W25QxxWritePage(%x, %x, %d) NumByteToWrite=%d",
                   __func__, pBuffer, WriteAddr, pageRemain, NumByteToWrite);
#endif
        W25QxxWritePage(pBuffer, WriteAddr, pageRemain);

        pBuffer += pageRemain;
        WriteAddr += pageRemain;
        NumByteToWrite -= pageRemain;
        if (NumByteToWrite > W25Qxx_PageSize) {
            pageRemain = W25Qxx_PageSize;
        } else {
            pageRemain = NumByteToWrite;
        }
    } while(NumByteToWrite != 0);

}

/**
 * read the flash to fill the pBuffer
 * @param pBuffer dest buffer to be filled
 * @param ReadAddr address to be read
 * @param NumByteToRead pBuffer size
*/
void W25Qxx_Read(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
    CHECK_SPI_HANDLE(spiHandle);

    int32_t ret = 0;
    uint8_t wbuf[4] = {
        W25Q_ReadData,
        (ReadAddr & 0xFF0000) >> 16,
        (ReadAddr& 0xFF00) >> 8,
        ReadAddr & 0xFF
    };
    uint8_t rbuf[4] = {0};
    struct SpiMsg msg = {0};

    msg.wbuf = wbuf;
    msg.rbuf = rbuf;
    msg.len = sizeof(wbuf);
    msg.keepCs = 0;
    msg.delayUs = 0;
    ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("%s: SpiTransfer: failed, ret(%d)", __func__, ret);
        return;
    }

    uint8_t *wbuf1 = (uint8_t*)OsalMemCalloc(NumByteToRead * sizeof(uint8_t));
    if (wbuf1 == NULL) {
        HDF_LOGE("%s: malloc failed", __func__);
        return;
    }
    memset_s(wbuf1, NumByteToRead, Dummy_Byte, NumByteToRead);
	
	uint8_t *wbuf2 = wbuf1;
	
    msg.wbuf = wbuf1;
    msg.rbuf = pBuffer;
    msg.len = NumByteToRead;
    msg.keepCs = 1;
    msg.delayUs = 0;
    ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("%s: SpiTransfer: failed, ret(%d)", __func__, ret);
    }

    //OsalMemFree(wbuf1);
	OsalMemFree(wbuf2);
	
    return;
}

/**
 * read JEDEC ID
 * @return e.g 0xef4018 for w25q128
*/
uint32_t W25Qxx_ReadJedecID(void)
{
    if (spiHandle == NULL) {
        HDF_LOGE("%s: spi handler hasn't been inited", __func__);
        return 0;
    }

    int32_t ret = 0;
    uint32_t flashId = 0;
    uint8_t rbuff1[4] = { 0 };
    uint8_t wbuff1[4] = { W25Q_JedecDeviceID, 0, 0, 0 };
    struct SpiMsg msg1 = {0};
    msg1.wbuf = wbuff1;
    msg1.rbuf = rbuff1;
    msg1.len = sizeof(wbuff1);
    msg1.keepCs = 1;
    msg1.delayUs = 0;
    ret = SpiTransfer(spiHandle, &msg1, 1);
    if (ret != 0) {
        HDF_LOGE("%s: SpiTransfer: failed, ret(%d)", __func__, ret);
    } else {
        flashId = msg1.rbuf[1]<<16 | msg1.rbuf[2]<<8 | msg1.rbuf[3];
    }

    return flashId;
}


void W25Qxx_PowerDown(void)
{
    CHECK_SPI_HANDLE(spiHandle);

    uint8_t wbuf[1] = {W25Q_PowerDown};
    uint8_t rbuf[1] = {0};
    struct SpiMsg msg = {0};
    msg.wbuf = wbuf;
    msg.rbuf = rbuf;
    msg.len = sizeof(wbuf);
    msg.keepCs = 1;
    msg.delayUs = 0;
    int32_t ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("%s: SpiTransfer: failed, ret(%d)", __func__, ret);
    }

    return;
}

void W25Qxx_WAKEUP(void)
{
    CHECK_SPI_HANDLE(spiHandle);

    uint8_t wbuf[1] = {W25Q_ReleasePowerDown};
    uint8_t rbuf[1] = {0};
    struct SpiMsg msg = {0};
    msg.wbuf = wbuf;
    msg.rbuf = rbuf;
    msg.len = sizeof(wbuf);
    msg.keepCs = 1;
    msg.delayUs = 0;
    int32_t ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("%s: SpiTransfer: failed, ret(%d)", __func__, ret);
    }

    return;
}

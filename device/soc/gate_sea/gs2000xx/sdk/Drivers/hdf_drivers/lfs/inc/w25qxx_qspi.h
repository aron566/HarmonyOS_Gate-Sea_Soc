
#ifndef __W25Qxx_QSPI_H__
#define __W25Qxx_QSPI_H__

#include <stdio.h>
#include "hdf_log.h"
#include "osal_mem.h"
#include "spi_if.h"


extern HDF_STATUS W25Qxx_InitQspiFlash(uint32_t busNum, uint32_t csNum);
extern HDF_STATUS W25Qxx_DeInitQspiFlash(void);

extern void W25Qxx_QSPI_EraseSector(uint32_t SectorAddr);
extern void W25Qxx_QSPI_EraseWholeChip(void);

extern void W25Qxx_QSPI_Write_NoCheck(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite);
extern void W25Qxx_QSPI_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);
//extern void W25Qxx_Read_enable(uint8_t* pBuffer, uint32_t ReadAddr);
extern uint32_t W25Qxx_QSPI_ReadJedecID(void);

extern void W25Qxx_Qspi_PowerDown(void);
extern void W25Qxx_Qspi_WAKEUP(void);

#endif /* __W25Qxx_H__ */

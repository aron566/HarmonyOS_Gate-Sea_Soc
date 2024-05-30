
#ifndef __W25Qxx_H__
#define __W25Qxx_H__

#include <stdio.h>
#include "hdf_log.h"
#include "osal_mem.h"
#include "spi_if.h"

extern HDF_STATUS W25Qxx_InitSpiFlash(uint32_t busNum, uint32_t csNum);
extern HDF_STATUS W25Qxx_DeInitSpiFlash(void);

extern void W25Qxx_EraseSector(uint32_t SectorAddr);
extern void W25Qxx_EraseWholeChip(void);

extern void W25Qxx_Write_NoCheck(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite);
extern void W25Qxx_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);
extern uint32_t W25Qxx_ReadJedecID(void);

extern void W25Qxx_PowerDown(void);
extern void W25Qxx_WAKEUP(void);

#endif /* __W25Qxx_H__ */

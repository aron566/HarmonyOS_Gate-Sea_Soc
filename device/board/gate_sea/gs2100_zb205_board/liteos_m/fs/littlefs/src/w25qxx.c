#include "w25qxx.h"
#include "qspi.h"

uint32_t W25QXX_TYPE = W25Q128;

void W25QXX_Init(void)
{
    QSPI_Init();
    W25QXX_TYPE = W25QXX_ReadID();
}

uint8_t W25QXX_ReadSR(uint8_t regno)
{
    uint8_t byte = 0, command = 0;

    switch(regno)
    {
        case 1:
            command = W25X_ReadStatusReg1;
            break;

        case 2:
            command = W25X_ReadStatusReg2;
            break;

        case 3:
            command = W25X_ReadStatusReg3;
            break;

        default:
            command = W25X_ReadStatusReg1;
            break;
    }

    QSPI_Send_CMD(command, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_1_LINE);
    QSPI_Receive(&byte, 1);
    return byte;
}

void W25QXX_Write_Enable(void)
{
    QSPI_Send_CMD(W25X_WriteEnable, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);
}

void W25QXX_Write_Disable(void)
{
    QSPI_Send_CMD(W25X_WriteDisable, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);
}

uint32_t W25QXX_ReadID(void)
{
    uint8_t temp[2];
    uint16_t deviceid;
    QSPI_Send_CMD(W25X_ManufactDeviceID, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);
    QSPI_Receive(temp, 2);
    deviceid = (temp[0] << 8) | temp[1];
    return deviceid;
}

void W25QXX_Read(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
    //printf("hihope W25QXX_Read enter\n");
    //printf("hihope ReadAddr = %lu, NumByteToRead = %lu\n", ReadAddr, NumByteToRead);
    QSPI_Send_CMD(W25X_FastReadData, ReadAddr, 8, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);
    //printf("hihope W25QXX_Read QSPI_Send_CMD ok\n");
    QSPI_Receive(pBuffer, NumByteToRead);
    //printf("hihope W25QXX_Read ok\n");
}

void W25QXX_Write_Page(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
    W25QXX_Write_Enable();
    QSPI_Send_CMD(W25X_PageProgram, WriteAddr, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);
    QSPI_Transmit(pBuffer, NumByteToWrite);
    W25QXX_Wait_Busy();
}

void W25QXX_Write_NoCheck(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
    uint32_t pageremain;
    pageremain = 256 - WriteAddr % 256;

    if(NumByteToWrite <= pageremain)pageremain = NumByteToWrite; 

    while(1)
    {
        W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);

        if(NumByteToWrite == pageremain)break;

        else 
        {
            pBuffer += pageremain;
            WriteAddr += pageremain;

            NumByteToWrite -= pageremain;

            if(NumByteToWrite > 256)pageremain = 256;

            else pageremain = NumByteToWrite;
        }
    }
}

uint8_t W25QXX_BUFFER[4096];
void W25QXX_Write(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    uint8_t * W25QXX_BUF;
    W25QXX_BUF = W25QXX_BUFFER;
    secpos = WriteAddr / 4096;
    secoff = WriteAddr % 4096;
    secremain = 4096 - secoff;

    //printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);
    if(NumByteToWrite <= secremain)secremain = NumByteToWrite;

    while(1)
    {
        W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096); 

        for(i = 0; i < secremain; i++)
        {
            if(W25QXX_BUF[secoff + i] != 0XFF)break;
        }

        if(i < secremain)
        {
            W25QXX_Erase_Sector(secpos);

            for(i = 0; i < secremain; i++)
            {
                W25QXX_BUF[i + secoff] = pBuffer[i];
            }

            W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096);

        }

        else W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain);

        if(NumByteToWrite == secremain)break;

        else
        {
            secpos++;
            secoff = 0; 

            pBuffer += secremain;
            WriteAddr += secremain; 
            NumByteToWrite -= secremain;

            if(NumByteToWrite > 4096)secremain = 4096;

            else secremain = NumByteToWrite;
        }
    };
}

void W25QXX_Erase_Sector(uint32_t Dst_Addr)
{
    //Dst_Addr *= 4096;
    W25QXX_Write_Enable();
    W25QXX_Wait_Busy();
    QSPI_Send_CMD(W25X_SectorErase, Dst_Addr, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_NONE);
    W25QXX_Wait_Busy();
}


void W25QXX_Wait_Busy(void)
{
    while((W25QXX_ReadSR(1) & 0x01) == 0x01);
}

/*
*
*/
#include  <gsmcu_hal.h>
#include "bps_flash.h"
#include "string.h"
#include "gsmcu_flash.h"
#include "ohos_init.h"
#include "ohos_types.h"
#include "los_task.h"
bool DiagnosticDataFlashAddr(uint32_t start_addr, uint32_t dlen)
{
	if((start_addr < FLASH_START_ADDR) ||
		((start_addr + dlen) > FLASH_START_ADDR + MAX_DATA_FLASH_SIZE))
	{
		return false;
	}
	return true;
}

void EraseDataFlash(uint32_t start_addr, uint32_t dlen)
{
	if ((start_addr&0xffffff)==0)
	{
		return;
	}
	uint32_t head_section, tail_section;
	uint32_t section_num;
	uint32_t i;
	uint32_t addr_idx;
	head_section = start_addr % DATA_FLASH_SECTION_SIZE;
    head_section = DATA_FLASH_SECTION_SIZE - head_section;
	if(dlen > head_section)
	{
		if(0 != head_section)
		{
			FLASH_EarseBlock(start_addr);
		}
		addr_idx = start_addr + head_section;
		dlen -= head_section;
		section_num = dlen / DATA_FLASH_SECTION_SIZE;
		tail_section = dlen % DATA_FLASH_SECTION_SIZE;
		for(i = 0; i < section_num; i++)
		{
			FLASH_EarseBlock(addr_idx);
			addr_idx += DATA_FLASH_SECTION_SIZE;
		}
		if(0 != tail_section)
		{
			FLASH_EarseBlock(addr_idx);
		}
	}
	else
	{
		FLASH_EarseBlock(start_addr);
	}
}

void WriteDataFlash(uint32_t start_addr, uint8_t * pbuf, uint32_t dlen)
{
	if ((start_addr&0xffffff)==0)
	{
		return;
	}
	uint32_t head_data_len, tail_data_len;
	uint32_t page_num;
	uint32_t data_idx = 0;
	uint32_t i;
	head_data_len = start_addr % DATA_FLASH_PAGE_SIZE;
    head_data_len = DATA_FLASH_PAGE_SIZE - head_data_len;
	if(dlen > head_data_len)
	{
		FLASH_ProgramPackageBytes(start_addr, pbuf, head_data_len);
		data_idx += head_data_len;
		dlen -= head_data_len;
		page_num = dlen / DATA_FLASH_PAGE_SIZE;
		tail_data_len = dlen % DATA_FLASH_PAGE_SIZE;
		for(i = 0; i < page_num; i++)
		{
			FLASH_ProgramPackageBytes(start_addr + data_idx, &pbuf[data_idx], DATA_FLASH_PAGE_SIZE);
			data_idx += DATA_FLASH_PAGE_SIZE;
		}
		FLASH_ProgramPackageBytes(start_addr + data_idx, &pbuf[data_idx], tail_data_len);

	}
	else
	{
		FLASH_ProgramPackageBytes(start_addr, pbuf, dlen);
	}
}

void ReadDataFlash(uint32_t start_addr, uint8_t * pbuf, uint32_t dlen)
{
	if(!DiagnosticDataFlashAddr(start_addr, dlen))
	{
		return;
	}
	FLASH_Close();
	memcpy(pbuf, (uint32_t*)start_addr, dlen);
}



void EraseFlash(uint32_t start_addr, uint32_t dlen)
{
	if ((start_addr&0xffffff)==0)
	{
		return;
	}
	//擦除只能以DATA_FLASH_SECTION_SIZE为单位进行擦除，传入的参数横跨多少个BLOCK就擦多少个
	//计算擦除的起始地址
	uint32_t realStartAddr = 0;
	uint32_t blockNum = 0;
	uint32_t i = 0;
	uint32_t headLen = start_addr % DATA_FLASH_SECTION_SIZE;
	UINT32 intSave;
	if(headLen)
		realStartAddr = start_addr - headLen;
	else
		realStartAddr = start_addr;

	//计算需要擦除多少个block
	if(dlen > DATA_FLASH_SECTION_SIZE - headLen)
	{
		uint32_t leftByte = dlen - (DATA_FLASH_SECTION_SIZE - headLen);
		if(leftByte%DATA_FLASH_SECTION_SIZE)
			blockNum = leftByte/DATA_FLASH_SECTION_SIZE + 1 + 1;
		else
			blockNum = leftByte/DATA_FLASH_SECTION_SIZE + 1;
	}
	else
		blockNum = 1;

	//擦除
	intSave = LOS_IntLock();
	for(;i < blockNum;i++)
	{
		FLASH_EarseBlock(realStartAddr + i*DATA_FLASH_SECTION_SIZE);
	}
	LOS_IntRestore(intSave);

}
void WriteFlash(uint32_t start_addr, uint8_t * pbuf, uint32_t dlen)
{
	if ((start_addr&0xffffff)==0)
	{
		return;
	}
	uint32_t headLen = start_addr % DATA_FLASH_PAGE_SIZE;
	uint32_t currentStartAddr = start_addr;
	uint32_t nextStartAddr = start_addr - headLen + DATA_FLASH_PAGE_SIZE;
	UINT32 intSave;
	for (; dlen;)
	{
		uint32_t currentWriteNum = 0;
		if (nextStartAddr - currentStartAddr < dlen)
		{
			currentWriteNum = nextStartAddr - currentStartAddr;
		}
		else
		{
			currentWriteNum = dlen;
		}
		UINT32 intSave;
		intSave = LOS_IntLock();
		FLASH_ProgramPackageBytes(currentStartAddr, pbuf, currentWriteNum);
		LOS_IntRestore(intSave);
		currentStartAddr += currentWriteNum;
		nextStartAddr += DATA_FLASH_PAGE_SIZE;
		dlen -= currentWriteNum;
		pbuf += currentWriteNum;
	}
}

void ReadFlash(uint32_t start_addr, uint8_t * pbuf, uint32_t dlen)
{
	FLASH_Close();
	memcpy(pbuf, (uint32_t*)start_addr, dlen);
}

//保护头512K数据
void Protection1MFlash(void)
{
	if (FlashStatusRegNum == 3)
	{
		FLASH_WriteFlashStatus(1, (1 << 4 | 1 << 5 | 1 << 2) | (0x2 << 8) | ((1 << 6) << 16));
	}
	else if (FlashStatusRegNum == 2)
	{
		FLASH_WriteFlashStatus(1, (1 << 4 | 1 << 5 | 1 << 2) | (0x2 << 8));
	}
	else
	{
		__BKPT(1);
	}

}
void Unprotection1MFlash(void)
{
	if (FlashStatusRegNum == 3)
	{
		FLASH_WriteFlashStatus(1, (0x2 << 8) | (1 << 6));
	}
	else if (FlashStatusRegNum == 2)
	{
		FLASH_WriteFlashStatus(1, (0x2 << 8));
	}
	else
	{
		__BKPT(1);
	}
}

void Protection1MFlashNoVolatile(void)
{
	DataFlashInit();
    uint8_t reg=FLASH_GetFalshStatus(1);
    uint8_t reg2= FLASH_GetFalshStatus(2);
	if((reg2&(1<<1)) != (1 << 1))
	{
		FLASH_WriteQeFlag();
	}
	if ((reg&(1 << 4 | 1 << 5 | 1 << 2))==(1 << 4 | 1 << 5 | 1 << 2))
	{
		DataFlashClose();
		return;
	}

	if (FlashStatusRegNum == 3)
	{
		FLASH_WriteFlashStatusNoVolatile(1, (1 << 4 | 1 << 5 | 1 << 2) | (0x3e << 8) | ((1 << 6) << 16));
	}
	else if (FlashStatusRegNum == 2)
	{
		FLASH_WriteFlashStatusNoVolatile(1, (1 << 4 | 1 << 5 | 1 << 2) | (0x6<< 8));
	}
	else
	{
		__BKPT(1);
	}
	DataFlashClose();
}

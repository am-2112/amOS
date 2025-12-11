#include "FAT32.h"

/*verify integrity of bpb_common and signature first (also to ensure it is not another filesystem type such as ntfs)*/
bool ValidateCommonBPB(BPB_FAT *bpb)
{
	bool valid_fat_system = true;

	print(L"Validating BPB: \r\n");
	if (bpb->common.BPB_BytesPerSec < 512 || bpb->common.BPB_BytesPerSec > 4096 || bpb->common.BPB_SecPerClus % 2 != 0)
	{
		valid_fat_system = false;
	}
	else if (bpb->common.BPB_SecPerClus == 0 || bpb->common.BPB_SecPerClus % 2 != 0)
	{
		valid_fat_system = false;
	}
	else if (bpb->common.BPB_RsvdSecCnt == 0)
	{
		valid_fat_system = false;
	}
	else if (bpb->common.BPB_Num_FATs == 0)
	{
		valid_fat_system = false;
	}
	else if (bpb->common.BPB_Media < 0xF8 && bpb->common.BPB_Media != 0xF0)
	{
		valid_fat_system = false;
	}
	else if (bpb->common.BPB_TotSec16 == 0 && bpb->common.BPB_TotSec32 == 0)
	{
		valid_fat_system = false;
	}
	else if (bpb->common.BPB_TotSec16 > 0 && bpb->common.BPB_TotSec32 > 0)
	{
		valid_fat_system = false;
	}
	else if (bpb->common.BPB_FATSz16 == 0 && bpb->fat32.BPB_FATSz32 == 0)
	{
		valid_fat_system = false;
	}
	else if (bpb->Signature_word != 0xAA55)
	{
		valid_fat_system = false;
	}

	if (!valid_fat_system)
	{
		print(L"Invalid BPB Sector!\r\n");
	}

	return valid_fat_system;
}

/*
--Because this involves lots of sector reading, may want to start developing a caching system so that disk reads don't need to be done multiple times for the same sector
>>if dat is nullptr, or the sector number is mismatched, then a new disk read will occur*/
FAT16_ENTRY FAT16(uint16_t index, uint8_t fatTableNo, BPB_FAT *bpb, GENERIC_PARTITION *partition, void **dat, EFI_LBA *sector)
{
	uint16_t ThisFATSecNum = (fatTableNo * bpb->common.BPB_FATSz16) + (bpb->common.BPB_RsvdSecCnt + ((index * 2) / bpb->common.BPB_BytesPerSec)); // sector number of FAT sector containing entry for cluster [index] in fat table [fatTableNo]
	uint16_t ThisFATEntOffset = (index * 2) % bpb->common.BPB_BytesPerSec;																		  // modulo to get remainder

	/*read in new sector from disk if needed*/
	if (!dat || *sector != ThisFATSecNum)
	{
		*dat = malloc(partition->buffer->Get_Block_Size(partition->buffer->_buff));
		partition->Read_Block(partition->_partition, ThisFATSecNum, partition->buffer->Get_Block_Size(partition->buffer->_buff), *dat);
	}
	*sector = ThisFATSecNum;
	return ((FAT16_ENTRY *)(*dat))[ThisFATEntOffset];
}

FAT32_ENTRY FAT32(uint32_t index, uint8_t fatTableNo, BPB_FAT *bpb, GENERIC_PARTITION *partition, void **dat, EFI_LBA *sector)
{
	uint32_t ThisFATSecNum = (fatTableNo * bpb->fat32.BPB_FATSz32) + (bpb->common.BPB_RsvdSecCnt + ((index * 4) / bpb->common.BPB_BytesPerSec)); // sector number of FAT sector containing entry for cluster [index] in fat table [fatTableNo]
	uint32_t ThisFATEntOffset = (index * 4) % bpb->common.BPB_BytesPerSec;																		 // modulo to get remainder

	/*read in new sector from disk if needed*/
	if (!dat || *sector != ThisFATSecNum)
	{
		*dat = malloc(partition->buffer->Get_Block_Size(partition->buffer->_buff));
		partition->Read_Block(partition->_partition, ThisFATSecNum, partition->buffer->Get_Block_Size(partition->buffer->_buff), *dat);
	}
	*sector = ThisFATSecNum;
	return ((FAT32_ENTRY *)(*dat))[ThisFATEntOffset] & 0x0FFFFFFF;
}

void Read_FAT16_Table(BPB_FAT *bpb, GENERIC_PARTITION *partition, GENERIC_FILESYSTEM **out)
{
	/*testing fat table integrity by checking reserved entries (FAT[0] contains BPB_MEDIA in low 8 bits (other set to 1), and second reserved is set by format utility to EOC value)*/
	print(L"Reading first FAT16 reserved entry: \r\n");
	void *table = nullptr;
	EFI_LBA sector = 0;
	FAT16_ENTRY entry = FAT16(0, 0, bpb, partition, &table, &sector);
	print(L"Success\r\n");
	if ((uint8_t)(entry & 255) == bpb->common.BPB_Media)
	{
		print(L"Reading second FAT16 reserved entry: \r\n");
		entry = FAT16(1, 0, bpb, partition, &table, &sector);
		print(L"Success\r\n");

		if (!entry & 0x8000)
		{
			print(L"Volume is not clean!\r\n");
		}
		if (!entry & 0x4000)
		{
			print(L"Disk encountered read/write errors on last mount!\r\n");
		}

		print(L"Finished checking reserved entries\r\n");
	}
	else
	{
		print(L"Invalid table!\r\n");
	}
}

void Read_FAT32_Table(BPB_FAT *bpb, GENERIC_PARTITION *partition, GENERIC_FILESYSTEM **out)
{
	/*testing fat table integrity by checking reserved entries (FAT[0] contains BPB_MEDIA in low 8 bits (other set to 1), and second reserved is set by format utility to EOC value)*/
	print(L"Reading first FAT32 reserved entry: \r\n");
	void *table = nullptr;
	EFI_LBA sector = 0;
	FAT32_ENTRY entry = FAT32(0, 0, bpb, partition, &table, &sector);
	print(L"Success\r\n");
	if ((uint8_t)(entry & 255) == bpb->common.BPB_Media)
	{
		print(L"Reading second FAT32 reserved entry: \r\n");
		entry = FAT32(1, 0, bpb, partition, &table, &sector);
		print(L"Success\r\n");

		if (!entry & 0x08000000)
		{
			print(L"Volume is not clean!\r\n");
		}
		if (!entry & 0x04000000)
		{
			print(L"Disk encountered read/write errors on last mount!\r\n");
		}

		print(L"Finished checking reserved entries\r\n");
	}
	else
	{
		print(L"Invalid table!\r\n");
	}
}

/*will validate extended bpb as well as fields such as bpb_totSec16 which rely on fat type for validation*/
void AttemptMountAsFAT(BPB_FAT *bpb, GENERIC_PARTITION *partition, GENERIC_FILESYSTEM **out)
{
	/*begin by attempting to mount as fat32, then fat16 (efi partition default), then fat12; checking validity of extended bpb along the way*/
	/*fat type determined solely by the count of clusters on the volume*/
	uint32_t RootDirSections = ((bpb->common.BPB_RootEntCnt * 32) + (bpb->common.BPB_BytesPerSec - 1)) / bpb->common.BPB_BytesPerSec; //(rounding up division)
	uint32_t FATSz;
	uint32_t TotSec;
	uint32_t DataSec;
	if (bpb->common.BPB_FATSz16 != 0)
	{
		FATSz = bpb->common.BPB_FATSz16;
	}
	else
	{
		FATSz = bpb->fat32.BPB_FATSz32;
	}

	if (bpb->common.BPB_TotSec16 != 0)
	{
		TotSec = bpb->common.BPB_TotSec16;
	}
	else
	{
		TotSec = bpb->common.BPB_TotSec32;
	}
	DataSec = TotSec - (bpb->common.BPB_RsvdSecCnt + (bpb->common.BPB_Num_FATs * FATSz) + RootDirSections); // determine data region sector count
	uint32_t CountOfClusters = DataSec / bpb->common.BPB_SecPerClus;

	if (CountOfClusters < 4085)
	{
		print(L"Volume is FAT12; Unsupported\r\n");
	}
	else if (CountOfClusters < 65525)
	{
		print(L"Volume is FAT16\r\n");
		Read_FAT16_Table(bpb, partition, out);
	}
	else
	{
		print(L"Volume is FAT32\r\n");
		if (bpb->common.BPB_FATSz16 != 0) /*must be 0 for fat32 format (doesn't hold the other way around)*/
		{
			print(L"Invalid FAT32!\r\n");
			return;
		}
		else
		{
			Read_FAT32_Table(bpb, partition, out);
		}
	}
}

void ConstructGenericFromFAT32(GENERIC_PARTITION *partition, GENERIC_FILESYSTEM **out)
{
	/*ensure support for filesystem on partition (if not supported, return nullptr)
	 * then, fill out required function pointers into filesystem struct
	 */
	*out = malloc(sizeof(GENERIC_FILESYSTEM));

	print(L"Reading BPB:\r\n");

	UINTN bSize = partition->buffer->Get_Block_Size(partition->buffer->_buff);

	BPB_FAT *bpb = malloc(partition->buffer->Get_Block_Size(partition->buffer->_buff));
	partition->Read_Block(partition->_partition, 0, partition->buffer->Get_Block_Size(partition->buffer->_buff), bpb);

	*out = nullptr;
	if (ValidateCommonBPB(bpb))
	{
		AttemptMountAsFAT(bpb, partition, out);
	}
}
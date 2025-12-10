#include "GPT.h"

EFI_STATUS Read_GPT_Block(IN void* _partition, IN EFI_LBA LBA, IN UINTN BufferSize, OUT void* Buffer) {
	GPT_PARTITION* partition = (GPT_PARTITION*)_partition;
	if (LBA > partition->entry->EndingLBA)
		return EFI_INVALID_PARAMETER; /*avoid reading outside the partition*/
	print(L"Starting LBA: ");
	CHAR16* slba = UINT64ToUnicode(partition->entry->StartingLBA);
	print(slba);
	print(L"\r\n");
	return partition->dsk->device->Read_Block(partition->dsk->device->_buff, partition->entry->StartingLBA + LBA, BufferSize, Buffer);
}

void ConstructGenericFromGPT(GPT_PARTITION* gpt, GENERIC_PARTITION** out) {
	*out = malloc(sizeof(GENERIC_PARTITION));
	**out = (GENERIC_PARTITION){
		._partition = gpt,
		.Read_Block = Read_GPT_Block,
		.buffer = gpt->dsk->device
	};
}
void ConstructGenericArrayFromGPT(GPT_PARTITION* gpt, GENERIC_PARTITION** out, IN UINTN amount) {
	*out = malloc(sizeof(GENERIC_PARTITION) * amount);
	for (UINTN i = 0; i < amount; i++) {
		(*out)[i] = (GENERIC_PARTITION){
			._partition = &gpt[i],
			.Read_Block = Read_GPT_Block,
			.buffer = gpt[i].dsk->device
		};
	}
}

UINTN GetGPTPartitionCount(IN void* _disk) {
	GPT_DISK* disk = (GPT_DISK*)_disk;
	return disk->hdr->NumberOfPartitionEntries;
}

GENERIC_PARTITION* GetGPTPartition(IN void* _disk, IN UINTN partitionIndex) {
	GPT_DISK* disk = (GPT_DISK*)_disk;
	if (partitionIndex >= disk->hdr->NumberOfPartitionEntries)
		return nullptr;

	GPT_PARTITION* gpt = malloc(sizeof(GPT_PARTITION));
	GENERIC_PARTITION* generic;

	*gpt = (GPT_PARTITION){
		.dsk = disk,
		.entry = (EFI_PARTITION_ENTRY*)(((uint8_t*)(disk->entries)) + (disk->hdr->SizeOfPartitionEntry * partitionIndex)),
		.entryNumber = partitionIndex
	};

	ConstructGenericFromGPT(gpt, &generic);
	return generic;
}

GENERIC_PARTITION* GetGPTPartitions(IN void* _disk, OUT UINTN* partitionCount) {
	GPT_DISK* disk = (GPT_DISK*)_disk;
	*partitionCount = disk->hdr->NumberOfPartitionEntries;
	GPT_PARTITION* gpt = malloc(sizeof(GPT_PARTITION) * disk->hdr->NumberOfPartitionEntries);
	GENERIC_PARTITION* generic;

	for (UINTN i = 0; i < disk->hdr->NumberOfPartitionEntries; i++) {
		EFI_PARTITION_ENTRY* currentEntry = (EFI_PARTITION_ENTRY*)((uint8_t*)(disk->entries) + (disk->hdr->SizeOfPartitionEntry * i));
		/*check if the entry is empty (then assume all future ones will also be empty - since a minimum of 16kb is to be reserved, even if there aren't that many partitions)*/
		if (currentEntry->Attributes & 0x02 == 1 || (currentEntry->PartitionTypeGUID.Data1 == 0 && currentEntry->PartitionTypeGUID.Data2 == 0 && currentEntry->PartitionTypeGUID.Data3 == 0 && currentEntry->PartitionTypeGUID.Data4 == 0) || currentEntry->StartingLBA == 0 || currentEntry->EndingLBA == 0) {
			*partitionCount = i;
			ConstructGenericArrayFromGPT(gpt, &generic, i);
			return generic;
		}

		*(gpt + i) = (GPT_PARTITION){
			.dsk = disk,
			.entry = (EFI_PARTITION_ENTRY*)(((uint8_t*)(disk->entries)) + (disk->hdr->SizeOfPartitionEntry * i)),
			.entryNumber = i
		};
	}
	ConstructGenericArrayFromGPT(gpt, &generic, disk->hdr->NumberOfPartitionEntries);
	return generic;
}

void ConstructGenericFromGPTDisk(GENERIC_BUFFER* device, GENERIC_DISK** out) {
	GPT_DISK* gpt = ValidateGPTHeader(device);
	*out = nullptr;
	if (gpt == nullptr)
		return;

	*out = malloc(sizeof(GENERIC_DISK));
	**out = (GENERIC_DISK){
		.disk = gpt,
		.GetPartitionCount = GetGPTPartitionCount,
		.GetPartition = GetGPTPartition,
		.GetPartitions = GetGPTPartitions
	};
}

/*expecting a pointer to one of the devices in blockDevices (nullptr if invalid)*/
GPT_DISK* ValidateGPTHeader(GENERIC_BUFFER* device) {
	GPT_DISK* dsk = malloc(sizeof(GPT_DISK));
	GPT_HEADER* hdr = malloc(device->Get_Block_Size(device->_buff)); //have to oversize to block size; reserving the rest
	GPT_PARTITION_ENTRY* entries = nullptr;

	EFI_LBA lba = 0;
check_failed: //will repeat checks if the first header is invalid
	if (lba == 0) {
		lba = 1;
	}
	else if (lba == 1) {
		lba = hdr->AlternateLBA;
		if (hdr->AlternateLBA == 0 || hdr->AlternateLBA == 1) { //ensure no infinite loop if AlternateLBA is invalid / not present
			free(dsk); free(hdr);
			if (entries)
				free(entries);
			return nullptr;
		}
	}
	else {
		free(dsk); free(hdr);
		if (entries)
			free(entries);
		return nullptr;
	}

	//print(L"Getting GPT Header: ");
	Status = device->Read_Block(device->_buff, lba, device->Get_Block_Size(device->_buff), hdr);
	if (EFI_ERROR(Status)) {
		//print(L"Failed\r\n");
		goto check_failed;
	}
	else {
		//print(L"Success\r\n");
		if (hdr->Signature != 0x5452415020494645) {
			//print(L"    Invalid Signature!\r\n");
			goto check_failed;
		}
		else {
			print(L"    Valid Signature\r\n");
			UINT32 CRC32 = hdr->HeaderCRC32;
			hdr->HeaderCRC32 = 0;
			UINT32 CalculatedCRC32;
			ST->BootServices->CalculateCrc32(hdr, hdr->HeaderSize, &CalculatedCRC32);
			if (CalculatedCRC32 == CRC32) {
				hdr->HeaderCRC32 = CRC32;
				print(L"    Header CRC matched\r\n");

				if (hdr->MyLBA == 1) {
					print(L"    Valid header LBA\r\n");

					/*Will now read in the PartitionEntryArray and calculate its crc32*/
					print(L"    Reading Partition Entry Array: ");

					UINTN amountOfBlocksNeeded = ((hdr->NumberOfPartitionEntries * hdr->SizeOfPartitionEntry) + (device->Get_Block_Size(device->_buff) - 1)) / device->Get_Block_Size(device->_buff); //addition to ensure rounding up
					entries = malloc(amountOfBlocksNeeded * device->Get_Block_Size(device->_buff)); //to get successive entries, will need to do pointer arithmetic to skip reserved sections of [SizeOfPartitionEntry - 128] size

					Status = device->Read_Block(device->_buff, hdr->PartitionEntryLBA, amountOfBlocksNeeded * device->Get_Block_Size(device->_buff), entries);
					if (EFI_ERROR(Status)) {
						print(L"Failed\r\n");
						goto check_failed;

					}
					else {
						print(L"Success\r\n");

						ST->BootServices->CalculateCrc32(entries, hdr->NumberOfPartitionEntries * hdr->SizeOfPartitionEntry, &CalculatedCRC32);
						if (CalculatedCRC32 != hdr->PartitionEntryArrayCRC32) {
							//print(L"        Partition Entry CRC does not match\r\n");
							goto check_failed;
						}
						else {
							print(L"        Partition Entry CRC matched\r\n");
							print(L"Status: Validated GPT Header\r\n");
						}

					}
				}
				else {
					//print(L"    Invalid header LBA\r\n");
					goto check_failed;
				}
			}
			else {
				print(L"    Header CRC does not match\r\n");
				goto check_failed;
			}
		}
	}

	if (lba != 1) {
		//call function to repair the primary gpt header using the backup
	}

	*dsk = (GPT_DISK){
		.hdr = hdr,
		.entries = entries,
		.device = device
	};
	return dsk;
}
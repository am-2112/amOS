#include "GPT.h"

FREE_BLOCK_IO_NODE* freeNode;
BLOCK_IO_NODE* blockDevices = nullptr;
UINTN blockAmount = 0; /*current*/
UINTN maxBlockAmount;

void RemoveFromBlockList(EFI_HANDLE ImageHandle, BLOCK_IO_NODE* device) {
	blockAmount--;
	
	EFI_GUID Block_Protocol_GUID = EFI_BLOCK_IO_PROTOCOL_GUID;
	ST->BootServices->CloseProtocol(device->handle, &Block_Protocol_GUID, ImageHandle, NULL);
	device->device = nullptr;
	
	FREE_BLOCK_IO_NODE* newFreeNode = malloc(sizeof(FREE_BLOCK_IO_NODE));
	*newFreeNode = (FREE_BLOCK_IO_NODE) {
		.prev = freeNode,
		.self = device
	};
	freeNode = newFreeNode;
}

/*expecting a pointer to one of the devices in blockDevices (nullptr if invalid)*/
GPT_DISK* ValidateGPTHeader(EFI_BLOCK_IO_PROTOCOL* device) {
	GPT_DISK* dsk = malloc(sizeof(GPT_DISK));
	GPT_HEADER* hdr = malloc(device->Media->BlockSize); //have to oversize to block size; reserving the rest
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

	print(L"Getting GPT Header: ");
	Status = device->ReadBlocks(device, device->Media->MediaId, lba, device->Media->BlockSize, hdr);
	if (EFI_ERROR(Status)) {
		print(L"Failed\r\n");
		goto check_failed;
	}
	else {
		print(L"Success\r\n");
		if (hdr->Signature != 0x5452415020494645) {
			print(L"    Invalid Signature!\r\n");
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

					UINTN amountOfBlocksNeeded = ((hdr->NumberOfPartitionEntries * hdr->SizeOfPartitionEntry) + (device->Media->BlockSize - 1)) / device->Media->BlockSize; //addition to ensure rounding up
					entries = malloc(amountOfBlocksNeeded * device->Media->BlockSize); //to get successive entries, will need to do pointer arithmetic to skip reserved sections of [SizeOfPartitionEntry - 128] size

					Status = device->ReadBlocks(device, device->Media->MediaId, hdr->PartitionEntryLBA, amountOfBlocksNeeded * device->Media->BlockSize, entries);
					if (EFI_ERROR(Status)) {
						print(L"Failed\r\n");
						goto check_failed;

					}
					else {
						print(L"Success\r\n");

						ST->BootServices->CalculateCrc32(entries, hdr->NumberOfPartitionEntries * hdr->SizeOfPartitionEntry, &CalculatedCRC32);
						if (CalculatedCRC32 != hdr->PartitionEntryArrayCRC32) {
							print(L"        Partition Entry CRC does not match\r\n");
							goto check_failed;
						}
						else {
							print(L"        Partition Entry CRC matched\r\n");
							print(L"Status: Validated GPT Header\r\n");
						}

					}
				}
				else {
					print(L"    Invalid header LBA\r\n");
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
		.entries = entries
	};
	return dsk;
}

void AddToBlockList(EFI_HANDLE ImageHandle, EFI_HANDLE deviceHandle) {
	EFI_GUID Block_Protocol_GUID = EFI_BLOCK_IO_PROTOCOL_GUID;
	EFI_BLOCK_IO_PROTOCOL* device;
	Status = ST->BootServices->OpenProtocol(deviceHandle, &Block_Protocol_GUID, (VOID**)&device, ImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
	if (EFI_ERROR(Status))
		return;
	
	print(L"Obtained Block Protocol From Handle");
	if (device->Media->LogicalPartition) {
		print(L" >> Logical Partition\r\n");
	} else {
		print(L" >> Physical Device\r\n");
	}
	
	/*should still investigate logical partitions in case they are the only partition (in which case LogicalPartition is true even if the handle represents the physical device)*/
	GPT_DISK* dsk = ValidateGPTHeader(device);
	if (dsk == nullptr)
		return;
	
	/*allocate more memory, or attempt to use up free array elements*/
	UINTN index = blockAmount;
	if (blockAmount == maxBlockAmount) {
		maxBlockAmount *= 1.5;
		BLOCK_IO_NODE* newDeviceLocation = malloc(sizeof(BLOCK_IO_NODE) * maxBlockAmount);
		
		UINTN i = blockAmount;
		while (--i)
			newDeviceLocation[i] = blockDevices[i]; //copy
			
		free(blockDevices);
		blockDevices = newDeviceLocation;
		
	} else if (freeNode != nullptr) {
		index = freeNode->self - blockDevices;
		
		FREE_BLOCK_IO_NODE* currentNode = freeNode;
		freeNode = freeNode->prev;
		free(currentNode);
	}
	
	/*add device to block array*/
	blockAmount++;
	blockDevices[index] = (BLOCK_IO_NODE) {
		.handle = deviceHandle,
		.device = device,
		.dsk = dsk
	};
}

void InitialiseBlockDeviceList(EFI_HANDLE ImageHandle) {
	
	/*in case initialise is called more than once*/
	if (blockDevices != nullptr) {
		for (UINTN i = 0; i < blockAmount; i++) {
			EFI_GUID Block_Protocol_GUID = EFI_BLOCK_IO_PROTOCOL_GUID;
			ST->BootServices->CloseProtocol(blockDevices[i].handle, &Block_Protocol_GUID, ImageHandle, NULL);
		}
		free(blockDevices);
		blockAmount = 0;
		
		while (freeNode != nullptr) {
			FREE_BLOCK_IO_NODE* currentNode = freeNode;
			if (freeNode->prev != nullptr) {
				freeNode = freeNode->prev;
			}
			free(currentNode);
		}
	}
	freeNode = nullptr;
	
	EFI_GUID Block_Protocol_GUID = EFI_BLOCK_IO_PROTOCOL_GUID;
	EFI_HANDLE* new_block_handles;
	Status = ST->BootServices->LocateHandleBuffer(ByProtocol, &Block_Protocol_GUID, nullptr, &maxBlockAmount, &new_block_handles);
	if (EFI_ERROR(Status))
		return;
	
	blockDevices = malloc(sizeof(BLOCK_IO_NODE) * maxBlockAmount);
	for (UINT8 i = 0; i < maxBlockAmount; i++) {
		AddToBlockList(ImageHandle, new_block_handles[i]);
	}
	
	free(new_block_handles);
}
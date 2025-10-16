#include "GPT.h"

EFI_STATUS Read_GPT_Block(IN void* partition, IN EFI_LBA LBA, IN UINTN BufferSize, OUT void* Buffer) {
	return ((GPT_PARTITION*)partition)->device->device->ReadBlocks(((GPT_PARTITION*)partition)->device->device, ((GPT_PARTITION*)partition)->device->device->Media->MediaId, LBA, BufferSize, Buffer);
}

void ConstructGenericFromGPT(GPT_PARTITION* gpt, GENERIC_PARTITION* out) {
	*out = (GENERIC_PARTITION){
		.partition = gpt,
		.Read_Block = Read_GPT_Block
	};
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
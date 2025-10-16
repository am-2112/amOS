#include "Diskmgr.h"

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
	*newFreeNode = (FREE_BLOCK_IO_NODE){
		.prev = freeNode,
		.self = device
	};
	freeNode = newFreeNode;
}

void AddToBlockList(EFI_HANDLE ImageHandle, EFI_HANDLE deviceHandle) {
	/*EFI_GUID Block_Protocol_GUID = EFI_BLOCK_IO_PROTOCOL_GUID;
	EFI_BLOCK_IO_PROTOCOL* device;
	Status = ST->BootServices->OpenProtocol(deviceHandle, &Block_Protocol_GUID, (VOID**)&device, ImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
	if (EFI_ERROR(Status))
		return;

	print(L"Obtained Block Protocol From Handle");
	if (device->Media->LogicalPartition) {
		print(L" >> Logical Partition\r\n");
	}
	else {
		print(L" >> Physical Device\r\n");
	}

	/*should still investigate logical partitions in case they are the only partition (in which case LogicalPartition is true even if the handle represents the physical device)*/
	/*GPT_DISK* dsk = ValidateGPTHeader(device);
	if (dsk == nullptr)
		return;

	/*allocate more memory, or attempt to use up free array elements*/
	/*for (int i = 0; i < dsk->hdr->NumberOfPartitionEntries; i++) {
		UINTN index = blockAmount;
		if (blockAmount == maxBlockAmount) {
			maxBlockAmount *= 1.5;
			BLOCK_IO_NODE* newDeviceLocation = malloc(sizeof(BLOCK_IO_NODE) * maxBlockAmount);

			UINTN i = blockAmount;
			while (--i)
				newDeviceLocation[i] = blockDevices[i]; //copy

			free(blockDevices);
			blockDevices = newDeviceLocation;

		}
		else if (freeNode != nullptr) {
			index = freeNode->self - blockDevices;

			FREE_BLOCK_IO_NODE* currentNode = freeNode;
			freeNode = freeNode->prev;
			free(currentNode);
		}

		/*add device to block array*/
		/*blockAmount++;
		blockDevices[index] = (BLOCK_IO_NODE){
			.handle = deviceHandle,
			.device = device,
		};
		ConstructGenericFromGPT(dsk, blockDevices[index].dsk);
	}*/
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
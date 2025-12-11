#include "Diskmgr.h"
#include "GPT.h"

BOOLEAN initialised = false;

EFI_STATUS ReadFromBlockIO(IN void *_buff, IN EFI_LBA LBA, IN UINTN BufferSize, OUT void *Buffer)
{
	BLOCK_IO_NODE *buff = (BLOCK_IO_NODE *)_buff;
	if (LBA > buff->device->Media->LastBlock)
		return EFI_INVALID_PARAMETER;
	buff->device->ReadBlocks(buff->device, buff->device->Media->MediaId, LBA, BufferSize, Buffer);
}
UINTN Get_Block_Size(IN void *_buff)
{
	BLOCK_IO_NODE *buff = (BLOCK_IO_NODE *)_buff;
	return buff->device->Media->BlockSize;
}

/*add a by-reference version? also repeat for other ConstructGeneric functions?*/
void ConstructGenericFromBlockIO(IN BLOCK_IO_NODE *block_io, OUT GENERIC_BUFFER **buffer)
{
	*buffer = malloc(sizeof(GENERIC_BUFFER));
	**buffer = (GENERIC_BUFFER){
		._buff = block_io,
		.Read_Block = ReadFromBlockIO,
		.Get_Block_Size = Get_Block_Size};
}

/*add corresponding CloseDiskManager or likewise*/
void InitDiskManager(EFI_HANDLE ImageHandle)
{

	if (!initialised)
	{
		initialised = true;

		EFI_GUID Block_Protocol_GUID = EFI_BLOCK_IO_PROTOCOL_GUID;
		EFI_HANDLE *new_block_handles;
		UINTN blockAmount;
		Status = ST->BootServices->LocateHandleBuffer(ByProtocol, &Block_Protocol_GUID, nullptr, &blockAmount, &new_block_handles);
		if (EFI_ERROR(Status))
			return;

		/*start checking for partition structures (start with gpt, then mbr)*/
		for (UINTN i = 0; i < blockAmount; i++)
		{
			EFI_GUID Block_Protocol_GUID = EFI_BLOCK_IO_PROTOCOL_GUID;
			EFI_BLOCK_IO_PROTOCOL *device;
			Status = ST->BootServices->OpenProtocol(new_block_handles[i], &Block_Protocol_GUID, (VOID **)&device, ImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
			if (EFI_ERROR(Status))
			{
				print(L"Failed to obtain block protocol from handle\r\n");
				continue;
			}

			/*print(L"Obtained Block Protocol From Handle");
			if (device->Media->LogicalPartition) {
				print(L" >> Logical Partition\r\n");
			}
			else {
				print(L" >> Physical Device\r\n");
			}*/

			GENERIC_BUFFER *buffer;
			BLOCK_IO_NODE *node = malloc(sizeof(BLOCK_IO_NODE));
			*node = (BLOCK_IO_NODE){
				.device = device,
				.handle = new_block_handles[i]};
			ConstructGenericFromBlockIO(node, &buffer);

			GENERIC_DISK *gpt;
			ConstructGenericFromGPTDisk(buffer, &gpt);
			if (gpt == nullptr)
			{
				// try detect mbr partition
				continue;
			}

			/*display some debugging information*/
			print(L"partition count: ");
			// CHAR16* pCount = UINT16ToUnicode(gpt->GetPartitionCount(gpt->disk)); //really need to make larger versions of these for other number sizes
			// print(pCount);
			// print(L"\r\n");
			// for (UINTN p = 0; p < gpt->GetPartitionCount(gpt->disk); p++) {

			//}
			UINTN pCount;
			GENERIC_PARTITION *partitions = gpt->GetPartitions(gpt->disk, &pCount);
			CHAR16 *partCount = UINT64ToUnicode(pCount);
			print(partCount);
			print(L"\r\n");
			/*then, attempt to parse any filesystems on the drive

			*/
			EFI_INPUT_KEY Key;
			for (UINTN p = 0; p < pCount; p++)
			{
				GENERIC_FILESYSTEM *fSystem;
				ConstructGenericFromFAT32(&partitions[p], &fSystem);
				/*Waiting for keystoke*/
				while ((Status = ST->ConIn->ReadKeyStroke(ST->ConIn, &Key)) == EFI_NOT_READY)
					;
			}

			free(partitions);
		}

		free(new_block_handles);
	}
}
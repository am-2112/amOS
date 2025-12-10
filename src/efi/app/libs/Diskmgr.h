#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include "../../include/efi/efi.h"
#include "../../include/interfaces/PartitionInterface.h"
#include "../../include/interfaces/FileSystemInterface.h"
#include "FAT32.h"

#pragma pack(1)

typedef struct _BLOCK_IO_NODE {
	EFI_HANDLE handle;
	EFI_BLOCK_IO_PROTOCOL* device; //if this is nullptr, then the node is empty and should be ignored
} BLOCK_IO_NODE; /*removed struct _GPT_DISK since this will be generic for all block_io devices that are identified to be valid (ie. even for mbr or other partition styles)*/

/*abstraction layer for viewing general disk details, as well as all partitions on the disk*/
typedef struct _GENERIC_DISK {
	void* disk;
	UINTN(*GetPartitionCount)(IN void* disk);
	GENERIC_PARTITION*(*GetPartition)(IN void* disk, IN UINTN partitionIndex);
	GENERIC_PARTITION*(*GetPartitions)(IN void* disk, OUT UINTN* partitionCount);
} GENERIC_DISK;



/*Detect suitable physical devices, set up the drive-partition storage, drive letters, and protocol events for device changes*/
extern void InitDiskManager(EFI_HANDLE imageHandle);
extern void ConstructGenericFromBlockIO(IN BLOCK_IO_NODE* block_io, OUT GENERIC_BUFFER** buffer);

extern BOOLEAN FileExists(CHAR16* filePath);
extern void* OpenFile(CHAR16* filePath);

#pragma pack()

#endif
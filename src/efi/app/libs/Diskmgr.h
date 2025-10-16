#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include "../../include/efi/efi.h"
#include "../../include/interfaces/PartitionInterface.h"
#include "../../include/interfaces/FileSystemInterface.h"
#include "FAT32.h"

#pragma pack(1)

/*Detect suitable physical devices, set up the drive-partition storage, drive letters, and protocol events for device changes*/
extern void InitDiskManager(EFI_HANDLE imageHandle);

#pragma pack()

#endif
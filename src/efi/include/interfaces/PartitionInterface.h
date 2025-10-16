#ifndef GENERIC_PARTITION_H
#define GENERIC_PARTITION_H

#include "efi_compiler.h"
#include "efi_defines.h"
#include "EFI_SYSTEM_TABLE.h"
#include "g_stdlib.h"
#include "EFI_BLOCK_IO2_PROTOCOL.h"

typedef struct _GENERIC_PARTITION {
	void* partition;
	EFI_STATUS (*Read_Block)(IN void* partition, IN EFI_LBA LBA, IN UINTN BufferSize, OUT void* Buffer);
} GENERIC_PARTITION;


typedef struct _BLOCK_IO_NODE {
	EFI_HANDLE handle;
	EFI_BLOCK_IO_PROTOCOL* device; //if this is nullptr, then the node is empty and should be ignored
	GENERIC_PARTITION dsk;
} BLOCK_IO_NODE; /*removed struct _GPT_DISK since this will be generic for all block_io devices that are identified to be valid (ie. even for mbr or other partition styles)*/

#endif
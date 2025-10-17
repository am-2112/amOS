#ifndef GENERIC_PARTITION_H
#define GENERIC_PARTITION_H

#include "efi_compiler.h"
#include "efi_defines.h"
#include "EFI_SYSTEM_TABLE.h"
#include "g_stdlib.h"
#include "EFI_BLOCK_IO2_PROTOCOL.h"

/*abstraction layer for block devices (or virtual, if emulated through software)*/
typedef struct _GENERIC_BUFFER {
	void* _buff;
	EFI_STATUS(*Read_Block)(IN void* _buff, IN EFI_LBA LBA, IN UINTN BufferSize, OUT void* Buffer);
	UINTN(*Get_Block_Size)(IN void* _buff);
} GENERIC_BUFFER;


/*_partition should have specific partition details (eg. gpt partition entries), as well as a pointer to the underlying disk structure (eg. gpt disk)*/
typedef struct _GENERIC_PARTITION {
	void* _partition;
	GENERIC_BUFFER* buffer;
	EFI_STATUS(*Read_Block)(IN void* partition, IN EFI_LBA LBA, IN UINTN BufferSize, OUT void* Buffer);
} GENERIC_PARTITION;

#endif
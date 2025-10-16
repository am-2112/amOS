#ifndef GENERIC_FILESYSTEM_H
#define GENERIC_FILESYSTEM_H

#include "efi_compiler.h"
#include "efi_defines.h"
#include "EFI_SYSTEM_TABLE.h"
#include "g_stdlib.h"
#include "PartitionInterface.h"

typedef struct _GENERIC_FILESYSTEM {
	GENERIC_PARTITION partition;
} GENERIC_FILESYSTEM;

#endif
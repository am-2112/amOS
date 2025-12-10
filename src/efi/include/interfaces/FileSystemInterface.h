#ifndef GENERIC_FILESYSTEM_H
#define GENERIC_FILESYSTEM_H

#include "efi_compiler.h"
#include "efi_defines.h"
#include "EFI_SYSTEM_TABLE.h"
#include "g_stdlib.h"
#include "PartitionInterface.h"

INTERFACE_DECL(_GENERIC_FILESYSTEM);

typedef struct _GENERIC_FILESYSTEM {
	void* _structure;
	BOOLEAN hasValidFileSystem;
	GENERIC_PARTITION partition;
	BOOLEAN(*FileExists)(struct _GENERIC_FILESYSTEM* fSystem, CHAR16* filePath);
	void* (*OpenFile)(struct _GENERIC_FILESYSTEM* fSystem, CHAR16* filePath);
} GENERIC_FILESYSTEM;

#endif
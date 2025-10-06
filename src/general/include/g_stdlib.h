#ifndef _STANDARD_
#define _STANDARD_

#include "EFI_SYSTEM_TABLE.h"
#include "efi_defines.h"
#include "c_types.h"

extern EFI_SYSTEM_TABLE *ST;
extern EFI_BOOT_SERVICES* BootServices;
extern EFI_STATUS Status;

extern void* malloc(UINT64 size);
extern void free(void* ptr);
extern void _changeAllocator(void* all);

extern bool print(CHAR16* str);

#endif
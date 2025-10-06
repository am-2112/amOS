#ifndef _EFI_SYSTEM_TABLE_
#define _EFI_SYSTEM_TABLE_

#include "c_defines.h"
#include "EFI_RUNTIME_SERVICES.h"
#include "EFI_BOOT_SERVICES.h"
#include "EFI_IO.h"

#define EFI_SYSTEM_TABLE_SIGNATURE      0x5453595320494249

typedef struct _EFI_SYSTEM_TABLE {
    EFI_TABLE_HEADER                Hdr;

    CHAR16                          *FirmwareVendor;
    UINT32                          FirmwareRevision;

    EFI_HANDLE                      ConsoleInHandle;
    SIMPLE_INPUT_INTERFACE          *ConIn;

    EFI_HANDLE                      ConsoleOutHandle;
    SIMPLE_TEXT_OUTPUT_INTERFACE    *ConOut;

    EFI_HANDLE                      StandardErrorHandle;
    SIMPLE_TEXT_OUTPUT_INTERFACE    *StdErr;

    EFI_RUNTIME_SERVICES            *RuntimeServices;
    EFI_BOOT_SERVICES               *BootServices;

    UINTN                           NumberOfTableEntries;
    EFI_CONFIGURATION_TABLE         *ConfigurationTable;

} EFI_SYSTEM_TABLE;


#endif
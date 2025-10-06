//#include <efi.h>
//#include <efilib.h>

#include "efi_compiler.h"
#include "efi_defines.h"
#include "EFI_SYSTEM_TABLE.h"
#include "g_str.h"
#include "g_stdlib.h"
#include "EFI_BLOCK_IO2_PROTOCOL.h"
#include "GPT.h"

void DisplayCommands();
EFI_STATUS GetVolumes(EFI_HANDLE ImageHandle, UINTN* amount, EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*** interfaces, EFI_HANDLE** handles);
EFI_STATUS ListVolumeInfo(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL** interface, UINT8 index);
UINTN cin(IN CHAR16* out, IN UINTN amount);

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_INPUT_KEY Key;
	
    /* Store the system table for future use in other functions */
    ST = SystemTable;
	_changeAllocator(ST->BootServices);

    /* Say hi */
	if (!print(L"============|Boot Loader|============\r\n"))
		return Status;
	
	/*Calculating and displaying table revision*/
	UINT32 rev = ST->Hdr.Revision;
	UINT16 major = rev >> 16;
	UINT16 minor = rev & 0x00FF;
	UINT8 minorU = minor / 10;
	UINT8 minorL = minor % 10;
	
	CHAR16* majPtr = UINT16ToUnicode(major);
	CHAR16* minUPtr = UINT16ToUnicode(minorU);
	CHAR16* minLPtr = UINT16ToUnicode(minorL);
	
	if (!print(L"Revision: "))
		return Status;
	if (!print(majPtr))
		return Status;
	if (!print(L"."))
		return Status;
	if (!print(minUPtr))
		return Status;
	
	if (minorL != 0) {
		if (!print(L"."))
			return Status;
		if (!print(minLPtr)) 
			return Status;
	}
	
	if (!print(L"\r\n")) 
		return Status;
	
	free(majPtr);
	free(minUPtr);
	free(minLPtr);

    /*Reset input buffer (to erase other inputs up to this point)*/
    Status = ST->ConIn->Reset(ST->ConIn, FALSE);
    if (EFI_ERROR(Status))
        return Status;

    /*Waiting for keystoke*/
    while ((Status = ST->ConIn->ReadKeyStroke(ST->ConIn, &Key)) == EFI_NOT_READY) ;

	InitialiseBlockDeviceList(ImageHandle);
	
	/*Waiting for keystoke*/
	while ((Status = ST->ConIn->ReadKeyStroke(ST->ConIn, &Key)) == EFI_NOT_READY);
	
    return Status;
}
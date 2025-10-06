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

	EFI_GUID Block_Protocol_GUID = EFI_BLOCK_IO_PROTOCOL_GUID;
	EFI_HANDLE* hBuffer;
	UINTN amount;
	Status = ST->BootServices->LocateHandleBuffer(ByProtocol, &Block_Protocol_GUID, nullptr, &amount, &hBuffer);
	if (EFI_ERROR(Status))
		return Status;
	
	EFI_BLOCK_IO_PROTOCOL** temp = malloc(sizeof(EFI_BLOCK_IO_PROTOCOL*) * amount);
	for (UINT8 i = 0; i < amount; i++) {
		Status = ST->BootServices->OpenProtocol(hBuffer[i], &Block_Protocol_GUID, (VOID**)&temp[i], ImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
		if (EFI_ERROR(Status))
			return Status;
			
		print(L"Obtained Block Protocol From Handle");
		if (temp[i]->Media->LogicalPartition) {
			print(L" >> Logical Partition\r\n");
		} else {
			print(L" >> Physical Device\r\n");
		}
		
		/*should still investigate logical partitions in case they are the only partition (in which case LogicalPartition is true even if the handle represents the physical device)*/
		GPT_HEADER* hdr = malloc(temp[i]->Media->BlockSize); //have to oversize to block size; reserving the rest
		print(L"Getting GPT Header: ");
		Status = temp[i]->ReadBlocks(temp[i], temp[i]->Media->MediaId, 1, temp[i]->Media->BlockSize, hdr);
		if (EFI_ERROR(Status)) {
			print(L"Failed\r\n");
		} else {
			print(L"Success\r\n");
			if (hdr->Signature != 0x5452415020494645) {
				print(L"Invalid Signature!\r\n");
			} else {
				print(L"Valid Signature\r\n");
			}
		}
		
		
	}
	
	
	/*Waiting for keystoke*/
	while ((Status = ST->ConIn->ReadKeyStroke(ST->ConIn, &Key)) == EFI_NOT_READY);
	
    return Status;
}
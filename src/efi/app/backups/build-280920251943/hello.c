//#include <efi.h>
//#include <efilib.h>

#include "efi_compiler.h"
#include "efi_defines.h"
#include "EFI_SYSTEM_TABLE.h"
#include "g_str.h"
#include "g_stdlib.h"

void DisplayCommands();
EFI_STATUS GetVolumes(EFI_HANDLE ImageHandle, UINTN* amount, EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*** interfaces);
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

	

	/*Exit efi boot services and load kernel (after getting the MemoryMap first)*/	
	
	/*would it be possible to load up the efi application along with code to detect new usb devices, and load changes / new files on the same device?*/
	/*and also to run it in a different process so that said process can be stopped by user input if desired*/
	UINTN volumeCount;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL** volumes;
	GetVolumes(ImageHandle, &volumeCount, &volumes);
	UINT8 volCount = volumeCount;
	CHAR16* volC = UINT16ToUnicode(volCount);
	print(L"Total Volumes: ");
	print(volC);
	print(L"\r\n");
	for (UINT8 i = 0; i < volumeCount; i++) {
		ListVolumeInfo(volumes, i);
	}
	DisplayCommands();
	print(L"\r\nEnter Command:");
	
	UINT8 selectedVolume = -1;
	do {
		print(L"\r\n>");
		CHAR16* input = malloc(128);
		UINTN charCount = cin(input, 128); //a command limit of 64 characters
		
		if (input[0] == 0x08) { //0x08 is the efi scan code for Delete
			print(L"Exiting");
			break;
			
		} else if (input[0] == 'r') {
			print(L"Refreshing:\r\n");
			GetVolumes(ImageHandle, &volumeCount, &volumes);
			volCount = volumeCount;
			CHAR16* volC = UINT16ToUnicode(volCount);
			print(L"Total Volumes: ");
			print(volC);
			print(L"\r\n");
			for (UINT8 i = 0; i < volumeCount; i++) {
				ListVolumeInfo(volumes, i);
			}
			
		} else if (input[0] == 'v' && input[1] == 'o' && input[2] == 'l') {
			if (charCount < 5) {
				print(L"Invalid command");
			} else {		
				selectedVolume = UnicodeToUINT16(input + 4);
				if (selectedVolume == -1 || selectedVolume > volCount) {
					print(L"Invalid volume selection\r\n");
					selectedVolume = -1;
				} else {
					print(L"    Volume ");
					print(UINT16ToUnicode(selectedVolume));
					print(L" Selected");
				}
			}
			
		} else if (input[0] == 'l' && input[1] == 'i' && input[2] == 's' && input[3] == 't') {		
			if (selectedVolume == -1) {
				print(L"No volume selected");
			} else {
				
			}
			
		} else if (input[0] == 'c' && input[1] == 'd') {
			if (selectedVolume == -1) {
				print(L"No volume selected");
			} else {
				
			}
			
		} else if (input[0] == 0x0003) {
			print(L"Culling child process:");
			
		} else if (input[0] == '?') {
			DisplayCommands();
			
		} else {
			print(L"Unrecognised Command");
		}
		
		
		
	} while (true);
	
	
	/*Waiting for keystoke*/
	while ((Status = ST->ConIn->ReadKeyStroke(ST->ConIn, &Key)) == EFI_NOT_READY);
	
    return Status;
}

void DisplayCommands() {
	print(L"Commands:\r\n");
	print(L"    [Del] - exit\r\n"); //0x007F
	print(L"    r - refresh volume list\r\n");
	print(L"    vol [?] - select volume (using index)\r\n");
	print(L"    list - list subdirectories\r\n");
	print(L"    cd [?] - visit specified directory on current volume\r\n");
	print(L"    ^c - cull current child process\r\n"); //^c is same as 0x0003 or End-Of-text character in unicode
	print(L"    ? - see commands list\r\n");
}

EFI_STATUS GetVolumes(EFI_HANDLE ImageHandle, UINTN* amount, EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*** interfaces) {
	print(L"\r\nLoading Present Volumes:\r\n");
	print(L"Locating Handles for Simple File System Protocol\r\n");
	EFI_GUID Simple_File_System_Guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
	EFI_HANDLE* pBuffer;
	Status = ST->BootServices->LocateHandleBuffer(ByProtocol, &Simple_File_System_Guid, nullptr, amount, &pBuffer);
	if (EFI_ERROR(Status))
		return Status;
	
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL** temp = malloc(sizeof(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*) * *amount);
	for (UINT8 i = 0; i < *amount; i++) {
		Simple_File_System_Guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
		Status = ST->BootServices->OpenProtocol(pBuffer[i], &Simple_File_System_Guid, (VOID**)&temp[i], ImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
		if (EFI_ERROR(Status))
			return Status;
			
		print(L"Obtained Protocol From Handle\r\n");
	}
	
	*interfaces = temp;
	
	ST->BootServices->FreePool(pBuffer);
	print(L"Success\r\n");
	return EFI_SUCCESS;
}

EFI_STATUS ListVolumeInfo(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL** interface, UINT8 index) {
	EFI_FILE_PROTOCOL* Root;
	Status = interface[index]->OpenVolume(interface[index], &Root);
	if (EFI_ERROR(Status))
		return Status;
			
	EFI_GUID info = EFI_FILE_SYSTEM_INFO_ID;
	EFI_FILE_SYSTEM_INFO* Buffer;
	UINTN BufferSize = 0;
	
	Root->GetInfo(Root, &info, &BufferSize, Buffer); //first call will set BufferSize to correct amount
	BootServices->AllocatePool(EfiLoaderData, BufferSize, (VOID**)&Buffer);
	
	Status = Root->GetInfo(Root, &info, &BufferSize, Buffer);
	if (EFI_ERROR(Status))
		return Status;
	
	CHAR16* ind = UINT16ToUnicode(index);
	print(L"[");
	print(ind);
	print(L"] ");
	free(ind);
	
	print(L"Volume Name: ");
	print(Buffer->VolumeLabel);
	print(L"\r\n");
		
	ST->BootServices->FreePool(Buffer);
	
	Root->Close(Root);
	return EFI_SUCCESS;
}

UINTN cin(OUT CHAR16* out, IN UINTN amount) {
	EFI_INPUT_KEY Key;
	UINTN current = 0;
	do { 
		UINTN evInd;
		ST->BootServices->WaitForEvent(1, &ST->ConIn->WaitForKey, &evInd);
		ST->ConIn->ReadKeyStroke(ST->ConIn, &Key);
		if (Key.UnicodeChar != 0 && Key.UnicodeChar != 0x000D) { //do not save enter press to the buffer
			if (Key.UnicodeChar == 0x0008) { //handle backspaces
				if (current > 0) {
					*out = '\0';
					--out; //ensure prev character was actually deleted
					--current;
					*out = 0x0008;
					print(out);
					print(L" "); //space to overwrite previous letter
					print(out);
				}
			} else {
				*out = Key.UnicodeChar;
				++out;
				++current;
				*out = '\0'; //null-terminator to ensure following print works as intended
				print(out - 1);
			}
		} else if (Key.UnicodeChar != 0x000D) {
			*out = Key.ScanCode;
			++out;
			++current;
		}
	} while (Key.UnicodeChar != 0 && Key.UnicodeChar != 0x000D &&  amount--); //if any unprintable characters (such as delete or esc) or enter are pressed then this will return
	
	*out = '\0';
	print(L"\r\n");
	return current;
}
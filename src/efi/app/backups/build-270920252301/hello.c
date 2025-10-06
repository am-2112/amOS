//#include <efi.h>
//#include <efilib.h>

#include "efi_compiler.h"
#include "efi_defines.h"
#include "EFI_SYSTEM_TABLE.h"
#include "g_str.h"
#include "g_stdlib.h"

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
	
	print(L"Detecting USB ports\r\n");

	/*Need to use USB2_HC protocol (LocateProtocol?) and GetCapability to get port count (and then maybe use usbfn protocol and usbio protocol to abstract for each individual port)*/
	EFI_GUID USB2_HC_GUID = EFI_USB2_HC_PROTOCOL_GUID;
	VOID* INTERFACE;
	Status = ST->BootServices->LocateProtocol(&USB2_HC_GUID, NULL, &INTERFACE);
	if (EFI_ERROR(Status))
		return Status;
	
	/*Get host controller capabilities*/
	UINT8 MaxSpeed = 0;
	UINT8 PortNumber = 0;
	UINT8 is64BitCapable = 0;
	
	EFI_USB2_HC_PROTOCOL* USB2_HC_INTERFACE = INTERFACE;
	Status = USB2_HC_INTERFACE->GetCapability(USB2_HC_INTERFACE, &MaxSpeed, &PortNumber, &is64BitCapable);
	if (EFI_ERROR(Status))
		return Status;
	
	print(L"Host Controller Capabilities: \r\nMax Speed: ");
	if (MaxSpeed == EFI_USB_SPEED_FULL) {
		print(L"Full Speed [12 Mbs]");
	} else if (MaxSpeed == EFI_USB_SPEED_LOW) {
		print(L"Low Speed [1.5 Mbs]");
	} else if (MaxSpeed == EFI_USB_SPEED_HIGH) {
		print(L"High Speed [480 Mbs]");
	} else if (MaxSpeed == EFI_USB_SPEED_SUPER) {
		print(L"Super Speed [4.8 Gbps]");
	}
	
	CHAR16* _PortNumber = UINT16ToUnicode(PortNumber);
	print(L"\r\nPort Count: ");
	print(_PortNumber);
	
	print(L"\r\n64-Bit Capable: ");
	if (is64BitCapable) {
		print(L"True\r\n");
	} else {
		print(L"False\r\n");
	}
	
	
	
	
	
	/*Loop through ports for changes each time user uses keyboard (and stop when x is pressed)*/
	do {
		EFI_USB_PORT_STATUS Port;
		CHAR16* currentPort;
		for (UINT8 i = 0; i < PortNumber; i++) {
			Status = USB2_HC_INTERFACE->GetRootHubPortStatus(USB2_HC_INTERFACE, i, &Port);
			if (EFI_ERROR(Status))
				return Status;
			
			currentPort = UINT16ToUnicode(i);
			if (Port.PortStatus & USB_PORT_STAT_CONNECTION == 1) {
				print(L"Port ");
				print(currentPort);
				print(L" Connected\r\n");
			}
			if (Port.PortChangeStatus & USB_PORT_STAT_C_CONNECTION == 1) {
				print(L"Port ");
				print(currentPort);
				print(L" Connection Changed\r\n");
			}
		}
		
		/*Locate handle buffers for the file protocol (using EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID for now - since only care about simple FAT support for further uefi/boot applications when testing)*/
		print(L"Locating Handles for Simple File System Protocol\r\n");
		EFI_GUID Simple_File_System_Guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
		UINTN noHandles;
		EFI_HANDLE* pBuffer;
		Status = ST->BootServices->LocateHandleBuffer(ByProtocol, &Simple_File_System_Guid, nullptr, &noHandles, &pBuffer);
		if (EFI_ERROR(Status))
			return Status;
		
		print(L"Success\r\n");
		
		for (UINT8 i = 0; i < noHandles; i++) {
			Simple_File_System_Guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
			EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* File_Interface;
			Status = ST->BootServices->OpenProtocol(pBuffer[i], &Simple_File_System_Guid, (VOID**)&File_Interface, ImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
			if (EFI_ERROR(Status))
				return Status;
			
			print(L"Obtained Protocol From Handle\r\n");
			
			EFI_FILE_PROTOCOL* Root;
			Status = File_Interface->OpenVolume(File_Interface, &Root);
			if (EFI_ERROR(Status))
				return Status;
			
			print(L"Opened Volume\r\n");
			
			EFI_GUID info = EFI_FILE_SYSTEM_INFO_ID;
			EFI_FILE_SYSTEM_INFO* Buffer;
			UINTN BufferSize = 0;
			Root->GetInfo(Root, &info, &BufferSize, Buffer); //first call will set BufferSize to correct amount
			BootServices->AllocatePool(EfiLoaderData, BufferSize, (VOID**)&Buffer);
			Status = Root->GetInfo(Root, &info, &BufferSize, Buffer);
			if (EFI_ERROR(Status))
				return Status;
			
			print(L"Got Info:\r\n");
			
			print(L"Volume Name: ");
			print(Buffer->VolumeLabel);
			print(L"\r\n");
			
			ST->BootServices->FreePool(Buffer);
		}
		
		
		ST->BootServices->FreePool(pBuffer);	
		while ((Status = ST->ConIn->ReadKeyStroke(ST->ConIn, &Key)) == EFI_NOT_READY);
	} while (Key.UnicodeChar != 'x');

	print(L"Finished\r\n");
	
	/*Waiting for keystoke*/
	while ((Status = ST->ConIn->ReadKeyStroke(ST->ConIn, &Key)) == EFI_NOT_READY);
	
    return Status;
}
#include "g_stdlib.h"

EFI_STATUS Status;
EFI_BOOT_SERVICES* BootServices;
EFI_SYSTEM_TABLE *ST;

void* malloc(UINT64 size) {
	if (size == 0 || BootServices == nullptr)
		return nullptr;
	
	/*Keeping things simple for now by just using pools*/
	void* buff;
	EFI_STATUS Status = BootServices->AllocatePool(EfiLoaderData,(UINTN)size, &buff);
	if (EFI_ERROR(Status))
        return nullptr;
	
	return buff;
}
void free(void* ptr) {
	if (!ptr)
		return;
	
	BootServices->FreePool(ptr);
}

void _changeAllocator(void* all) {
	BootServices = all;
}

bool print(CHAR16* str) {
	if (str == nullptr)
		return false;
	
	Status = ST->ConOut->OutputString(ST->ConOut, str); // EFI Applications use Unicode and CRLF, a la Windows
    if (EFI_ERROR(Status))
        return false;
	
	return true;
}
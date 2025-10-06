efiinc = ..////include

geninc = ..//..////general//include

efilibloc = libs

boot: entry dependencies
	$(GCC_PATH)//x86_64-w64-mingw32-gcc -nostdlib -Wl,-dll -shared -Wl,--subsystem,10 -e efi_main -o $(BUILD_PATH)//BOOTX64.EFI build//*.o

entry: entry.c
	$(GCC_PATH)//x86_64-w64-mingw32-gcc -ffreestanding -I$(efiinc) -I$(geninc) -I$(efilibloc) -c -o "build//entry.o" entry.c

dependencies: stdlib efilib

stdlib: $(geninc)//g_stdlib.c
	$(GCC_PATH)//x86_64-w64-mingw32-gcc -ffreestanding -I$(efiinc) -I$(geninc) -c -o "build//g_stdlib.o" $(geninc)//g_stdlib.c

efilib: $(efilibloc)//GPT.c
	$(GCC_PATH)//x86_64-w64-mingw32-gcc -ffreestanding -I$(efiinc) -I$(geninc) -c -o "build//GPT.o" $(efilibloc)//GPT.c
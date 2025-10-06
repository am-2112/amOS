# amOS
Creating a primitive OS for x64 systems using the UEFI spec (including a simple bootloader)

## Current Plans
* Using BLOCK_IO_PROTOCOL to read logical blocks from device for GPT partition recognition (and parsing FAT32 filesystem later)
* Develop a simple cli to run other efi applications (made interruptible for debug purposes)
* Long term: Kernel, running executables, a layer to mimick windows api, compiler (self-hosting), desktop functionality

## How to Build: 
The build process uses GNU compiler (specifically, makefile runs x86_64-w64-mingw32-gcc)

Run the makefile at efi/app/makefile, the arguments `GCC_PATH` and `BUILD_PATH` correspond to the location of the gcc exe, and the build location respectively. 

If a different compiler is used, the application run can be changed in the makefile. 

For the uefi application to be bootable, the build path needs to be on a GPT-partitioned drive inside the path EFI/BOOT (ie. "A://EFI//BOOT"); the final file will be named BOOTX64.EFI.

`make GCC_PATH="C://Users//me//Desktop//MSYS//mingw64//bin" BUILD_PATH="A://EFI//BOOT"`

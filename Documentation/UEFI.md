### [Table of Contents](Table%20Of%20Contents.md#table-of-contents)
# **UEFI**
## Overview
UEFI stands for Unified Extensible Firmware Interface, and is a specification developed to replace bios as an interface between device firmware and OS software. Platforms implementing UEFI allows bootloaders and operating systems to function without needing highly specific drivers (although they are still ideal for optimal performance). <br/>
UEFI allows access to boot devices through a set of protocol interfaces, through drivers designed for accessing devices in the *pre-boot* environment (not to replace OS-specific drivers). As such, the UEFI Driver Model is only designed to support of execution of drivers in the *pre-boot* environment.

### The specification[^1] defines several sections:
1. The Boot Manager
    - Used to load drivers and apps written to the spec
2. EFI System Table and Partitions
    - Describes an EFI system table passed to every compliant driver and app, and defines a GUID-based partitioning scheme
3. Block Transition Table
    - Layout and rules for block I/O which provides power fail write atomicity of a single block
4. Boot Services
    - Definitions of the fundamental services present in a UEFI-compliant system before OS is booted
5. Runtime Services
    - Definitions for services present both before *and after* an OS is booted
6. Protocols
7. The EFI Byte Code Virtual Machine
8. Firmware Update and Reporting
9. Network Protocols
10. Secure Boot & Driver Signing
11. Human Interface Infrastructure (HII)
12. User Identification and Secure Technologies
13. Miscellaneous Protocols (Timestamp Protocol and Reset Notification Protocol)

### As outlined in the spec, the design of UEFI is based on a few fundamental elelements:
* **Reuse of existing table-based interfaces** - for maintaining compatibility
* **System Partition** - defining a sharable system partition and file system
* **Boot Services** - which provides interfaces for devices and system functionality during boot time; abstracted through *handles* and *protocols*
* **Runtime Services** - presenting a minimal set of runtime services to abstract base platform hardware resources that may be needed by the OS during normal operation

<a name="fig1.1"></a>
![Shows the principal components of UEFI and their relationship to platform hardware and OS Software. It illustrates the interactions of various components of a UEFI spec-compliant system used to accomplish platform and OS boot](https://github.com/user-attachments/assets/7d6ee5ff-a4e2-4fa4-b2cf-d2d186e0d36d)[^1]

## How will UEFI be used?
Particularly for an individual like myself, UEFI is very useful for providing a simple set of drivers for (most) devices that need to be used by the OS. This means that I do not need to focus on writing code to interface with firmware for each specific device my OS wants to support, but rather my bootloader only needs to be compatible with one universal interface. <br/>
Making proper use of the protocols within the UEFI spec will make designing an exokernel slightly easier, for the most simple / needed abstraction is provided by UEFI.

UEFI includes a standard entry point at `/efi/boot/BOOTX64.efi` (fat12, fat16 or fat32 format) and all UEFI images (BOOTX64.efi included) use a subset of the PE32+ image format with a modified header signature.

As shown in the previous [figure](#fig1.1), the UEFI OS Loader is the next layer, and the final one between the hardware and the OS. As the name suggests, this is a special type of UEFI application which takes over control of the system from firmware conforming to the UEFI spec (ie. from the standard entry point mentioned above, and from any other linked UEFI images used). <br/>
It can only use memory it has allocated from the firmware and can only use UEFI services and protocols to access devices the firmware exposes. If loading is successful, it can use the Boot Service `EFI_BOOT_SERVICES.ExitBootServices()`. This terminates all boot services, including memory management and thus the UEFI OS loader is responsible for continued operation. <br/>

From this point on, the only services and protocols available to us will be those defined in the Runtime Services, and it will be up to the kernel to manage everything (once control is passed to it by the OS Loader). See below for a summary of boot service interfaces, followed by runtime services. Afterwards, we will deduce the full extent of what needs to happen during the boot window (ie. before exiting boot services). <br/>
**Unless otherwise specified**, protocol interface structure is not allocated from runtime memory and as such, protocol member functions should not be called at runtime.

## Boot Services


### Global Boot Service Interfaces

### Device Handle-based Boot Service interfaces

### Device Protocols

### Protocol Services

### Conclusion

## Runtime Services
According to the UEFI Spec[^1] the primary purpose of the runtime services is to abstract minor parts of the hardware implementation of the platform from the OS. All of these interfaces are non-blocking and can be called with interrupts disabled if desired. <br/> 
At runtime however, the services are only available if switched into a flat physical addressing mode (unless the runtime service `SetVirtualAddressMap()` is used, at which case these services can only be called in a virtual addressing mode).

Memory used by runtime services must be reserved and not used by the OS. Also, the memory will never be directly manipulated by the OS or its components. UEFI is responsible for defining the resources used by runtime services, so the OS can synchronise with those resources when runtime service calls are made, or guarantee that the OS never uses those resources

![Table 2.2: UEFI Runtime Services](https://github.com/user-attachments/assets/24f606fe-7686-4286-87e6-e5ad2d50f7ed)


### Conclusion
There are a few key runtime services, but there are not many of them so basically everything the OS needs has to be provided by the OS Loader / Kernel. <br/>
A few notable functions include `SetVirtualAddressMap` which switches runtime functions from physical to virtual addressing, as well as `ConvertPointer()` which converts a pointer between the two. The variable functions seem to be linked to the EFI variable store, so may not be used much outside of EFI functions. The other notable functions are the 4 time related functions `GetTime() SetTime() GetWakeupTime() SetWakeupTime()`.

[^1]: https://uefi.org/sites/default/files/resources/UEFI_Spec_Final_2.11.pdf

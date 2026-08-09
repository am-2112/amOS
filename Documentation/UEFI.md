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

## EFI System Table
The EFI System Table contains pointers to the runtime and boot services tables (this is how the majority of the services available from UEFI are accessed). After a call to `EFI_BOOT_SERVICES.ExitBootServices()`, pointers pertaining to boot services, console devices, and text protocols become invalid. All elements in the service tables are pointers to functions (ie. Boot Services and Runtime Services). <br/>
So while Boot or Runtime Services are being used by anything, the EFI System Table and the corresponding tables for Boot and Runtime Services should not be overwritten

## Boot Services
All of the services mentioned below are only available *before* a successful call to `EFI_BOOT_SERVICES.ExitBootService`. This function is called to terminate all boot services. The UEFI OS Loader must ensure it has the system's current memory map by the time it does this by calling `EFI_BOOT_SERVICES.GetMemoryMap()`. The OS Loader can still call to Memory Allocation Services after the first call to this service. Afterwards, the UEFI OS Loader owns all **available** memory in the system, including any memory marked as `EfiBootServicesCode` and `EfiBootServicesData`.

### Event, Timer and Task Priority Services
These functions are used during preboot to create, close, signal, and wait for events; to set timers; and to raise and restore task priority levels. <br/>
Execution in boot service environment occurs at different task priority levels (TPLs) where higher priority tasks may interrupt tasks executing at a lower priority level. However, there are restrictions on the TPL levels at which many UEFI service functions and protocol interface functions can execute. In order of priority: `TPL_APPLICATION TPL_CALLBACK TPL_NOTIFY`.

The functions are listed below: 
![Table 7.1: Event, Timer, and Task Priority Functions](https://github.com/user-attachments/assets/e7f2e123-64e2-476f-bc7a-f54d37730145)


### Memory Allocation Services
These functions are used during preboot to allocate and free memory, and to obtain the system's memory map. <br/>
Importantly for us, an executing EFI image must **only** use the memory **it** has allocated. Also, if an executing EFI image exits and returns control to firmware, it must free all resources it has **explicitly allocated** - including all memory pages, pool allocations, open file handles etc. (this excludes memory allocated by firmware to load an image).

It is the caller's responsibility to free memory allocated by a service. Unless stated otherwise, the UEFI spec assumes that these memory buffers are allocated with `AllocatePool()` and freed with `FreePool()`.

The functions are listed below:
![Table 7.8: Memory Allocation Functions](https://github.com/user-attachments/assets/a14c2143-f8fe-4bf9-b33b-487c0af5d6fc)

Calling `GetMemoryMap()` is mandatory before terminating boot services. This returns a copy of the current memory map, which is an array of memory descriptors - each of which describing a contiguous block of memory. The map describes **all** memory, regardless of how it is being used. It includes blocks allocated by `AllocatePages()` and `AllocatePool()` as well as blocks being used by firmware for its own purpose. <br/>
Any address regions **not** backed by physical hardware does not return a range description, and any memory not supposed to be accessed by the OS is returned as `EfiReservedMemoryType`. All other memory can be used at our own discretion.


### Protocol Handler Services
These allow applications to install a protocol on a device handle, identify handles that support given protocols, determine if handles support given protocols, and so forth. <br/>
The firmware is responsible for maintaining a database showing which protocols are attached to each device handle. This is built dynamically by calling `EFI_BOOT_SERVICES.InstallProtocolInterface()`. Protocols do not have to be added to existing device handles, but also added to create a new device handle. This function is only available to UEFI drivers or firmware itself, and each attached protocol is represented as a GUID/Interface pointer pair (where the GUID is the name of the protocol, and the interface points to a protocol instance). Each protocol handler is logically a UEFI driver.

Access to devices is initiated by calling `EFI_BOOT_SERVICES.HandleProtocol()` which determines if a handle supports a given protocol. If so, a pointer to the matching Protocol Interface structure is returned. <br/>
Agents (eg. the UEFI OS Loader) calling this function are not tracked, so it is not safe to uninstall or reinstall protocol interfaces. As such, it is ideal to instead use `EFI_BOOT_SERVICES.OpenProtocol()` and `EFI_BOOT_SERVICES.CloseProtocol()` and likewise named functions. These adds and removes elements from a list of agents consuming a protocol interface. <br/>
Another useful function may be `EFI_BOOT_SERVICES.LocateProtocol()` which searches the handle database for the first protocol instance that matches the search criteria (which can then be consumed to access devices).

Most other functions in this grouping are used by development and implementation of drivers and firmware, not for use by the UEFI OS Loader.

The functions are listed below:

![Table 7.14: Protocol Interface Functions @ pg.162-163](https://github.com/user-attachments/assets/491df293-cb5e-4f36-b531-d2c064a022d1)


### Image Services
This handles loading UEFI Applications, UEFI Boot Services Drivers, and EFI Runtime Drivers. A UEFI OS Loader is a type of UEFI Application. 

Boot Service Drivers service memory and stays resident until boot services terminate, whereas runtime drivers stay loaded and resident during runtime (whose memory must be allocated in a single allocation). <br/>
UEFI applications should not install any protocol interfaces or handles, and are loaded in sequential order by the boot manager (and only **one** UEFI application may dynamically load another).

UEFI applications can load another with `EFI_BOOT_SERVICES.LoadImage()` and `EFI_BOOT_SERVICES.StartImage()`. It seems that by using the SourceBuffer param, images can be loaded directly from memory (rather than from disk, if using DevicePath param). `StartImage()` directly transfers control to the newly loaded UEFI application until it finishes executing.

The functions are listed below:

![Table 7.31: Image Functions @ pg. 197](https://github.com/user-attachments/assets/c7dd0b77-342d-4b5c-8549-ffaca7065193)

`EFI_BOOT_SERVICES.ExitBootService` is particularly noteworthy. This function is called to terminate all boot services. The UEFI OS Loader must ensure it has the system's current memory map by the time it does this by calling `EFI_BOOT_SERVICES.GetMemoryMap()`. The OS Loader can still call to Memory Allocation Services after the first call to this service. Afterwards, the UEFI OS Loader owns all **available** memory in the system, including any memory marked as `EfiBootServicesCode` and `EfiBootServicesData` (but not anything used by the runtime services).


### Miscellaneous Boot Services


### Conclusion
As mentioned at the start of the section, the UEFI OS Loader must at minimum, have the current memory map by calling `EFI_BOOT_SERVICES.GetMemoryMap()`, before terminating boot services.

## Runtime Services
According to the UEFI Spec[^1] the primary purpose of the runtime services is to abstract minor parts of the hardware implementation of the platform from the OS. All of these interfaces are non-blocking and can be called with interrupts disabled if desired. <br/> 
At runtime however, the services are only available if switched into a flat physical addressing mode (unless the runtime service `SetVirtualAddressMap()` is used, at which case these services can only be called in a virtual addressing mode).

Memory used by runtime services must be reserved and not used by the OS. Also, the memory will never be directly manipulated by the OS or its components. UEFI is responsible for defining the resources used by runtime services, so the OS can synchronise with those resources when runtime service calls are made, or guarantee that the OS never uses those resources

![Table 2.2: UEFI Runtime Services](https://github.com/user-attachments/assets/24f606fe-7686-4286-87e6-e5ad2d50f7ed)


### Conclusion
There are a few key runtime services, but there are not many of them so basically everything the OS needs has to be provided by the OS Loader / Kernel. <br/>
A few notable functions include `SetVirtualAddressMap` which switches runtime functions from physical to virtual addressing, as well as `ConvertPointer()` which converts a pointer between the two. The variable functions seem to be linked to the EFI variable store, so may not be used much outside of EFI functions. The other notable functions are the 4 time related functions `GetTime() SetTime() GetWakeupTime() SetWakeupTime()`.

[^1]: https://uefi.org/sites/default/files/resources/UEFI_Spec_Final_2.11.pdf  
If looking for a specific image in the document, the caption will mention the section number to visit in the contents page (eg. 7.1 => 7. Services - Boot Services ==> .1 Event, Timer and Task Priority Services) or will directly include the page number(s) the image comes from

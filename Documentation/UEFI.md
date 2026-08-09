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

![Shows the principal components of UEFI and their relationship to platform hardware and OS Software. It illustrates the interactions of various components of a UEFI spec-compliant system used to accomplish platform and OS boot](https://github.com/user-attachments/assets/7d6ee5ff-a4e2-4fa4-b2cf-d2d186e0d36d)[^1]

## How will UEFI be used?
Particularly for an individual like myself, UEFI is very useful for providing a simple set of drivers for (most) devices that need to be used by the OS. This means that I do not need to focus on writing code to interface with firmware for each specific device my OS wants to support, but rather my bootloader only needs to be compatible with one universal interface. <br/>
Making proper use of the protocols within the UEFI spec will make designing an exokernel slightly easier, for the most simple / needed abstraction is provided by UEFI.

UEFI includes a standard entry point at `/efi/boot/BOOTX64.efi` (fat12, fat16 or fat32 format) and all UEFI images (BOOTX64.efi included) use a subset of the PE32+ image format with a modified header signature

[^1]: https://uefi.org/sites/default/files/resources/UEFI_Spec_Final_2.11.pdf

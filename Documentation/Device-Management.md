### [Table of Contents](Table%20Of%20Contents.md#table-of-contents)
# Device Management
## Overview
One of the most important parts of the kernel will be providing secure access to devices connected to the CPU. We can find information about these through the ACPI table given from boot. Information and access to devices will be given through a subsystem called the Device Manager[^1]. This starts with device detection. <br/>
Devices are usually organised in a tree structure, starting at the top with ACPI. From here the PCI bus can be discovered (for example), and then from there a USB controller could be discovered[^2]...
## ACPI
### Overview
ACPI (Advanced Configuration and Power Interface) is a spec which allows for discovery of the hardware configuration of a computer. It also allows for things like controlling the amount of power devices are given, controlling / checking thermals (ie. temperature sensors and fan speeds etc.) and more[^3]. <br/>
The ACPI specification[^4] defines ACPI hardware interfaces, software interfaces, and data structures. The following figure shows an overview of how components should interface between each other: 

![Figure 1-1 OSPM/ACPI Global System](https://github.com/user-attachments/assets/4b060de8-ef81-4034-bf7c-919822536872)

As seen, the bits in blue corresponds to OS specific technology. We will need to develop an ACPI Driver to understand the tables and interfaces that ACPI provides.

[^1]: https://wiki.osdev.org/Device_Management
[^2]: https://www.reddit.com/r/osdev/comments/vybawn/device_detection/ (u/Octocontrabass)
[^3]: https://wiki.osdev.org/ACPI
[^4]: https://uefi.org/sites/default/files/resources/ACPI_6_3_final_Jan30.pdf (circa Dec 2018)

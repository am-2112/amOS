### [Table of Contents](Table%20Of%20Contents.md#table-of-contents)
# **Kernel**
## Kernel Overview
A kernel is the central part of an operating system which manages resources such as memory and devices, as well as providing an interface for using those resources. It will also usually manage protection / security, and multitasking.

### Types of Kernels
#### Monolithic vs Micro
Kernels where device drivers and services run as *part* of the kernel is considered monolithic (regardless of whether parts are dynamically loaded, or if everything is a single binary) - compared to microkernels, whose drivers and services largely run in the user space.[^1]
#### Exokernel
Exokernels are orthoganal to this, and instead focus on the idea that security is separated from abstraction. Exokernels require no forced abstractions, and instead of providing security at the abstraction level (eg. through Unix-style permissions for filesystems), exokernels will provide security on the unabstracted hardware level (eg. to disk sectors).[^2]

For example, in the MIT exokernel[^3], the kernel identifies disk blocks to the application by their physical block address. When the program initializes use of the disk, it provides the kernel with a callback that the kernel can use to determine which blocks the program controls. This callback is used by the kernel to verify that the program claims *only* the block that was allocated (in addition to those it already controlled)

Overrideable abstractions are implemented in library operating systems (libOSes) [^2]

### How does this apply to amOS?

amOS will use a (mostly) monolithic exokernel.

Monolithic kernels provide efficiency for systems by reducing the amount of context switches and messaging (especially on x86 architectures where task switches are expensive operations)[^1]. <br/>
Exokernels represents as little abstraction as possible, to give programs as much control over the hardware as possible (in a secure way) for programs to take advantage of the present hardware - which can result in drastic performance improvements (see the Cheetah web server from Engler et al. running up to eight times faster than the competition[^2]).

While it will take great consideration during the design phase to make a good interface for an exokernel, putting the burden of abstractions in separate libOSes should improve the active development process by making things easier to prototype.

[^1]: https://wiki.osdev.org/Kernal
[^2]: https://wiki.osdev.org/Exokernel
[^3]: https://en.wikipedia.org/wiki/Exokernel#Design

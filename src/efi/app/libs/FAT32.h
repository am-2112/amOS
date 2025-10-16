#ifndef FAT32_H
#define FAT32_H

#include "efi.h"
#include "PartitionInterface.h"
#include "FileSystemInterface.h"
#include "std.h"

extern void TryConstructGenericFromFAT32(GENERIC_PARTITION* partition, GENERIC_FILESYSTEM* out);

#endif
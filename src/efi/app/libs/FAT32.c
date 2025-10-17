#include "FAT32.h"

void ConstructGenericFromFAT32(GENERIC_PARTITION* partition, GENERIC_FILESYSTEM* out) {
	/*ensure support for filesystem on partition (if not supported, return nullptr)
	* then, fill out required function pointers into filesystem struct
	*/
}
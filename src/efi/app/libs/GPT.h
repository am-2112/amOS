#ifndef _GPT_
#define _GPT_

#include "../../include/efi/efi.h"
#include "../../../general/include/std.h"
#include "../../include/interfaces/PartitionInterface.h"
#include "Diskmgr.h"

static EFI_GUID EFI_SYSTEM_PARTITION = {.Data1 = 0xC12A7328, .Data2 = 0xF81F, .Data3 = 0x11D2, .Data4 = {0xBA, 0x4B, 0x00, 0xA0, 0xC9, 0x3E, 0xC9, 0x3B}};
#pragma pack(1)

INTERFACE_DECL(_GPT_HEADER);
INTERFACE_DECL(_GPT_DISK);

typedef struct _GPT_HEADER {
	UINT64 Signature;
	UINT32 Revision;
	
	UINT32 HeaderSize;
	UINT32 HeaderCRC32;
	
	UINT32 Reserved;
	
	EFI_LBA MyLBA;
	EFI_LBA AlternateLBA;
	
	EFI_LBA FirstUsableLBA;
	EFI_LBA LastUsableLBA;
	
	EFI_GUID DiskGUID;
	
	UINT64 PartitionEntryLBA;
	UINT32 NumberOfPartitionEntries;
	UINT32 SizeOfPartitionEntry;
	UINT32 PartitionEntryArrayCRC32;
} GPT_HEADER;

typedef struct _GPT_DISK {
	GPT_HEADER* hdr;
	struct _GPT_PARTITION_ENTRY* entries; //entry count defined in hdr->NumberOfPartitionEntries
} GPT_DISK;

typedef struct {
	GPT_HEADER* hdr;
	BLOCK_IO_NODE* device;
	struct _GPT_PARTITION_ENTRY* entry;
} GPT_PARTITION;

extern EFI_STATUS Read_GPT_Block(IN void* partition, IN EFI_LBA LBA, IN UINTN BufferSize, OUT void* Buffer);
extern void ConstructGenericFromGPT(GPT_PARTITION* gpt, GENERIC_PARTITION* out);

static EFI_GUID EFI_PARTITION_INFO_PROTOCOL_GUID = { .Data1 = 0x8cf2f62c, .Data2 = 0xbc9b, .Data3 = 0x4821, .Data4 = {0x80, 0x8d, 0xec, 0x9e, 0xc4, 0x21, 0xa1, 0xa0} };
static EFI_GUID EMPTY_GUID = { .Data1 = 0x00000000, .Data2 = 0x0000, .Data3 = 0x0000, .Data4 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} };
#define PARTITION_TYPE_OTHER 0x00 
#define PARTITION_TYPE_GPT 0x02 

typedef struct _GPT_PARTITION_ENTRY {
	EFI_GUID PartitionTypeGUID;
	EFI_GUID UniquePartitionGUID;
	
	EFI_LBA StartingLBA;
	EFI_LBA EndingLBA;
	
	UINT64 Attributes; //bit 0 defines the partition as a Required Partition
	
	CHAR16 PartitionName[36];
} EFI_PARTITION_ENTRY, GPT_PARTITION_ENTRY;

extern GPT_DISK* ValidateGPTHeader(EFI_BLOCK_IO_PROTOCOL* device);

#pragma pack()

#endif
#ifndef _GPT_
#define _GPT_

#include "efi_compiler.h"
#include "efi_defines.h"
#include "EFI_SYSTEM_TABLE.h"
#include "g_stdlib.h"
#include "EFI_BLOCK_IO2_PROTOCOL.h"

static EFI_GUID EFI_SYSTEM_PARTITION = {.Data1 = 0xC12A7328, .Data2 = 0xF81F, .Data3 = 0x11D2, .Data4 = {0xBA, 0x4B, 0x00, 0xA0, 0xC9, 0x3E, 0xC9, 0x3B}};
#pragma pack(1)

INTERFACE_DECL(_FREE_BLOCK_IO_NODE);
INTERFACE_DECL(_GPT_HEADER);
INTERFACE_DECL(_GPT_DISK);

typedef struct {
	EFI_HANDLE handle;
	EFI_BLOCK_IO_PROTOCOL* device; //if this is nullptr, then the node is empty and should be ignored
	
	struct _GPT_DISK* dsk;
} BLOCK_IO_NODE;

/*device representing deleted pointers for reuse before using new memory (treated like a stack)*/
typedef struct _FREE_BLOCK_IO_NODE {
	struct _FREE_BLOCK_IO_NODE* prev;
	BLOCK_IO_NODE* self;
} FREE_BLOCK_IO_NODE;

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

static EFI_GUID EFI_PARTITION_INFO_PROTOCOL_GUID = {.Data1 = 0x8cf2f62c, .Data2 = 0xbc9b, .Data3 = 0x4821, .Data4 = {0x80, 0x8d, 0xec, 0x9e, 0xc4, 0x21, 0xa1, 0xa0}};
#define PARTITION_TYPE_OTHER 0x00 
#define PARTITION_TYPE_MBR 0x01 
#define PARTITION_TYPE_GPT 0x02 

typedef struct _GPT_PARTITION_ENTRY {
	EFI_GUID PartitionTypeGUID;
	EFI_GUID UniquePartitionGUID;
	
	EFI_LBA StartingLBA;
	EFI_LBA EndingLBA;
	
	UINT64 Attributes; //bit 0 defines the partition as a Required Partition
	
	CHAR16 PartitionName[36];
} EFI_PARTITION_ENTRY, GPT_PARTITION_ENTRY;

typedef struct { 
	UINT8 BootIndicator; 
	UINT8 StartHead; 
	UINT8 StartSector; 
	UINT8 StartTrack; 
	UINT8 OSIndicator; 
	UINT8 EndHead; 
	UINT8 EndSector; 
	UINT8 EndTrack; 
	UINT8 StartingLBA[4]; 
	UINT8 SizeInLBA[4]; 
} MBR_PARTITION_RECORD;

typedef struct { 
	UINT32 Revision;
	UINT32 Type; 
	UINT8 System; 
	UINT8 Reserved[7]; 
	union { 
		MBR_PARTITION_RECORD Mbr; 
		EFI_PARTITION_ENTRY Gpt; 
	} Info; 
} EFI_PARTITION_INFO_PROTOCOL;

extern void InitialiseBlockDeviceList(EFI_HANDLE ImageHandle);
extern void AddToBlockList(EFI_HANDLE ImageHandle, EFI_HANDLE deviceHandle);
extern void RemoveFromBlockList(EFI_HANDLE ImageHandle, BLOCK_IO_NODE* device);
extern GPT_DISK* ValidateGPTHeader(EFI_BLOCK_IO_PROTOCOL* device);

#pragma pack()

#endif
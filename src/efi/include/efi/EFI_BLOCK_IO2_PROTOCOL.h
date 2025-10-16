#ifndef _EFI_BLOCK_IO2_PROTOCOL_H
#define _EFI_BLOCK_IO2_PROTOCOL_H

INTERFACE_DECL(_EFI_BLOCK_IO2_PROTOCOL);
INTERFACE_DECL(_EFI_BLOCK_IO_PROTOCOL);
INTERFACE_DECL(_EFI_DEVICE_PATH_TO_TEXT_PROTOCOL);

static EFI_GUID EFI_BLOCK_IO2_PROTOCOL_GUID = {.Data1 = 0xa77b2472, .Data2 = 0xe282, .Data3 = 0x4e9f, .Data4 = {0xa2, 0x45, 0xc2, 0xc0, 0xe2, 0x7b, 0xbc, 0xc1}};

typedef UINT64 EFI_LPA;

typedef struct { 
	EFI_EVENT Event; 
	EFI_STATUS TransactionStatus; 
} EFI_BLOCK_IO2_TOKEN;

typedef 
EFI_STATUS 
(EFIAPI *EFI_BLOCK_RESET_EX) ( 
	IN struct _EFI_BLOCK_IO2_PROTOCOL *This, 
	IN BOOLEAN ExtendedVerification 
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_BLOCK_READ_EX) ( 
	IN struct _EFI_BLOCK_IO2_PROTOCOL *This, 
	IN UINT32 MediaId, IN EFI_LBA LBA, 
	IN OUT EFI_BLOCK_IO2_TOKEN *Token, 
	IN UINTN BufferSize, OUT VOID *Buffer
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_BLOCK_WRITE_EX) ( 
	IN struct _EFI_BLOCK_IO2_PROTOCOL *This, 
	IN UINT32 MediaId, 
	IN EFI_LBA LBA, 
	IN OUT EFI_BLOCK_IO2_TOKEN *Token, 
	IN UINTN BufferSize, 
	IN VOID *Buffer 
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_BLOCK_FLUSH_EX) ( 
	IN struct _EFI_BLOCK_IO2_PROTOCOL *This, 
	IN OUT EFI_BLOCK_IO2_TOKEN *Token
);

typedef struct { 
	UINT32 MediaId; 
	BOOLEAN RemovableMedia; 
	BOOLEAN MediaPresent; 
	BOOLEAN LogicalPartition; 
	BOOLEAN ReadOnly; 
	BOOLEAN WriteCaching; 
	UINT32 BlockSize; 
	UINT32 IoAlign; 
	EFI_LBA LastBlock; 
	EFI_LBA LowestAlignedLba; //added in Revision 2 
	UINT32 LogicalBlocksPerPhysicalBlock; //added in Revision 2 
	UINT32 OptimalTransferLengthGranularity; // added in Revision 3 
} EFI_BLOCK_IO_MEDIA;

typedef 
EFI_STATUS 
(EFIAPI *EFI_BLOCK_RESET) ( 
	IN struct _EFI_BLOCK_IO_PROTOCOL *This, 
	IN BOOLEAN ExtendedVerification 
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_BLOCK_READ) ( 
	IN struct _EFI_BLOCK_IO_PROTOCOL *This, 
	IN UINT32 MediaId, 
	IN EFI_LBA LBA, 
	IN UINTN BufferSize, 
	OUT VOID *Buffer
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_BLOCK_WRITE) ( 
	IN struct _EFI_BLOCK_IO_PROTOCOL *This, 
	IN UINT32 MediaId, 
	IN EFI_LBA LBA, 
	IN UINTN BufferSize, 
	IN VOID *Buffer 
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_BLOCK_FLUSH) ( 
	IN struct _EFI_BLOCK_IO_PROTOCOL *This
);

static EFI_GUID EFI_BLOCK_IO_PROTOCOL_GUID = {.Data1 = 0x964e5b21, .Data2 = 0x6459, .Data3 = 0x11d2, .Data4 = {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

typedef struct _EFI_BLOCK_IO_PROTOCOL { 
	UINT64 Revision; 
	EFI_BLOCK_IO_MEDIA *Media; 
	EFI_BLOCK_RESET Reset; 
	EFI_BLOCK_READ ReadBlocks; 
	EFI_BLOCK_WRITE WriteBlocks; 
	EFI_BLOCK_FLUSH FlushBlocks; 
} EFI_BLOCK_IO_PROTOCOL;

typedef struct _EFI_BLOCK_IO2_PROTOCOL { 
	EFI_BLOCK_IO_MEDIA *Media; 
	EFI_BLOCK_RESET_EX Reset; 
	EFI_BLOCK_READ_EX ReadBlocksEx; 
	EFI_BLOCK_WRITE_EX WriteBlocksEx; 
	EFI_BLOCK_FLUSH_EX FlushBlocksEx; 
} EFI_BLOCK_IO2_PROTOCOL;

#endif
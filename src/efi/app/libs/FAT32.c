#include "FAT32.h"

void ConstructGenericFromFAT32(GENERIC_PARTITION* partition, GENERIC_FILESYSTEM** out) {
	/*ensure support for filesystem on partition (if not supported, return nullptr)
	* then, fill out required function pointers into filesystem struct
	*/
	*out = malloc(sizeof(GENERIC_FILESYSTEM));

	print(L"Reading BPB:\r\n");

	UINTN bSize = partition->buffer->Get_Block_Size(partition->buffer->_buff);

	BPB_FAT_32* bpb = malloc(partition->buffer->Get_Block_Size(partition->buffer->_buff));
	partition->Read_Block(partition->_partition, 0, partition->buffer->Get_Block_Size(partition->buffer->_buff), bpb);

	/*verify integrity of bpb first (also ensure it is not another filesystem type such as ntfs)*/



	/*begin by attempting to mount as fat32, then fat16 (efi partition default), then fat12*/
	/*fat type determined solely by the count of clusters on the volume*/
	uint32_t RootDirSections = ((bpb->BPB_Common.BPB_RootEntCnt * 32) + (bpb->BPB_Common.BPB_BytesPerSec - 1)) / bpb->BPB_Common.BPB_BytesPerSec; //(rounding up division)
	uint32_t FATSz;
	uint32_t TotSec;
	uint32_t DataSec;
	if (bpb->BPB_Common.BPB_FATSz16 != 0) {
		FATSz = bpb->BPB_Common.BPB_FATSz16;
	}
	else {
		FATSz = bpb->BPB_FATSz32;
	}

	if (bpb->BPB_Common.BPB_TotSec16 != 0) {
		FATSz = bpb->BPB_Common.BPB_TotSec16;
	}
	else  {
		FATSz = bpb->BPB_Common.BPB_TotSec32;
	}
	DataSec = TotSec - (bpb->BPB_Common.BPB_RsvdSecCnt + (bpb->BPB_Common.BPB_Num_FATs * FATSz) + RootDirSections); //determine data region sector count
	uint32_t CountOfClusters = DataSec / bpb->BPB_Common.BPB_SecPerClus;

	if (CountOfClusters < 4085) {
		print(L"Volume is FAT12\r\n");
	}
	else if (CountOfClusters < 65525) {
		print(L"Volume is FAT16\r\n");
	}
	else {
		print(L"Volume is FAT32\r\n");
	}

	return;
	if (bpb->BPB_Common.BPB_BytesPerSec != 512 && bpb->BPB_Common.BPB_BytesPerSec != 1024 && bpb->BPB_Common.BPB_BytesPerSec != 2048 && bpb->BPB_Common.BPB_BytesPerSec != 4096) {
		*out = nullptr; /*must not be FAT32 type*/
		return;
	}
	if (bpb->BPB_Common.BPB_Num_FATs == 0) {
		*out = nullptr;
		return;
	}

	/*display some debugging information*/
	CHAR16* info = UINT16ToUnicode(bpb->BPB_Common.BPB_BytesPerSec);
	print(L"    Bytes per sector: ");
	print(info);
	print(L"\r\n");

	info = UINT16ToUnicode(bpb->BPB_Common.BPB_SecPerClus);
	print(L"    Sectors per cluster: ");
	print(info);
	print(L"\r\n");
	free(info);

	info = UINT16ToUnicode(bpb->BPB_Common.BPB_Num_FATs);
	print(L"    Number of FATs: ");
	print(info);
	print(L"\r\n");
	free(info);

	info = UINT32ToUnicode(bpb->BPB_Common.BPB_TotSec32);
	print(L"    Total sectors on volume: ");
	print(info);
	print(L"\r\n");
	free(info);

	info = UINT32ToUnicode(bpb->BPB_FATSz32);
	print(L"    Sectors per FAT: ");
	print(info);   
	print(L"\r\n");
	free(info);

	info = UINT32ToUnicode(bpb->BPB_RootClus);
	print(L"    First cluster of root directory: ");
	print(info);
	print(L"\r\n");
	free(info);
}
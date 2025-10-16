#ifndef MBR_H
#define MBR_H

#define PARTITION_TYPE_MBR 0x01 

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

#endif
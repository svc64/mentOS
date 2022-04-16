#include <stddef.h>
#include "stdlib.h"
#include "ff.h"
#include "diskio.h"
#include "print.h"
#include "ramdisk.h"

extern volatile unsigned char _end; // where our kernel image ends
uint8_t *ramdisk = (uint8_t *)(&_end) + sizeof(struct ramdisk_header);

#define SECTOR_SIZE 512 // Sector size

DSTATUS disk_status(BYTE pdrv) {
	return 0;
}

DSTATUS disk_initialize (BYTE pdrv) {
	return 0;
}

DRESULT disk_read(BYTE pdrv, BYTE *buf, LBA_t sector, UINT count) {
	memcpy(buf, (void *)((uintptr_t)(ramdisk) + (sector * SECTOR_SIZE)), count * SECTOR_SIZE);
	return RES_OK;
}

#if FF_FS_READONLY == 0

DRESULT disk_write(BYTE pdrv, const BYTE *buf, LBA_t sector, UINT count) {
	memcpy((void *)((uintptr_t)(ramdisk) + (sector * SECTOR_SIZE)), buf, count * SECTOR_SIZE);
	return RES_OK;
}

#endif

DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void *buf) {
	switch (cmd) {
		case GET_SECTOR_SIZE:
			*(DWORD *)buf = SECTOR_SIZE;
			break;
		case GET_BLOCK_SIZE:
			*(DWORD *)buf = SECTOR_SIZE;
			break;
		case CTRL_SYNC:
		case CTRL_TRIM:
			return RES_OK;
		case GET_SECTOR_COUNT:
			return RES_PARERR;
	}
	return RES_OK;
}

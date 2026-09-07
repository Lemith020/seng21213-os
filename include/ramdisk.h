#ifndef RAMDISK_H
#define RAMDISK_H

#include "types.h"

#define DISK_SIZE (1024 * 1024)      // 1 MB
#define BLOCK_SIZE 4096              // 4 KB per block
#define NUM_BLOCKS (DISK_SIZE / BLOCK_SIZE) // 256 blocks

void ramdisk_init(void);
void ramdisk_read(uint32_t block_num, void *buf);
void ramdisk_write(uint32_t block_num, void *buf);

#endif

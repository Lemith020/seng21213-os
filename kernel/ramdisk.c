#include "ramdisk.h"
#include "vga.h"

static uint8_t ramdisk[DISK_SIZE]; // BSS - auto zero-initialised

void ramdisk_init(void) {
}

void ramdisk_read(uint32_t block_num, void *buf) {
    if (block_num >= NUM_BLOCKS) {
        return;
    }
    uint8_t *dest = (uint8_t *)buf;
    uint8_t *src = ramdisk + (block_num * BLOCK_SIZE);
    for (int i = 0; i < BLOCK_SIZE; i++) {
        dest[i] = src[i];
    }
}

void ramdisk_write(uint32_t block_num, void *buf) {
    if (block_num >= NUM_BLOCKS) {
        return;
    }
    uint8_t *src = (uint8_t *)buf;
    uint8_t *dest = ramdisk + (block_num * BLOCK_SIZE);
    for (int i = 0; i < BLOCK_SIZE; i++) {
        dest[i] = src[i];
    }
}

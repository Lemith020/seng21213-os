#include "fs.h"
#include "ramdisk.h"

static superblock_t sb;
static uint8_t inode_bitmap[4096];
static uint8_t block_bitmap[4096];
static dir_entry_t directory[MAX_FILES];
#define MAX_INODES 32
static inode_t inode_table[MAX_INODES];

static void bitmap_set(uint8_t *bitmap, int index) {
    bitmap[index / 8] |= (1 << (index % 8));
}

static void bitmap_clear(uint8_t *bitmap, int index) {
    bitmap[index / 8] &= ~(1 << (index % 8));
}

static int bitmap_test(uint8_t *bitmap, int index) {
    return (bitmap[index / 8] >> (index % 8)) & 1;
}

static int alloc_free_bit(uint8_t *bitmap, int max) {
    for (int i = 0; i < max; i++) {
        if (!bitmap_test(bitmap, i)) {
            bitmap_set(bitmap, i);
            return i;
        }
    }
    return -1;
}

static int k_strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) { a++; b++; }
    return (uint8_t)*a - (uint8_t)*b;
}

static void k_strcpy(char *dest, const char *src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

static int dir_find(const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].inode_num != -1 && k_strcmp(directory[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

static int dir_add(const char *name, int32_t inode_num) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].inode_num == -1) {
            k_strcpy(directory[i].name, name);
            directory[i].inode_num = inode_num;
            return 0;
        }
    }
    return -1;
}

static void dir_remove(const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].inode_num != -1 && k_strcmp(directory[i].name, name) == 0) {
            directory[i].inode_num = -1;
            break;
        }
    }
}

void fs_init(void) {
    ramdisk_init();
    ramdisk_read(0, &sb);

    if (sb.magic != MAGIC_NUMBER) {
        sb.magic = MAGIC_NUMBER;
        sb.total_blocks = 256;
        sb.total_inodes = MAX_INODES;
        sb.inode_bitmap_block = 2;
        sb.block_bitmap_block = 3;
        sb.inode_table_start = 4;
        sb.data_start_block = 8;

        for (int i = 0; i < 4096; i++) {
            inode_bitmap[i] = 0;
            block_bitmap[i] = 0;
        }

        for (int i = 0; i < 8; i++) {
            bitmap_set(block_bitmap, i);
        }

        for (int i = 0; i < MAX_INODES; i++) {
            inode_table[i].used = 0;
            inode_table[i].size = 0;
            for (int j = 0; j < 8; j++) inode_table[i].direct_blocks[j] = 0;
        }

        for (int i = 0; i < MAX_FILES; i++) {
            directory[i].inode_num = -1;
        }

        ramdisk_write(0, &sb);
        ramdisk_write(sb.inode_bitmap_block, inode_bitmap);
        ramdisk_write(sb.block_bitmap_block, block_bitmap);
        ramdisk_write(sb.inode_table_start, inode_table);
        ramdisk_write(1, directory);
    } else {
        ramdisk_read(sb.inode_bitmap_block, inode_bitmap);
        ramdisk_read(sb.block_bitmap_block, block_bitmap);
        ramdisk_read(sb.inode_table_start, inode_table);
        ramdisk_read(1, directory);
    }
}

int fs_open(const char *name, int mode) {
    int idx = dir_find(name);
    if (idx != -1) {
        return directory[idx].inode_num;
    }

    if (mode & FS_MODE_CREATE) {
        int inode_num = alloc_free_bit(inode_bitmap, MAX_INODES);
        if (inode_num == -1) return -1;

        inode_table[inode_num].used = 1;
        inode_table[inode_num].size = 0;
        for (int i = 0; i < 8; i++) inode_table[inode_num].direct_blocks[i] = 0;

        if (dir_add(name, inode_num) == -1) {
            bitmap_clear(inode_bitmap, inode_num);
            return -1;
        }

        ramdisk_write(sb.inode_bitmap_block, inode_bitmap);
        ramdisk_write(sb.inode_table_start, inode_table);
        ramdisk_write(1, directory);

        return inode_num;
    }

    return -1;
}

int fs_write(int fd, const void *buf, uint32_t size) {
    if (fd < 0 || fd >= MAX_INODES || !inode_table[fd].used) return -1;
    if (size > MAX_FILE_SIZE) size = MAX_FILE_SIZE;

    inode_t *inode = &inode_table[fd];
    uint32_t blocks_needed = (size + 4096 - 1) / 4096;

    for (uint32_t i = 0; i < blocks_needed; i++) {
        if (inode->direct_blocks[i] == 0) {
            int b = alloc_free_bit(block_bitmap, 256);
            if (b == -1) return -1;
            inode->direct_blocks[i] = b;
        }
    }

    const uint8_t *src = (const uint8_t *)buf;
    uint32_t bytes_left = size;
    for (uint32_t i = 0; i < blocks_needed; i++) {
        uint32_t chunk = (bytes_left > 4096) ? 4096 : bytes_left;
        uint8_t block_buf[4096];
        
        for (int j = 0; j < 4096; j++) block_buf[j] = 0;
        for (uint32_t j = 0; j < chunk; j++) {
            block_buf[j] = src[j];
        }

        ramdisk_write(inode->direct_blocks[i], block_buf);
        src += chunk;
        bytes_left -= chunk;
    }

    inode->size = size;

    ramdisk_write(sb.block_bitmap_block, block_bitmap);
    ramdisk_write(sb.inode_table_start, inode_table);

    return size;
}

int fs_read(int fd, void *buf, uint32_t size) {
    if (fd < 0 || fd >= MAX_INODES || !inode_table[fd].used) return -1;

    inode_t *inode = &inode_table[fd];
    if (size > inode->size) size = inode->size;

    uint8_t *dest = (uint8_t *)buf;
    uint32_t bytes_left = size;
    uint32_t block_idx = 0;

    while (bytes_left > 0 && block_idx < 8) {
        if (inode->direct_blocks[block_idx] == 0) break;

        uint8_t block_buf[4096];
        ramdisk_read(inode->direct_blocks[block_idx], block_buf);

        uint32_t chunk = (bytes_left > 4096) ? 4096 : bytes_left;
        for (uint32_t i = 0; i < chunk; i++) {
            *dest++ = block_buf[i];
        }

        bytes_left -= chunk;
        block_idx++;
    }

    return size;
}

void fs_close(int fd) {
    (void)fd;
}

int fs_unlink(const char *name) {
    int idx = dir_find(name);
    if (idx == -1) return -1;

    int32_t inode_num = directory[idx].inode_num;
    inode_t *inode = &inode_table[inode_num];

    for (int i = 0; i < 8; i++) {
        if (inode->direct_blocks[i] != 0) {
            bitmap_clear(block_bitmap, inode->direct_blocks[i]);
            inode->direct_blocks[i] = 0;
        }
    }

    inode->used = 0;
    inode->size = 0;
    bitmap_clear(inode_bitmap, inode_num);
    dir_remove(name);

    ramdisk_write(sb.inode_bitmap_block, inode_bitmap);
    ramdisk_write(sb.block_bitmap_block, block_bitmap);
    ramdisk_write(sb.inode_table_start, inode_table);
    ramdisk_write(1, directory);

    return 0;
}

void fs_ls(void (*print_func)(const char *)) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].inode_num != -1) {
            print_func("   ");
            print_func(directory[i].name);
            print_func("\n");
        }
    }
}

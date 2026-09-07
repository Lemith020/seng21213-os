#ifndef FS_H
#define FS_H

#include "types.h"

#define MAGIC_NUMBER 0xDEADBEEF
#define MAX_FILES 32
#define MAX_FILE_SIZE (8 * 4096) // 8 blocks * 4KB = 32KB

typedef struct {
    uint32_t magic;
    uint32_t total_blocks;
    uint32_t total_inodes;
    uint32_t inode_bitmap_block;
    uint32_t block_bitmap_block;
    uint32_t inode_table_start;
    uint32_t data_start_block;
} superblock_t;

typedef struct {
    uint32_t size;
    uint32_t direct_blocks[8];
    uint8_t used;
} inode_t;

typedef struct {
    char name[28];
    int32_t inode_num;
} dir_entry_t;

void fs_init(void);
int fs_open(const char *name, int mode);
int fs_write(int fd, const void *buf, uint32_t size);
int fs_read(int fd, void *buf, uint32_t size);
void fs_close(int fd);
int fs_unlink(const char *name);
void fs_ls(void (*print_func)(const char *));

#define FS_MODE_CREATE 1

#endif

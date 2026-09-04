#include "pmm.h"

#define PAGE_SIZE 4096
#define MAX_FRAMES (1024 * 1024 / 4) // 4GB / 4KB

static unsigned char bitmap[MAX_FRAMES / 8];
static uint32_t total_memory = 0;
static uint32_t free_memory = 0;

void pmm_init(uint32_t mem_lower, uint32_t mem_upper) {
    (void)mem_lower;
    total_memory = (mem_upper + 1024) * 1024; // Convert KB to bytes
    free_memory = total_memory;
    
    // Mark all frames as free initially
    for (uint32_t i = 0; i < (MAX_FRAMES / 8); i++) {
        bitmap[i] = 0;
    }
}

uint32_t pmm_alloc_page(void) {
    for (uint32_t i = 0; i < MAX_FRAMES; i++) {
        if (!(bitmap[i / 8] & (1 << (i % 8)))) {
            bitmap[i / 8] |= (1 << (i % 8));
            free_memory -= PAGE_SIZE;
            return i * PAGE_SIZE;
        }
    }
    return 0; // Out of memory
}

void pmm_free_page(uint32_t p) {
    uint32_t frame = p / PAGE_SIZE;
    bitmap[frame / 8] &= ~(1 << (frame % 8));
    free_memory += PAGE_SIZE;
}

uint32_t pmm_get_total_memory(void) {
    return total_memory;
}

uint32_t pmm_get_free_memory(void) {
    return free_memory;
}

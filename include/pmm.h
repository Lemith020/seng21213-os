#ifndef PMM_H
#define PMM_H

#include <stdint.h>

void pmm_init(uint32_t mem_lower, uint32_t mem_upper);
uint32_t pmm_alloc_page(void);
void pmm_free_page(uint32_t p);
uint32_t pmm_get_total_memory(void);
uint32_t pmm_get_free_memory(void);

#endif

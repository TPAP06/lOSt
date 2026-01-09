// kernel/memory/pmm.h - Physical Memory Manager

#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stdbool.h>

// Page size (4KB)
#define PAGE_SIZE 4096
#define PAGE_ALIGN(addr) (((addr) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

// Initialize physical memory manager
void pmm_init(uint32_t total_memory_kb);

// Allocate a physical page (returns physical address)
void* pmm_alloc_page(void);

// Free a physical page
void pmm_free_page(void* page);

// Get memory statistics
uint32_t pmm_get_total_memory(void);
uint32_t pmm_get_used_memory(void);
uint32_t pmm_get_free_memory(void);

// Get number of free pages
uint32_t pmm_get_free_pages(void);

#endif // PMM_H
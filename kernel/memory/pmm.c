// kernel/memory/pmm.c - Physical Memory Manager

#include "pmm.h"
#include "../lib/string.h"

// Bitmap to track pages (1 bit per 4KB page)
// We'll allocate this statically for simplicity
#define MAX_PAGES 32768  // Track up to 128MB (32768 * 4KB)
static uint32_t page_bitmap[MAX_PAGES / 32];  // 1024 uint32_t = 4KB

static uint32_t total_pages = 0;
static uint32_t used_pages = 0;
static uint32_t first_free_page = 0;

// Set a bit in the bitmap
static inline void bitmap_set(uint32_t page)
{
    if (page >= total_pages) return;
    page_bitmap[page / 32] |= (1 << (page % 32));
}

// Clear a bit in the bitmap
static inline void bitmap_clear(uint32_t page)
{
    if (page >= total_pages) return;
    page_bitmap[page / 32] &= ~(1 << (page % 32));
}

// Test if a bit is set
static inline bool bitmap_test(uint32_t page)
{
    if (page >= total_pages) return true;
    return (page_bitmap[page / 32] & (1 << (page % 32))) != 0;
}

// Find first free page
static uint32_t find_first_free(void)
{
    for (uint32_t i = first_free_page; i < total_pages; i++) {
        if (!bitmap_test(i)) {
            return i;
        }
    }
    return (uint32_t)-1;  // No free pages
}

// Initialize PMM
void pmm_init(uint32_t total_memory_kb)
{
    // Clear bitmap
    memset(page_bitmap, 0, sizeof(page_bitmap));
    
    // Calculate total pages
    // Memory layout: assume usable memory starts at 2MB (to be safe)
    // and extends to total_memory_kb
    uint32_t usable_memory = total_memory_kb * 1024;  // Convert to bytes
    
    // Start allocating from 2MB (0x200000)
    uint32_t start_addr = 0x200000;
    
    if (usable_memory > start_addr) {
        total_pages = (usable_memory - start_addr) / PAGE_SIZE;
    } else {
        total_pages = 1024;  // Fallback: at least 4MB
    }
    
    // Limit to our bitmap size
    if (total_pages > MAX_PAGES) {
        total_pages = MAX_PAGES;
    }
    
    used_pages = 0;
    first_free_page = 0;
}

// Allocate a page
void* pmm_alloc_page(void)
{
    uint32_t page = find_first_free();
    
    if (page == (uint32_t)-1) {
        return NULL;  // Out of memory
    }
    
    bitmap_set(page);
    used_pages++;
    
    // Update first_free_page hint
    if (page == first_free_page) {
        first_free_page = find_first_free();
    }
    
    // Convert page number to physical address
    // Pages start at 2MB (0x200000)
    return (void*)(0x200000 + (page * PAGE_SIZE));
}

// Free a page
void pmm_free_page(void* page_addr)
{
    if (page_addr == NULL) return;
    
    uint32_t addr = (uint32_t)page_addr;
    
    // Check if address is valid
    if (addr < 0x200000) return;
    
    // Calculate page number
    uint32_t page = (addr - 0x200000) / PAGE_SIZE;
    
    if (page >= total_pages) return;
    
    if (bitmap_test(page)) {
        bitmap_clear(page);
        used_pages--;
        
        // Update hint if this is lower
        if (page < first_free_page) {
            first_free_page = page;
        }
    }
}

// Get statistics
uint32_t pmm_get_total_memory(void)
{
    return (total_pages * PAGE_SIZE) / 1024;  // KB
}

uint32_t pmm_get_used_memory(void)
{
    return (used_pages * PAGE_SIZE) / 1024;  // KB
}

uint32_t pmm_get_free_memory(void)
{
    return ((total_pages - used_pages) * PAGE_SIZE) / 1024;  // KB
}

uint32_t pmm_get_free_pages(void)
{
    return total_pages - used_pages;
}
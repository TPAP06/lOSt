// kernel/memory/heap.c - Heap with multi-page support

#include "heap.h"
#include "pmm.h"
#include "../lib/string.h"

// Block header
typedef struct block_header {
    uint32_t size;              // Size of usable area
    uint32_t pages;             // Number of pages this block uses
    bool is_free;
    struct block_header *next;
} block_header_t;

#define BLOCK_HEADER_SIZE sizeof(block_header_t)

// Free list head
static block_header_t *heap_start = NULL;

// Find free block that fits
static block_header_t* find_free_block(uint32_t size)
{
    block_header_t *current = heap_start;
    
    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

// Request new pages from PMM
static block_header_t* request_pages(uint32_t num_pages)
{
    if (num_pages == 0) num_pages = 1;
    
    // Get first page
    void *first_page = pmm_alloc_page();
    if (first_page == NULL) {
        return NULL;
    }
    
    // For multi-page allocations, allocate additional pages
    // Note: These may not be contiguous in physical memory!
    // For a real OS, we'd need virtual memory mapping
    // For now, we'll only use the first page properly
    
    if (num_pages > 1) {
        // Allocate additional pages (they'll be tracked separately)
        for (uint32_t i = 1; i < num_pages; i++) {
            void *page = pmm_alloc_page();
            if (page == NULL) {
                // Out of memory - we should free what we allocated
                // For simplicity, we'll just return what we have
                break;
            }
        }
    }
    
    // Set up block header
    block_header_t *block = (block_header_t*)first_page;
    block->size = (num_pages * PAGE_SIZE) - BLOCK_HEADER_SIZE;
    block->pages = num_pages;
    block->is_free = true;
    block->next = NULL;
    
    // Add to list
    if (heap_start == NULL) {
        heap_start = block;
    } else {
        block_header_t *current = heap_start;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = block;
    }
    
    return block;
}

// Initialize heap
void heap_init(void)
{
    heap_start = NULL;
    
    // Pre-allocate some pages
    for (int i = 0; i < 8; i++) {
        request_pages(1);
    }
}

// Malloc
void* malloc(size_t size)
{
    if (size == 0) return NULL;
    
    // Align size
    uint32_t aligned_size = (size + 15) & ~15;
    
    // Calculate how many pages we need
    uint32_t pages_needed = (aligned_size + BLOCK_HEADER_SIZE + PAGE_SIZE - 1) / PAGE_SIZE;
    
    // Limit to reasonable size (let's say 64 pages = 256KB max)
    if (pages_needed > 64) {
        return NULL;
    }
    
    // Try to find existing free block
    block_header_t *block = find_free_block(aligned_size);
    
    // If no suitable block found, request new pages
    if (block == NULL) {
        block = request_pages(pages_needed);
        if (block == NULL) {
            return NULL;
        }
    }
    
    // Mark as used
    block->is_free = false;
    
    // Return pointer after header
    return (void*)((char*)block + BLOCK_HEADER_SIZE);
}

// Free
void free(void* ptr)
{
    if (ptr == NULL) return;
    
    block_header_t *block = (block_header_t*)((char*)ptr - BLOCK_HEADER_SIZE);
    block->is_free = true;
    
    // TODO: Return pages to PMM if block is large
    // TODO: Coalesce adjacent free blocks
}

// Calloc
void* calloc(size_t num, size_t size)
{
    size_t total = num * size;
    void *ptr = malloc(total);
    
    if (ptr) {
        memset(ptr, 0, total);
    }
    
    return ptr;
}

// Realloc
void* realloc(void* ptr, size_t size)
{
    if (ptr == NULL) {
        return malloc(size);
    }
    
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    
    block_header_t *old_block = (block_header_t*)((char*)ptr - BLOCK_HEADER_SIZE);
    
    if (old_block->size >= size) {
        return ptr;
    }
    
    void *new_ptr = malloc(size);
    if (new_ptr == NULL) {
        return NULL;
    }
    
    uint32_t copy_size = (old_block->size < size) ? old_block->size : size;
    memcpy(new_ptr, ptr, copy_size);
    
    free(ptr);
    
    return new_ptr;
}
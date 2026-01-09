// kernel/memory/heap.c - Simple heap allocator

#include "heap.h"
#include "pmm.h"
#include "../lib/string.h"

// Block header for allocated memory
typedef struct block_header {
    uint32_t size;              // Size of block (including header)
    bool is_free;               // Is this block free?
    struct block_header *next;  // Next block in list
} block_header_t;

#define BLOCK_HEADER_SIZE sizeof(block_header_t)
#define ALIGN_SIZE 16

// Head of free list
static block_header_t *heap_start = NULL;

// Align size to ALIGN_SIZE boundary
static inline uint32_t align_size(uint32_t size)
{
    return (size + ALIGN_SIZE - 1) & ~(ALIGN_SIZE - 1);
}

// Find a free block that fits
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

// Request more memory from PMM
static block_header_t* request_memory(uint32_t size)
{
    // Allocate pages
    uint32_t pages_needed = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    
    block_header_t *block = NULL;
    
    for (uint32_t i = 0; i < pages_needed; i++) {
        void *page = pmm_alloc_page();
        if (page == NULL) {
            return NULL;  // Out of memory
        }
        
        if (i == 0) {
            block = (block_header_t*)page;
        }
    }
    
    if (block) {
        block->size = pages_needed * PAGE_SIZE;
        block->is_free = false;
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
    }
    
    return block;
}

// Initialize heap
void heap_init(void)
{
    heap_start = NULL;
}

// Malloc
void* malloc(size_t size)
{
    if (size == 0) return NULL;
    
    // Align size and add header
    uint32_t total_size = align_size(size + BLOCK_HEADER_SIZE);
    
    // Find free block or request new memory
    block_header_t *block = find_free_block(total_size);
    
    if (block == NULL) {
        block = request_memory(total_size);
        if (block == NULL) {
            return NULL;  // Out of memory
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
    
    // Get block header
    block_header_t *block = (block_header_t*)((char*)ptr - BLOCK_HEADER_SIZE);
    
    // Mark as free
    block->is_free = true;
    
    // TODO: Coalesce adjacent free blocks (optimization for later)
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
    
    // Get old block
    block_header_t *old_block = (block_header_t*)((char*)ptr - BLOCK_HEADER_SIZE);
    
    // If new size fits in old block, just return it
    if (old_block->size >= size + BLOCK_HEADER_SIZE) {
        return ptr;
    }
    
    // Allocate new block
    void *new_ptr = malloc(size);
    if (new_ptr == NULL) {
        return NULL;
    }
    
    // Copy old data
    uint32_t old_size = old_block->size - BLOCK_HEADER_SIZE;
    uint32_t copy_size = (old_size < size) ? old_size : size;
    memcpy(new_ptr, ptr, copy_size);
    
    // Free old block
    free(ptr);
    
    return new_ptr;
}
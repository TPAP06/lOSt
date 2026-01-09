// kernel/memory/heap.h - Simple heap allocator

#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stddef.h>

// Initialize heap
void heap_init(void);

// Allocate memory
void* malloc(size_t size);

// Free memory
void free(void* ptr);

// Reallocate memory
void* realloc(void* ptr, size_t size);

// Allocate and zero memory
void* calloc(size_t num, size_t size);

#endif // HEAP_H
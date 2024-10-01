#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define PAGE_SIZE 4096
#define KERNEL_BASE 0xffffffff80000000

// Physical memory management
void init_physical_memory(uint64_t mem_size);
void* allocate_physical_page();
void free_physical_page(void* page);

// Virtual memory management
void init_virtual_memory();
void map_page(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags);
void unmap_page(uint64_t virtual_addr);
uint64_t get_physical_address(uint64_t virtual_addr);

// Heap memory management
void init_heap();
void* kmalloc(size_t size);
void kfree(void* ptr);

// Utility functions
uint64_t read_cr3();
void write_cr3(uint64_t value);

// Memory information
typedef struct {
    uint64_t total_memory;
    uint64_t free_memory;
    uint64_t used_memory;
    uint64_t reserved_memory;
} MemoryInfo;

void get_memory_info(MemoryInfo* info);

#endif // MEMORY_H

#include "memory.h"
#include "vga.h"

#define BITMAP_SIZE 32768 // 32768 * 64 = 2097152 pages = 8GB of RAM

static uint64_t* physical_bitmap;
static uint64_t total_pages;
static uint64_t free_pages;

// Heap
#define HEAP_START 0xffffffff80400000
#define HEAP_SIZE  0x400000 // 4MB initial heap

typedef struct HeapBlock {
    size_t size;
    bool is_free;
    struct HeapBlock* next;
} HeapBlock;

static HeapBlock* heap_start;

void init_physical_memory(uint64_t mem_size) {
    total_pages = mem_size / PAGE_SIZE;
    free_pages = total_pages;

    // Allocate bitmap at a fixed address
    physical_bitmap = (uint64_t*)0xffffffff80200000;
    
    // Clear bitmap
    for (uint64_t i = 0; i < BITMAP_SIZE; i++) {
        physical_bitmap[i] = 0;
    }

    // Mark first 1MB as used
    uint64_t reserved_pages = 256; // 1MB / 4KB
    for (uint64_t i = 0; i < reserved_pages; i++) {
        uint64_t idx = i / 64;
        uint64_t bit = i % 64;
        physical_bitmap[idx] |= (1ULL << bit);
    }
    free_pages -= reserved_pages;
}

void* allocate_physical_page() {
    for (uint64_t i = 0; i < BITMAP_SIZE; i++) {
        if (physical_bitmap[i] != 0xFFFFFFFFFFFFFFFF) {
            for (int j = 0; j < 64; j++) {
                if ((physical_bitmap[i] & (1ULL << j)) == 0) {
                    physical_bitmap[i] |= (1ULL << j);
                    free_pages--;
                    return (void*)((i * 64 + j) * PAGE_SIZE);
                }
            }
        }
    }
    return NULL;
}

void free_physical_page(void* page) {
    uint64_t page_num = (uint64_t)page / PAGE_SIZE;
    uint64_t idx = page_num / 64;
    uint64_t bit = page_num % 64;
    physical_bitmap[idx] &= ~(1ULL << bit);
    free_pages++;
}

// Page table manipulation functions
static uint64_t* get_next_level(uint64_t* table, uint64_t index, bool allocate) {
    if ((table[index] & 1) == 0) {
        if (!allocate) return NULL;
        uint64_t new_table = (uint64_t)allocate_physical_page();
        if (new_table == 0) return NULL;
        table[index] = new_table | 3; // present + writable
        return (uint64_t*)(new_table + KERNEL_BASE);
    }
    return (uint64_t*)((table[index] & ~0xFFF) + KERNEL_BASE);
}

void map_page(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags) {
    uint64_t pml4_index = (virtual_addr >> 39) & 0x1FF;
    uint64_t pdpt_index = (virtual_addr >> 30) & 0x1FF;
    uint64_t pd_index = (virtual_addr >> 21) & 0x1FF;
    uint64_t pt_index = (virtual_addr >> 12) & 0x1FF;

    uint64_t* pml4 = (uint64_t*)read_cr3();
    uint64_t* pdpt = get_next_level(pml4, pml4_index, true);
    uint64_t* pd = get_next_level(pdpt, pdpt_index, true);
    uint64_t* pt = get_next_level(pd, pd_index, true);

    pt[pt_index] = physical_addr | flags;
}

void unmap_page(uint64_t virtual_addr) {
    uint64_t pml4_index = (virtual_addr >> 39) & 0x1FF;
    uint64_t pdpt_index = (virtual_addr >> 30) & 0x1FF;
    uint64_t pd_index = (virtual_addr >> 21) & 0x1FF;
    uint64_t pt_index = (virtual_addr >> 12) & 0x1FF;

    uint64_t* pml4 = (uint64_t*)read_cr3();
    uint64_t* pdpt = get_next_level(pml4, pml4_index, false);
    if (!pdpt) return;
    uint64_t* pd = get_next_level(pdpt, pdpt_index, false);
    if (!pd) return;
    uint64_t* pt = get_next_level(pd, pd_index, false);
    if (!pt) return;

    pt[pt_index] = 0;
}

uint64_t get_physical_address(uint64_t virtual_addr) {
    uint64_t pml4_index = (virtual_addr >> 39) & 0x1FF;
    uint64_t pdpt_index = (virtual_addr >> 30) & 0x1FF;
    uint64_t pd_index = (virtual_addr >> 21) & 0x1FF;
    uint64_t pt_index = (virtual_addr >> 12) & 0x1FF;

    uint64_t* pml4 = (uint64_t*)read_cr3();
    uint64_t* pdpt = get_next_level(pml4, pml4_index, false);
    if (!pdpt) return 0;
    uint64_t* pd = get_next_level(pdpt, pdpt_index, false);
    if (!pd) return 0;
    uint64_t* pt = get_next_level(pd, pd_index, false);
    if (!pt) return 0;

    return (pt[pt_index] & ~0xFFF) | (virtual_addr & 0xFFF);
}

void init_heap() {
    heap_start = (HeapBlock*)HEAP_START;
    heap_start->size = HEAP_SIZE - sizeof(HeapBlock);
    heap_start->is_free = true;
    heap_start->next = NULL;

    // Map heap pages
    for (uint64_t addr = HEAP_START; addr < HEAP_START + HEAP_SIZE; addr += PAGE_SIZE) {
        void* page = allocate_physical_page();
        if (page == NULL) {
            vga_writestring("Failed to allocate heap pages\n");
            return;
        }
        map_page(addr, (uint64_t)page, 3); // present + writable
    }
}

void* kmalloc(size_t size) {
    size = (size + 15) & ~15; // Align to 16 bytes

    HeapBlock* current = heap_start;
    while (current) {
        if (current->is_free && current->size >= size + sizeof(HeapBlock)) {
            if (current->size > size + sizeof(HeapBlock) + 16) {
                // Split the block
                HeapBlock* new_block = (HeapBlock*)((char*)current + sizeof(HeapBlock) + size);
                new_block->size = current->size - size - sizeof(HeapBlock);
                new_block->is_free = true;
                new_block->next = current->next;
                
                current->size = size;
                current->next = new_block;
            }
            current->is_free = false;
            return (void*)((char*)current + sizeof(HeapBlock));
        }
        current = current->next;
    }
    return NULL; // Out of memory
}

void kfree(void* ptr) {
    if (!ptr) return;

    HeapBlock* block = (HeapBlock*)((char*)ptr - sizeof(HeapBlock));
    block->is_free = true;

    // Coalesce with next block if it's free
    if (block->next && block->next->is_free) {
        block->size += sizeof(HeapBlock) + block->next->size;
        block->next = block->next->next;
    }

    // Coalesce with previous block if it's free
    HeapBlock* prev = heap_start;
    while (prev && prev->next != block) {
        prev = prev->next;
    }
    if (prev && prev->is_free) {
        prev->size += sizeof(HeapBlock) + block->size;
        prev->next = block->next;
    }
}

uint64_t read_cr3() {
    uint64_t cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}

void write_cr3(uint64_t value) {
    asm volatile("mov %0, %%cr3" : : "r"(value));
}

void get_memory_info(MemoryInfo* info) {
    info->total_memory = total_pages * PAGE_SIZE;
    info->free_memory = free_pages * PAGE_SIZE;
    info->used_memory = (total_pages - free_pages) * PAGE_SIZE;
    info->reserved_memory = 1024 * 1024; // 1MB reserved
}

void init_virtual_memory() {
    // Identity map the first 1GB
    for (uint64_t addr = 0; addr < 0x40000000; addr += PAGE_SIZE) {
        map_page(addr, addr, 3); // present + writable
    }

    // Map kernel to higher half
    for (uint64_t addr = 0; addr < 0x40000000; addr += PAGE_SIZE) {
        map_page(addr + KERNEL_BASE, addr, 3); // present + writable
    }

    // Load new page table
    write_cr3(read_cr3());
}

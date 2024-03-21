#pragma once
#include <stdint.h>
#include <stddef.h>
#include "terminal.h"
#include <limine.h>
#define PAGE_STATUS_UNUSABLE 0
#define PAGE_STATUS_USABLE   1
#define PAGE_STATUS_INVALID_INDEX -1   
inline volatile struct limine_memmap_request limine_memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};


struct MemoryMap {
    void* base;
    size_t pageCount;
    size_t pageSize;
    size_t byteSize;
    size_t MaxReclaimableIndex;
    inline void setPageStatus(size_t i, char status) {
       char* pageSet = &((char*)base)[i/8];
       *pageSet &= ~(1 << i%8);
       *pageSet |= (status << i%8);
    }
    inline char getPageStatus(size_t i) {
       if(i>pageCount) {
          return -1;                
       }
       char* pageSet = &((char*)base)[i/8];
       return ((*pageSet) & (1<<i%8))>>i%8;
    }
    int8_t deallocate(void* where, size_t paCount);
    void* allocate(size_t paCount);
};

inline struct MemoryMap GlobalMemoryMap={NULL,0,0};

/*
#define LIMINE_MEMMAP_USABLE                 0
#define LIMINE_MEMMAP_RESERVED               1
#define LIMINE_MEMMAP_ACPI_RECLAIMABLE       2
#define LIMINE_MEMMAP_ACPI_NVS               3
#define LIMINE_MEMMAP_BAD_MEMORY             4
#define LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE 5
#define LIMINE_MEMMAP_KERNEL_AND_MODULES     6
#define LIMINE_MEMMAP_FRAMEBUFFER            7
*/
Pixel limine_memmap_type_to_color(uint64_t type);
const char* limine_memmap_type_to_string(uint64_t type);

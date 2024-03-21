#include "memmap.h"
int8_t MemoryMap::deallocate(void* where, size_t paCount) {
    if(!where) return -1;
    size_t wherep = ((size_t)where/4096);
    auto entry = limine_memmap_request.response->entries[MaxReclaimableIndex];
    size_t pageLen=((entry->base+entry->length)/4096);
    if(wherep+paCount>pageLen) return -2;
    for(size_t i=wherep; i<wherep+paCount; ++i) {
       setPageStatus(i, PAGE_STATUS_USABLE);
    }
    return 0;
}
void* MemoryMap::allocate(size_t paCount) {
   for(size_t i=0; i<MaxReclaimableIndex; ++i) {
       if(limine_memmap_request.response->entries[i]->type == LIMINE_MEMMAP_USABLE) {
           size_t pageBegin  = limine_memmap_request.response->entries[i]->base/4096;
           size_t pIndex = 0;
           size_t pageLength = limine_memmap_request.response->entries[i]->length/4096;
           if(pageLength<paCount) continue;
           for(; pIndex < pageLength-paCount+1; pIndex++) {
               char status = PAGE_STATUS_USABLE;
               size_t pi_Index=0;
               for(; pi_Index<paCount; pi_Index++) {
                   if(getPageStatus(pageBegin+pIndex+pi_Index)==PAGE_STATUS_UNUSABLE) {
                      status = PAGE_STATUS_UNUSABLE;
                      break;
                   }
               }
               if(status==PAGE_STATUS_USABLE) {
                   for(size_t j=0; j<paCount; j++) {
                      setPageStatus(j+pIndex+pageBegin, PAGE_STATUS_UNUSABLE);
                   }
                   return (void*)((pageBegin+pIndex)*4096);
               } 
          }
       }
   }
   return NULL;
}

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
Pixel limine_memmap_type_to_color(uint64_t type) {
  switch(type) {
        case LIMINE_MEMMAP_USABLE:
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
        case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
           return Pixel(0x00,0xFF,0x00,0xFF);
        case LIMINE_MEMMAP_RESERVED:
           return Pixel(0x00,0x00,0xFF,0xFF);
        case LIMINE_MEMMAP_FRAMEBUFFER:
           return Pixel(0x88,0x88,0x88,0xFF);
        case LIMINE_MEMMAP_BAD_MEMORY:
           return Pixel(0xFF,0x00,0x00,0xFF);
        default:
           return Pixel(0xFF,0xFF,0xFF,0xFF);
  }
}
const char* limine_memmap_type_to_string(uint64_t type) {
   switch(type) {
        case LIMINE_MEMMAP_USABLE:
           return "usable";
        case LIMINE_MEMMAP_RESERVED:
           return "reserved";
        case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
           return "acpi reclaimable";
        case LIMINE_MEMMAP_ACPI_NVS:
           return "acpi nvs";
        case LIMINE_MEMMAP_BAD_MEMORY:
           return "bad memory";
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
           return "bootloader reclaimable";
        case LIMINE_MEMMAP_KERNEL_AND_MODULES:
           return "kernel and modules";
        case LIMINE_MEMMAP_FRAMEBUFFER:
           return "framebuffer";
        default:
           return "unknown";
   }
}



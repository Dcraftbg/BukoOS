#include <limine.h>
#include <cstddef>
#include <limits.h>
#include "libs/memory.h"
#include "libs/display.h"
#include "libs/terminal.h"
#include "config.h"
#define SYS_DIST_URL "https://github.com/Dcraftbg/BukoOS"
#define SYS_VERSION "0.3.1A"
#ifdef BUKO_DEBUG
   #define SYS_MODE "Debug"
#elif defined(BUKO_RELEASE)
   #define SYS_MODE "Release"
#elif defined(BUKO_DIST)
   #define SYS_MODE "Dist"
#else
   #error "Running in non recognized more! Please run with BUKO_DEBUG, BUKO_RELEASE or BUKO_DIST"
#endif
#define MEMORY_DISPLAY_SIZE (112.0/2.0)
static volatile limine_framebuffer_request limine_framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

static volatile limine_memmap_request limine_memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};
#define PAGE_STATUS_UNUSABLE 0
#define PAGE_STATUS_USABLE   1
#define PAGE_STATUS_INVALID_INDEX -1                // note: only returned by getPageStatus if the index is out of range
// Consider:
// DisplayInfo globalInfo={0};

#include "libs/string.h"
struct MemoryMap {
    void* base;
    size_t pageCount;
    size_t pageSize;
    size_t byteSize;
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
    char deallocate(void* where, size_t paCount) {
        if(!where) return -1;
        size_t wherep = ((size_t)where/4096);
        for(size_t i=wherep; i<wherep+paCount; ++i) {
           setPageStatus(i, PAGE_STATUS_USABLE);
        }
        return 0;
    }
    void* allocate(size_t paCount) {
        if(paCount>=pageCount) return NULL;
        for(size_t i=0; i<pageCount-paCount; ++i) {
            char status = PAGE_STATUS_USABLE;
            for(size_t paIndex=0; paIndex<paCount; paIndex++) {
                if(getPageStatus(i+paIndex)==PAGE_STATUS_UNUSABLE) {
                   status=PAGE_STATUS_UNUSABLE;
                   break;
                }
            }
            if(status==PAGE_STATUS_USABLE) {
                for(size_t j=0; j<paCount; j++) {
                   setPageStatus(i+j, PAGE_STATUS_UNUSABLE);
                }
                return (void*)(i*4096); // convert it to real address
            }
        }
        return NULL;
    }
};
inline size_t addressToIndex(void* addr) {
   return ((uintptr_t)addr)/4096;
}
MemoryMap GlobalMemoryMap={NULL,0,0};

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
           //return Pixel(0x88,0xFF,0x88,0xFF);   // light green

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
#include "libs/memory.h"

extern "C" void kernel() {    
	if( limine_framebuffer_request.response == NULL || limine_framebuffer_request.response->framebuffer_count != 1 || limine_framebuffer_request.response->framebuffers[0] -> bpp != 32 )
		for(;;);
    if( limine_memmap_request.response == NULL)
        for(;;);
    size_t height = limine_framebuffer_request.response->framebuffers[0]->height;
    size_t width = limine_framebuffer_request.response->framebuffers[0]->width;
    size_t pitch = limine_framebuffer_request.response->framebuffers[0]->pitch;
	Pixel* pixels = (Pixel*) limine_framebuffer_request.response -> framebuffers[ 0 ] -> address;
    Pixel* orgpixels = pixels;
    for(size_t y=0; y<height-20; ++y) {
        for (size_t x=0; x<width; ++x) {
             pixels[x] = CONFIG_BACKGROUND_COLOR;
         }
        pixels = (Pixel*)((uintptr_t)pixels +  pitch);
    }
    for(size_t y=height-20; y<height; ++y) {
        for (size_t x=0; x<width; ++x) {
        #ifndef CONFIG_NO_PROGRESSBAR
            pixels[x] = CONFIG_PROGRESS_BAR_COLOR;
        #else
            pixels[x] = CONFIG_BACKGROUND_COLOR;
        #endif
        }
        pixels = (Pixel*)((uintptr_t)pixels +  pitch);
    }
    pixels=orgpixels;
    DisplayInfo display(pixels, width, height, pitch);
    stdTerminal::pb_printf(display, "Loading display information...");
    volatile bool running=true;


    putStr(display, "Hello and welcome to BukoOS!\n");
    putStr(display, "Distribution from Dcraftbg " SYS_DIST_URL "\n"); 
    stdTerminal::printf(display, "System version %s in %s mode\n", SYS_VERSION, SYS_MODE);
#ifdef BUKO_PRINT_MEM_INFO
    stdTerminal::printf(display, "Number of map entries available: %d\n", (int)limine_memmap_request.response->entry_count);
#endif
    size_t AvailableMemory=0;
    size_t totalMemory=0;
    int64_t MaxReclaimableIndex=-1;
    //size_t MAxReclaimableSize=0;
    int64_t BiggestUsableIndex=-1;
    size_t  BiggestUsableSize = 0;

    stdTerminal::pb_printf(display, "Loading memory information..." );
    for(size_t i = 0; i < limine_memmap_request.response->entry_count-1; ++i) {
#ifdef BUKO_PRINT_MEM_INFO
        if(i < 10)  stdTerminal::printf(display, "%d ", i);
        else stdTerminal::printf(display, "%d", i);
        stdTerminal::printf(display, "> base: %p length: %l (%l bytes) pages type: (%s)\n",limine_memmap_request.response->entries[i]->base, (int64_t)(limine_memmap_request.response->entries[i]->length/4096), (int64_t)(limine_memmap_request.response->entries[i]->length), limine_memmap_type_to_string(limine_memmap_request.response->entries[i]->type));
#endif
        switch(limine_memmap_request.response->entries[i]->type) {
                case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
                case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
                        MaxReclaimableIndex=i;
                        break;
                case LIMINE_MEMMAP_USABLE:
                        if(BiggestUsableSize<limine_memmap_request.response->entries[i]->length) {
                           BiggestUsableSize=limine_memmap_request.response->entries[i]->length;
                           BiggestUsableIndex=i;
                        }
                        break;
                default:
                        break;
        }
        if(limine_memmap_request.response->entries[i]->type==LIMINE_MEMMAP_USABLE) AvailableMemory+=limine_memmap_request.response->entries[i]->length;
        totalMemory+=limine_memmap_request.response->entries[i]->length;
    }
    if(MaxReclaimableIndex==-1) {
        stdTerminal::printf(display, "Warning: Could not find ANY Bootloader/ACPI reclaimable memory entry");
        for(;;) asm volatile("hlt");
    } 
    if(BiggestUsableIndex==-1) {
        stdTerminal::printf(display, "Warning: Could not find ANY Usable memory entry");
        for(;;) asm volatile("hlt" : "+g"(running));
    } 


    totalMemory = limine_memmap_request.response->entries[limine_memmap_request.response->entry_count-2]->base + limine_memmap_request.response->entries[limine_memmap_request.response->entry_count-2]->length;
#ifdef BUKO_PRINT_MEM_INFO
    stdTerminal::printf(display, "Total available memory: %lb %lKib %lMb\n", AvailableMemory, AvailableMemory/1024, AvailableMemory/(1024*1000));
    //stdTerminal::printf(display, "Base: %l\n",limine_memmap_request.response->entries[limine_memmap_request.response->entry_count-2]->base);

    stdTerminal::printf(display, "All memory: %lb %lKib %lMib %lGib\n",totalMemory, totalMemory/1024, totalMemory/(1024*1000), totalMemory/(1024*1000*1000));
    for(size_t i = 0; i < 9; i++) {
        putC(display,'x', limine_memmap_type_to_color(i)); stdTerminal::printf(display, " - %s\n",limine_memmap_type_to_string(i));
    }
    putC(display, '<');
    for(size_t i = 0; i < limine_memmap_request.response->entry_count-1; ++i) {
        double ratio = ((double)limine_memmap_request.response->entries[i]->length/(double)totalMemory);
        for(size_t j=0; j < (size_t)(ratio*MEMORY_DISPLAY_SIZE+1); ++j) {
            putStr(display, "=", limine_memmap_type_to_color(limine_memmap_request.response->entries[i]->type));
        }
    }
    putC(display, '>');
    putC(display,'\n');
#endif
    {
        auto mapentry            = limine_memmap_request.response->entries[BiggestUsableIndex];
        GlobalMemoryMap.base     = (void*)mapentry->base;
        GlobalMemoryMap.pageCount= totalMemory/4096;//(limine_memmap_request.response->entries[limine_memmap_request.response->entry_count-1]->base + limine_memmap_request.response->entries[limine_memmap_request.response->entry_count-1]->length)/4096;//totalMemory/4096;
        GlobalMemoryMap.byteSize = GlobalMemoryMap.pageCount/8;
    }
    if(GlobalMemoryMap.byteSize==0) {
        stdTerminal::printf(display, "ERROR: Cannot allocate MemMapByteSize of 0\n");
        for(;;) asm volatile("hlt" : "+g"(running));
    } 
    {
       GlobalMemoryMap.pageSize = (GlobalMemoryMap.byteSize-1)/4096+1;
       memset((void*)GlobalMemoryMap.base, 0, GlobalMemoryMap.pageSize*4096);
    }
#ifdef BUKO_PRINT_MEM_INFO
    stdTerminal::printf(display, "Allocating memory map at: %p with size of: %l bytes (%l pages, real size(%l)) %l amount of pages mapped\n",GlobalMemoryMap.base,GlobalMemoryMap.pageSize*4096, GlobalMemoryMap.pageSize,GlobalMemoryMap.byteSize, GlobalMemoryMap.pageCount);
#endif
    stdTerminal::pb_printf(display, "Setting memory map entries...");
    for(size_t i=0; i < limine_memmap_request.response->entry_count-1; ++i) {
        auto entry = limine_memmap_request.response->entries[i];
        size_t entryPageCount = entry->length/4096;
        if(entry->type == LIMINE_MEMMAP_USABLE) { 
            size_t PageIndex=(entry->base/4096);
            //stdTerminal::printf(display,"Got to usable. Starting at page index: %l, length: %l\n", PageIndex, entryPageCount);
            for(size_t j =((i==(size_t)BiggestUsableIndex) ? GlobalMemoryMap.pageSize : 0); j < entryPageCount; ++j) {
                char* pageSet = &((char*)GlobalMemoryMap.base)[(PageIndex+j)/8];
                *pageSet |= (1 << (PageIndex+j)%8);
            }
        }
    }
#ifdef BUKO_TEST_MEMORY_PAGE_MAP
    stdTerminal::pb_printf(display, "Testing memory map...");
    bool passed=true;
    size_t failureEntryIndex=0;
    size_t failurePageIndex =0;

    for(size_t i=0; i < limine_memmap_request.response->entry_count-1; ++i) {
        if(!passed) break;
        auto entry = limine_memmap_request.response->entries[i];
        size_t PageStart = (entry->base / 4096);
        size_t PageCount = entry->length/4096;
        //stdTerminal::printf(display, "%l> Starting at %l with %l pages left\n",i,PageStart,PageCount);
        for(size_t j=PageStart+(i==(size_t)BiggestUsableIndex ? GlobalMemoryMap.pageSize : 0); j < PageStart+PageCount; ++j) {
           switch(entry->type) {
           case LIMINE_MEMMAP_USABLE:
                passed = GlobalMemoryMap.getPageStatus(j)==PAGE_STATUS_USABLE;
                if(!passed) {
                   failureEntryIndex=i;
                   failurePageIndex=j;
                }
                break;
           default:
                passed = GlobalMemoryMap.getPageStatus(j)==PAGE_STATUS_UNUSABLE;
                if(!passed) {
                   failureEntryIndex=i;
                   failurePageIndex=j;
                }
           }
        }
    }
    if(passed) {
        stdTerminal::printf(display,"Success! Passed verification check!\n");
    } else {
        stdTerminal::printf(display,"ERROR: Failed to verify check! at entry %l, page %l\n",(int64_t)failureEntryIndex,(int64_t)failurePageIndex);
    }
#endif
#if 0
    size_t pcount = 30000;
    void* buf = GlobalMemoryMap.allocate(pcount);
    stdTerminal::printf(display, "Allocated %l pages at %p\n", pcount, buf);
    stdTerminal::printf(display, "Freeing %l pages at %p\n", pcount, buf);
    GlobalMemoryMap.deallocate(buf, pcount);
    //stdTerminal::printf(display,"Allocated 79 pages at %p\n",GlobalMemoryMap.allocate(79));
    stdTerminal::printf(display,"Allocated %l pages at %p\n", pcount,GlobalMemoryMap.allocate(pcount));
#endif
    stdTerminal::pb_printf(display, "Done!");
    putStr(display, "> ");
    while(running) {
        asm volatile("hlt" : "+g"(running));
    }
}

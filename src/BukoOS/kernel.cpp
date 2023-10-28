#include <limine.h>
#include <cstddef>
#include <limits.h>
#include "libs/memory.h"
#include "libs/display.h"
#include "libs/terminal.h"
#include "libs/gdt.h"
#include "libs/idt.h"
#include "libs/portlib.h"
#include "libs/p2keyboard.h"
#include "libs/stdkeyboard.h"
#include "libs/memory.h"
#include "libs/memmap.h"
#include "libs/string.h"
#include "libs/kernelUtils.h"
#include "drivers/keyboard.h"
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
   #error "Running in non recognized mode! Please run with BUKO_DEBUG, BUKO_RELEASE or BUKO_DIST"
#endif
#define MEMORY_DISPLAY_SIZE (112.0/2.0)
// note: only returned by getPageStatus if the index is out of range^
static volatile limine_framebuffer_request limine_framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

// Sneak peak at the next stuff :pog:
#if 0
void timer_handler() {
   Kernel::outb(0x20, 0x20);
}
void mouse_handler() {
   KERNEL_PRINTF(display, "Gotten mouse input!\n");
   Kernel::outb(0x20, 0x20);
}
#endif

extern "C" void kernel() {    
    // NOTE: Maybe if bits are not 32, you could display information another way? Like maybe 
    // Emergency Cchar display. Like you can display only raw colors. Explaining that the OS requires the full
    // 32 bit display in order to run correctly (as a Workspace)
    // Maybe later on we can have a server system? Where you don't need a display, or at least the display
    // Isn't required, and no output is given to it.
    // I'm not sure...

    #ifndef BUKO_SERVER_BUILD
	  if( limine_framebuffer_request.response == NULL || limine_framebuffer_request.response->framebuffer_count != 1 || limine_framebuffer_request.response->framebuffers[0] -> bpp != 32 ) {
          // Switch to invalid display info
          // Probably display something here telling the user they have invalid info. Like if the response IS something. if its not, there is no point in reporting anything. I mean you literally can't
      }
      else {	
          display=DisplayInfo((Pixel*)limine_framebuffer_request.response->framebuffers[0]->address, limine_framebuffer_request.response->framebuffers[0]->width, limine_framebuffer_request.response->framebuffers[0]->height, limine_framebuffer_request.response->framebuffers[0]->pitch);
      }
    #endif
    if( limine_memmap_request.response == NULL)
        KERNEL_CRASH("Cannot load memmap request!");

    #ifndef BUKO_SERVER_BUILD
    if(!display.isInvalid()) {
       for(size_t y=0; y<display.height-20; ++y) {
           for (size_t x=0; x<display.width; ++x) {
                display.setAt(x,y, CONFIG_BACKGROUND_COLOR);
            }
       }
       for(size_t y=display.height-20; y<display.height; ++y) {
           for (size_t x=0; x<display.width; ++x) {
           #ifndef CONFIG_NO_PROGRESSBAR
               display.setAt(x,y,CONFIG_PROGRESS_BAR_COLOR);
           #else
               display.setAt(x,y,CONFIG_BACKGROUND_COLOR);
           #endif
           }
       }
    }
    #endif
    KERNEL_PB_PRINTF(display, "Loading display information...");
    bool running=true;


    KERNEL_LOG(display, "Hello and welcome to BukoOS!\n");
    KERNEL_LOG(display, "Distribution from Dcraftbg " SYS_DIST_URL "\n"); 
    KERNEL_PRINTF(display, "System version %s in %s mode\n", SYS_VERSION, SYS_MODE);
#ifdef BUKO_PRINT_MEM_INFO
    KERNEL_PRINTF(display, "Number of map entries available: %d\n", (int)limine_memmap_request.response->entry_count);
#endif
    size_t AvailableMemory=0;
    int64_t MaxReclaimableIndex=-1;

    int64_t BiggestUsableIndex=-1;
    size_t  BiggestUsableSize = 0;

    KERNEL_PB_PRINTF(display, "Loading memory information...");
    for(size_t i = 0; i < limine_memmap_request.response->entry_count; ++i) {
#ifdef BUKO_PRINT_MEM_INFO
        if(i < 10) KERNEL_PRINTF(display, "%d ", i);
        else KERNEL_PRINTF(display, "%d", i);
        KERNEL_PRINTF(display, "> base: %p length: %l (%l bytes) pages type: (%s)\n",limine_memmap_request.response->entries[i]->base, (int64_t)(limine_memmap_request.response->entries[i]->length/4096), (int64_t)(limine_memmap_request.response->entries[i]->length), limine_memmap_type_to_string(limine_memmap_request.response->entries[i]->type));
#endif
        switch(limine_memmap_request.response->entries[i]->type) {
                case LIMINE_MEMMAP_USABLE:
                if(BiggestUsableSize<limine_memmap_request.response->entries[i]->length) {
                   BiggestUsableSize=limine_memmap_request.response->entries[i]->length;
                   BiggestUsableIndex=i;
                }
                case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
                case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
                        MaxReclaimableIndex=i;
                        break;

                default:
                        break;
        }
        if(limine_memmap_request.response->entries[i]->type==LIMINE_MEMMAP_USABLE) AvailableMemory+=limine_memmap_request.response->entries[i]->length;
    }
    if(MaxReclaimableIndex==-1) {
        KERNEL_CRASH("Warning: Could not find ANY Bootloader/ACPI reclaimable memory entry");
    } 
    if(BiggestUsableIndex==-1) {
        KERNEL_CRASH("Warning: Could not find ANY Usable memory entry");
    } 
#ifdef BUKO_PRINT_MEM_INFO
    for(size_t i = 0; i < 9; i++) {
        KERNEL_LOG_COLOR(display,"x", limine_memmap_type_to_color(i)); stdTerminal::printf(display, " - %s\n",limine_memmap_type_to_string(i));
    }
    KERNEL_LOG(display, "<");
    for(size_t i = 0; i < limine_memmap_request.response->entry_count; ++i) {
        double ratio = ((double)limine_memmap_request.response->entries[i]->length/(double)totalMemory);
        for(size_t j=0; j < (size_t)(ratio*MEMORY_DISPLAY_SIZE+1); ++j) {
            KERNEL_LOG_COLOR(display, "=", limine_memmap_type_to_color(limine_memmap_request.response->entries[i]->type));
        }
    }
    KERNEL_LOG(display, ">");
    KERNEL_LOG(display,"\n");
#endif
    {
        auto mapentry            = limine_memmap_request.response->entries[BiggestUsableIndex];
        GlobalMemoryMap.base     = (void*)mapentry->base;
        GlobalMemoryMap.pageCount= (limine_memmap_request.response->entries[MaxReclaimableIndex]->base + limine_memmap_request.response->entries[MaxReclaimableIndex]->length)/4096;//(limine_memmap_request.response->entries[limine_memmap_request.response->entry_count-1]->base + limine_memmap_request.response->entries[limine_memmap_request.response->entry_count-1]->length)/4096;//totalMemory/4096;
        GlobalMemoryMap.MaxReclaimableIndex = MaxReclaimableIndex;
        GlobalMemoryMap.byteSize = GlobalMemoryMap.pageCount/8;
    }
    if(GlobalMemoryMap.byteSize==0) {
        KERNEL_PRINTF(display, "ERROR: Cannot allocate MemMapByteSize of 0\n");
        for(;;) asm volatile("hlt" : "+g"(running));
    } 
    {
       GlobalMemoryMap.pageSize = (GlobalMemoryMap.byteSize-1)/4096+1;
       memset((void*)GlobalMemoryMap.base, 0, GlobalMemoryMap.pageSize*4096);
    }
    KERNEL_PB_PRINTF(display, "Setting memory map entries...");
    for(size_t i=0; i < limine_memmap_request.response->entry_count; ++i) {
        auto entry = limine_memmap_request.response->entries[i];
        if(entry->type != LIMINE_MEMMAP_USABLE) continue;
        size_t entryPageCount = entry->length/4096;
        size_t PageIndex=(entry->base/4096);
        for(size_t j =((i==(size_t)BiggestUsableIndex) ? GlobalMemoryMap.pageSize : 0); j < entryPageCount; ++j) {
            char* pageSet = &((char*)GlobalMemoryMap.base)[(PageIndex+j)/8];
            *pageSet |= (1 << (PageIndex+j)%8);
        }
    }
    asm volatile ("cli");
    KERNEL_PB_PRINTF(display, "Initializing Global descriptor table...");    
    Kernel::GDT* gdt = (Kernel::GDT*)GlobalMemoryMap.allocate(1);
    *gdt = Kernel::GDT();

    Kernel::GDTDescriptor gdt_descriptor;

    gdt_descriptor.size = 4096;
    gdt_descriptor.addr = (uint64_t)gdt;
    asm volatile ("lgdt %0" : : "m"(gdt_descriptor));
    Kernel::IDT* idt = (Kernel::IDT*)GlobalMemoryMap.allocate(1);
    *idt = Kernel::IDT(idt);
    Kernel::IDTDescriptor idt_descriptor;
    idt_descriptor.size = 4096;
    idt_descriptor.addr = (uint64_t)(idt);

    KERNEL_PB_PRINTF(display, "Initializing gdt registers...");
    kernel_init_gdt_registers();
    KERNEL_PB_PRINTF(display, "Initialized gdt registers!");

    KERNEL_PB_PRINTF(display, "Initializing keyboard driver...");
    idt->Init(0x21, IDT_TYPE_HARDWARE, (void*)Driver::keyboard);
#if 0
    KERNEL_PB_PRINTF(display, "Initializing mouse driver...");
    idt->Init(0x28+4, IDT_TYPE_HARDWARE, (void*)mouse_handler); // mouse
#endif
    KERNEL_PB_PRINTF(display, "Loading idt...");
    asm volatile ("lidt %0" : : "m"(idt_descriptor));

    KERNEL_PB_PRINTF(display, "Enabling PIC and slave PIC");  
    Kernel::outb(0x20, 0x11);                                 
    Kernel::outb(0xA0, 0x11);                                 
    KERNEL_PB_PRINTF(display, "Setting master and slave PIC offset");
    Kernel::outb(0x21, 0x20);                                 
    Kernel::outb(0xA1, 0x28);                                 
    
    Kernel::outb(0x21, 0x04); // Tell master PIC there is a slave PIC at IRQ2                      
    Kernel::outb(0xA1, 0x02); // Tell slave PIC its cascade identity

    Kernel::outb(0x21, 0); // Initializing PIC       
    Kernel::outb(0xA1, 0); // Initializing PIC


    Kernel::outb(0x21, 0b11111101); // Disable all master PIC hardware interrupts, except keyboard
    Kernel::outb(0xA1, 0b11111111); // Disable all slave PIC hardware interrupts, except mouse

    KERNEL_PB_PRINTF(display, "Enabling interrupts...");
    asm volatile ("sti");
    KERNEL_PB_PRINTF(display, "Done!");
    running=true;
    while(running) {
        asm volatile("hlt" : "+g"(running));
        continue;
    }
    //asm volatile("hlt" : "+g"(running));
    KERNEL_PB_PRINTF(display, "Shutting down!");
}

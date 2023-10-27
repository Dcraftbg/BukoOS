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
#define PAGE_STATUS_INVALID_INDEX -1               
// note: only returned by getPageStatus if the index is out of range^
// Consider:
// DisplayInfo globalInfo={0};

#include "libs/string.h"
#define KERNEL_HULT() do {volatile bool running=true; for(;;) asm volatile("hlt" : "+g"(running));} while(0)
#ifndef BUKO_SERVER_BUILD
DisplayInfo display(NULL, 0, 0, 0);
namespace Kernel {
   void crash(DisplayInfo info, const char* reason) {
        Cursor_T& cursor=getGlobalCursor();
        cursor.x=0;
        cursor.y=0;
        for(size_t y=0; y<info.height; ++y) {
           for(size_t x=0; x<info.width; ++x) {
                info.setAt(x,y,Pixel(0,0,0,0xFF));
           }
        }
        stdTerminal::printf(info, "ERROR: %s. Please try restarting your System. If this error continues to occur, try to report it to Dcraftbg",reason);
        KERNEL_HULT();
   }
}
#define KERNEL_CRASH(reason) Kernel::crash(display, reason)
#else
#define KERNEL_CRASH(reason) do {volatile bool running=true; for(;;) asm volatile("hlt" : "+g"(running));} while(0)
#endif

#if defined(BUKO_DEBUG) || defined(BUKO_RELEASE)
#define KERNEL_ASSERT(x) do { if(!(x)) {KERNEL_PRINTF(display,"Assertion failed: %s, %s, %l", #x, __FILE__, __LINE__); KERNEL_HULT();}} while(0)
#else
#define KERNEL_ASSERT(x) do { if(!(x)) {KERNEL_CRASH(#x);}} while(0)
#endif

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
    char deallocate(void* where, size_t paCount) {
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
    void* allocate(size_t paCount) {
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
void printHello(int argc) {
   KERNEL_PRINTF(display, "Hallo Gotten Interrupt %d!\n",argc);
}
void keyboard_handler() {
   KERNEL_PRINTF(display, "Hello from keyboard handler!\n");
   Kernel::outb(0x20, 0x20);
}
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
#ifdef BUKO_TEST_MEMORY_PAGE_MAP
    KERNEL_PB_PRINTF(display, "Testing memory map...");
    bool passed=true;
    size_t failureEntryIndex=0;
    size_t failurePageIndex =0;

    for(size_t i=0; i < limine_memmap_request.response->entry_count; ++i) {
        if(!passed) break;
        auto entry = limine_memmap_request.response->entries[i];
        if(entry->type!=LIMINE_MEMMAP_USABLE) continue;
        size_t PageStart = (entry->base / 4096);
        size_t PageCount = entry->length/4096;
        for(size_t j=PageStart+(i==(size_t)BiggestUsableIndex ? GlobalMemoryMap.pageSize : 0); j < PageStart+PageCount; ++j) {
            passed = GlobalMemoryMap.getPageStatus(j)==PAGE_STATUS_USABLE;
            if(!passed) {
                failureEntryIndex=i;
                failurePageIndex=j;
            }
            break;
        }
    }
    if(passed) {
        KERNEL_PRINTF(display,"Success! Passed verification check!\n");
    } else {
        KERNEL_PRINTF(display,"ERROR: Failed to verify check! at entry %l, page %l\n",(int64_t)failureEntryIndex,(int64_t)failurePageIndex);
    }
#endif
    asm volatile ("cli");
    KERNEL_PB_PRINTF(display, "Initializing Global descriptor table...");    
    Kernel::GDT* gdt = (Kernel::GDT*)GlobalMemoryMap.allocate(1);
    *gdt = Kernel::GDT();

    Kernel::GDTDescriptor gdt_descriptor;
//    KERNEL_PRINTF(display, "gdt? %p\n", gdt);
    gdt_descriptor.size = 4096;
    gdt_descriptor.addr = (uint64_t)gdt;
#if 0
    KERNEL_PRINTF(display, "gdt: %p\n", gdt);
    KERNEL_PRINTF(display, "gdt_descriptor: %p %d\n", gdt_descriptor.addr,(uint32_t)gdt_descriptor.size);
    KERNEL_PRINTF(display, "gdt->nullDescriptor   : %p\n", gdt->nullDescriptor   );
    KERNEL_PRINTF(display, "gdt->kernelCodeSegment: %p\n", gdt->kernelCodeSegment);
    KERNEL_PRINTF(display, "gdt->kernelDataSegment: %p\n", gdt->kernelDataSegment);
    KERNEL_PRINTF(display, "gdt->syscallsSegment  : %p\n", gdt->syscallsSegment  );
    KERNEL_PRINTF(display, "gdt->userCodeSegment  : %p\n", gdt->userCodeSegment  );
    KERNEL_PRINTF(display, "gdt->userDataSegment  : %p\n", gdt->userDataSegment  );
#endif
    asm volatile ("lgdt %0" : : "m"(gdt_descriptor));
    Kernel::IDT* idt = (Kernel::IDT*)GlobalMemoryMap.allocate(1);
    *idt = Kernel::IDT(idt);
    Kernel::IDTDescriptor idt_descriptor;
    idt_descriptor.size = 4096;
    idt_descriptor.addr = (uint64_t)(idt);
#if 0
    KERNEL_PRINTF(display, "idt: %p\n",idt);
    KERNEL_PRINTF(display, "idt_descriptor: %p %d\n", idt_descriptor.addr,(uint32_t)idt_descriptor.size);
#endif
    idt->Init(0x80,0xEF00, (void*)printHello);
    KERNEL_PB_PRINTF(display, "Initializing gdt registers...");
    kernel_init_gdt_registers();
    KERNEL_PB_PRINTF(display, "Initialized gdt registers!");

    KERNEL_PB_PRINTF(display, "Loading idt...");
    asm volatile ("lidt %0" : : "m"(idt_descriptor));
    KERNEL_PB_PRINTF(display, "Initializing keyboard driver...");
    idt->Init(0x21, IDT_TYPE_HARDWARE, (void*)keyboard_handler);
    KERNEL_PB_PRINTF(display, "Enabling PIC and slave PIC");  
    Kernel::outb(0x20, 0x11);                                 
    Kernel::outb(0x20, 0x11);                                 
                                                              
    Kernel::outb(0x21, 0x20);                                 
    Kernel::outb(0xA1, 0x28);                                 
                                                              
    Kernel::outb(0x21, 0x04);                                 
    Kernel::outb(0xA1, 0x02);                                 
                                                             
    Kernel::outb(0x21, 0x01);                                 
    Kernel::outb(0xA1, 0x01);                                 
                                                             
    Kernel::outb(0x21, 0);                                    
    Kernel::outb(0xA1, 0);                                    

    //Kernel::outb(0x21, Kernel::inb(0x21) & 0xFD);
    //Kernel::outb(0x64, 0x60); // Command port
    //Kernel::outb(0x60, 0xAA); // Data port (0xAA is a typical initialization command)
    KERNEL_PB_PRINTF(display, "Enabling interrupts...");
    asm volatile ("sti");

#if 0
    KERNEL_PB_PRINTF(display, "Testing interrupts...");
    {
      int foo=4;
      asm volatile ("movq %0, %%rdi" : : "r"((int64_t)foo));
      asm volatile ("int $0x80");
    }
    asm volatile ("movq $1, %rdi");
    asm volatile ("int $0x80");
    asm volatile ("movq $1, %rdi");
    asm volatile ("int $0x80");
#endif
    KERNEL_PB_PRINTF(display, "Done!");
    //KERNEL_RESET_CURSOR();
    running=true;
    //char _ = Kernel::inb(0x64);   
    while(running) {
        continue;
        char keyboard = Kernel::inb(0x64);
        if(keyboard&0x01) {
          char c = Kernel::inb(0x60);
          int result=0;
          //KERNEL_PRINTF(display, "%d", (int)c);
          //continue;
          if(c!=0) {
             switch(c) {
                case QWERTY_US_CHARCODE_A_PRESS: result='A'; break;
                case QWERTY_US_CHARCODE_B_PRESS: result='B'; break;
                case QWERTY_US_CHARCODE_C_PRESS: result='C'; break;
                case QWERTY_US_CHARCODE_D_PRESS: result='D'; break;
                case QWERTY_US_CHARCODE_E_PRESS: result='E'; break;
                case QWERTY_US_CHARCODE_F_PRESS: result='F'; break;
                case QWERTY_US_CHARCODE_G_PRESS: result='G'; break;
                case QWERTY_US_CHARCODE_H_PRESS: result='H'; break;
                case QWERTY_US_CHARCODE_I_PRESS: result='I'; break;
                case QWERTY_US_CHARCODE_J_PRESS: result='J'; break;
                case QWERTY_US_CHARCODE_K_PRESS: result='K'; break;
                case QWERTY_US_CHARCODE_L_PRESS: result='L'; break;
                case QWERTY_US_CHARCODE_M_PRESS: result='M'; break;
                case QWERTY_US_CHARCODE_N_PRESS: result='N'; break;
                case QWERTY_US_CHARCODE_O_PRESS: result='O'; break;
                case QWERTY_US_CHARCODE_P_PRESS: result='P'; break;
                case QWERTY_US_CHARCODE_Q_PRESS: result='Q'; break;
                case QWERTY_US_CHARCODE_R_PRESS: result='R'; break;
                case QWERTY_US_CHARCODE_S_PRESS: result='S'; break;
                case QWERTY_US_CHARCODE_T_PRESS: result='T'; break;
                case QWERTY_US_CHARCODE_U_PRESS: result='U'; break;
                case QWERTY_US_CHARCODE_V_PRESS: result='V'; break;
                case QWERTY_US_CHARCODE_W_PRESS: result='W'; break;
                case QWERTY_US_CHARCODE_X_PRESS: result='X'; break;
                case QWERTY_US_CHARCODE_Y_PRESS: result='Y'; break;
                case QWERTY_US_CHARCODE_Z_PRESS: result='Z'; break;
                case QWERTY_US_CHARCODE_1_PRESS: result='1'; break; 
                case QWERTY_US_CHARCODE_2_PRESS: result='2'; break;
                case QWERTY_US_CHARCODE_3_PRESS: result='3'; break;
                case QWERTY_US_CHARCODE_4_PRESS: result='4'; break;
                case QWERTY_US_CHARCODE_5_PRESS: result='5'; break;
                case QWERTY_US_CHARCODE_6_PRESS: result='6'; break;
                case QWERTY_US_CHARCODE_7_PRESS: result='7'; break;
                case QWERTY_US_CHARCODE_8_PRESS: result='8'; break;
                case QWERTY_US_CHARCODE_9_PRESS: result='9'; break;
                case QWERTY_US_CHARCODE_0_PRESS: result='0'; break;
                case QWERTY_US_LEFT_SHIFT_PRESS: result=BUKO_KEY_LEFT_SHIFT; break;
                case QWERTY_US_RIGHT_SHIFT_PRESS: result=BUKO_KEY_RIGHT_SHIFT; break;
                case QWERTY_US_LEFT_CONTR_PRESS: result=BUKO_KEY_LEFT_CONTROL; break;
                case QWERTY_US_BACKTICK_PRESS  : result=BUKO_KEY_BACKTICK; break;
                case QWERTY_US_ESCAPE_PRESS    : result=BUKO_KEY_ESCAPE; break;
                case QWERTY_US_ENTER_PRESS     : result=BUKO_KEY_ENTER; break;
                case QWERTY_US_BACKSPACE_PRESS : result=BUKO_KEY_BACKSPACE; break;
                case QWERTY_US_LEFT_ALT_PRESS  : result=BUKO_KEY_LEFT_ALT;   break;
                case QWERTY_US_SPACE: result=BUKO_KEY_SPACE; break;
          }
        }
        switch(result) {
        case 0: break;
        case BUKO_KEY_LEFT_SHIFT: 
        case BUKO_KEY_LEFT_ALT:
        case BUKO_KEY_LEFT_CONTROL:
        case BUKO_KEY_RIGHT_SHIFT:
        case BUKO_KEY_ENTER:
            putC(display, '\n');
        break;
        case BUKO_KEY_SPACE:
            putC(display, ' ');
        break;
        default: {
           if(result >= 'A' && result <= 'Z') {
                putC(display, result);
           }
           else if(result >= '0' && result <= '9') {
                putC(display, result);
           }
        } 
        }
    }
    }
    //asm volatile("hlt" : "+g"(running));
    KERNEL_PB_PRINTF(display, "Shutting down!");
}

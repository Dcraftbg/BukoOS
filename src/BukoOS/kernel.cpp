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
#include "libs/PCI.h"
#include "drivers/keyboard.h"
#include "config.h"


#define SYS_DIST_URL "https://github.com/Dcraftbg/BukoOS"
#define SYS_VERSION "0.4.2A"
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




#define BUKO_DEVICE_NVM_CODE 0x108


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
//char cmdBuffer[4096]={0};
//size_t cmdLen=0;

struct RealTime_T {
  int8_t second;
  int8_t minute;
  int8_t hour;

  int8_t weekday;
  int8_t monthday;
  int8_t month;
  int8_t year;
  int8_t century;
};
inline uint64_t getFullYearFromTime(RealTime_T& time) {
   uint64_t full_year=time.century*100+time.year;
   return full_year;
}
inline int8_t getCMOSregister(int8_t reg) {
  Kernel::outb(0x70, (1 << 7) | (reg)); // 1 << 7 to disable NMI... Maybe I could make it into its own function
  return Kernel::inb(0x71);
}
#define BCD_TO_BINARY(bcd) (((bcd & 0xF0) >> 1) + ((bcd & 0xF0) >> 3) + (bcd & 0xf))

#define CMOS_CONFIG_24_HOUR_CLOCK 2
#define CMOS_CONFIG_BINARY 4
uint8_t CMOSConfig=0; 
inline uint8_t getHourNormal(uint8_t hour) {
  if(!(CMOSConfig & CMOS_CONFIG_24_HOUR_CLOCK) && (hour & 0x80)) {
     hour = ((hour & 0x7F)+12)%24;
  }
  //if (hour & 0x80) {
  //   KERNEL_PRINTF(display, "Enabled AM/PM and hour is PM mode!\n");
  //}
  return hour;
}
inline void initCMOSConfig() {
  uint8_t statB = getCMOSregister(0x0B);
  CMOSConfig = statB; // TODO: Not sure if this works perfectly fine all the time bu yes
}
inline uint8_t CMOS_update_in_progress() {
   Kernel::outb(0x70, 0x0A);
   return (Kernel::inb(0x71) & 0x80);
}
inline bool RealTime_T_eq(RealTime_T& current, RealTime_T& other) {
  return current.second== other.second && current.minute == other.minute &&
         current.hour  == other.hour   && current.weekday == other.weekday &&
         current.monthday == other.monthday && current.monthday == other.monthday &&
         current.month == other.month && current.year == other.year &&
         current.century == other.century;
}
RealTime_T* timeFromCMOS(RealTime_T* last) {
  RealTime_T currentTime={0};
  currentTime.second  =last->second  ; 
  currentTime.minute  =last->minute  ; 
  currentTime.hour    =last->hour    ; 
  currentTime.weekday =last->weekday ; 
  currentTime.monthday=last->monthday; 
  currentTime.month   =last->month   ; 
  currentTime.year    =last->year    ; 
  currentTime.century =last->century ; 
  do { 
        last->second  =currentTime.second  ; 
        last->minute  =currentTime.minute  ; 
        last->hour    =currentTime.hour    ; 
        last->weekday =currentTime.weekday ; 
        last->monthday=currentTime.monthday; 
        last->month   =currentTime.month   ; 
        last->year    =currentTime.year    ; 
        last->century =currentTime.century ;
        while(CMOS_update_in_progress());
        currentTime.second   = getCMOSregister(0x00);
        currentTime.minute   = getCMOSregister(0x02);
        currentTime.hour     = getCMOSregister(0x04);
        currentTime.weekday  = getCMOSregister(0x06);
        currentTime.monthday = getCMOSregister(0x07);
        currentTime.month    = getCMOSregister(0x08);
        currentTime.year     = getCMOSregister(0x09);
        currentTime.century  = getCMOSregister(0x32);
  } while(!RealTime_T_eq(currentTime,*last));
  //initCMOSConfig();
  if(!(CMOSConfig & CMOS_CONFIG_BINARY)) {
     last->second   = BCD_TO_BINARY(last->second  ); 
     last->minute   = BCD_TO_BINARY(last->minute  ); 
     last->hour     = BCD_TO_BINARY(last->hour    ); 
     last->weekday  = BCD_TO_BINARY(last->weekday ); 
     last->monthday = BCD_TO_BINARY(last->monthday); 
     last->month    = BCD_TO_BINARY(last->month   ); 
     last->year     = BCD_TO_BINARY(last->year    ); 
     last->century  = BCD_TO_BINARY(last->century ); 
  }
  last->hour = getHourNormal(last->hour);
  return last;
}
void keyboard_handler(BukoKeyboardAction actionType, int key) {
   static bool shifted=false;
   switch(key) {
        case 0: break;
        case BUKO_KEY_LEFT_SHIFT:
        case BUKO_KEY_RIGHT_SHIFT:
             shifted=actionType!=BUKO_KEYBOARD_ACTION_RELEASE;
        break;
        case BUKO_KEY_LEFT_ALT: 
        case BUKO_KEY_LEFT_CONTROL:
        break;
        case BUKO_KEY_ENTER:
            if(actionType) putC(display, '\n');
        break;
        case BUKO_KEY_SPACE:
            if(actionType) putC(display, ' ');
        break;
        case BUKO_KEY_COMA:
            if(actionType) {if(shifted) putC(display, '<'); else putC(display, ',');} break;
        case BUKO_KEY_DOT:
            if(actionType) {if(shifted) putC(display, '>'); else putC(display, '.');} break;
        case BUKO_KEY_SINGLE_QUOTE:
            if(actionType) {if(shifted) putC(display, '"'); else putC(display, '\'');} break;
        case BUKO_KEY_SEMICOLON:
            if(actionType) {if(shifted) putC(display, ':'); else putC(display, ';');} break;
        case BUKO_KEY_OPEN_SQUARE:
            if(actionType) {if(shifted) putC(display, '{'); else putC(display, '[');} break;
        case BUKO_KEY_CLOSE_SQUARE:
            if(actionType) {if(shifted) putC(display, '}'); else putC(display, ']');} break;
        case BUKO_KEY_MINUS:
            if(actionType) {if(shifted) putC(display, '_'); else putC(display, '-');} break;
        case BUKO_KEY_EQUALS:
            if(actionType) {if(shifted) putC(display, '+'); else putC(display, '=');} break;
        default: {
           if(key >= 'A' && key <= 'Z' && actionType!=BUKO_KEYBOARD_ACTION_RELEASE) {
                putC(display, shifted ? key : key-'A'+'a');
           }
           else if(key >= '0' && key <= '9' && actionType!=BUKO_KEYBOARD_ACTION_RELEASE) {
                putC(display, key);
           }
        } 
   }

}
extern "C" void kernel() {   

    static bool running=true;
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


    Kernel::outb(0x21, 0b11111111); // Disable all master PIC hardware interrupts, except keyboard
    Kernel::outb(0xA1, 0b11111111); // Disable all slave PIC hardware interrupts
    // ------------------Done configuring PIC------------------
    //size_t foundDevices=0;
    // ------------------Printing vendor IDs------------------
    for(uint8_t bus=0; bus<255; ++bus) {
        for(uint8_t slot=0; slot<32; ++slot) {
            uint16_t vendor_id = pci_config_read_word(bus, slot, 0, 0);
            if(vendor_id!=0xFFFF) {
                uint16_t device          = pci_config_read_word(bus, slot, 0, 2);
                uint16_t class_code    ; 
                uint8_t  prog_interface; 
                uint8_t  revision_ID   ;
                uint32_t bar0_base_addr = pci_config_read_dword(bus, slot, 0, 0x10) & 0xFFFFFFF0;



                uint8_t  base_class    ; 
                uint8_t  subclass      ; 
                {
                    uint32_t general        = pci_config_read_dword(bus, slot, 0, 8);
                    class_code    = (general >> 16) & 0xFFFF;
                    base_class    = (general >> 24) & 0xFF;
                    subclass      = (general >> 16) & 0xFF;
                    prog_interface= (general >> 8 ) & 0xFF;
                    revision_ID   = (general      ) & 0xFF;
                }
                (void)prog_interface;
                (void)revision_ID;
                
                KERNEL_PRINTF(display,"[PCI] Found device with vendor: %p device: %p class: %d subclass: %d bar0_base_addr: %p\n",(int64_t)vendor_id, (int64_t)device,(int32_t)base_class, (int32_t)subclass,(int64_t)bar0_base_addr);
                switch(class_code) {
                case BUKO_DEVICE_NVM_CODE: {
                    void* addr = (void*)((uint64_t)bar0_base_addr);
                    uint64_t cap = *((uint64_t*)((char*)addr+0));
                    uint32_t vs  = *((uint32_t*)((char*)addr+8));
                    uint32_t INTMS = *((uint32_t*)((char*)addr+12));
                    uint32_t INTMC = *((uint32_t*)((char*)addr+16));
                    uint32_t CC = *((uint32_t*)((char*)addr+20));
                    uint32_t CCSTS = *((uint32_t*)((char*)addr+24));
                    uint32_t AQA = *((uint32_t*)((char*)addr+28));
                    uint64_t AQU = *((uint64_t*)((char*)addr+32));
                    uint64_t ACQ = *((uint64_t*)((char*)addr+40));
                    
                    //KERNEL_PRINTF(display, "[PCI] (NVME) Info:\nCapabilities: %p\nVersion: %p\nINTMS: %p\nINTMC: %p\nCC: %p\nCCSTS: %p\nAQA: %p\nAQU: %p\nACQ: %p\n",
                    //                                        (uint64_t)cap,(uint64_t)vs, (uint64_t)INTMS, (uint64_t)INTMC, (uint64_t)CC,(uint64_t)CCSTS, (uint64_t)AQA,(uint64_t)AQU,(uint64_t)ACQ);
                    KERNEL_PRINTF(display, "[PCI] (NVME) Stats:\n");
                    KERNEL_PRINTF(display, "Version: %l.%l\n",(uint64_t)((vs>>16)&0xFF),(uint64_t)((vs>>8)&0xFF));
                    //KERNEL_PRINTF(display, "Capabilities: %p\n",(uint64_t)cap);
                    KERNEL_PRINTF(display, "MQES: %d\n",(int32_t)(cap&0xFFFF));
                    KERNEL_PRINTF(display, "CQR: %d\n",(int32_t)((cap>>16)&1));
                    KERNEL_PRINTF(display, "AMOS: %d\n",(int32_t)((cap>>17)&0b11));
                    KERNEL_PRINTF(display, "TO: %d\n",(int32_t)((cap>>24)&0xFF));
                    KERNEL_PRINTF(display, "DSTRD: %d\n",(int32_t)((cap>>32)&0b1111));
                    KERNEL_PRINTF(display, "NSSRS: %d\n",(int32_t)((cap>>36)&1));
                    KERNEL_PRINTF(display, "CSS: %d\n",(int32_t)((cap>>37)&0xFF));                              
                    KERNEL_PRINTF(display, "BPS: %d\n",(int32_t)((cap>>45)&1));
                    KERNEL_PRINTF(display, "CPS: %d\n",(int32_t)((cap>>46)&0b11));
                    KERNEL_PRINTF(display, "MPSMIN: %d\n",(int32_t)((cap>>48)&0b1111));
                    KERNEL_PRINTF(display, "MPSMAX: %d\n",(int32_t)((cap>>52)&0b1111));
                    KERNEL_PRINTF(display, "PMRS: %d\n",(int32_t)((cap>>56)&0b1));
                    KERNEL_PRINTF(display, "CMBS: %d\n",(int32_t)((cap>>57)&0b1));
                    KERNEL_PRINTF(display, "NSSS: %d\n",(int32_t)((cap>>58)&0b1));               
                    KERNEL_PRINTF(display, "CRMS: %d\n",(int32_t)((cap>>59)&0b11));                
                }
                break;
                }
                //if(foundDevices % 3 == 0) putC(display, '\n');
                //foundDevices++;
            }
        }
    } 
    // ---------------Done configuring PCI---------------
    

    // --------------Getting date and time----------------
    initCMOSConfig();
    RealTime_T time={0}; 
    timeFromCMOS(&time);
    //KERNEL_PRINTF(display,"year: %d\n"    ,(int)getFullYearFromTime(time));
    //KERNEL_PRINTF(display,"month: %d\n"   ,(int)time.month);
    //KERNEL_PRINTF(display,"day: %d\n"     ,(int)time.monthday);
    //KERNEL_PRINTF(display,"weekday:  %d\n",(int)time.weekday);
    //KERNEL_PRINTF(display,"hour: %d\n"    ,(int)time.hour);
    //KERNEL_PRINTF(display,"minute: %d\n"  ,(int)time.minute);
    //KERNEL_PRINTF(display,"second: %d\n"  ,(int)time.second);
    //KERNEL_PRINTF(display,"CMOS Config state (B): %d\n",(int)CMOSConfig);
    //KERNEL_PRINTF(display,"CMOS is configured to use %s and %s\n",CMOSConfig & CMOS_CONFIG_24_HOUR_CLOCK ? "standard time": "am/pm", CMOSConfig & CMOS_CONFIG_BINARY ? "binary mode" : "bsd mode");
    {
      char minbuf[3]={0};

      stdString::itostr(minbuf, 2, time.minute);
      if(time.minute < 10) {
        minbuf[1]=minbuf[0];
        minbuf[0]='0';
      }
      KERNEL_PRINTF(display,"%d:%s: %d/%d/%d",(int)time.hour,minbuf, (int)time.monthday,(int)time.month, (int)getFullYearFromTime(time));
    }
    KERNEL_PB_PRINTF(display, "Enabling interrupts...");
    asm volatile ("sti");
    KERNEL_PB_PRINTF(display, "Done!");
    while(running) {
        asm volatile("hlt" : "+g"(running));
        continue;
    }
    //asm volatile("hlt" : "+g"(running));
    KERNEL_PB_PRINTF(display, "Shutting down!");
}

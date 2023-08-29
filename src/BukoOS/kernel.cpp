#include <limine.h>
#include <cstddef>
#include <limits.h>
#include "libs/memory.h"
#include "libs/display.h"
#include "libs/terminal.h"
#define BUKO_PRINT_MEM_INFO
#define SYS_DIST_URL "https://github.com/Dcraftbg/BukoOS"
#define SYS_VERSION "0.1.0A"
#ifdef BUKO_DEBUG
   #define SYS_MODE "Debug"
#elif defined(BUKO_RELEASE)
   #define SYS_MODE "Release"
#elif defined(BUKO_DIST)
   #define SYS_MODE "Dist"
#else
   #error "Running in non recognized more! Please run with BUKO_DEBUG, BUKO_RELEASE or BUKO_DIST"
#endif
#define MEMORY_DISPLAY_SIZE (112/2)
// #include "libs/terminal.cpp"
// #include "libs/memory.cpp"

static volatile limine_framebuffer_request limine_framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

static volatile limine_memmap_request limine_memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

// Consider:
// DisplayInfo globalInfo={0};

// TODO: Remove super comments and normal comments that were previously only used for testing
#include "libs/string.h"
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
           return Pixel(0x00,0xFF,0x00,0xFF);
        case LIMINE_MEMMAP_RESERVED:
           return Pixel(0x00,0x00,0xFF,0xFF);
           //return Pixel(0x88,0xFF,0x88,0xFF);   // light green
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
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
/*
    uint8_t rskip = 1;
    uint8_t bskip = 0;
    uint8_t gskip = 0;
*/
    
	if( limine_framebuffer_request.response == NULL || limine_framebuffer_request.response->framebuffer_count != 1 || limine_framebuffer_request.response->framebuffers[0] -> bpp != 32 )
		for(;;);
    if( limine_memmap_request.response == NULL)
        for(;;);
    size_t height = limine_framebuffer_request.response->framebuffers[0]->height;
    size_t width = limine_framebuffer_request.response->framebuffers[0]->width;
    size_t pitch = limine_framebuffer_request.response->framebuffers[0]->pitch;
	Pixel* pixels = (Pixel*) limine_framebuffer_request.response -> framebuffers[ 0 ] -> address;
    Pixel* orgpixels = pixels;
    for(size_t y=0; y<height; ++y) {
        for (size_t x=0; x<width; ++x) {
             pixels[x] = {0, 0, 0, 0};
             pixels[x].a = 0xFF;   
        }
        pixels = (Pixel*)((uintptr_t)pixels +  pitch);
    }
    pixels=orgpixels;
    
#if 0
    pixels[0].r = 0xFF;
    pixels[1].r = 0xFF;          
    pixels[2].r = 0xFF;
    pixels = (Pixel*) ((uintptr_t) pixels + pitch);
    pixels[0].r = 0xFF;
    pixels = (Pixel*) ((uintptr_t) pixels + pitch);
    pixels[0].r = 0xFF;
    pixels[1].r = 0xFF;
    pixels[2].r = 0xFF;
    pixels = (Pixel*) ((uintptr_t) pixels + pitch);
    pixels[0].r = 0xFF;
    pixels = (Pixel*) ((uintptr_t) pixels + pitch);
    pixels[0].r = 0xFF;
    pixels[1].r = 0xFF;
    pixels[2].r = 0xFF;
    pixels = orgpixels;
#endif
    DisplayInfo display(pixels, width, height, pitch);
#if 0

    {
        Pixel res = {0};
        res.a=0xFF;
        res.r=0xFF;
        display.setAt(1,1,res);
    }
#endif
#if 0
    Pixel res = {0};
    res.a=0xFF;
    res.r=0xFF;
    putChar(display,0,0,'H',res); 
    putChar(display,8,0,'e',res);
    putChar(display,16,0,'l',res);
#endif
    //putChar(display,0,0,'H',res); 
    Cursor_T& cursor = getGlobalCursor();
    volatile bool running=true;
    //putStr(display,"\n");
    //putStr(display,"\tHello World!\r\tI overwrote that shit :D");

    putStr(display, "Hello and welcome to BukoOS!\n");
    putStr(display, "Distribution from DcraftBg " SYS_DIST_URL "\n"); 
    //putStr(display, "System version ");    putStr(display, SYS_VERSION);    putStr(display, "\n");
    stdTerminal::printf(display, "System version %s in %s mode\n", SYS_VERSION, SYS_MODE);
    stdTerminal::printf(display, "Number of map entries available: %d\n", (int)limine_memmap_request.response->entry_count);
    size_t AvailableMemory=0;
    size_t totalMemory=0;
    for(size_t i = 0; i < limine_memmap_request.response->entry_count; ++i) {
        if(i < 10)  stdTerminal::printf(display, "%d ", i);
        else stdTerminal::printf(display, "%d", i);
        stdTerminal::printf(display, "> base: %p length: %l type: (%s)\n",limine_memmap_request.response->entries[i]->base, (int64_t) limine_memmap_request.response->entries[i]->length, limine_memmap_type_to_string(limine_memmap_request.response->entries[i]->type));
        if(limine_memmap_request.response->entries[i]->type==LIMINE_MEMMAP_USABLE) AvailableMemory+=limine_memmap_request.response->entries[i]->length;
        totalMemory+=limine_memmap_request.response->entries[i]->length;
    } 
    stdTerminal::printf(display, "Total available memory: %lb %lKib %lMb\n", AvailableMemory, AvailableMemory/1024, AvailableMemory/(1024*1000));
    #ifdef BUKO_PRINT_MEM_INFO
    for(size_t i = 0; i < 9; i++) {
        putC(display,'x', limine_memmap_type_to_color(i)); stdTerminal::printf(display, " - %s\n",limine_memmap_type_to_string(i));
    }
    putC(display, '<');
    for(size_t i = 0; i < limine_memmap_request.response->entry_count; ++i) {
        double ratio = ((double)limine_memmap_request.response->entries[i]->length/(double)totalMemory);
        //stdTerminal::printf(display, "%l> ratio*112.0 %l: ",i,(int64_t)(ratio*112.0+1));
        for(size_t j=0; j < (size_t)(ratio*MEMORY_DISPLAY_SIZE+1); ++j) {
            putStr(display, "=", limine_memmap_type_to_color(limine_memmap_request.response->entries[i]->type));
        }
        //putC(display, '\n');
        //break;
    }
    putC(display, '>');
    putC(display,'\n');
    #endif
    /*
    char buf[12]={0};
    itostr(buf, 12, 69);
    putStr(display, buf);
    */
    putStr(display, "> ");
    //stdTerminal::printf(display, "Hello, Blah blah: %s","World");
    //char Buffer[500]={0};
    //stdTerminal::sprintf(Buffer,500,"Hello, Blah blah: %s", "World");
    //putStr(display,Buffer);
    while(running) {
        asm volatile("hlt" : "+g"(running));
    }
}

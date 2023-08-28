#include <limine.h>
#include <cstddef>
#include <limits.h>
#include "libs/memory.h"
#include "libs/display.h"
#include "libs/terminal.h"

#define SYS_DIST_URL "https://github.com/Dcraftbg/BukoOS"
#define SYS_VERSION "0.0.1A"
#ifdef BUKO_DEBUG
   #define SYS_MODE "Debug"
#elif defined(BUKO_RELEASE)
   #define SYS_MODE "Release"
#elif defined(BUKO_DIST)
   #define SYS_MODE "Dist"
#else
   #error "Running in non recognized more! Please run with BUKO_DEBUG, BUKO_RELEASE or BUKO_DIST"
#endif
// #include "libs/terminal.cpp"
// #include "libs/memory.cpp"

static volatile limine_framebuffer_request limine_framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};



// Consider:
// DisplayInfo globalInfo={0};
// TODO: Remove super comments and normal comments that were previously only used for testing
extern "C" void kernel() {
/*
    uint8_t rskip = 1;
    uint8_t bskip = 0;
    uint8_t gskip = 0;
*/

	if( limine_framebuffer_request.response == NULL || limine_framebuffer_request.response -> framebuffer_count != 1 || limine_framebuffer_request.response -> framebuffers[ 0 ] -> bpp != 32 )
		for(;;);

    size_t height = limine_framebuffer_request.response -> framebuffers[ 0 ] -> height;
    size_t width = limine_framebuffer_request.response -> framebuffers[ 0 ] -> width;
    size_t pitch = limine_framebuffer_request.response -> framebuffers[ 0 ] -> pitch;
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
    putStr(display, "> ");
    //stdTerminal::printf(display, "Hello, Blah blah: %s","World");
    //char Buffer[500]={0};
    //stdTerminal::sprintf(Buffer,500,"Hello, Blah blah: %s", "World");
    //putStr(display,Buffer);
    while(running) {
        asm volatile("" : "+g"(running));
        cursor.x=0;
        cursor.y=0;
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

#elif 0
        	for( uint64_t y = 0; y < height; y++ ) {
		        for( uint64_t x = 0; x < width; x++ ) {
                        if(pixels[x].r == UINT8_MAX-rskip) rskip = -rskip;
                        if(pixels[x].b == UINT8_MAX-bskip) bskip = -bskip;
                        if(pixels[x].g == UINT8_MAX-gskip) gskip = -gskip;
                        pixels[x].r = pixels[x].r+rskip;
                        pixels[x].b = pixels[x].b+bskip;
                        pixels[x].g = pixels[x].g+gskip;
                        pixels[x].a = 0xFF;
                } 
		        pixels = (Pixel*) ((uintptr_t) pixels + pitch);
	       }
#endif
           pixels=orgpixels;
    }
	for(;;);
}

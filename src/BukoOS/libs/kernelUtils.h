#pragma once
#define KERNEL_HULT() do {volatile bool running=true; for(;;) asm volatile("hlt" : "+g"(running));} while(0)
#ifndef BUKO_SERVER_BUILD
inline DisplayInfo display(NULL, 0, 0, 0);
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



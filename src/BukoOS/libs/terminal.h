#pragma once
#include "display.h"
#include "zap-light16.h"
#include "../config.h"
struct Cursor_T {
  size_t x;
  size_t y;
};
#ifndef BUKO_SERVER_BUILD
Cursor_T& getGlobalCursor();
void putStr(DisplayInfo info, const char* str, Pixel color=CONFIG_TEXT_COLOR);
void putC(DisplayInfo info, char c, Pixel color=CONFIG_TEXT_COLOR);
void putStrAt(DisplayInfo info, Cursor_T& cursor, const char* str, Pixel color=CONFIG_TEXT_COLOR);
void putCAt(DisplayInfo info, Cursor_T& cursor, char c, Pixel color=CONFIG_TEXT_COLOR);
//void putChar(DisplayInfo info,size_t xOff, size_t yOff, char chr, Pixel color);
namespace stdTerminal {
    void printf(DisplayInfo info, const char* formatter, ...);      
    void printfAt(DisplayInfo info, Cursor_T& cursor, const char* formatter, ...);
    void pb_printf(DisplayInfo info, const char* formatter, ...);
    size_t sprintf(char* buffer, size_t size, const char* formatter, ...);
}


#define KERNEL_LOG(info, str) putStr(info, str) 
#define KERNEL_LOG_COLOR(info, str, color) putStr(info, str, color) 
#define KERNEL_PRINTF(info, ...) stdTerminal::printf(info, __VA_ARGS__) 
#ifdef CONFIG_NO_PROGRESSBAR
#define KERNEL_PB_PRINTF(info, ...)
#else
#define KERNEL_PB_PRINTF(info, ...) stdTerminal::pb_printf(info, __VA_ARGS__) 
#endif
#define KERNEL_PRINTF_AT(info, ...) stdTerminal::printfAt(info, __VA_ARGS__) 
#define KERNEL_RESET_CURSOR() do{Cursor_T& cursor=getGlobalCursor(); cursor.x=0; cursor.y=0;} while(0)
#else
namespace stdTerminal {
    size_t sprintf(char* buffer, size_t size, const char* formatter, ...);
}
#define KERNEL_LOG(info, str)
#define KERNEL_LOG_COLOR(info, str, color)
#define KERNEL_PRINTF(info, ...)
#define KERNEL_PB_PRINTF(info, ...)
#define KERNEL_PRINTF_AT(info, ...)
#define KERNEL_RESET_CURSOR()
#endif



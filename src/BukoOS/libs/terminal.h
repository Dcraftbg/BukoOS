#pragma once
#include "display.h"
#include "zap-light16.h"
#include "../config.h"
struct Cursor_T {
  size_t x;
  size_t y;
};
Cursor_T& getGlobalCursor();
void putStr(DisplayInfo info, const char* str, Pixel color=CONFIG_TEXT_COLOR);
void putC(DisplayInfo info, char c, Pixel color=CONFIG_TEXT_COLOR);
//void putChar(DisplayInfo info,size_t xOff, size_t yOff, char chr, Pixel color);
namespace stdTerminal {
    void printf(DisplayInfo info, const char* formatter, ...);       
    size_t sprintf(char* buffer, size_t size, const char* formatter, ...);
}

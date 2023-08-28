#include "terminal.h"
Cursor_T cursor={0,0};
Cursor_T& getGlobalCursor() {
    return cursor;
}
void putChar(DisplayInfo info,size_t xOff, size_t yOff, char chr, Pixel color) {
    unsigned char* fontPtr = fontGlythBuffer+(chr*fontHeader.charsize);
    for(size_t y=yOff; y < yOff + 16; ++y) {
        for (size_t x = xOff; x < xOff+8; ++x) {
            if ((*fontPtr & (0b10000000 >> (x-xOff))) > 0) {
                info.setAt(x,y,color); // TODO: this can probably be optimised to use it raw, but idk
            }
        }
        fontPtr++;
    }
}


void putStr(DisplayInfo info, const char* str, Pixel color/*=Pixel(0xFF,0xFF,0xFF,0xFF)*/) {
   while(*str!='\0') {
        switch (*str) {
        case '\n':
                cursor.x=0;
                cursor.y+=16;
                break;
        case '\t':
                cursor.x+=8*4;
                break;
        case '\r':
                cursor.x=0;
        case ' ':
                cursor.x+=8;
                break;
        default:
                putChar(info,cursor.x, cursor.y,*str,color);
                cursor.x+=8;
        }
        str++;
   }
}
void putC(DisplayInfo info, char c, Pixel color) {
    switch (c) {
        case '\n':
                cursor.x=0;
                cursor.y+=16;
                break;
        case '\t':
                cursor.x+=8*4;
                break;
        case '\r':
                cursor.x=0;
        case ' ':
                cursor.x+=8;
                break;
        default:
                putChar(info,cursor.x, cursor.y,c,color);
                cursor.x+=8;
        }
}

#include <cstdarg>

void stdTerminal::printf(DisplayInfo info, const char* formatter, ...) {
   va_list args; 
   va_start(args,formatter);
   char* format = (char*) formatter;
   while(*format != '\0') {
           switch (*format) {
           case '%':
                format++;
                switch(*format) {
                case 's': {
                       const char* str=va_arg(args, const char*);
                       putStr(info,str);

                } break;
                case '\0':
                       putC(info, '%');
                       return;
                default:
                       putC(info, '%'); 
                       putC(info, *format);
                } 
           break;
           default:
                putC(info, *format);
           }
           format++;
   } 
   va_end(args);
}
size_t stdTerminal::sprintf(char* buffer, size_t size, const char* formatter, ...) {
    size_t bytesWritten=0;
    char* format = (char*) formatter;
    va_list args;
    va_start(args, formatter);
    while (*format != '\0' && bytesWritten<size) {
           switch (*format) {
           case '%':
                format++;
                switch(*format) {
                case 's': {
                       char* str=(char*)(va_arg(args, const char*));
                       while(*str != '\0' && bytesWritten < size) {
                          buffer[bytesWritten] = *str;
                          str++;
                          bytesWritten++;
                       }
                } break;
                case '\0':
                       buffer[bytesWritten]='%';
                       return bytesWritten;
                default:
                       buffer[bytesWritten]='%';
                       if (bytesWritten+1 < size) buffer[bytesWritten] = *format;
                } 
           break;
           default:
                buffer[bytesWritten] = *format;
           }
           bytesWritten++;
           format++;
    }
    va_end(args);
    return bytesWritten;
}

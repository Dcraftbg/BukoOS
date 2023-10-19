#include "terminal.h"
#include <cstdarg>
#ifndef BUKO_SERVER_BUILD
// TODO: Include cursor in display, since we might have multiple displays at once
Cursor_T cursor={0,0};
Cursor_T& getGlobalCursor() {
    return cursor;
}
void putCharAt(DisplayInfo info,Cursor_T& cursor, char chr, Pixel color) {
    if(info.isInvalid()) return;
    unsigned char* fontPtr = fontGlythBuffer+(chr*fontHeader.charsize);
    for(size_t y=cursor.y; y < cursor.y+16; ++y) {
        for (size_t x = cursor.x; x < cursor.x+8; ++x) {
            if ((*fontPtr & (0b10000000 >> (x-cursor.x))) > 0) {
                info.setAt(x,y,color); // TODO: this can probably be optimised to use it raw, but idk
            }
        }
        fontPtr++;
    }
}
void putCAt(DisplayInfo info, Cursor_T& cursor, char c, Pixel color) {
   if(info.isInvalid() || cursor.y >= info.height) return;

   switch (c) {
        case '\n':
                cursor.x=0;
                cursor.y+=16;
                break;
        case '\t':
                if(cursor.x+(8*4) >= info.width) {
                        cursor.y+=16;
                        cursor.x =0;
                }
                cursor.x+=8*4;
                break;
        case '\r':
                cursor.x=0;
                break;
        case ' ':
                if(cursor.x+8 >= info.width) {
                        cursor.y+=16;
                        cursor.x = 0;
                }
                cursor.x+=8;
                break;
        default:
                if(cursor.x+8 >= info.width) {
                        cursor.y+=16;
                        cursor.x = 0;
                }
                putCharAt(info,cursor,c,color);
                cursor.x+=8;
        }
}
void putStrAt(DisplayInfo info, Cursor_T& cursor, const char* str, Pixel color/*=CONFIG_TEXT_COLOR*/) {
    if(info.isInvalid() || cursor.y >= info.height) return;
    while(*str!='\0') {
        switch (*str) {
        case '\n':
                cursor.x=0;
                cursor.y+=16;
                break;
        case '\t':
                if(cursor.x+(8*4) >= info.width) {
                        cursor.y+=16;
                        cursor.x =0;
                }
                cursor.x+=8*4;
                break;
        case '\r':
                cursor.x=0;
        case ' ':
                if(cursor.x+8 >= info.width) {
                        cursor.y+=16;
                        cursor.x = 0;
                }
                cursor.x+=8;
                break;
        default:
                if(cursor.x+8 >= info.width) {
                        cursor.y+=16;
                        cursor.x = 0;
                }
                putCharAt(info,cursor,*str,color);
                cursor.x+=8;
        }
        str++;
   }
}
void putStr(DisplayInfo info, const char* str, Pixel color/*=Pixel(0xFF,0xFF,0xFF,0xFF)*/) {
   if(info.isInvalid() || cursor.y >= info.height) return;
   while(*str!='\0') {
        switch (*str) {
        case '\n':
                cursor.x=0;
                cursor.y+=16;
                break;
        case '\t':
                if(cursor.x+(8*4) >= info.width) {
                        cursor.y+=16;
                        cursor.x =0;
                }
                cursor.x+=8*4;
                break;
        case '\r':
                cursor.x=0;
        case ' ':
                if(cursor.x+8 >= info.width) {
                        cursor.y+=16;
                        cursor.x = 0;
                }
                cursor.x+=8;
                break;
        default:
                if(cursor.x+8 >= info.width) {
                    cursor.y+=16;
                    cursor.x = 0;
                }
                putC(info,*str,color);
        }
        str++;
   }
}
void putC(DisplayInfo info, char c, Pixel color) {
   if(info.isInvalid() || cursor.y >= info.height) return;

   switch (c) {
        case '\n':
                cursor.x=0;
                cursor.y+=16;
                break;
        case '\t':
                if(cursor.x+(8*4) >= info.width) {
                        cursor.y+=16;
                        cursor.x =0;
                }
                cursor.x+=8*4;
                break;
        case '\r':
                cursor.x=0;
                break;
        case ' ':
                if(cursor.x+8 >= info.width) {
                        cursor.y+=16;
                        cursor.x = 0;
                }
                cursor.x+=8;
                break;
        default:
                if(cursor.x+8 >= info.width) {
                        cursor.y+=16;
                        cursor.x = 0;
                }
                putCharAt(info,cursor,c,color);
                cursor.x+=8;
        }
}


#include "string.h"
#include "memory.h"
void stdTerminal::printfAt(DisplayInfo info, Cursor_T& cursor, const char* formatter, ...) {
   if(info.isInvalid()) return;
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
                case 'd': {
                       int val = va_arg(args, int);
                       char buf[13] = {0}; // 13 is the max amount of digits needed to represent a 32 bit integer
                       stdString::itostr(buf, 13, val);
                       putStr(info, buf);
                } break;
                case 'l': {
                       int64_t val = va_arg(args, int64_t);
                       char buf[20] = {0};
                       stdString::itostr(buf, 20, val);
                       putStr(info, buf);
                } break;
                case 'x': {
                       int32_t val = va_arg(args, int32_t);
                       char buf[20] = {0};
                       stdString::itohex(buf,20,val);
                       putStr(info,buf);
                } break;
                case 'p': {
                       void* val = va_arg(args, void*);
                       char buf[30] = {0};
                       stdString::itohex(buf, 30, (uint64_t)val);
                       putStr(info,"0x");
                       putStr(info, buf);
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
void stdTerminal::printf(DisplayInfo info, const char* formatter, ...) {
   if(info.isInvalid()) return;
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
                case 'd': {
                       int val = va_arg(args, int);
                       char buf[13] = {0}; // 13 is the max amount of digits needed to represent a 32 bit integer
                       stdString::itostr(buf, 13, val);
                       putStr(info, buf);
                } break;
                case 'l': {
                       int64_t val = va_arg(args, int64_t);
                       char buf[20] = {0};
                       stdString::itostr(buf, 20, val);
                       putStr(info, buf);
                } break;
                case 'x': {
                       int32_t val = va_arg(args, int32_t);
                       char buf[20] = {0};
                       stdString::itohex(buf,20,val);
                       putStr(info,buf);
                } break;
                case 'p': {
                       void* val = va_arg(args, void*);
                       char buf[30] = {0};
                       stdString::itohex(buf, 30, (uint64_t)val);
                       putStr(info,"0x");
                       putStr(info, buf);
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
void stdTerminal::pb_printf(DisplayInfo info, const char* formatter, ...) {
   if(info.isInvalid()) return;
   for(size_t y=info.height-20; y<info.height; y++) {
        for(size_t x=0; x< info.width; x++) {
           info.setAt(x,y,CONFIG_PROGRESS_BAR_COLOR);
        }
   }
   Cursor_T pbCursor={0};
   pbCursor.y = info.height-18;
   pbCursor.x = 0;
   
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
                       putStrAt(info, pbCursor,str);
                } break;
                case 'd': {
                       int val = va_arg(args, int);
                       char buf[13] = {0}; // 13 is the max amount of digits needed to represent a 32 bit integer
                       stdString::itostr(buf, 13, val);
                       putStrAt(info, pbCursor, buf);
                } break;
                case 'l': {
                       int64_t val = va_arg(args, int64_t);
                       char buf[20] = {0};
                       stdString::itostr(buf, 20, val);
                       putStrAt(info, pbCursor, buf);
                } break;
                case 'x': {
                       int32_t val = va_arg(args, int32_t);
                       char buf[20] = {0};
                       stdString::itohex(buf,20,val);
                       putStrAt(info,pbCursor,buf);
                } break;
                case 'p': {
                       void* val = va_arg(args, void*);
                       char buf[20] = {0};
                       stdString::itohex(buf, 20, (int64_t)val);
                       putStrAt(info, pbCursor,"0x");
                       putStrAt(info, pbCursor,buf);
                } break;
                case '\0':
                       putCAt(info, pbCursor, '%');
                       return;
                default:
                       putCAt(info, pbCursor, '%'); 
                       putCAt(info, pbCursor,*format);
                } 
           break;
           default:
                putCAt(info,pbCursor,*format);
           }
           format++;
   } 
   va_end(args);
}
#endif
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


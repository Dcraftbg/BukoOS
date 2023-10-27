#pragma once
#include <stdint.h>
namespace Kernel {
inline uint8_t inb(uint16_t port) {
   uint8_t ret;
   asm volatile("inb %1, %0" : "=a"(ret) : "d"(port));
   return ret; 
}
inline void outb(uint16_t port, uint8_t data) {
   asm volatile("outb %0, %1" : "=a"(data) : "d"(port)); 
}
}; // Kernel

#pragma once
#include <stdint.h>
namespace Kernel {
inline uint8_t inb(uint16_t port) {
   uint8_t ret;
   asm volatile("inb %1, %0" : "=a"(ret) : "d"(port));
   return ret; 
}
inline void outb(uint16_t port, uint8_t data) {
   asm volatile("outb %1, %0" : : "d"(port), "a"(data)); 
}
inline uint32_t inl(uint16_t port) {
   uint32_t ret;
   asm volatile("inl %1, %0" : "=a"(ret) : "d"(port));
   return ret; 
}
inline void outl(uint16_t port, uint32_t data) {
   asm volatile("outl %1, %0" : : "d"(port), "a"(data)); 
}
}; // Kernel

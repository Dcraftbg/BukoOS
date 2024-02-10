#pragma once
#include <stdint.h>
#include <stddef.h>

#define IDT_TYPE_EXCEPTION 0x8E00
#define IDT_TYPE_HARDWARE  0x8F00
#define IDT_TYPE_SOFTWARE  0xEF00




enum {
   IDT_EXCEPTION_TYPE_DIV_ZERO=0,
};


namespace Kernel {
 
    struct IDTException {
        uint64_t cr2;
		uint64_t r15;
		uint64_t r14;
		uint64_t r13;
		uint64_t r12;
		uint64_t r11;
		uint64_t r10;
		uint64_t r9;
		uint64_t r8;
		uint64_t rbp;
		uint64_t rdi;
		uint64_t rsi;
		uint64_t rdx;
		uint64_t rcx;
		uint64_t rbx;
		uint64_t rax;
        uint64_t type;
        uint64_t code;
        uint64_t rip;
        uint64_t cs;
        uint64_t flags;
        uint64_t rsp;
        uint64_t ss;
    } __attribute__((packed));
    struct IDTEntry {
		uint16_t	base_low;
		uint16_t	gdt_descriptor;
		uint16_t	type;
		uint16_t	base_middle;
		uint32_t	base_high;
		uint32_t	reserved;
   };
   struct IDTDescriptor {
        uint16_t size;
        uint64_t addr;
   } __attribute__((packed));
   class IDT {
   public:
       IDTEntry* idt;
       void Init(size_t index, int16_t type, void* handler);
       IDT(void* idt);
   };
}

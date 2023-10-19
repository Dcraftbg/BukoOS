#include "idt.h"
void Kernel::IDT::Init(size_t index, void (*handler)()) {
    uint64_t handler_address = (uint64_t)handler;
    idt[index].offset_1 = handler_address & 0xFFFF;
    idt[index].selector = 0x08;  // Assuming your code segment selector is 0x08 (kernel code segment)
    idt[index].ist = 0;
    idt[index].type_attributes = 0x8E; // 0x8E is an interrupt gate with privilege level 0
    idt[index].offset_2 = (handler_address >> 16) & 0xFFFF;
    idt[index].offset_3 = (handler_address >> 32) & 0xFFFFFFFF;
    idt[index].zero = 0;
}


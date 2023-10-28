#include "idt.h"
#include "gdt.h"
static volatile void idt_default_handler() {
   (void)idt_default_handler;
}
Kernel::IDT::IDT(void* idt) {
   this->idt=(Kernel::IDTEntry*)idt;
   for(size_t i=0; i<256; ++i) {
        Init(i, 0xEF00, (void*)idt_default_handler);
   }
}
void Kernel::IDT::Init(size_t index, int16_t type, void* handler) {
	idt[index].type = type;
	idt[index].gdt_descriptor = sizeof( ( ( struct Kernel::GDT*) 0 ) -> kernelCodeSegment);
	idt[index].base_low = (uint16_t)(uint64_t) handler;
	idt[index].base_middle = (uint64_t) handler >> 16;
	idt[index].base_high = (uint64_t) handler >> 32;
}


#include "idt.h"
#include "gdt.h"
#include <libs/terminal.h>
// Hidden handlers implemented in idt.nasm
extern "C" void _bukoos_idt_exception_division();
extern "C" void idt_sprocious_int();
// This might be getting called by exception handlers as well! Not Good!
static volatile void idt_default_handler() {
   KERNEL_PRINTF(display, "Calling idt_default_handler! ");
   (void)idt_default_handler;
   asm volatile ("iretq");
}
extern "C" void BukoOS_idt_exception_handler(Kernel::IDTException* exception) {
    KERNEL_PRINTF(display, "Gotten exception of type: %d; ",(int)exception->type);
    return;
}
Kernel::IDT::IDT(void* idt) {
   this->idt=(Kernel::IDTEntry*)idt;
   for(size_t i=32; i<256; ++i) {
        Init(i, IDT_TYPE_SOFTWARE, (void*)idt_default_handler);
   }
   Init(0, IDT_TYPE_EXCEPTION, (void*)_bukoos_idt_exception_division);
   Init(255, IDT_TYPE_HARDWARE, (void*)idt_sprocious_int);
}
void Kernel::IDT::Init(size_t index, int16_t type, void* handler) {
	idt[index].type = type;
	idt[index].gdt_descriptor = sizeof( ( ( struct Kernel::GDT*) 0 ) -> kernelCodeSegment);
	idt[index].base_low = (uint16_t)(uint64_t) handler;
	idt[index].base_middle = (uint64_t) handler >> 16;
	idt[index].base_high = (uint64_t) handler >> 32;
}



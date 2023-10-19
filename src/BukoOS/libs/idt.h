#include <stdint.h>
#include <stddef.h>
namespace Kernel {
   struct IDTEntry {
      uint16_t offset_1;        // offset bits 0..15
      uint16_t selector;        // a code segment selector in GDT or LDT
      uint8_t  ist;             // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
      uint8_t  type_attributes; // gate type, dpl, and p fields
      uint16_t offset_2;        // offset bits 16..31
      uint32_t offset_3;        // offset bits 32..63
      uint32_t zero;            // reserved
   } __attribute__((packed));
   struct IDTDescriptor {
        uint16_t size;
        uint64_t addr;
   } __attribute__((packed));
   class IDT {
   public:
       IDTEntry idt[256] = {0};
       void Init(size_t index, void (*handler)());
       IDT() {}
   };
}

#include <stdint.h>
#include <stddef.h>
#define IDT_TYPE_EXCEPTION 0x8E00
#define IDT_TYPE_HARDWARE 0x8F00
#define IDT_TYPE_SOFTWARE 0xEF00
namespace Kernel {
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

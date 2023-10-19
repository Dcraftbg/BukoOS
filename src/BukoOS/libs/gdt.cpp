#include "gdt.h"
Kernel::GDTEntry::GDTEntry (uint32_t base, uint32_t limit, uint8_t access_byte, uint8_t flags) {
   limit_1 = limit & 0xFFFF;
   base_1 = base & 0xFFFF;
   base_2 = (base >> 16) & 0xFF;
   access = access_byte;
   flags_and_limit = (((uint16_t)flags) << 8) | (limit >> 16);
   base_3 = (base >> 24) & 0xFF;
#if 0
        m_rawEntry[0] = (limit >> 0)  & 0xFF;
        m_rawEntry[1] = (limit >> 8)  & 0xFF;
        m_rawEntry[6] = (limit >> 16) & 0xFF;

        m_rawEntry[2] = (base >> 0)  & 0xFF;
        m_rawEntry[3] = (base >> 8)  & 0xFF;
        m_rawEntry[4] = (base >> 16) & 0xFF;
        m_rawEntry[7] = (base >> 24) & 0xFF;

        m_rawEntry[5] = access_byte;
        m_rawEntry[6] |= flags << 4;
#endif
}

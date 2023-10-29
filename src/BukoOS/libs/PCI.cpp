#include "PCI.h"
uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
   uint32_t address = (uint32_t)(((uint32_t)bus) << 16) | (((uint32_t)slot)<<11) | 
           (((uint32_t)func) << 8) | (offset&0xFC) | ((uint32_t)0x80000000);
   Kernel::outl(0xCF8, address);
   return (uint16_t)((Kernel::inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
}


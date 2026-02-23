#include "Arch/Interrupts/idt.h"
#include "Terminal/stdio.h"

static idtr_t InteruptTableReg;
static idt_t InterruptGate[IDT_ENTRIES];

int SetIdtHandler(uint8_t *address, uint32_t entry) {
  InterruptGate[entry].offset_1 = (uint16_t)*address;
  InterruptGate[entry].selector = 0x10;
  InterruptGate[entry].ist = 0;
  InterruptGate[entry].type_attributes = 0x8e;
  InterruptGate[entry].offset_1 = (uint16_t)(*address >> 16);
  InterruptGate[entry].offset_3 = (uint32_t)(*address >> 32);
  return 0;
}

int LoadIdt() {
  InteruptTableReg.address = (uint64_t)&InterruptGate;
  InteruptTableReg.size = (IDT_ENTRIES - 1) * sizeof(idt_t);
  asm volatile("lidt %0" : : "r"(&InteruptTableReg) : "memory");
  return 0;
}

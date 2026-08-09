#include "Arch/Interrupts/idt.h"
#include "Terminal/stdio.h"

static idtr_t interrupt_table_reg;
static idt_t interrupt_gate[IDT_ENTRIES];

int SetIdtHandler(uint64_t address, uint32_t entry) {
  interrupt_gate[entry].offset_1 = (uint16_t)address;
  interrupt_gate[entry].selector = 0x10;
  interrupt_gate[entry].ist = 0;
  interrupt_gate[entry].type_attributes = 0x8e;
  interrupt_gate[entry].offset_2 = (uint16_t)(address >> 16);
  interrupt_gate[entry].offset_3 = (uint32_t)(address >> 32);
  interrupt_gate[entry].zero = 0;
  return 0;
}

int LoadIdt() {
  interrupt_table_reg.address = (uint64_t)&interrupt_gate;
  interrupt_table_reg.size = IDT_ENTRIES * sizeof(idt_t) - 1;
  asm volatile("lidt %0" : : "m"(&interrupt_table_reg) : "memory");
  return 0;
}

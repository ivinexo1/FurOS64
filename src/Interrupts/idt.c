#include "../include/idt.h"

// interrupt descriptor table
// 256 gates each 16 bytes

idt_register_t idt_reg;
idt_gate_t idt[IDT_ENTRIES];

void set_idt_gate(int n, uint64_t handler) {
    idt[n].offset_low = LOW_16(handler);
    idt[n].offset_mid = MID_16(handler);
    idt[n].offset_high = HIGH_32(handler);
    idt[n].selector = 0x08;
    idt[n].ist = 0x00;
    idt[n].type_attr = 0x8E;
    idt[n].reserved = 0;
}

void load_idt() {
  idt_reg.base = (uint64_t) &idt;
  idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
  asm volatile("lidt (%0)": : "r" (&idt_reg));
}
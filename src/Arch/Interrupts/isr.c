#include "Arch/Interrupts/isr.h"
#include "Arch/Interrupts/idt.h"
#include "Terminal/stdio.h"

isr_t interrupt_handlers[ISR_ENTRIES];

void isr_install() {
}

void isr_handler(registers_t *r) {
    PrintString(exception_msg[r->int_no]);
    asm volatile("hlt");
}

void irq_handler(registers_t *r) {
    if (interrupt_handlers[r->int_no] != 0) {
        isr_t handler = interrupt_handlers[r->int_no];
        handler(r);
    }
    // if (r->int_no >= 40) {
    //     outb(0xA0, 0x20);
    // }
    // outb(0x20, 0x20);
}

void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

char *exception_msg[] = {
    "DIVISION BY 0",
    "DEBUG",
    "NON MASKABLE INTERRUPT",
    "BREAKPOINT",
    "INTO DETECTED OVERFLOW",
    "OUT OF BOUNDS",
    "INVALID OPCODE",
    "NO COPROCESSOR",

    "DOUBLE FAULT",
    "COPROCESSOR SEGMENT OVERRUN",
    "BAD TSS",
    "SEGMENT NOT PRESENT",
    "STACK FAULT",
    "GENERAL PROTECTION FAULT",
    "PAGE FAULT",
    "UNKNOWN INTERRUPT",

    "COPROCESSOR FAULT",
    "ALIGNMENT CHECK",
    "MACHINE CHECK",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",

    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
};
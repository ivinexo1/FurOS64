#include "Arch/Interrupts/isr.h"
#include "Arch/Interrupts/idt.h"
#include "Terminal/stdio.h"

isr_t interrupt_handlers[ISR_ENTRIES];

void isr_install()
{
    SetIDTGate(0, (uint64_t)isr0);
    SetIDTGate(1, (uint64_t)isr1);
    SetIDTGate(2, (uint64_t)isr2);
    SetIDTGate(4, (uint64_t)isr3);
}

void isr_handler(registers_t *r)
{
}

void irq_handler(registers_t *r)
{
}

__attribute__ ((interrupt)) void register_interrupt_handler(uint8_t n, isr_t handler)
{
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
#include "Arch/Interrupts/isr.h"
#include "Arch/Interrupts/idt.h"
#include "Terminal/stdio.h"

isr_t interrupt_handlers[ISR_ENTRIES];

void ISRInstall()
{
    LoadIDT();
}

void ISRHandler(registers_t *r)
{
}

void IRQHandler(registers_t *r)
{
}

void RegisterInterruptHandler(uint8_t n, isr_t handler)
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
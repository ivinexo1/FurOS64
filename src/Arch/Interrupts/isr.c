#include "Arch/Interrupts/isr.h"
#include "Arch/Interrupts/idt.h"
#include "Terminal/stdio.h"

isr_t interrupt_handlers[ISR_ENTRIES];

void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}
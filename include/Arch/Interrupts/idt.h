#include <stdint.h>

#define IDT_ENTRIES 256

typedef struct {
  uint16_t size;
  uint64_t address;
} __attribute__((packed)) idtr_t;

typedef struct {
  uint16_t offset_1;
  uint16_t selector;
  uint8_t  ist;
  uint8_t  type_attributes;
  uint16_t offset_2;
  uint32_t offset_3;
  uint32_t zero;
} __attribute__((packed)) idt_t;
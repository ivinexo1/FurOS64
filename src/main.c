#include "./font3.c"
#include "Drivers/vga.h"
#include "Terminal/stdio.h"
#include <stdint.h>

typedef struct {
  uint32_t Type;
  uint8_t *PhysicalStart;
  uint8_t *VirtualStart;
  uint64_t NumberOfPages;
  uint64_t Attribute;
} EFI_MEMORY_DESCRIPTOR;

void kernel(Pixel *framebuffer, uint64_t framebuffersizem, uint32_t vert,
            uint32_t hori, uint32_t ppsl, EFI_MEMORY_DESCRIPTOR memmap) {
  initVga(framebuffer, framebuffersizem, hori);
  initTerminal(vert, hori);
  PrintChar(0, 0, 'A');
  while (1)
    ;
}

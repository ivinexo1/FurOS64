#include "./font3.c"
#include "Drivers/vga.h"
#include "MemoryManagment/MemoryManager.h"
#include "Terminal/stdio.h"
#include "stdlib/string.h"
#include <cpuid.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct {
  uint64_t MemoryMapSize;
  uint64_t DescriptorSize;
  uint64_t MapKey;
  uint32_t DescriptorVersion;
  uint64_t MemoryMap;
} MEMORY_MAP_DESCRIPTOR;

typedef struct {
  Pixel *framebuffer;
  uint64_t framebuffersize;
  uint32_t VerticalResolution;
  uint32_t HorizontalResolution;
  uint32_t PixelsPerScanline;
  MEMORY_MAP_DESCRIPTOR *MemoryMapDescriptor;
} kernel_args;
static kernel_args *KernelArgs = 0;

void kernel() {
  asm volatile("mov %%rdi, %0" : "=r"(KernelArgs)::"memory");
  asm volatile("cli");
  for (uint64_t i = 0; i < KernelArgs->framebuffersize / 16; i++) {
    KernelArgs->framebuffer[i].bule = 0xff;
  }
  // asm volatile("hlt");
  initVga(KernelArgs->framebuffer, KernelArgs->framebuffersize,
          KernelArgs->PixelsPerScanline);
  initTerminal(KernelArgs->VerticalResolution,
               KernelArgs->HorizontalResolution);
  PrintString("Hello\n");
  //   InitMemoryManagment(KernelArgs->MemoryMap, KernelArgs->retSize,
  //                       KernelArgs->MemMapSize);
  while (1)
    ;
}

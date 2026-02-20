#include "./font3.c"
#include "Drivers/vga.h"
#include "MemoryManagment/MemoryManager.h"
#include "Terminal/stdio.h"
#include "stdlib/string.h"
#include <cpuid.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct {
  Pixel *framebuffer;
  uint64_t framebuffersize;
  uint32_t VerticalResolution;
  uint32_t HorizontalResolution;
  uint32_t PixelsPerScanline;
  uint64_t retSize;
  uint64_t MemMapSize;
  EFI_MEMORY_DESCRIPTOR *MemoryMap;
} kernel_args;

void kernel() {
  kernel_args *KernelArgs = 0;
  asm volatile("mov %%rdi, %0" : "=r"(KernelArgs)::"memory");
  for (uint64_t i = 0; i < KernelArgs->framebuffersize / 4; i++) {
    KernelArgs->framebuffer[i].bule = 0xff;
  }
  initVga(KernelArgs->framebuffer, KernelArgs->framebuffersize,
          KernelArgs->PixelsPerScanline);
  initTerminal(KernelArgs->VerticalResolution,
               KernelArgs->HorizontalResolution);
  uint64_t cr0, *cr3, cr4, efer;
  uint32_t eferH, eferL;
  asm volatile("mov %%cr3, %0" : "=r"(cr3)::"memory");
  asm volatile("mov %%cr4, %0" : "=r"(cr4)::"memory");
  asm volatile("rdmsr" : "=a"(eferL), "=d"(eferH) : "c"(0xC0000080) : "memory");
  efer = eferL | ((uint64_t)eferH << 32);
  cr3 = (uint64_t *)((uint64_t)cr3 & (~0xfff));
  uint64_t *pml4 = (uint64_t *)*cr3;
  PrintHex(cr0);
  PrintChar('\n');
  PrintHex(((uint64_t *)((uint64_t)*(uint64_t *)((uint64_t)pml4 & ~0xfff) &
                         ~0xfff))[0x7]);
  PrintChar('\n');
  PrintHex(cr4);
  PrintChar('\n');
  PrintHex(efer);
  //  InitMemoryManagment(KernelArgs->MemoryMap, KernelArgs->retSize,
  //                      KernelArgs->MemMapSize);
  while (1)
    ;
}

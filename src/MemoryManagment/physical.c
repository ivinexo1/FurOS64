#include "MemoryManagment/MemoryManager.h"
#include "Terminal/stdio.h"
#include "kernel_args.h"
#include <stddef.h>
#include <stdint.h>

static EFI_MEMORY_DESCRIPTOR *memory_map;
static uint64_t descriptor_size;
static uint64_t memory_map_size;
static uint8_t *frame_map = NULL;
static uint64_t frame_map_size;
static uint64_t *temp_mapping_region;
static void *dynamic_memory;

int memcpy(void *restrict dest, const void *restrict src, size_t n) {
  for (size_t i = 0; i < n; i++) {
    ((uint8_t *)dest)[i] = ((uint8_t *)src)[i];
  }
  return 0;
}

int AllocFrame(uint8_t *frame_pointer) {
  frame_pointer = (uint8_t *)((uint64_t)frame_pointer / 0x1000);
  uint64_t offset = (uint64_t)frame_pointer / 8;
  uint64_t bit_in_byte = (uint64_t)frame_pointer % 8;
  if (((frame_map[offset] >> bit_in_byte) & 1) != 0) {
    return -1;
  }
  frame_map[offset] |= (uint8_t)(1 << bit_in_byte);
  return 0;
}

int FreeFrame(uint8_t *frame_pointer) {
  uint64_t offset = (uint64_t)frame_pointer / 8;
  uint64_t bit_in_byte = (uint64_t)frame_pointer % 8;
  if (((frame_map[offset] >> bit_in_byte) & 1) != 1) {
    return -1;
  }
  frame_map[offset] &= ~(uint8_t)(1 << bit_in_byte);
  return 0;
}

int RefreshPages() {
  asm volatile("mov %%cr3, %%rax\n\r"
               "mov %%rax, %%cr3" ::
                   : "memory");
  return 0;
}

void *TempMap(void *restrict physical_address) {
  size_t i = 1;
  while (temp_mapping_region[i] & 1) {
    if (i > 511) {
      return NULL;
    }
  }
  if (i > 511) {
    return NULL;
  }
  temp_mapping_region[i] = (uint64_t)physical_address & PTE_ADDR_MASK | 3;
  return temp_mapping_region + i * 0x1000;
}

int FreeTempMap(void *restrict virtual_address) {
  temp_mapping_region[(((uint64_t)virtual_address >> 12) & 0x1ff) / 8] &=
      ~(uint64_t)0x1;
  RefreshPages();
  return 0;
}

int InitMemoryManagment(void *in_memory_map, uint64_t in_descriptor_size,
                        uint64_t in_memory_map_size, void *in_frame_map,
                        uint64_t in_frame_map_size,
                        uint64_t *in_temp_mapping_region) {
  memory_map = in_memory_map;
  memory_map_size = in_memory_map_size;
  descriptor_size = in_descriptor_size;
  frame_map = in_frame_map;
  frame_map_size = in_frame_map_size;
  temp_mapping_region = in_temp_mapping_region;
  dynamic_memory = temp_mapping_region + 0x1000 * 512;

  return 0;
}

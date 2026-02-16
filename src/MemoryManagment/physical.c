#include "MemoryManagment/MemoryManager.h"
#include "Terminal/stdio.h"
#include <stddef.h>
#include <stdint.h>

static EFI_MEMORY_DESCRIPTOR *memory_map;
static uint64_t descriptor_size;
static uint64_t memory_map_size;
static uint8_t *frame_map = NULL;
static uint64_t frame_map_size;

int memcpy(void *from, void *to, uint64_t size) {
  for (int i = 0; i < size; i++) {
    *((uint8_t *)to + i) = *((uint8_t *)from + i);
  }
  return 0;
}

int AllocFrame(uint8_t *frame_pointer) {
  frame_pointer = (uint8_t *)((uint64_t)frame_pointer / 0x1000);
  uint64_t offset = (uint64_t)frame_pointer / 8;
  uint64_t bit_in_byte = (uint64_t)frame_pointer % 8;
  if (((frame_map[offset] >> bit_in_byte) & 1) != 0) {
    PrintString("frame alloc error\n");
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

int InitMemoryManagment(EFI_MEMORY_DESCRIPTOR *in_memory_map,
                        uint64_t in_descriptor_size,
                        uint64_t in_memory_map_size) {
  uint64_t pages_in_mem_count = 0;
  uint64_t entries_in_memmap = in_memory_map_size / in_descriptor_size;
  //  PrintNum(entries_in_memmap);
  //  PrintChar('\n');
  EFI_MEMORY_DESCRIPTOR *map_entry;
  for (int i = 0; i < in_memory_map_size; i += in_descriptor_size) {
    map_entry = ((EFI_MEMORY_DESCRIPTOR *)((uint8_t *)in_memory_map + i));
    pages_in_mem_count += map_entry->NumberOfPages;
  }
  frame_map_size = pages_in_mem_count;
  uint64_t pages_of_frame_map = frame_map_size / 0x1000 + 1;
  // PrintNum(pages_of_frame_map);
  // PrintChar('\n');

  uint64_t mammap_current_entry = 0;
  while (frame_map == NULL && mammap_current_entry < entries_in_memmap) {
    map_entry =
        ((EFI_MEMORY_DESCRIPTOR *)((uint8_t *)in_memory_map +
                                   mammap_current_entry * in_descriptor_size));
    if (map_entry->NumberOfPages >= pages_of_frame_map) {
      if (map_entry->Type == EfiConventionalMemory) {
        frame_map = map_entry->PhysicalStart;
      }
    }
    mammap_current_entry++;
  }
  if (frame_map == NULL) {
    PrintString("Couldnt find large enought segment for frame map\n");
    return -1;
  }
  PrintHex((uint64_t)frame_map);
  PrintChar('\n');
  PrintChar('a');

  for (int i = 0; i < frame_map_size; i++) {
    frame_map[i] = 0;
  }
  PrintChar('a');
  // for (uint64_t i = 0; i < 76; i++) {
  //   AllocFrame((void *)(i * 0x1000));
  //   AllocFrame((void *)(i * 0x1000));
  //   if (AllocFrame((void *)(i * 0x1000)) == -1) {
  //     PrintString("Problem with frame allocaion\n");
  //     return -1;
  //   }
  // }
  //   PrintHex(0);
  //   PrintHex(frame_map[0]);
  return 0;
}

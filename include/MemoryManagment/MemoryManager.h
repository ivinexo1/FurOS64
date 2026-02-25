#include <stddef.h>
#include <stdint.h>

#define PTE_ADDR_MASK 0x000FFFFFFFFFF000ULL

typedef enum {
  EfiReservedMemoryType,
  EfiLoaderCode,
  EfiLoaderData,
  EfiBootServicesCode,
  EfiBootServicesData,
  EfiRuntimeServicesCode,
  EfiRuntimeServicesData,
  EfiConventionalMemory,
  EfiUnusableMemory,
  EfiACPIReclaimMemory,
  EfiACPIMemoryNVS,
  EfiMemoryMappedIO,
  EfiMemoryMappedIOPortSpace,
  EfiPalCode,
  EfiPersistentMemory,
  EfiUnacceptedMemoryType,
  EfiMaxMemoryType
} EFI_MEMORY_TYPE;

typedef struct {
  EFI_MEMORY_TYPE Type;
  uint8_t *PhysicalStart;
  uint8_t *VirtualStart;
  uint64_t NumberOfPages;
  uint64_t Attribute;
} EFI_MEMORY_DESCRIPTOR;

int memcpy(void *restrict dest, const void *restrict src, size_t n);

int InitMemoryManagment(void *in_memory_map, uint64_t in_descriptor_size,
                        uint64_t in_memory_map_size, void *in_frame_map,
                        uint64_t in_frame_map_size,
                        uint64_t *in_temp_mapping_region);

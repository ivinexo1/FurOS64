#include <stdint.h>

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

int memcpy(void *from, void *to, uint64_t size);

int InitMemoryManagment(EFI_MEMORY_DESCRIPTOR *in_memory_map,
                        uint64_t in_descriptor_size,
                        uint64_t in_memory_map_size);

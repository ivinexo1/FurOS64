#include <stdint.h>

typedef struct {
  uint8_t blue;
  uint8_t green;
  uint8_t red;
  uint8_t reserved;
} Pixel;

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
  uint8_t *FrameMap;
  uint64_t FrameMapSize;
} kernel_args;
static kernel_args *KernelArgs = 0;

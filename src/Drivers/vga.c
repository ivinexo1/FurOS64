#include "Drivers/vga.h"
#include <stdint.h>

static Pixel *framebuffer;
static uint64_t size;
static uint32_t pixel_per_scanline;

void initVga(Pixel *inFramebuffer, uint64_t inSize, uint32_t inPixelscanline) {
  framebuffer = inFramebuffer;
  size = inSize;
  pixel_per_scanline = inPixelscanline;
  uint32_t color = 0x000000;
  for (uint64_t i = 0; i < size; i++) {
    framebuffer[i] = *(Pixel *)&color;
  }
}

uint32_t PrintPixel(uint32_t x, uint32_t y, uint32_t color) {
  framebuffer[x + y * pixel_per_scanline] = *(Pixel *)&color;
  return 0;
}

#include <stdint.h>

typedef struct {
  uint8_t bule;
  uint8_t green;
  uint8_t red;
  uint8_t reserved;
} Pixel;

void initVga(Pixel *inFramebuffer, uint64_t inSize, uint32_t inPixelscanline);
uint32_t PrintPixel(uint32_t x, uint32_t y, uint32_t color);

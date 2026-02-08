#include "Terminal/stdio.h"
#include "../font3.c"
#include "Drivers/vga.h"
#include <stdint.h>

static uint32_t x = 0;
static uint32_t y = 0;
static uint32_t max_vertical = 0;
static uint32_t max_horizontal = 0;
static uint32_t bgcolor = 0;
static uint32_t fgcolor = 0xffffff;

void initTerminal(uint32_t inVertical, uint32_t inHorizontal) {
  x = 0;
  y = 0;
  max_vertical = inVertical / FONT_HEIGHT;
  max_horizontal = inHorizontal / FONT_WIDTH;
}

int PrintChar(uint32_t x, uint32_t y, char character) {
  char printed;
  if (0x40 < character && character < 0x5b) {
    printed = character - 0x41 + 10;
  }
  for (int i = 0; i < FONT_HEIGHT; i++) {
    for (int j = 0; j < FONT_WIDTH; j++) {
      if (font_data[printed][j * FONT_WIDTH + i]) {
        PrintPixel(i + x, j + y, fgcolor);
      } else {
        PrintPixel(i + max_vertical, j + max_horizontal, bgcolor);
      }
    }
  }
  return 0;
}

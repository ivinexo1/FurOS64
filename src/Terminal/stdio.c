#include "Terminal/stdio.h"
#include "../font3.c"
#include "Drivers/vga.h"
#include "stdlib/string.h"
#include <stdint.h>

static uint32_t x = 0;
static uint32_t y = 0;
static uint32_t max_vertical = 0;
static uint32_t max_horizontal = 0;
static uint32_t bgcolor = 0x000000;
static uint32_t fgcolor = 0xffffff;

void initTerminal(uint32_t inVertical, uint32_t inHorizontal) {
  x = 0;
  y = 0;
  max_vertical = inVertical / FONT_HEIGHT;
  max_horizontal = inHorizontal / FONT_WIDTH;
}

int PrintChar(char character) {
  char printed;

  if (character == 0x20) {

    x++;
  } else if (character == 0xa) {
    y++;
    x = 0;
  } else {
    if (0x40 < character && character < 0x5b) {
      printed = character - 0x41 + 10;
    } else if (0x2f < character && character < 0x3a) {
      printed = character - 0x30;
    } else if (character > 0x60) {
      printed = character - 0x61 + 36;
    }

    for (int i = 0; i < FONT_HEIGHT; i++) {
      for (int j = 0; j < FONT_WIDTH; j++) {
        if (font_data[printed][j * FONT_WIDTH + i]) {
          PrintPixel(i + x * FONT_WIDTH, j + y * FONT_HEIGHT, fgcolor);
        } else {
          PrintPixel(i + x * FONT_WIDTH, j + y * FONT_HEIGHT, bgcolor);
        }
      }
    }

    x++;
  }
  if (x >= max_horizontal) {
    y++;
    x = 0;
  }
  return 0;
}

int PrintNum(uint32_t number) {
  uint8_t numstring[10];
  uint8_t numsize = 0;

  if (number == 0) {
    PrintChar('0');
    return 0;
  }

  while (number != 0) {
    numstring[numsize] = (uint8_t)(number % 10) + '0';
    number = number / 10;
    numsize++;
  }
  for (uint8_t i = numsize; i > 0; i--) {
    PrintChar(numstring[i - 1]);
  }
  return 0;
}

int PrintHex(uint64_t number) {
  uint8_t numstring[16];
  uint8_t numsize = 0;

  if (number == 0) {
    PrintChar('0');
    return 0;
  }

  while (number != 0) {
    numstring[numsize] = (uint8_t)(number % 0x10);
    if (numstring[numsize] < 10) {
      numstring[numsize] += '0';
    } else {
      numstring[numsize] += 55;
    }
    number = number / 0x10;
    numsize++;
  }
  for (uint8_t i = numsize; i > 0; i--) {
    PrintChar(numstring[i - 1]);
  }
  return 0;
}

int PrintString(char *string) {
  uint32_t stringlen = strlen(string);
  for (int i = 0; i < stringlen; i++) {
    PrintChar(string[i]);
  }
  return 0;
}

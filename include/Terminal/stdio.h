#include <stdint.h>

#define FONT_HEIGHT 40
#define FONT_WIDTH 40

void initTerminal(uint32_t inVertical, uint32_t inHorizontal);
int PrintChar(uint32_t x, uint32_t y, char character);

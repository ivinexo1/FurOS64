#include <stdint.h>

#define FONT_HEIGHT 40
#define FONT_WIDTH 40

void initTerminal(uint32_t inVertical, uint32_t inHorizontal);
int PrintChar(char character);
int PrintNum(uint32_t number);
int PrintHex(uint64_t number);
int PrintString(char *string);

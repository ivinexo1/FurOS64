#include "stdlib/string.h"
#include <stdint.h>

uint32_t strlen(char *string) {
  uint32_t stringsize = 0;
  uint32_t stringindex = 0;
  while (string[stringindex] != 0) {
    stringindex++;
  }
  return stringindex;
}

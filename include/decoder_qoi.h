#include <stdint.h>

typedef struct {
    unsigned char r, g, b, a;
} qoi_rgba;

typedef struct {
    unsigned int width;
    unsigned int height;
    unsigned char channels;
    unsigned char colorspace;
} qoi_desc;

extern const uint8_t _binary_image_qoi_start[];
extern const uint8_t _binary_image_qoi_end[];

int decode_qoi();
#include <include/decoder_qoi.h>

typedef struct {
    unsigned char r, g, b, a;
} qoi_rgba_t;

typedef struct {
    unsigned int width;
    unsigned int height;
    unsigned char channels;
    unsigned char colorspace;
} qoi_desc;
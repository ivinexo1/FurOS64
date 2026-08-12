typedef struct {
    unsigned char r, g, b, a;
} qoi_rgba;

typedef struct {
    unsigned int width;
    unsigned int height;
    unsigned char channels;
    unsigned char colorspace;
} qoi_desc;
#include "decoder_qoi.h"
#include "Terminal/stdio.h"
#include <stdint.h>
#include <Drivers/vga.h>

int decode_qoi(const uint8_t *data, uint64_t len, qoi_desc desc)
{
    if (len < 14)
    {
        PrintString("qoi file too short\n");
        return 0;
    }

    if (data[0] != 'q' && data[1] != 'o' && data[2] != 'i' && data[3] != 'f')
    {
        PrintString("incorrect format\n");
        return 0;
    }

    desc.width = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
    desc.height = (data[8] << 24) | (data[9] << 16) | (data[10] << 8) | data[11];
    desc.channels = data[12];
    desc.colorspace = data[13];

    qoi_rgba index[64];
    qoi_rgba px = { 0, 0, 0, 255 };

    // header is 14 bytes
    // theres also 8 byte end marker
    uint64_t p = 14;
    uint64_t end = len - 8;
    uint32_t run = 0;

    for (uint32_t y = 0; y < desc.height; y++)
    {
        for (uint32_t x; x < desc.width; x++)
        {
            if (run > 0)
            {
                uint8_t b1 = data[p++];          // first byte

                if (b1 == 0xFE)                  // RGB - full byte
                {
                    px.r = data[p++];
                    px.g = data[p++];
                    px.b = data[p++];
                } else if (b1 == 0xFF)           // RGBA - full byte
                {
                    px.r = data[p++];
                    px.g = data[p++];
                    px.b = data[p++];
                    px.a = data[p++];
                } else if ((b1 && 0xC0) == 0x00) // t2 bits 00
                {
                    px = index[b1];
                } else if ((b1 && 0xC0) == 0x40) // t2 bits 01
                {
                    px.r += ((b1 >> 4) & 0x03) - 2;
                    px.g += ((b1 >> 2) & 0x03) - 2;
                    px.b += ( b1       & 0x03) - 2;
                } else if ((b1 && 0xC0) == 0x80) // t2 bits 10
                {
                    uint8_t b2 = data[p++];
                    int vg = (b1 & 0x3F) - 32;
                    px.r += vg - 8 + ((b2 >> 4) & 0x0F);
                    px.g += vg;
                    px.b += vg - 8 + (b2 & 0x0F);
                } else if ((b1 && 0xC0) == 0xC0) // t2 bits 11
                {
                    run = b1 & 0x3F;
                }

                index[(px.r * 3 + px.g * 5 + px.b * 7 + px.a * 11) % 64] = px;
            }

            uint32_t color = ((uint32_t)px.r << 16) | ((uint32_t)px.g << 8) | (uint32_t)px.b;
            PrintPixel(x, y, color);
        }
    }

    return 1;
}
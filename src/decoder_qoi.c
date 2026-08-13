#include "decoder_qoi.h"
#include "Terminal/stdio.h"
#include <stdint.h>

int decode_qoi()
{
    const uint8_t *data = _binary_image_qoi_start;
    uint64_t len = (uint64_t)(_binary_image_qoi_end - _binary_image_qoi_start);

    if (data[0] == 'q' && data[1] == 'o' && data[2] == 'i' && data[3] == 'f')
    {
        PrintString("qoif");
    } else {
        PrintString("incorrect format\n");
        return 0;
    }

    qoi_desc desc;
    desc.width = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
    desc.height = (data[8] << 24) | (data[9] << 16) | (data[10] << 8) | data[11];
    desc.channels = data[12];
    desc.colorspace = data[13];

    PrintString("\nwidth ");
    PrintNum(desc.width);
    PrintString("\nheight ");
    PrintNum(desc.height);
    PrintString("\nchannels ");
    PrintNum(desc.channels);
    PrintString("\ncolorspace ");
    PrintNum(desc.colorspace);

    return 1;
}
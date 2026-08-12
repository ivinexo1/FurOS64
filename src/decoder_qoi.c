#include "decoder_qoi.h"
#include <stdint.h>

int decode_qoi()
{
    const uint8_t *data = _binary_image_qoi_start;
    uint64_t len = (uint64_t)(_binary_image_qoi_end - _binary_image_qoi_start);

    if (data[0] == 'q' && data[1] == 'o' && data[2] == 'i' && data[3] == 'f')
    {
        return 1;
    } else {
        return 0;
    }

    qoi_desc desc;
    desc.width = 0;
    desc.height = 0;
    desc.channels = 0;
    desc.channels = 0;
}
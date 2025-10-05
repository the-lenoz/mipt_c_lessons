#include "debug_utilites.hpp"



void to_hex(char* dst, void* data, size_t data_size)
{
    unsigned char byte = 0;
    for (size_t j = 0; j < data_size; ++j)
    {
        byte = ((unsigned char*)data)[j];
        dst[j*3] = hex_numbers[(byte & 0xf0) >> 4];
        dst[j*3 + 1] = hex_numbers[(byte & 0x0f) >> 0];
        if (j < data_size - 1)
        {
            dst[j*3 + 2] = ' ';
        }
    }
}


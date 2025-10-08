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



unsigned int calc_shift_hash(const unsigned char* val, size_t len, unsigned int state)
{
    if (len == 0) return state;
    return calc_shift_hash(val + 1, len - 1, 
        (unsigned int)(((unsigned long) val[0] * state) % (unsigned long)(MAX_HASH_STATE))) ^ ((unsigned int)(val[0] << 4) + val[0]);
}



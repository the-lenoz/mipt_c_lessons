#include "AVXXX_N.hpp"
#include <string.h>


int long_add(int8_t* a, int8_t* b, int8_t* dst, uint32_t count)
{
    uint8_t carry = 0;
    uint8_t a_val = 0, b_val = 0;

    for (uint32_t i = 0; i < (uint32_t)count; ++i)
    {
        memcpy(&a_val, a + i, 1);
        memcpy(&b_val, b + i, 1);
        carry = a_val + b_val + carry;
        
        memcpy(dst + i, &carry, 1);
        carry = carry / 256;
    }
    return 0;
}

int long_sub(int8_t* a, int8_t* b, int8_t* dst, uint32_t count)
{
    uint8_t carry = 0;
    int32_t result = 0;
    uint8_t a_val = 0, b_val = 0;

    for (uint32_t i = 0; i < (uint32_t)count; ++i)
    {
        memcpy(&a_val, a + i, 1);
        memcpy(&b_val, b + i, 1);
        result = a_val - b_val - carry;
        
        memcpy(dst + i, &result, 1);
        carry = result >= 0 ? 0 : 1;
    }
    return 0;
}

int long_mul(int8_t* a, int8_t* b, int8_t* dst, uint32_t count)
{
    //NOT YET IMPLEMENTED

    uint8_t carry = 0;
    int32_t result = 0;
    uint8_t a_val = 0, b_val = 0;

    for (uint32_t i = 0; i < (uint32_t)count; ++i)
    {
        memcpy(&a_val, a + i, 1);
        memcpy(&b_val, b + i, 1);
        result = a_val - b_val - carry;
        
        memcpy(dst + i, &result, 1);
        carry = result >= 0 ? 0 : 1;
    }
    return 0;
}

int long_div(int8_t* a, int8_t* b, int8_t* dst, uint32_t count)
{
    //NOT YET IMPLEMENTED

    uint8_t carry = 0;
    int32_t result = 0;
    uint8_t a_val = 0, b_val = 0;

    for (uint32_t i = 0; i < (uint32_t)count; ++i)
    {
        memcpy(&a_val, a + i, 1);
        memcpy(&b_val, b + i, 1);
        result = a_val - b_val - carry;
        
        memcpy(dst + i, &result, 1);
        carry = result >= 0 ? 0 : 1;
    }
    return 0;
}
    
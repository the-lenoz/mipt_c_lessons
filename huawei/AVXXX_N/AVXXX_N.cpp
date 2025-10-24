#include "AVXXX_N.hpp"
#include <string.h>

#define OPTIMIZATION_CASE(OP, type, dst, a, b) case sizeof(type): *(type*)dst = OP(*(type*)a, *(type*)b); break;

#define OPTIMIZE_LONG(OP, unoptimized, cnt, dst, a, b)  \
    do switch (cnt)                                     \
    {                                                   \
        OPTIMIZATION_CASE(OP, int8_t, dst, a, b)        \
        OPTIMIZATION_CASE(OP, int16_t, dst, a, b)       \
        OPTIMIZATION_CASE(OP, int32_t, dst, a, b)       \
        OPTIMIZATION_CASE(OP, int64_t, dst, a, b)       \
        default: unoptimized break;                     \
    } while(0)



int long_add(int8_t* a, int8_t* b, int8_t* dst, uint32_t count)
{
    uint8_t carry = 0;

    #define OPERATION(a, b) ((a) + (b))
    OPTIMIZE_LONG(OPERATION, 
    {
        for (uint32_t i = 0; i < (uint32_t)count; ++i)
        {
            carry = *(a + i) + *(b + i) + carry;
            
            memcpy(dst + i, &carry, 1);
            carry = carry / 256;
        }
    }, 
    count, dst, a, b);
    

    #undef OPERATION
    return 0;
}

int long_sub(int8_t* a, int8_t* b, int8_t* dst, uint32_t count)
{
    uint8_t carry = 0;
    int32_t result = 0;

    #define OPERATION(a, b) ((a) - (b))
    OPTIMIZE_LONG(OPERATION, 
    {
        for (uint32_t i = 0; i < (uint32_t)count; ++i)
        {
            result = *(a + i) - *(b + i) - carry;
            
            memcpy(dst + i, &result, 1);
            carry = result >= 0 ? 0 : 1;
        }
    },
    count, dst, a, b);
    #undef OPERATION
    return 0;
}

int long_mul(int8_t* a, int8_t* b, int8_t* dst, uint32_t count)
{
    #define OPERATION(a, b) ((a) * (b))
    OPTIMIZE_LONG(OPERATION, 
    {
        //NOT YET IMPLEMENTED
    },
    count, dst, a, b);
    #undef OPERATION
    return 0;
}

int long_div(int8_t* a, int8_t* b, int8_t* dst, uint32_t count)
{
    #define OPERATION(a, b) ((a) / (b))
    OPTIMIZE_LONG(OPERATION, 
    {
        //NOT YET IMPLEMENTED
    },
    count, dst, a, b);
    #undef OPERATION
    return 0;
}
    
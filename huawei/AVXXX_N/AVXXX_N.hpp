#ifndef AVXXX_N_DECLARED
#define AVXXX_N_DECLARED

#include <stdint.h>


int long_add(int8_t* a, int8_t* b, int8_t* dst, uint32_t count);
int long_sub(int8_t* a, int8_t* b, int8_t* dst, uint32_t count);
int long_mul(int8_t* a, int8_t* b, int8_t* dst, uint32_t count);
int long_div(int8_t* a, int8_t* b, int8_t* dst, uint32_t count);

#endif //AVXXX_N_DECLARED

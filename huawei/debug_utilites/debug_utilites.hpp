#ifndef DEBUG_UTILITES_DECLARED
#define DEBUG_UTILITES_DECLARED

#define HASH_BASE_SEED 997*991
#define MAX_HASH_STATE (1L << 31) - 1

#include <stddef.h>


const char *const hex_numbers = "0123456789abcdef";
const unsigned int CANARY = 0xDEADBEEF;


void to_hex(char* dst, void* data, size_t data_size);


unsigned int calc_shift_hash(const unsigned char* val, size_t len, unsigned int state);

#endif // DEBUG_UTILITES_DECLARED
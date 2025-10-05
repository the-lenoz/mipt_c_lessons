#ifndef DEBUG_UTILITES_DECLARED
#define DEBUG_UTILITES_DECLARED

#include <stddef.h>

const char *const hex_numbers = "0123456789abcdef";


void to_hex(char* dst, void* data, size_t data_size);



#endif // DEBUG_UTILITES_DECLARED
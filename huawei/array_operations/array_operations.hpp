#ifndef ARRAY_OPERATIONS_DECLARED
#define ARRAY_OPERATIONS_DECLARED

#include <stdlib.h>

struct my_string
{
    char* str;
    size_t len;
};

size_t split_buffer(char* buffer, char delim, my_string** out_ptr);

size_t count_char(const char* buffer, char c);

#endif // ARRAY_OPERATIONS_DECLARED
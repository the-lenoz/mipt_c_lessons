#ifndef ARRAY_OPERATIONS_DECLARED
#define ARRAY_OPERATIONS_DECLARED

#include <stdlib.h>

struct my_string
{
    char* str;
    size_t len;
};

size_t split_buffer(char* buffer, char delim, my_string** out_ptr);


#endif // ARRAY_OPERATIONS_DECLARED
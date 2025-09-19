#include "array_operations.hpp"
#include <cstddef>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include "error_handling/my_assert.hpp"

size_t split_buffer(char* buffer, char delim, my_string** out_ptr)
{
    assert(buffer != NULL);
    assert(out_ptr != NULL);

    char* old_buffer = buffer;
    
    size_t number_on_lines = count_char(buffer, '\n');
    *out_ptr = (my_string*) calloc(number_on_lines, sizeof(my_string));


    for (size_t i = 0; i < number_on_lines; ++i)
    {
        (*out_ptr)[i].str = buffer;
        
        buffer = strchr(buffer, delim);
        if (buffer == NULL) break;
        
        (*out_ptr)[i].len = (size_t) (buffer - old_buffer - 1);

        old_buffer = buffer;
        buffer++;
    }

    return number_on_lines;
}

size_t count_char(const char* buffer, char c)
{
    size_t n = 0;
    while (1) {
        buffer = strchr(buffer, c);
        if (buffer == NULL) break;
        ++buffer;
        ++n;
    }
    return n;
}
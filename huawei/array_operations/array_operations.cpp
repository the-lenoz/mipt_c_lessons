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
    
    size_t buffer_len = 0;
    size_t buffer_memory_size = 10;

    *out_ptr = (my_string*) calloc(buffer_memory_size, sizeof(my_string));

    my_string* tmp_ptr = NULL;
    char* old_buffer = buffer;

    while(1)
    {
        if (buffer_len >= buffer_memory_size)
        {
            buffer_memory_size *= 2;
            //printf("Buffer memory size: %zu\n", buffer_memory_size);
            tmp_ptr = (my_string*) calloc(buffer_memory_size, sizeof(my_string));

            memcpy(tmp_ptr, *out_ptr, (buffer_len) * sizeof(my_string));
            free(*out_ptr);
            *out_ptr = tmp_ptr;
        }
        
        (*out_ptr)[buffer_len].str = buffer;
        
        buffer = strchr(buffer, delim);
        if (buffer == NULL) break;
        
        (*out_ptr)[buffer_len].len = (size_t) (buffer - old_buffer - 1);
        
        //printf("Line number: %zu, line len: %zu, line_ptr: %p, line: %-.*s\n", buffer_len, (*out_ptr)[buffer_len].len, 
        //    (*out_ptr)[buffer_len].str, (int)(*out_ptr)[buffer_len].len, (*out_ptr)[buffer_len].str);

        old_buffer = buffer;

        buffer++;
        buffer_len++;   
    }

    return buffer_len;
}
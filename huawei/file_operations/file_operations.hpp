#ifndef FILE_OPERATIONS_DECLARED
#define FILE_OPERATIONS_DECLARED

#include "array_operations.hpp"
#include <stdlib.h>

int get_file_size(const char* file_path);

char* allocate_and_read(const char* file_path);

size_t allocate_and_read_lines(const char* file_path, my_string** lines_buffer);



#endif // FILE_OPERATIONS_DECLARED
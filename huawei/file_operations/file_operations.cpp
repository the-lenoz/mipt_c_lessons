#include "file_operations.hpp"
#include "array_operations/array_operations.hpp"
#include "array_operations/array_operations.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include "error_handling/my_assert.hpp"

int get_file_size(const char* file_path)
{
    struct stat st;
    stat(file_path, &st);
    return (int) st.st_size;
}

char* allocate_and_read(const char* file_path)
{
    int file_size = get_file_size(file_path);

    char* buffer = (char*) calloc((size_t) file_size + 1, sizeof(char));

    int fd = open(file_path, O_RDONLY);
    if (fd == -1)
    {
        return NULL;
    }

    long readed_bytes = read(fd, buffer, (size_t) file_size);
    if (readed_bytes == -1)
    {
        return NULL;
    }
    if (readed_bytes < file_size)
    {
        buffer[readed_bytes] = '\0';
    }

    return buffer;
}

size_t allocate_and_read_lines(const char* file_path, my_string** lines_buffer)
{
    assert(file_path != NULL);

    char* buffer = allocate_and_read(file_path);
    
    return split_buffer(buffer, '\n', lines_buffer); 
}
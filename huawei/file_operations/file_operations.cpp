#include "file_operations.hpp"
#include "array_operations/array_operations.hpp"
#include "array_operations/array_operations.hpp"
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include "error_handling/my_assert.hpp"
#include "mystr.hpp"

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
    
    close(fd);

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

size_t allocate_and_write_lines(const char* file_path, my_string* lines_buffer, size_t lines_num, size_t max_file_size)
{
    assert(file_path != NULL);
    size_t lines_written = 0;

    char* file_buffer = (char*) calloc(max_file_size + 1, sizeof(char));

    for (size_t i = 0; i < lines_num; ++i)
    {
        if (lines_buffer[i].len != 0 && !is_empty_mystr(lines_buffer[i]))
        {
            strncat(file_buffer, lines_buffer[i].str, lines_buffer[i].len);
            strncat(file_buffer, "\n", 1);
        }
    }

    int fd = open(file_path, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fd == -1)
    {
        return 0;
    }
    write(fd, file_buffer, strlen(file_buffer));
    free(file_buffer);

    close(fd);
    return lines_written; 
}

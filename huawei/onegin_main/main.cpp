#include <cstddef>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "array_operations.hpp"
#include "error_handling/my_assert.hpp"
#include "file_operations/file_operations.hpp"
#include "logger/logger.hpp"
#include "mystr.hpp"
#include "sort/sort.hpp"
#include "status.hpp"


const char* onegin_input_file_path = "onegin.txt";
const char* onegin_output_file_path = "new_onegin.txt";

LogTarget log_targets[] = 
{
    {}, // stdout
    {"/tmp/log.html"}
};
const int log_targets_count = sizeof(log_targets) / sizeof(log_targets[0]);


int main(int argc, const char** argv)
{
    assert(argv != NULL);
    assert(argc != 0);  
    assert(argv[0] != NULL);
    int fd = 0;

    LOG_START(argv[0], log_targets_count, log_targets);


    printf("Reading...\n");
    my_string* onegin_buffer = NULL;
    size_t lines_num = allocate_and_read_lines(onegin_input_file_path, &onegin_buffer);

    if (onegin_buffer == NULL)
    {
        LOG_ERROR(MAKE_ERROR_STRUCT(CANNOT_OPEN_FILE_ERROR));
    }

    my_string whole_onegin_buffer = onegin_buffer[0];
    size_t onegin_len = mc_strlen(whole_onegin_buffer.str);

    printf("Number of lines: %zu\n", lines_num);
    
    printf("=============================================================\n");

    fd = open(onegin_output_file_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1)
    {
        return -1;
    }
    write(fd, "Sorted (from start to end):\n", 28);
    close(fd);

    printf("Sorting forwards...\n");

    bubble_sort(onegin_buffer, lines_num, sizeof(onegin_buffer[0]),
        lexycographic_alpha_my_str_comparator, -1);

    printf("Writing...\n");
    allocate_and_write_lines(onegin_output_file_path, onegin_buffer, lines_num, onegin_len);

    fd = open(onegin_output_file_path, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fd == -1)
    {
        return -1;
    }
    write(fd, "\n=================================================================\nSorted (from end to start):\n", 95);
    close(fd);

    printf("Sorting backwards...\n");

    bubble_sort(onegin_buffer, lines_num, sizeof(onegin_buffer[0]),
        lexycographic_alpha_my_str_reverse_comparator, -1);

    printf("Writing...\n");
    allocate_and_write_lines(onegin_output_file_path, onegin_buffer, lines_num, onegin_len);

    fd = open(onegin_output_file_path, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fd == -1)
    {
        return -1;
    }
    write(fd, "====================================================================\n\nOriginal:\n", 80);
    write(fd, whole_onegin_buffer.str, onegin_len);

    close(fd);
    printf("Exiting!\n");

    free(whole_onegin_buffer.str);
    free(onegin_buffer);

    return 0;
}
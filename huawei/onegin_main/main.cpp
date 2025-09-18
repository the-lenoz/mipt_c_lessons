#include <cstddef>
#include <stdio.h>
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

    LOG_START(argv[0], log_targets_count, log_targets);


    my_string* onegin_buffer = NULL;
    size_t lines_num = allocate_and_read_lines(onegin_input_file_path, &onegin_buffer);

    if (onegin_buffer == NULL)
    {
        LOG_ERROR(MAKE_ERROR_STRUCT(CANNOT_OPEN_FILE_ERROR));
    }

    printf("Number of lines: %zu\n", lines_num);
    
    printf("=============================================================\n");

    printf("%s", onegin_buffer[0].str);

    printf("=============================================================\n");

    /*for (size_t i = 0; i < lines_num; ++i)
    {
        if (onegin_buffer[i].len == 0) continue;
        printf("Line len: %zu, line_ptr: %p, ", onegin_buffer[i].len, 
            onegin_buffer[i].str);
        printf("line: %-.*s\n", (int)onegin_buffer[i].len, onegin_buffer[i].str);
    }

    const char* array[] = {"a", "b", "d", "c", "-a", "/b", "aa", "ba", "bb"};*/

    printf("[\n");
    for (size_t i = 0; i < lines_num; ++i)
    {
        if ((int)onegin_buffer[i].len)
            printf("'%-.*s',\n", (int)onegin_buffer[i].len, onegin_buffer[i].str);
    }
    printf("]\n\n");

    bubble_sort(onegin_buffer, lines_num, sizeof(onegin_buffer[0]),
     lexycographic_alpha_my_str_comparator, -1);

    printf("[\n");
    for (size_t i = 0; i < lines_num; ++i)
    {
        if ((int)onegin_buffer[i].len)
            printf("'%-.*s',\n", (int)onegin_buffer[i].len, onegin_buffer[i].str);
    }
    printf("]\n\n");

    return 0;
}
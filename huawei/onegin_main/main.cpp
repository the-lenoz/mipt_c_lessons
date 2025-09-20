#include <cstddef>
#include <cstdlib>
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

LogTarget log_targets[] = 
{
    {}, // stdout
    {"/tmp/log.html"}
};
const int log_targets_count = sizeof(log_targets) / sizeof(log_targets[0]);


int with_onegin_files(const char* input_path, const char* output_path);


int main(int argc, const char** argv)
{
    assert(argv != NULL);
    assert(argc != 0);  
    assert(argv[0] != NULL);

    LOG_START(argv[0], log_targets_count, log_targets);
    
    if (argc < 3)
    {
        LOG_MESSAGE("Usage: onegin [input.txt] [output.txt]", ERROR);
        return -1;
    }
    
    return with_onegin_files(argv[argc - 2], argv[argc - 1]);
}

int with_onegin_files(const char* input_path, const char* output_path)
{
    assert(input_path != NULL);
    assert(output_path != NULL);
    
    my_string* onegin_buffer = NULL;
    
    LOG_MESSAGE("Чтение...", INFO);
    
    size_t lines_num = allocate_and_read_lines(input_path, &onegin_buffer);
    if (onegin_buffer == NULL)
    {
        LOG_ERROR(MAKE_ERROR_STRUCT(CANNOT_OPEN_FILE_ERROR));
        return -1;
    }

    char* whole_onegin_buffer = onegin_buffer[0].str;
    
    size_t onegin_len = mc_strlen(whole_onegin_buffer);
    
    write_string_to_file(output_path, "Sorted (from start to end):\n", 1);
    
    LOG_MESSAGE("Сортировка по префиксу...", INFO);
    quick_sort(onegin_buffer, lines_num, sizeof(onegin_buffer[0]),
        lexycographic_alpha_my_str_comparator, -1);

    LOG_MESSAGE("Запись в файл...", INFO);
    write_lines(output_path, onegin_buffer, lines_num, onegin_len);
    
    write_string_to_file(output_path,
         "\n=================================================================\nSorted (from end to start):\n", 0);

    LOG_MESSAGE("Сортировка по суффиксу...", INFO);
    quick_sort(onegin_buffer, lines_num, sizeof(onegin_buffer[0]),
        lexycographic_alpha_my_str_reverse_comparator, -1);

    LOG_MESSAGE("запись в файл...", INFO);
    write_lines(output_path,
         onegin_buffer, lines_num, onegin_len);

    write_string_to_file(output_path,
         "\n====================================================================\nOriginal:\n", 0);
    write_string_to_file(output_path, whole_onegin_buffer, 0);

    LOG_MESSAGE("Очистка и освобождение ресурсов...", INFO);

    free(whole_onegin_buffer);
    free(onegin_buffer);

    return 0;
}
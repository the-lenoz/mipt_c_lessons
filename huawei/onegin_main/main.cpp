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


const char* onegin_input_file_path = "onegin.txt";
const char* onegin_output_file_path = "new_onegin.txt";

LogTarget log_targets[] = 
{
    {}, // stdout
    {"/tmp/log.html"}
};
const int log_targets_count = sizeof(log_targets) / sizeof(log_targets[0]);


int create_and_write_output(my_string* onegin_buffer, size_t lines_num, my_string whole_onegin_buffer);


int main(int argc, const char** argv)
{
    assert(argv != NULL);
    assert(argc != 0);  
    assert(argv[0] != NULL);

    LOG_START(argv[0], log_targets_count, log_targets);

    LOG_MESSAGE("Чтение...", INFO);

    my_string* onegin_buffer = NULL;
    size_t lines_num = allocate_and_read_lines(onegin_input_file_path, &onegin_buffer);

    if (onegin_buffer == NULL)
    {
        LOG_ERROR(MAKE_ERROR_STRUCT(CANNOT_OPEN_FILE_ERROR));
        return -1;
    }

    my_string whole_onegin_buffer = onegin_buffer[0];

    create_and_write_output(onegin_buffer, lines_num, whole_onegin_buffer);

    LOG_MESSAGE("Очистка и освобождение ресурсов...", INFO);

    free(whole_onegin_buffer.str);
    free(onegin_buffer);

    return 0;
}

int create_and_write_output(my_string* onegin_buffer, size_t lines_num, my_string whole_onegin_buffer)
{
    char message_buf[256] = {};

    size_t onegin_len = mc_strlen(whole_onegin_buffer.str);

    int arr[] = {2, 1, 3, 5, 8, 9, 7};

    printf("[");
    for (int i = 0; i < sizeof(arr)/sizeof(arr[0]); ++i) printf("%d, ", arr[i]);
    printf("\b\b]\n");
    quick_sort(arr, sizeof(arr)/sizeof(arr[0]), sizeof(arr[0]), basic_int_comparator, -1);
    printf("[");
    for (int i = 0; i < sizeof(arr)/sizeof(arr[0]); ++i) printf("%d, ", arr[i]);
    printf("\b\b]\n");

    snprintf(message_buf, 256, "Количество строк: %zu", lines_num);
    LOG_MESSAGE(message_buf, INFO);
    
    LOG_MESSAGE("=============================================================", INFO);

    append_string_to_file(onegin_output_file_path, "Sorted (from start to end):\n", 1);

    LOG_MESSAGE("Сортировка по префиксу...", INFO);

    quick_sort(onegin_buffer, lines_num, sizeof(onegin_buffer[0]),
        lexycographic_alpha_my_str_comparator, -1);

    LOG_MESSAGE("Запись в файл...", INFO);
    allocate_and_write_lines(onegin_output_file_path, onegin_buffer, lines_num, onegin_len);

    append_string_to_file(onegin_output_file_path,
         "\n=================================================================\nSorted (from end to start):\n", 0);

    LOG_MESSAGE("Сортировка по суффиксу...", INFO);
    bubble_sort(onegin_buffer, lines_num, sizeof(onegin_buffer[0]), lexycographic_alpha_my_str_reverse_comparator, -1);
    //bubble_sort(onegin_buffer, lines_num, sizeof(onegin_buffer[0]),
    //    lexycographic_alpha_my_str_reverse_comparator, -1);

    LOG_MESSAGE("запись в файл...", INFO);
    allocate_and_write_lines(onegin_output_file_path,
         onegin_buffer, lines_num, onegin_len);

    append_string_to_file(onegin_output_file_path,
         "====================================================================\n\nOriginal:\n", 0);
    append_string_to_file(onegin_output_file_path, whole_onegin_buffer.str, 0);

    return 0;
}
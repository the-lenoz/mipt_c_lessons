#ifndef SORT_DECLARED
#define SORT_DECLARED

#include <stdlib.h>

#define FAST_COPY_BLOCK_SIZE 64

typedef int (*comparator_t)(const void*, const void*);

int bubble_sort(void* array, size_t array_len, size_t elem_size, comparator_t comparator, int direction);

void quick_sort(void* array, size_t array_len, size_t elem_size, comparator_t comparator, int direction);

size_t quick_sort_partition(void* array, size_t array_len, size_t elem_size, comparator_t comparator);

void swap(void* a, void* b, size_t elem_size);

int basic_int_comparator(const void* a, const void* b);

#endif // SORT_DECLARED
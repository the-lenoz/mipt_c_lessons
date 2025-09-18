#ifndef SORT_DECLARED
#define SORT_DECLARED

#include <stdlib.h>

typedef int (*comparator_t)(void*, void*);

int bubble_sort(void* array, size_t array_len, size_t elem_size, comparator_t comparator, int direction);

int basic_int_comparator(void* a, void* b);

#endif // SORT_DECLARED
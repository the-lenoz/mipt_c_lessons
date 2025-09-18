#include "sort.hpp"
#include "error_handling/my_assert.hpp"
#include <string.h>
#include <stdio.h>

int bubble_sort(void* array, size_t array_len, size_t elem_size, comparator_t comparator, int direction)
{
    assert(array != NULL);
    assert(elem_size != 0);

    if (array_len <= 1) return 0;

    void* tmp = calloc(1, elem_size);

    int replacements = 0;

    for (size_t i = 0; i < array_len - 1; ++i)
    {
        for (size_t j = 0; j < array_len - 1 - i; ++j)
        {
            if (comparator((char*)array + (j * elem_size), (char*)array + (j * elem_size + elem_size)) * direction < 0)
            {
                memccpy(tmp, (char*)array + (j * elem_size), 1, elem_size);
                memccpy((char*)array + (j * elem_size), (char*)array + (j * elem_size + elem_size), 1, elem_size);
                memccpy((char*)array + (j * elem_size + elem_size), tmp, 1, elem_size);
                replacements++;
            }
        }
    }
    free(tmp);
    return replacements;
}

int basic_int_comparator(void* a, void* b)
{
    return *((int*)a) - *((int*)b);
}
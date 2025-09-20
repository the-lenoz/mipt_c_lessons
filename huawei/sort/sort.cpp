#include "sort.hpp"
#include "error_handling/my_assert.hpp"
#include <cstdint>
#include <cstdlib>
#include <string.h>
#include <stdio.h>

int bubble_sort(void* array, size_t array_len, size_t elem_size, comparator_t comparator, int direction)
{
    assert(array != NULL);
    assert(elem_size != 0);

    if (array_len <= 1) return 0;

    int replacements = 0;

    for (size_t i = 0; i < array_len - 1; ++i)
    {
        for (size_t j = 0; j < array_len - 1 - i; ++j)
        {
            //printf("Sort: compairing array[%zu] <> array[%zu]. \n", j, j+1);
            if (comparator((char*)array + (j * elem_size), (char*)array + ((j+1) * elem_size)) * direction < 0)
            {                
                swap(((char*)array) + (j * elem_size),
                     ((char*)array) + j * elem_size + elem_size, elem_size);                
                replacements++;
            }
        }
    }
    return replacements;
}

void quick_sort(void* array, size_t array_len, size_t elem_size, comparator_t comparator, int direction)
{
    assert(array != NULL);
    assert(elem_size != 0);

    if (array_len > 1)
    {
        size_t partition = quick_sort_partition(array, array_len, elem_size, comparator);
        quick_sort(array, partition, elem_size, comparator, direction);
        quick_sort((char*)array + (partition * elem_size), 
            array_len - partition, elem_size, comparator, direction);
    }
}

size_t quick_sort_partition(void* array, size_t array_len, size_t elem_size, comparator_t comparator) //TODO
{
    assert(array != NULL);
    assert(elem_size != 0);

    char* v = (char*) array + (array_len - 1) * elem_size;
    char* i = (char*)array;

    for (char* j = (char*)array; (size_t)(j - (char*)array) < ((array_len - 1) * elem_size); j += elem_size)
    {
        if (comparator(j, v) <= 0)
        {
            swap(i, j, elem_size);

            if (v == i) v = j; // Preserve v value
            if (v == j) v = i;

            i += elem_size;
        }
    }
    swap(i, (char*) array + (array_len - 1) * elem_size, elem_size);
    return (size_t)((char*)i - (char*)array) / elem_size;
}  

void swap(void* a, void* b, size_t elem_size) // With alignment check
{
    if (a == b) return;

    int8_t block[FAST_COPY_BLOCK_SIZE] = {};
    size_t copy_size = 0;

    while (elem_size != 0)
    {
        copy_size = (elem_size & (FAST_COPY_BLOCK_SIZE - 1)) ? elem_size & (FAST_COPY_BLOCK_SIZE - 1) : FAST_COPY_BLOCK_SIZE;
        printf("Exchanging %p <-> %p : elem_size=%zu\n", a, b, elem_size);
        memcpy(block, a, copy_size);
        memcpy(a, b, copy_size);
        memcpy(b, block, copy_size);
        a = (char*)a + copy_size;
        b = (char*)b + copy_size;
        elem_size -= copy_size;
    }
}

int basic_int_comparator(const void* a, const void* b)
{
    return *((const int*)a) - *((const int*)b);
}
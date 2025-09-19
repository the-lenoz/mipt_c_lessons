#include "sort.hpp"
#include "error_handling/my_assert.hpp"
#include "status.hpp"
#include <cstdlib>
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
            //printf("Sort: compairing array[%zu] <> array[%zu]. \n", j, j+1);
            if (comparator((char*)array + (j * elem_size), (char*)array + ((j+1) * elem_size)) * direction < 0)
            {                
                swap(tmp, ((char*)array) + (j * elem_size),
                     ((char*)array) + j * elem_size + elem_size, elem_size);                
                replacements++;
            }
        }
    }
    free(tmp);
    return replacements;
}

void quick_sort(void* array, size_t array_len, size_t elem_size, comparator_t comparator, int direction)
{
    assert(array != NULL);
    assert(elem_size != 0);

    if (array_len > 2)
    {
        size_t partition = quick_sort_partition(array, array_len, elem_size, comparator);
        quick_sort(array, partition, elem_size, comparator, direction);
        quick_sort((char*)array + (partition * elem_size), 
            array_len - partition, elem_size, comparator, direction);
    }
    else if (array_len == 2)
    {
        char* tmp = (char*)calloc(1, elem_size);
        if (comparator(array, (char*)array + elem_size) * direction < 0) swap(tmp, array, (char*)array + elem_size, elem_size);
        free(tmp);
    }
}

size_t quick_sort_partition(void* array, size_t array_len, size_t elem_size, comparator_t comparator) //TODO
{
    assert(array != NULL);
    assert(elem_size != 0);

    char* v = (char*)calloc(1, elem_size);
    char* tmp = (char*)calloc(1, elem_size);

    memcpy(v, (char*) array + (array_len - 1) * elem_size, elem_size);

    char* i = (char*)array;

    
    for (char* j = (char*)array; (size_t)(j - (char*)array) < ((array_len - 1) * elem_size); j += elem_size)
    {
        if (comparator(j, v) <= 0)
        {
            swap(tmp, i, j, elem_size);
            i += elem_size;
        }
    }
    swap(tmp, i, (char*) array + (array_len - 1) * elem_size, elem_size);
    free(tmp);
    free(v);
    return (size_t)((char*)i - (char*)array) / elem_size;
}  

void swap(void* tmp, void* a, void* b, size_t elem_size)
{
    if (a == b) return;
    //printf("Exchanging: %p <-> %p\n", a, b);
    memcpy(tmp, a,  elem_size);
    memcpy(a, b, elem_size);
    memcpy(b, tmp, elem_size);
    //printf("Replaced\n");
}

int basic_int_comparator(const void* a, const void* b)
{
    return *((int*)a) - *((int*)b);
}
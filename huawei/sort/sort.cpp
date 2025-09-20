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

    int64_t block64 = 0;
    int32_t block32 = 0;
    int16_t block16 = 0;
    int8_t block8 = 0;

    while (elem_size != 0)
    {
        if (elem_size >> 3 && ((size_t)a & 7) == 0 && ((size_t)b & 7) == 0) 
        {
            block64 = *(int64_t*)a;
            *(int64_t*)a = *(int64_t*)b;
            *(int64_t*)b = block64;
            a = (char*)a + 8;
            b = (char*)b + 8;
            elem_size -= 8;
        }
        else if (elem_size >> 2 && ((size_t)a & 3) == 0 && ((size_t)b & 3) == 0)
        {
            block32 = *(int32_t*)a;
            *(int32_t*)a = *(int32_t*)b;
            *(int32_t*)b = block32;
            a = (char*)a + 4;
            b = (char*)b + 4;
            elem_size -= 4;
        }
        else if (elem_size >> 1 && ((size_t)a & 1) == 0 && ((size_t)b & 1) == 0)
        {
            block16 = *(int16_t*)a;
            *(int16_t*)a = *(int16_t*)b;
            *(int16_t*)b = block16;
            a = (char*)a + 2;
            b = (char*)b + 2;
            elem_size -= 2;
        }
        else 
        {
            block8 = *(int8_t*)a;
            *(int8_t*)a = *(int8_t*)b;
            *(int8_t*)b = block8;
            a = (char*)a + 1;
            b = (char*)b + 1;
            --elem_size;
        }
    }
}

int basic_int_comparator(const void* a, const void* b)
{
    return *((const int*)a) - *((const int*)b);
}
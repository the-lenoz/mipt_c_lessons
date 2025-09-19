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
            //printf("Sort: compairing array[%zu] <> array[%zu]. \n", j, j+1);
            if (comparator((char*)array + (j * elem_size), (char*)array + ((j+1) * elem_size)) * direction < 0)
            {
                /*
                ((my_string*)tmp)->len = ((my_string*)array + j)->len;
                ((my_string*)tmp)->str = ((my_string*)array + j)->str;

                ((my_string*)array + j)->len = ((my_string*)array + j + 1)->len;
                ((my_string*)array + j)->str = ((my_string*)array + j + 1)->str;

                ((my_string*)array + j + 1)->len = ((my_string*)tmp)->len;
                ((my_string*)array + j + 1)->str = ((my_string*)tmp)->str;
                */
                
                //printf("Exchanging: array[%d] <-> array[%d]\n", (int)j, (int)j+1);
                memcpy(tmp, ((char*)array) + (j * elem_size),  elem_size);
                //printf("Writing to array[%zu]\n", j);
                memcpy(((char*)array) + (j * elem_size), (char*)array + (j * elem_size + elem_size), elem_size);
                //printf("Writing to array[%zu]\n", j+1);
                memcpy(((char*)array) + ((j + 1) * elem_size), tmp, elem_size);
                //printf("Replaced\n");
                
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
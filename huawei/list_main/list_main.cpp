#include "list/list.hpp"
#include <cstdio>



int main(void)
{
    CircularListIterator iter = {};
    CircularListIterator_init(&iter);

    if (CircularListIterator_insert_after(&iter, 0) != ITERATOR_OK)
    {
        fprintf(stderr, "Ошибка!\n");
    }
    for (int i = 1; i < 100; ++i)
    {
        CircularListIterator_insert_after(&iter, (double)i + 3.14);
    }
    CircularListIterator_insert_after(&iter, 1.5);
    CircularListIterator_insert_after(&iter, 2);

    CircularListIterator_dump(&iter, "/tmp/");

    CircularListIterator_next(&iter);
    fprintf(stderr, "LAST_ELEM_NEXT = %ld\n", iter.last_element.next_phys_index);

    CircularListIterator_delete_next(&iter);

    CircularListIterator_dump(&iter, "/tmp/");


    CircularListIterator_destroy(&iter);

    return 0;
}
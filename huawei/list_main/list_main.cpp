#include "list/list.hpp"
#include <cstdio>



int main(void)
{
    List list;
    List_init(&list, 32);

    List_dump(&list, stderr);

    size_t ind = 0;
    for (int i = 0; i < 5; ++i)
    {
        fprintf(stderr, "INSERT pi -> 0\n");
        List_insert(&list, 0, 3.1415926 + i, &ind);
        fprintf(stderr, "index = %zu\n", ind);
        List_dump(&list, stderr);
    }
    for (int i = 0; i < 5; ++i)
    {
        fprintf(stderr, "DELETE 0\n");
        List_delete(&list, i);
        List_dump(&list, stderr);
    }

    List_destroy(&list);
    return 0;
}
#include "list/list.hpp"
#include <cstdio>



int main(void)
{
    CircularList list;
    CircularList_init(&list, 128);
    iterator_t iter = CircularList_get_null_iterator(&list);

    CircularList_dump(&list, "/tmp/");
    printf("dump1; iter=%d\n", iter);
    getchar();

    for (double i = 0.123; i < 123; i += 1)
    {
        if (CircularList_insert_after(&list, &iter, i) != LIST_OKAY) return -fprintf(stderr, "ERROR!\n");
	CircularList_next(&list, &iter);
    }

    CircularList_dump(&list, "/tmp/");

    //printf("dumpd; added: %lf, iter: %d\n", CircularList_next(&list, &iter), iter);
    //getchar();

    iter = CircularList_get_null_iterator(&list);
    iterator_t tmp_iter = {};
    for (int i = 0; i < 40; ++i)
    {
	CircularList_next(&list, &iter);

	printf("ITER: %ld\n", iter);
	CircularList_copy_iterator(&list, &iter, &tmp_iter);
	CircularList_next(&list, &iter);
        if (CircularList_delete(&list, &tmp_iter) != LIST_OKAY) return -fprintf(stderr, "error deleting\n");
    }

    CircularList_dump(&list, "/tmp/");

    CircularList_destroy(&list);

    printf("dump3,finish; iter=%d\n", iter);

    return 0;
}

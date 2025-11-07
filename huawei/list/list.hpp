#ifndef LIST_DECLARED
#define LIST_DECLARED

#include <cstddef>
#include <cstdio>
#include <sys/types.h>


typedef double list_elem_type;

typedef ssize_t iterator_t;

enum ListStatusType
{
    LIST_OKAY,
    LIST_STRUCTURE_INVALID,
    LIST_INDEX_INVALID
};

struct _ListNode
{
    list_elem_type value;
    ssize_t prev_phys_index;
    ssize_t next_phys_index;
};

struct CircularList 
{
    _ListNode *data;

    ssize_t free_head_phys_index;
    
    size_t capacity;
    size_t size;

    int hash;
};


int CircularList_init(CircularList *list, size_t initial_capacity);
int CircularList_destroy(CircularList *list);

ListStatusType CircularList_verify(CircularList *list);
int CircularList_dump(CircularList *list, const char* directory);
int CircularList_dump_graph(CircularList *list, FILE *fp);

ListStatusType CircularList_copy_iterator(CircularList *list, iterator_t *src, iterator_t* dst);
iterator_t CircularList_get_null_iterator(CircularList* list);


ListStatusType CircularList_insert_after(CircularList *list, iterator_t *iter, list_elem_type element);
ListStatusType CircularList_delete(CircularList *list, iterator_t *iter);

list_elem_type CircularList_next(CircularList *list, iterator_t *iter, ListStatusType *status = NULL);
list_elem_type CircularList_prev(CircularList *list, iterator_t *iter, ListStatusType *status = NULL);

#endif // LIST_DECLARED

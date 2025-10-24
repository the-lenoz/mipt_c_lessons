#ifndef LIST_DECLARED
#define LIST_DECLARED

#include <cstddef>
#include <cstdio>
#include <sys/types.h>


typedef double list_elem_type;


enum ListStatusType
{
    LIST_OKAY,
    LIST_STRUCTURE_INVALID,
    LIST_INDEX_INVALID
};


struct ListNode
{
    list_elem_type value;
    ssize_t prev_phys_index;
    ssize_t next_phys_index;
};

struct List 
{
    ListNode *data;

    ssize_t head_phys_index;
    ssize_t free_head_phys_index;
    
    size_t capacity;
    size_t size;
};


int List_init(List *list, size_t initial_capacity);
int List_destroy(List *list);

ListStatusType List_verify(List *list);
int List_dump(List *list, FILE* fp);


ListStatusType List_insert(List *list, size_t phys_index, list_elem_type element, size_t *dst_phys_index);
ListStatusType List_delete(List *list, size_t phys_index);

ListStatusType List_get(List *list, size_t phys_index, list_elem_type *dst);






#endif // LIST_DECLARED
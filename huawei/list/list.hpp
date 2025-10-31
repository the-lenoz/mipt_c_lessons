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

enum IteratorStatusType
{
    ITERATOR_OK,
    ITERATOR_STOP,
    ITERATOR_INIT_ERROR,
    ITERATOR_INVALID
};


struct _ListNode
{
    list_elem_type value;
    ssize_t prev_phys_index;
    ssize_t next_phys_index;
};

struct _CircularList 
{
    _ListNode *data;

    ssize_t free_head_phys_index;
    
    size_t capacity;
    size_t size;

    int hash;
};

struct CircularListIterator
{
    _CircularList* list;
    _ListNode last_element;
    IteratorStatusType last_iteration_status;

    int hash;
};

IteratorStatusType CircularListIterator_init(CircularListIterator* iter);
IteratorStatusType CircularListIterator_destroy(CircularListIterator* iter);

IteratorStatusType CircularListIterator_verify(CircularListIterator* iter);
IteratorStatusType CircularListIterator_dump(CircularListIterator* iter, const char* directory);

list_elem_type CircularListIterator_next(CircularListIterator* iter);
list_elem_type CircularListIterator_prev(CircularListIterator* iter);

IteratorStatusType CircularListIterator_insert_after(CircularListIterator* iter, list_elem_type element);
IteratorStatusType CircularListIterator_insert_before(CircularListIterator* iter, list_elem_type element);

IteratorStatusType CircularListIterator_delete_next(CircularListIterator* iter);
IteratorStatusType CircularListIterator_delete_prev(CircularListIterator* iter);



#endif // LIST_DECLARED
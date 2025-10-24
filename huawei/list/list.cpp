#include "my_assert.hpp"

#include "list.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>


int List_init(List *list, size_t initial_capacity)
{
    assert(list);

    *list = {};
    list->capacity = initial_capacity;
    list->data = (ListNode*) calloc(initial_capacity, sizeof(ListNode));

    list->size = 0;

    list->free_head_phys_index = 0;
    list->head_phys_index = 0;

    for (size_t i = 0; i < list->capacity; ++i)
    {
        list->data[i].next_phys_index = i + 1;
        list->data[i].prev_phys_index = -1;
        list->data[i].value = NAN; // !FOR DOUBLE ONLY!
    }
    list->data[list->capacity - 1].next_phys_index = 0;

    return 0;
}

int List_destroy(List *list)
{
    if (list == NULL)
    {
        return -1;
    }

    if (list->data != NULL)
    {
        free(list->data);
    }

    *list = {};

    return 0;
}


ListStatusType List_verify(List *list)
{
    ListStatusType status = LIST_OKAY;

    if (list == NULL)
    {
        status = LIST_STRUCTURE_INVALID;
    } 
    
    // TODO

    return status;
}

int List_dump(List *list, FILE* fp)
{
    if (list == NULL)
    {
        fprintf(fp, "List = NULL\n");
        return -1;
    }
    
    fprintf(fp, "List\n  data={\n");
    for (size_t i = 0; i < list->capacity; ++i)
    {
        fprintf(fp, "    ListNode(value=%lf,\tnext_index=%ld,\tprev_index=%ld),", 
                list->data[i].value, list->data[i].next_phys_index, list->data[i].prev_phys_index);
        if (list->data[i].prev_phys_index == -1)
        {
            fprintf(fp, " <--- GARBAGE");
        }
        fprintf(fp, "\n");
    }
    fprintf(fp, "  },\n");
    fprintf(fp, "  head_phys_index=%ld,\n"
                "  free_head_phys_index=%ld,\n"
                "  capacity=%zu,\n"
                "  size=%zu\n}\n", list->head_phys_index, list->free_head_phys_index, list->capacity, list->size);

    return 0;
}


ListStatusType List_insert(List *list, size_t phys_index, list_elem_type element, size_t *dst_phys_index)
{
    ListStatusType list_status = List_verify(list);
    if (list_status != LIST_OKAY)
    {
        return list_status;
    }

    if (phys_index < 0 || phys_index > list->size)
    {
        return LIST_INDEX_INVALID;
    }

    

    *dst_phys_index = list->free_head_phys_index;

    if ((ssize_t)phys_index == list->head_phys_index)
    {
        list->head_phys_index = *dst_phys_index;
    }

    list->free_head_phys_index = list->data[list->free_head_phys_index].next_phys_index;


    list->data[*dst_phys_index] = {};

    list->data[*dst_phys_index].next_phys_index=phys_index;
    list->data[*dst_phys_index].prev_phys_index=list->data[phys_index].prev_phys_index;
    list->data[*dst_phys_index].value=element;

    list->data[list->data[phys_index].prev_phys_index].next_phys_index = *dst_phys_index; // Перешиваем
    list->data[phys_index].prev_phys_index = *dst_phys_index;

    list->size++;

    return list_status;
}

ListStatusType List_delete(List *list, size_t phys_index)
{
    ListStatusType list_status = List_verify(list);
    if (list_status != LIST_OKAY)
    {
        return list_status;
    }

    if (phys_index >= list->size)
    {
        return LIST_INDEX_INVALID;
    }

    if ((ssize_t)phys_index == list->head_phys_index)
    {
        list->head_phys_index = list->data[phys_index].next_phys_index;
    }

    list->data[list->data[phys_index].prev_phys_index].next_phys_index = list->data[phys_index].next_phys_index; // Перешиваем
    list->data[list->data[phys_index].next_phys_index].prev_phys_index = list->data[phys_index].prev_phys_index;

    list->data[phys_index].next_phys_index = list->free_head_phys_index;
    list->data[phys_index].prev_phys_index = -1;
    
    list->free_head_phys_index = phys_index;

    list->size--;

    

    return list_status;
}


ListStatusType List_get(List *list, size_t phys_index, list_elem_type *dst)
{
    ListStatusType list_status = List_verify(list);
    if (list_status != LIST_OKAY)
    {
        return list_status;
    }

    if (phys_index >= list->size)
    {
        return LIST_INDEX_INVALID;
    }

    if (list->data[phys_index].prev_phys_index == -1)
    {
        return LIST_INDEX_INVALID;
    }

    *dst = list->data[phys_index].value;

    return list_status;
}
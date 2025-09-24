#include <stdlib.h>
#include "stack.hpp"
#include "error_handling/my_assert.hpp"
#include "status.hpp"

void stack_init(Stack* st, size_t init_stack_size)
{
    assert(st != NULL);

    st->last_operation_status_code = STACK_SUCCESS;
    st->size = init_stack_size;
    st->capacity = init_stack_size;
    st->data = (STACK_ELEM_TYPE*) calloc(st->capacity * 2, sizeof(STACK_ELEM_TYPE));

    if (st->data == NULL)
    {
        st->last_operation_status_code = STACK_CANNOT_ALLOCATE;
    }
}

void stack_destroy(Stack* st)
{
    assert(st != NULL);

    st->size = 0;
    st->capacity = 0;

    free(st->data);
    st->data = NULL;
}

void stack_push(Stack* st, STACK_ELEM_TYPE value)
{
    if (st->size == st->capacity)
    {
        if (st->capacity == 0)
        {
            st->capacity = 1;
        }
        else 
        {
            st->capacity *= 2;
            if (st->size >= st->capacity)
            {
                st->capacity = st->size + 1;
            }
        }

        st->data = (STACK_ELEM_TYPE*) realloc(st->data, st->capacity * sizeof(STACK_ELEM_TYPE));
        
        if (st->data == NULL)
        {
            st->last_operation_status_code = STACK_CANNOT_ALLOCATE;
            return;
        }
    }

    st->data[st->size++] = value;
}

STACK_ELEM_TYPE stack_pop(Stack* st)
{
    assert(st != NULL);

    if (st->size == 0)
    {
        st->last_operation_status_code = STACK_EMPTY_POP;
        return STACK_ELEM_TYPE{};
    }


}

StatusData stack_validate(Stack* st)
{
    if (st == NULL)
    {
        return MAKE_ERROR_STRUCT(INVALID_FUNCTION_PARAM);
    }
    else if (st->size)
    {

    }
}
#include <stdlib.h>
#include "stack.hpp"
#include "error_handling/my_assert.hpp"
#include "logger.hpp"
#include "status.hpp"

void stack_init(Stack* st, size_t init_stack_size)
{
    assert(st != NULL);

    st->last_operation_status_code = STACK_SUCCESS;
    st->size = 0;
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
    StatusData stack_validity = stack_validate(st);
    if (stack_validity.status_code != SUCCESS)
    {
        LOG_ERROR(stack_validity);
        st->last_operation_status_code = STACK_INVALID;
        return;
    }
    stack_fit_size(st);    

    st->data[st->size++] = value;
}

STACK_ELEM_TYPE stack_pop(Stack* st)
{
    assert(st != NULL);
    
    STACK_ELEM_TYPE elem = (STACK_ELEM_TYPE){};

    StatusData stack_validity = stack_validate(st);
    if (stack_validity.status_code != SUCCESS)
    {
        LOG_ERROR(stack_validity);
        st->last_operation_status_code = STACK_INVALID;
        return (STACK_ELEM_TYPE){};
    }
    else if (st->size == 0)
    {
        LOG_ERROR(MAKE_EXTENDED_ERROR_STRUCT(INVALID_FUNCTION_PARAM, "st is empty"));
        st->last_operation_status_code = STACK_EMPTY_POP;
    }
    else 
    {
        elem = st->data[--(st->size)];
        stack_fit_size(st);
    }
    return elem;    
}

int stack_fit_size(Stack* st)
{
    StatusData stack_validity = stack_validate(st);
    if (stack_validity.status_code != SUCCESS)
    {
        LOG_ERROR(stack_validity);
        st->last_operation_status_code = STACK_INVALID;
        return -1;
    }

    if (st->size < st->capacity / 2 - 1 || st->size > st->capacity - 1)
    {
        size_t new_capacity = st->size * 2 + 1;
        st->data = (STACK_ELEM_TYPE*) realloc(st->data, new_capacity * sizeof(STACK_ELEM_TYPE));
        
        if (st->data == NULL)
        {
            st->last_operation_status_code = STACK_CANNOT_ALLOCATE;
            print_error(MAKE_ERROR_STRUCT(CANNOT_ALLOCATE_MEMORY_ERROR));
            return -1;
        }

        st->capacity = new_capacity;
    }
    return 0;
}

StatusData stack_validate(Stack* st)
{
    if (st == NULL)
    {
        return MAKE_EXTENDED_ERROR_STRUCT(INVALID_FUNCTION_PARAM, "st is NULL");
    }
    else if (st->size > st->capacity)
    {
        return MAKE_EXTENDED_ERROR_STRUCT(INVALID_FUNCTION_PARAM, "st is invalid size");
    }

    return MAKE_SUCCESS_STRUCT(NULL);
}

void stack_dump(LogMessageType dump_message_type, Stack* st)
{
    if (st == NULL)
    {
        LOG_MESSAGE_F(dump_message_type, "(Stack)NULL");
    }
    else
    {
        LOG_MESSAGE_F(dump_message_type, "Stack {");
        LOG_MESSAGE_F(dump_message_type, "  size=%zu,", st->size);
        LOG_MESSAGE_F(dump_message_type, "  capacity=%zu,", st->capacity);
        LOG_MESSAGE_F(dump_message_type, "  data={", st->capacity);

        for (size_t i = 0; i < st->size; ++i)
        {
            LOG_MESSAGE_F(dump_message_type, "    [%zu]=VALUE", i);
        }

        LOG_MESSAGE_F(dump_message_type, "  }", st->capacity);
        LOG_MESSAGE_F(dump_message_type, "}\n", st->capacity);
    }
}
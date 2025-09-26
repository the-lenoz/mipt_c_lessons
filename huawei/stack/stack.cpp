#include <cstdlib>
#include <stdlib.h>
#include "stack.hpp"
#include "error_handling/my_assert.hpp"
#include "logger.hpp"
#include "status.hpp"

void stack_init(Stack* st)
{
    assert(st != NULL);

    st->last_operation_status = MAKE_SUCCESS_STRUCT(NULL);
    st->size = 0;
    st->capacity = 2;
    st->data = (STACK_ELEM_TYPE*) calloc(st->capacity * 2, sizeof(STACK_ELEM_TYPE));

    if (st->data == NULL)
    {
        st->last_operation_status = MAKE_ERROR_STRUCT(CANNOT_ALLOCATE_MEMORY_ERROR);
    }

    st->left_canary = create_canary();
    st->right_canary = create_canary();
    st->data[0] = st->left_canary;
    st->data[st->capacity - 1] = st->right_canary;
    st->hash = stack_calc_hash(st);
}

void stack_destroy(Stack* st)
{
    assert(st != NULL);

    free(st->data);
    
    *st = {};
}

void stack_push(Stack* st, STACK_ELEM_TYPE value)
{
    StatusData stack_validity = stack_validate(st);
    if (stack_validity.status_code != SUCCESS)
    {
        st->last_operation_status = stack_validity;
        return;
    }
    stack_fit_size(st);    

    st->data[st->size++ + 1] = value;
    st->hash = stack_calc_hash(st);
}

STACK_ELEM_TYPE stack_pop(Stack* st)
{
    assert(st != NULL);
    
    STACK_ELEM_TYPE elem = {};

    StatusData stack_validity = stack_validate(st);
    if (stack_validity.status_code != SUCCESS)
    {
        st->last_operation_status = stack_validity;
        return {};
    }
    else if (st->size == 0)
    {
        st->last_operation_status = MAKE_EXTENDED_ERROR_STRUCT(INVALID_FUNCTION_PARAM, "st is empty");
    }
    else 
    {
        elem = st->data[--(st->size) + 1];
        stack_fit_size(st);
    }
    st->hash = stack_calc_hash(st);
    return elem;    
}

int stack_fit_size(Stack* st)
{
    StatusData stack_validity = stack_validate(st);
    if (stack_validity.status_code != SUCCESS)
    {
        st->last_operation_status = stack_validity;
        return -1;
    }

    if (st->size < st->capacity / 2 - 3 || st->size > st->capacity - 3)
    {
        size_t new_capacity = st->size * 2 + 3;
        st->data = (STACK_ELEM_TYPE*) realloc(st->data, new_capacity * sizeof(STACK_ELEM_TYPE));
        
        if (st->data == NULL)
        {
            st->last_operation_status = MAKE_ERROR_STRUCT(CANNOT_ALLOCATE_MEMORY_ERROR);
            return -1;
        }

        st->capacity = new_capacity;

        st->data[st->capacity - 1] = st->right_canary;
        st->hash = stack_calc_hash(st);
    }
    return 0;
}

StatusData stack_validate(Stack* st)
{
    if (st == NULL)
    {
        return MAKE_EXTENDED_ERROR_STRUCT(INVALID_FUNCTION_PARAM, "st is NULL");
    }
    else if (stack_calc_hash(st) != st->hash)
    {
        return MAKE_EXTENDED_ERROR_STRUCT(INVALID_FUNCTION_PARAM, "st hash sum mismatch");
    }
    else if (st->size > st->capacity)
    {
        return MAKE_EXTENDED_ERROR_STRUCT(INVALID_FUNCTION_PARAM, "st is invalid size");
    }
    else if (st->capacity == 0)
    {
        return MAKE_EXTENDED_ERROR_STRUCT(INVALID_FUNCTION_PARAM, "st->capacity is zero");
    }
    else if (st->data[0] != st->left_canary)
    {
        return MAKE_EXTENDED_ERROR_STRUCT(INVALID_FUNCTION_PARAM, "st left canary broken");
    }
    else if (st->data[st->capacity - 1] != st->right_canary)
    {
        return MAKE_EXTENDED_ERROR_STRUCT(INVALID_FUNCTION_PARAM, "st right canary broken");
    }

    return MAKE_SUCCESS_STRUCT(NULL);
}

void stack_dump(Stack* st, LogMessageType dump_message_type)
{
    char val[sizeof(STACK_ELEM_TYPE) * 2 + 1] = {};
    unsigned char byte = 0;
    StatusData stack_validity = stack_validate(st);
    if (stack_validity.status_code != SUCCESS)
    {
        LOG_ERROR(stack_validity);
        return;
    }
    else
    {
        LOG_MESSAGE_F(dump_message_type, "Stack {");
        LOG_MESSAGE_F(dump_message_type, "  size=%zu,", st->size);
        LOG_MESSAGE_F(dump_message_type, "  capacity=%zu,", st->capacity);
        LOG_MESSAGE_F(dump_message_type, "  data={", st->capacity);
        
        for (size_t i = 0; i < st->size; ++i)
        {
            for (size_t j = 0; j < sizeof(STACK_ELEM_TYPE); ++j)
            {
                byte = ((unsigned char*)(&(st->data[i + 1])))[j];
                val[j*2] = hex_numbers[(byte & 0xF0) >> 4];
                val[j*2 + 1] = hex_numbers[(byte & 0x0F) >> 0];
            }
            LOG_MESSAGE_F(dump_message_type, "    [%zu]=0x%s", i, val);
        }
        
        LOG_MESSAGE_F(dump_message_type, "  },", st->capacity);
        LOG_MESSAGE_F(dump_message_type, "  hash=%u", st->hash);
        LOG_MESSAGE_F(dump_message_type, "}\n", st->capacity);
    }
}


unsigned int stack_calc_hash(Stack* st)
{
    size_t structure_len = (size_t) ((char*)&(st->hash) - (char*)st);

    return calc_shift_hash((unsigned char*)st, structure_len, HASH_BASE_SEED);
}


unsigned int calc_shift_hash(const unsigned char* val, size_t len, unsigned int state)
{
    if (len == 0) return state;
    return calc_shift_hash(val + 1, len - 1, 
        (unsigned int)(((unsigned long) val[0] * state) % (unsigned long)(MAX_HASH_STATE)))
            ^ ((unsigned int)(val[0] << 4) + val[0]);
}


STACK_ELEM_TYPE create_canary()
{
    STACK_ELEM_TYPE canary = {};
    for (size_t i = 0; i < sizeof(canary); ++i)
    {
        ((unsigned char*)(&canary))[i] = (unsigned char)((rand() & 255) | 1);
    }
    return canary;
}
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdlib.h>
#include "stack.hpp"
#include "debug_utilites/debug_utilites.hpp"
#include "error_handling/my_assert.hpp"
#include "status.hpp"


#define MAKE_STACK_ERROR(error_code) MAKE_CUSTOM_ERROR_STRUCT(dump_stack_error, \
    (__tmp_stack_err_code = error_code, memcpy(calloc(1, sizeof(StackErrorCode)), &__tmp_stack_err_code, sizeof(StackErrorCode))))

static StackErrorCode __tmp_stack_err_code;

static int stack_fit_size(Stack* st);

static int stack_is_fit_size(Stack* st);

static size_t stack_get_enough_size(Stack* st);

static STACK_ELEM_TYPE* stack_get_left_canary_address(Stack* st);

static STACK_ELEM_TYPE* stack_get_right_canary_address(Stack* st);

static unsigned int stack_calc_hash(Stack* st);

static STACK_ELEM_TYPE create_canary();

void stack_init(Stack* st)
{
    assert(st != NULL);

    st->last_operation_status = MAKE_SUCCESS_STRUCT(NULL);
    st->size = 0;
    st->capacity = stack_get_enough_size(st);
    st->data = (STACK_ELEM_TYPE*) calloc(st->capacity * 2, sizeof(STACK_ELEM_TYPE));

    if (st->data == NULL)
    {
        st->last_operation_status = MAKE_ERROR_STRUCT(CANNOT_ALLOCATE_MEMORY_ERROR);
    }

    st->left_canary = create_canary();
    st->right_canary = create_canary();
    *stack_get_left_canary_address(st) = st->left_canary;
    *stack_get_right_canary_address(st) = st->right_canary;

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
        st->hash = stack_calc_hash(st);
        return;
    }
    if (stack_fit_size(st) == -1)
    {
        st->last_operation_status = MAKE_EXTENDED_ERROR_STRUCT(INVALID_FUNCTION_PARAM, "Cannot fit stack");
        st->hash = stack_calc_hash(st);
        return;   
    }   
    
    st->data[++(st->size)] = value;

    st->last_operation_status = MAKE_SUCCESS_STRUCT(NULL);
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
        st->hash = stack_calc_hash(st);
        return elem;    
    }
    else 
    {
        elem = st->data[(st->size)--];
        st->hash = stack_calc_hash(st);
        if (stack_fit_size(st) == -1)
        {
            st->last_operation_status = MAKE_EXTENDED_ERROR_STRUCT(INVALID_FUNCTION_PARAM, "Cannot fit stack");
            st->hash = stack_calc_hash(st);
            return elem;   
        }
    }
    st->last_operation_status = MAKE_SUCCESS_STRUCT(NULL);
    st->hash = stack_calc_hash(st);
        return elem;    
}

static int stack_fit_size(Stack* st)
{
    StatusData stack_validity = stack_validate(st);
    if (stack_validity.status_code != SUCCESS)
    {
        st->last_operation_status = stack_validity;
        return -1;
    }

    if (!stack_is_fit_size(st))
    {
        size_t new_capacity = stack_get_enough_size(st);
        STACK_ELEM_TYPE* st_data_backup = st->data;

        st->data = (STACK_ELEM_TYPE*) realloc(st->data, new_capacity * sizeof(STACK_ELEM_TYPE));
        
        if (st->data == NULL)
        {
            st->data = st_data_backup;
            st->last_operation_status = MAKE_ERROR_STRUCT(CANNOT_ALLOCATE_MEMORY_ERROR);
            st->hash = stack_calc_hash(st);
            return -1;
        }

        st->capacity = new_capacity;

        *stack_get_right_canary_address(st) = st->right_canary;
        st->hash = stack_calc_hash(st);
    }
    return 0;
}

static int stack_is_fit_size(Stack* st)
{
    return st->size >= st->capacity / 2 - 3 && st->size <= st->capacity - 3;
}

static size_t stack_get_enough_size(Stack* st)
{
    return st->size * 2 + 3;
}

StatusData stack_validate(Stack* st)
{
    if (st == NULL)
    {
        return MAKE_EXTENDED_ERROR_STRUCT(INVALID_FUNCTION_PARAM, "st is NULL");
    }
    else if (stack_calc_hash(st) != st->hash)
    {
        return MAKE_STACK_ERROR(STACK_STRUCTURE_CORRUPT);
    }
    else if (st->size > st->capacity)
    {
        return MAKE_STACK_ERROR(STACK_INVALID_SIZE);
    }
    else if (st->capacity == 0)
    {
        return MAKE_STACK_ERROR(STACK_INVALID_SIZE);
    }
    else if (memcmp(&(st->data[0]), &(st->left_canary), sizeof(STACK_ELEM_TYPE)) != 0)
    {
        return MAKE_STACK_ERROR(STACK_MEMRY_CORRUPT);
    }
    else if (memcmp(&(st->data[st->capacity - 1]), &(st->right_canary), sizeof(STACK_ELEM_TYPE)) != 0)
    {
        return MAKE_STACK_ERROR(STACK_MEMRY_CORRUPT);
    }

    return MAKE_SUCCESS_STRUCT(NULL);
}

void stack_dump(Stack* st, FILE* fp)
{
    char val[sizeof(STACK_ELEM_TYPE) * 3 + 1] = {};
    
    StatusData stack_validity = stack_validate(st);
    if (stack_validity.status_code != SUCCESS)
    {
        fprint_error(fp, stack_validity);
    }

    if (st != NULL)
    {
        to_hex(val, &(st->left_canary), sizeof(STACK_ELEM_TYPE));

        fprintf(fp, "Stack {\n  size=%zu,\n  capacity=%zu,\n  left_canary=0x%s,\n  data={\n",
            st->size, st->capacity, val);

        if (st->data == NULL)
        {
            fprintf(fp, "    === NULLPTR ===\n");   
        }
        else
        {
            for (size_t i = 0; i < st->capacity; ++i)
            {
                to_hex(val, &(st->data[i]), sizeof(STACK_ELEM_TYPE));
                fprintf(fp, "    [%zu]=%s,", i, val);

                if (i == 0 || i == st->capacity - 1)
                {
                    fprintf(fp, "\t// CANARY");
                }
                else if (i > st->size && i < st->capacity) 
                {
                    fprintf(fp, "\t// GARBAGE");
                }

                fprintf(fp, "\n");
            }
        }
        
        to_hex(val, &(st->right_canary), sizeof(STACK_ELEM_TYPE));

        fprintf(fp, "  },\n  t_canary=0x%s,\n  hash=%u\n}\n", val, st->hash);
    }
}

static STACK_ELEM_TYPE* stack_get_left_canary_address(Stack* st)
{
    return &(st->data[0]);
}

static STACK_ELEM_TYPE* stack_get_right_canary_address(Stack* st)
{
    return &(st->data[st->capacity - 1]);
}


static unsigned int stack_calc_hash(Stack* st)
{
    size_t structure_len = (size_t) ((char*)&(st->hash) - (char*)st);
    return calc_shift_hash((unsigned char*)st, structure_len, HASH_BASE_SEED);
}


static STACK_ELEM_TYPE create_canary()
{
    STACK_ELEM_TYPE canary = {};

    for (size_t i = 0; i < sizeof(canary); ++i)
    {
        if (i < sizeof(CANARY))
        {
            ((unsigned char*)(&canary))[i] = ((const uint8_t*)&CANARY)[i];
        }
        else
        {
            ((unsigned char*)(&canary))[i] = (unsigned char)((rand() & 255) | 1);
        }
    }
    return canary;
}


int dump_stack_error(FILE* fp, StatusData error_data)
{
    assert(fp != NULL);
    assert(error_data.custom_status_data != NULL);

    StackErrorCode error = *((StackErrorCode*)(error_data.custom_status_data));

    switch (error) 
    {
        case STACK_EMPTY_POP:
            fprintf(fp, "StackError: cannot pop from empty stack.\n");
            break;
        case STACK_MEMRY_CORRUPT:
            fprintf(fp, "StackError: stack memory corruption detected.\n");
            break;
        case STACK_STRUCTURE_CORRUPT:
            fprintf(fp, "StackError: stack structure corruption detected.\n");
            break;
        case STACK_INVALID_SIZE:
            fprintf(fp, "StackError: stack size or capacity invalid.\n");
            break;
        default:
            fprintf(fp, "StackError: unimplemented error type.\n");
            break;
    }

    return 0;
}
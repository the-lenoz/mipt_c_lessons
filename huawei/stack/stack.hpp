#ifndef STACK_DECLARED
#define STACK_DECLARED

#include "logger.hpp"
#include "status.hpp"
#include <stdlib.h>

#define STACK_ELEM_TYPE char*

enum StackOperationStatusCode
{
    STACK_SUCCESS,
    STACK_EMPTY_POP,
    STACK_CANNOT_ALLOCATE,
    STACK_INVALID
};

struct Stack 
{
    STACK_ELEM_TYPE* data;
    size_t size;
    size_t capacity;
    StackOperationStatusCode last_operation_status_code;
};

void stack_init(Stack* st, size_t init_stack_size);

void stack_destroy(Stack* st);

void stack_push(Stack* st, STACK_ELEM_TYPE value);

STACK_ELEM_TYPE stack_pop(Stack* st);

int stack_fit_size(Stack* st);

StatusData stack_validate(Stack* st);

void stack_dump(LogMessageType dump_message_type, Stack* st);


#endif // STACK_DECLARED
#ifndef STACK_DECLARED
#define STACK_DECLARED

#include "status.hpp"
#include <stdlib.h>

#ifndef STACK_ELEM_TYPE

#warning "Undefined STACK_ELEM_TYPE. Using int by default."
#define STACK_ELEM_TYPE int

#endif // STACK_ELEM_TYPE

enum StackOperationStatusCode
{
    STACK_SUCCESS,
    STACK_EMPTY_POP,
    STACK_CANNOT_ALLOCATE
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

StatusData stack_validate(Stack* st);



#endif // STACK_DECLARED
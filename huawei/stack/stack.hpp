#ifndef STACK_DECLARED
#define STACK_DECLARED

#include "logger.hpp"
#include "status.hpp"
#include <stdlib.h>
#include <climits>

#define HASH_BASE_SEED 997*991
#define MAX_HASH_STATE (1L << 31) - 1

const char *const hex_numbers = "0123456789abcdef";


#define STACK_ELEM_TYPE double

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
    StatusData last_operation_status;
    STACK_ELEM_TYPE left_canary;
    STACK_ELEM_TYPE right_canary;
    unsigned int hash;
};

void stack_init(Stack* st);

void stack_destroy(Stack* st);

void stack_push(Stack* st, STACK_ELEM_TYPE value);

STACK_ELEM_TYPE stack_pop(Stack* st);

int stack_fit_size(Stack* st);

StatusData stack_validate(Stack* st);

void stack_dump(Stack* st, LogMessageType dump_message_type);

unsigned int stack_calc_hash(Stack* st);

unsigned int calc_shift_hash(const unsigned char* val, size_t len, unsigned int state);


STACK_ELEM_TYPE create_canary();


#endif // STACK_DECLARED
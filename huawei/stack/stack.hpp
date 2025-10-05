#ifndef STACK_DECLARED
#define STACK_DECLARED

#define TypeOf(x) _Generic((x),                         \
                        short: SHORT_T,                 \
                        unsigned short: USHORT_T,       \
                        int: INT_T,                     \
                        unsigned int: UINT_T,           \
                        long: LINT_T,                   \
                        unsigned long: ULINT_T,         \
                        char: CSTRING_T,                \
                        float: FLOAT_T,                 \
                        double: LFLOAT_T,               \
                        default: UNKNOWN_T              \
                    )

#include "logger.hpp"
#include "status.hpp"
#include <stdlib.h>


#define HASH_BASE_SEED 997*991
#define MAX_HASH_STATE (1L << 31) - 1



typedef double STACK_ELEM_TYPE;

enum StackOperationStatusCode
{
    STACK_SUCCESS,
    STACK_EMPTY_POP,
    STACK_CANNOT_ALLOCATE,
    STACK_INVALID
};

enum FormatedTypes
{
    SINT_T,
    USINT_T,

    INT_T,
    UINT_t,
    LINT_T,
    ULINT_T,

    FLOAT_T,
    LFLOAT_T,

    CSTRING_T,

    UNKNOWN_T
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

int stack_is_fit_size(Stack* st);

size_t stack_get_enough_size(Stack* st);

StatusData stack_validate(Stack* st);

void stack_dump(Stack* st, LogMessageType dump_message_type);

STACK_ELEM_TYPE* stack_get_left_canary_address(Stack* st);

STACK_ELEM_TYPE* stack_get_right_canary_address(Stack* st);

unsigned int stack_calc_hash(Stack* st);


unsigned int calc_shift_hash(const unsigned char* val, size_t len, unsigned int state);

STACK_ELEM_TYPE create_canary();


#endif // STACK_DECLARED
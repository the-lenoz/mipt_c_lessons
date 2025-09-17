#include <stdio.h>
#include "error_handling/my_assert.hpp"
#include "status.hpp"


int main(void)
{
    print_error(MAKE_ERROR_STRUCT(CANNOT_ALLOCATE_MEMORY_ERROR));

    return 0;
}
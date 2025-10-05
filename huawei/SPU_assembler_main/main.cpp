#include "terminal_decorator.hpp"
#include "my_assert.hpp"
#include <stdio.h>



int main(int argc, char** argv)
{
    assert(argv != NULL);

    if (argc != 3)
    {
        fprintf_red(stderr, "Usage: %s source.asm output.bcode\n", argv[0]);
        return -1;
    }

    

    return 0;
}
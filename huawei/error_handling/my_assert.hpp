#include <stdlib.h>

#include "status.hpp"

#ifndef NDEBUG
#define assert(expr) if(!(expr)) \
    do\
    {\
        print_error(MAKE_ERROR_STRUCT(ASSERTION_FAILED));\
        print_back_trace();\
        exit(1);\
    } while(0)

#else

#define assert(expr) (void)(expr)

#endif // NDEBUG
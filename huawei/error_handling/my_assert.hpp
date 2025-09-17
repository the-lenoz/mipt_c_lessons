#include <stdlib.h>

#include "status.hpp"

#define assert(expr) if(!(expr)) \
    {\
        print_error(MAKE_ERROR_STRUCT(ASSERTION_FAILED));\
        print_back_trace();\
        exit(1);\
    }

#ifndef SPU_ASSEMBLER_DECLARED
#define SPU_ASSEMBLER_DECLARED

#define MAX_COMMAND_STR_LEN M

#define COMMENT_START_CHAR      ';'
#define LABEL_CHAR              ':'
#define ARG_TYPE_PTR_MODIFIER   '$'
#define MAX_INSTRUCTION_STR_LEN 128
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#include "SPU.hpp"
#include "array_operations.hpp"


struct ASMError
{
    const char* filename;
    int line;
    int column;

    const char* description;
};


struct PreprocessedLine
{
    my_string line;
    int line_number;
};

struct ASMLabel
{
    char name[MAX_INSTRUCTION_STR_LEN];
    int line_number;
    uint32_t bytecode_offset;
};

struct AssembledLine
{
    char bytecode[MAX_INSTRUCTION_SIZE];
    size_t bytecode_size;

    int src_line_number;
    uint32_t bytecode_offset;

    char label_args_names[4][MAX_INSTRUCTION_STR_LEN];
    size_t label_args_num;
};

#endif // SPU_ASSEMBLER_DECLARED
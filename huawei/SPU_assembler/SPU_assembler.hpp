#ifndef SPU_ASSEMBLER_DECLARED
#define SPU_ASSEMBLER_DECLARED

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

    char label_args_names[MAX_ARGS_NUMBER][MAX_INSTRUCTION_STR_LEN];
    size_t label_args_num;
};

typedef ASMLabel STACK_ELEM_TYPE;

int process_asm_file(const char* input_path, const char* output_path);

AssembledLine* process_asm(PreprocessedLine* preprocessed_lines, size_t lines_num);

AssembledLine assemble_line(my_string source_line, size_t bytecode_offset, int source_line_number);




#endif // SPU_ASSEMBLER_DECLARED
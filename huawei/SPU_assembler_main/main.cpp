#include "SPU.hpp"
#include "SPU_assembler/SPU_assembler.hpp"
#include "array_operations.hpp"
#include "file_operations.hpp"
#include "mystr.hpp"
#include "status.hpp"
#include "terminal_decorator.hpp"
#include "my_assert.hpp"
#include <cctype>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define COMMENT_START_CHAR      ';'
#define ARG_TYPE_PTR_MODIFIER   '$'
#define MAX_INSTRUCTION_STR_LEN 32
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)


StatusData process_asm_file(const char* input_path, const char* output_path);

void preprocess_asm(my_string** source_lines_buffer, size_t* lines_num);

void process_asm(my_string** source_lines_buffer, void* bytecode_buffer, size_t* bytecode_len, size_t* lines_num);

size_t assemble_line(char* instruction_buffer, my_string source_line);


ASMError last_line_error = {};


int main(int argc, char** argv)
{
    assert(argv != NULL);

    if (argc != 3)
    {
        fprintf_red(stderr, "Usage: %s source.asm output.bcode\n", argv[0]);
        return -1;
    }

    const char* input_file_path = argv[1];
    const char* output_file_path = argv[2];

    StatusData status = process_asm_file(input_file_path, output_file_path);

    if (status.status_code != SUCCESS)
    {
        print_error(status);
        return -1;
    }

    return 0;
}


StatusData process_asm_file(const char* input_path, const char* output_path)
{
    printf_yellow("Processing ASM file: '%s'\n", input_path);
    printf_yellow("Reading\n");
    
    my_string* source_lines_buffer = NULL;
    size_t lines_num = allocate_and_read_lines(input_path, &source_lines_buffer);

    char* full_asm_buffer = source_lines_buffer[0].str;

    if (source_lines_buffer == NULL)
    {
        return MAKE_ERROR_STRUCT(CANNOT_OPEN_FILE_ERROR);
    }

    printf_green("Readed %zu lines\n", lines_num);    
    
    printf_yellow("Preprocessing\n");

    preprocess_asm(&source_lines_buffer, &lines_num);


    printf_yellow("Processing\n");

    void* bytecode_buffer = calloc(lines_num, MAX_INSTRUCTION_SIZE);
    size_t bytecode_len = 0;

    process_asm(&source_lines_buffer, bytecode_buffer, &bytecode_len, &lines_num);


    free(full_asm_buffer);
    free(source_lines_buffer);


    if (bytecode_buffer == NULL)
    {
        return MAKE_ERROR_STRUCT(CANNOT_ALLOCATE_MEMORY_ERROR);
    }
    

    FILE* output_file_ptr = fopen(output_path, "w");

    if (output_file_ptr == NULL)
    {
        free(bytecode_buffer);
        return MAKE_ERROR_STRUCT(CANNOT_OPEN_FILE_ERROR);
    }

    fwrite(bytecode_buffer, 1, bytecode_len, output_file_ptr);

    fclose(output_file_ptr);

    free(bytecode_buffer);

    return MAKE_SUCCESS_STRUCT(NULL);
}

void preprocess_asm(my_string** source_lines_buffer, size_t* lines_num)
{
    //remove comments and empty lines. LStrip lines
    size_t n = 0;
    size_t comment_index = 0;

    for (size_t i = 0; i < *lines_num; ++i)
    {

        (*source_lines_buffer)[i].len = (size_t) (strchrnul((*source_lines_buffer)[i].str, COMMENT_START_CHAR) - (*source_lines_buffer)[i].str);

        comment_index = (size_t) (strchrnul((*source_lines_buffer)[i].str, '\n') - (*source_lines_buffer)[i].str);

        if (comment_index < (*source_lines_buffer)[i].len)
        {
            (*source_lines_buffer)[i].len = comment_index;
        }
        if (!is_empty_mystr((*source_lines_buffer)[i]))
        {
            while (isspace((*source_lines_buffer)[i].str[0]) && (*source_lines_buffer)[i].len) // LStrip line
            {
                (*source_lines_buffer)[i].str++;
                (*source_lines_buffer)[i].len--;
            } 

            (*source_lines_buffer)[n++] = (*source_lines_buffer)[i];
        }
    }
    *lines_num = n;

    //TODO - include, define, etc.
}

void process_asm(my_string** source_lines_buffer, void* bytecode_buffer, size_t* bytecode_len, size_t* lines_num)
{

    char instruction_buffer[MAX_INSTRUCTION_SIZE] = {};
    size_t instruction_len = 0;

    *bytecode_len = 0;

    for (size_t i = 0; i < *lines_num; ++i)
    {
        printf_yellow("Assembling line: %zu, '%.*s' - \t", i, (int) (*source_lines_buffer)[i].len, (*source_lines_buffer)[i].str);

        last_line_error.description = NULL;
        instruction_len = assemble_line(instruction_buffer, (*source_lines_buffer)[i]);

        if (last_line_error.description != NULL)
        {
            fprintf_red(stderr, "%.*s\n", (int) (*source_lines_buffer)[i].len, (*source_lines_buffer)[i].str);
            for (int j = 0; j < last_line_error.column - 1; ++j)
            {
                fprintf(stderr, " ");
            }
            fprintf_red(stderr, "^\nSyntax error: %s\n%s:%d", last_line_error.description, last_line_error.filename, last_line_error.line);
        }
        
        memcpy((char*) bytecode_buffer + *bytecode_len, instruction_buffer, instruction_len);

        printf_green("Done\n");

        *bytecode_len += instruction_len;
    }
}


size_t assemble_line(char* instruction_buffer, my_string source_line)
{
    uint32_t args[4] = {};
    unsigned char arg_type = ARG_TYPE_ASIS;

    char operand_str[MAX_INSTRUCTION_STR_LEN + 1] = {};

    char* line_start = source_line.str;

    SPUInstruction instruction = {};

    int read_chars = 0;



    if (source_line.len == 0 || source_line.str == NULL)
    {
        return 0;
    }

    if (source_line.str[0] == ARG_TYPE_PTR_MODIFIER)
    {
        arg_type = ARG_TYPE_PTR;
        source_line.str++;
        source_line.len--;
    }

    sscanf(source_line.str, "%" STR(MAX_INSTRUCTION_STR_LEN) "s%n", operand_str, &read_chars);

    if (strlen(operand_str) == 0)
    {
        last_line_error = {
            .column = (int) (source_line.str - line_start),
            .description = "Can't parse opcode"
        };
        return 0;
    }
         
    source_line.len -= (size_t)read_chars;
    source_line.str += read_chars;

    for (size_t i = 0; i < instructions_number; ++i)
    {
        if (strcmp(instructions[i].name, operand_str) == 0)
        {
            instruction = instructions[i];
            break;
        }
    }

    instruction.opcode = instruction.opcode | arg_type;
    memcpy(instruction_buffer, &instruction.opcode, OPCODE_SIZE);

    instruction_buffer += OPCODE_SIZE;


    printf_yellow("Found command: '%s'\t", instruction.name);

    const char* format = NULL;
    
    for (size_t i = 0; i < instruction.args_num; ++i)
    {
        if (sscanf(source_line.str, "%s%n", operand_str, &read_chars) == 1)
        {
            if (mc_startswith(operand_str, "0x"))
            {
                format = "%x";
                memmove(operand_str, operand_str + 2, MAX_INSTRUCTION_SIZE - 2);
            }
            else
            {
                format = "%d";
            }

            if (sscanf(operand_str, format, &(args[i])) == 1)
            {
                printf_red("ARG%zu:%d,\t", i, args[i]);
            }
            else
            {
                last_line_error = {
                    .column = (int) (source_line.str - line_start),
                    .description = "Missing argument"
                };
                return 0; //SyntaxError
            }
            source_line.str += read_chars;
            source_line.len -= (unsigned)read_chars;
        }
        else 
        {
            last_line_error = {
                .column = (int) (source_line.str - line_start),
                .description = "Missing argument"
            };
            return 0; //SyntaxError
        }
    }

    memcpy(instruction_buffer, args, instruction.args_num * ARG_SIZE);

    return instruction.args_num * ARG_SIZE + OPCODE_SIZE;
}
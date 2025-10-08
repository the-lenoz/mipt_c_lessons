#include "SPU.hpp"
#include "SPU_assembler/SPU_assembler.hpp"
#include "array_operations.hpp"
#include "file_operations.hpp"
#include "logger.hpp"
#include "mystr.hpp"
#include "stack.hpp"
#include "status.hpp"
#include "terminal_decorator.hpp"
#include "my_assert.hpp"
#include <cctype>
#include <cstdlib>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>


int process_asm_file(const char* input_path, const char* output_path);

PreprocessedLine* preprocess_asm(my_string* source_lines_buffer, size_t* lines_num, Stack* labels);

AssembledLine* process_asm(PreprocessedLine* preprocessed_lines, size_t lines_num);

AssembledLine assemble_line(my_string source_line, size_t bytecode_offset, int source_line_number);


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

    return process_asm_file(input_file_path, output_file_path);
}


int process_asm_file(const char* input_path, const char* output_path)
{
    last_line_error = {};
    last_line_error.filename = input_path;

    printf_yellow("Processing ASM file: '%s'\n", input_path);
    printf_yellow("Reading\n");
    
    my_string* source_lines_buffer = NULL;
    size_t lines_num = allocate_and_read_lines(input_path, &source_lines_buffer);

    char* full_source_buffer = source_lines_buffer[0].str;

    if (source_lines_buffer == NULL)
    {
        print_error(MAKE_ERROR_STRUCT(CANNOT_OPEN_FILE_ERROR));
        return -1;
    }

    printf_green("Readed %zu lines\n", lines_num);    
    
    printf_yellow("Preprocessing\n");

    Stack preprocessed_labels = {};
    stack_init(&preprocessed_labels);

    PreprocessedLine* preprocessed_lines = preprocess_asm(source_lines_buffer, &lines_num, &preprocessed_labels);

    printf_yellow("Processing\n");

    AssembledLine* bytecode_lines = process_asm(preprocessed_lines, lines_num);

    free(preprocessed_lines);
    free(full_source_buffer);
    free(source_lines_buffer);

    if (bytecode_lines == NULL)
    {
        fprintf_red(stderr, "Errors found while assembling. Exiting.\n");
        stack_destroy(&preprocessed_labels);
        return -1;
    }

    ASMLabel label = {};
    Stack asm_labels = {};
    stack_init(&asm_labels);
    while (preprocessed_labels.size)
    {
        label = stack_pop(&preprocessed_labels);
        for (size_t i = 0; i < lines_num; ++i)
        {
            if (label.line_number == bytecode_lines[i].src_line_number)
            {
                label.bytecode_offset = bytecode_lines[i].bytecode_offset;
                stack_push(&asm_labels, label);
            }
        }
    }
    stack_destroy(&preprocessed_labels);

    char* bytecode_buffer = (char*) calloc(lines_num, MAX_INSTRUCTION_SIZE);

    if (bytecode_buffer == NULL)
    {
        print_error(MAKE_ERROR_STRUCT(CANNOT_ALLOCATE_MEMORY_ERROR));
        return -1;
    }

    size_t bytecode_len = 0;
    int32_t relative_address = 0;

    while (asm_labels.size)
    {
        label = stack_pop(&asm_labels);
        for (size_t i = 0; i < lines_num; ++i)
        {
            for (size_t j = 0; j < 4; ++j)
            {
                if (*(bytecode_lines[i].label_args_names[j]) == 0)
                {
                    continue;
                }
                
                if (strcmp(bytecode_lines[i].label_args_names[j], label.name) == 0)
                {
                    relative_address = (int32_t) label.bytecode_offset -  (int32_t) bytecode_lines[i].bytecode_offset;
                    printf_yellow("Writing label data LINE: %zu, arg[%zu] <- (%u - %u) = %d\n", i + 1, j, 
                        label.bytecode_offset, bytecode_lines[i].bytecode_offset, relative_address);
                    memcpy(bytecode_lines[i].bytecode + OPCODE_SIZE + j * ARG_SIZE, &relative_address, sizeof(relative_address));
                }
            }
        }
    }

    stack_destroy(&asm_labels);

    for (size_t i = 0; i < lines_num; ++i)
    {
        memcpy(bytecode_buffer + bytecode_len, bytecode_lines[i].bytecode, bytecode_lines[i].bytecode_size);
        bytecode_len += bytecode_lines[i].bytecode_size;
    }


    free(bytecode_lines);
    

    FILE* output_file_ptr = fopen(output_path, "w");

    if (output_file_ptr == NULL)
    {
        free(bytecode_buffer);
        print_error(MAKE_ERROR_STRUCT(CANNOT_OPEN_FILE_ERROR));
    }

    fwrite(bytecode_buffer, 1, bytecode_len, output_file_ptr);

    fclose(output_file_ptr);

    free(bytecode_buffer);

    printf_green("Assembled successfully\n");

    return 0;
}

PreprocessedLine* preprocess_asm(my_string* source_lines_buffer, size_t* lines_num, Stack* labels)
{
    PreprocessedLine* preprocessed_lines_buffer = (PreprocessedLine*) calloc(*lines_num, sizeof(PreprocessedLine));

    //remove comments and empty lines. LStrip lines
    size_t n = 0;
    size_t comment_index = 0;
    char* label_mark_ptr = NULL;

    ASMLabel label = {};

    for (size_t i = 0; i < *lines_num; ++i)
    {
        source_lines_buffer[i].len = (size_t) (strchrnul(source_lines_buffer[i].str, COMMENT_START_CHAR) - source_lines_buffer[i].str);

        comment_index = (size_t) (strchrnul(source_lines_buffer[i].str, '\n') - source_lines_buffer[i].str);

        if (comment_index < source_lines_buffer[i].len)
        {
            source_lines_buffer[i].len = comment_index;
        }

        while (source_lines_buffer[i].len && isspace(source_lines_buffer[i].str[0])) // LStrip line
        {
            source_lines_buffer[i].str++;
            source_lines_buffer[i].len--;
        }

        label_mark_ptr = (char*) memchr(source_lines_buffer[i].str, LABEL_CHAR, source_lines_buffer[i].len);
        if (label_mark_ptr != NULL) //LABEL
        {
            label = {};
            label.line_number = (int) i;
            strncpy(label.name, source_lines_buffer[i].str, (size_t) (label_mark_ptr - source_lines_buffer[i].str));
            stack_push(labels, label);

            source_lines_buffer[i].len -= (size_t) (label_mark_ptr - source_lines_buffer[i].str + 1);
            source_lines_buffer[i].str = label_mark_ptr + 1;

            printf_yellow("Found label '%s'\n", label.name);
        }

        if (!is_empty_mystr(source_lines_buffer[i]) || 1)
        {
            while (source_lines_buffer[i].len && isspace(source_lines_buffer[i].str[0])) // LStrip line
            {
                source_lines_buffer[i].str++;
                source_lines_buffer[i].len--;
            }

            preprocessed_lines_buffer[n].line = source_lines_buffer[i];
            preprocessed_lines_buffer[n++].line_number = (int) i;
        }
    }
    *lines_num = n;

    //TODO - include, define, etc.

    return preprocessed_lines_buffer;
}

AssembledLine* process_asm(PreprocessedLine* preprocessed_lines, size_t lines_num)
{
    AssembledLine* assembled_lines_buffer = (AssembledLine*) calloc(lines_num, sizeof(AssembledLine));

    size_t bytecode_offset = 0;

    int error = 0;

    for (size_t i = 0; i < lines_num; ++i)
    {
        printf_yellow("Assembling line: %zu, '%.*s' - \t", i, (int) preprocessed_lines[i].line.len, preprocessed_lines[i].line.str);

        last_line_error.description = NULL;
        last_line_error.column = 0;
        last_line_error.line = 0;

        assembled_lines_buffer[i] = assemble_line(preprocessed_lines[i].line, bytecode_offset,
            preprocessed_lines[i].line_number);

        if (last_line_error.description != NULL)
        {
            fprintf_red(stderr, "%.*s\n", (int) preprocessed_lines[i].line.len, preprocessed_lines[i].line.str);
            for (int j = 0; j < last_line_error.column - 1; ++j)
            {
                fprintf(stderr, " ");
            }
            fprintf_red(stderr, "^\nSyntax error: %s\n%s:%d:%d\n", 
                last_line_error.description, last_line_error.filename, last_line_error.line + 1, last_line_error.column);
            error = 1;
            printf_red("Error\n");
            continue;
        }
        bytecode_offset += assembled_lines_buffer[i].bytecode_size;

        printf_green("Done\n");
    }

    if (error)
    {
        free(assembled_lines_buffer);
        return NULL;
    }

    return assembled_lines_buffer;
}


AssembledLine assemble_line(my_string source_line, size_t bytecode_offset, int source_line_number)
{
    uint32_t args[4] = {};
    unsigned char arg_type = ARG_TYPE_ASIS;

    char operand_str[MAX_INSTRUCTION_STR_LEN + 1] = {};

    char* line_start = source_line.str;

    SPUInstruction instruction = {};

    int read_chars = 0;

    AssembledLine result = {};

    size_t current_inline_bytecode_offset = 0;

    result.bytecode_offset = (uint32_t) bytecode_offset;
    result.src_line_number = source_line_number;

    result.label_args_num = 0;

    if (source_line.len == 0 || source_line.str == NULL)
    {
        result.bytecode_size = 0;
        return result;
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
        last_line_error.line = source_line_number;
        last_line_error.column = (int) (source_line.str - line_start);
        last_line_error.description = "Can't parse opcode";

        result.bytecode_size = 0;
        return result;
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

    if (instruction.name == NULL)
    {
        last_line_error.line = source_line_number;
        last_line_error.column = (int) (source_line.str - line_start);
        last_line_error.description = "Unknown instruction";

        result.bytecode_size = 0;
        return result; 
    }

    instruction.opcode = instruction.opcode | arg_type;
    memcpy(result.bytecode, &instruction.opcode, OPCODE_SIZE);

    current_inline_bytecode_offset += OPCODE_SIZE;

    printf_yellow("Found command: '%s'\t", instruction.name);

    const char* format = NULL;
    
    for (size_t i = 0; i < instruction.args_num; ++i)
    {
        memset(operand_str, 0, MAX_INSTRUCTION_STR_LEN + 1);
        read_chars = 0;
        if (sscanf(source_line.str, "%*[ \t]%n%" STR(MAX_INSTRUCTION_STR_LEN) "[0-9a-zA-Z_-]%n", 
            &read_chars, operand_str, &read_chars) == 1)
        {
            if (isdigit(operand_str[0]) || operand_str[0] == '-')
            {
                if (mc_startswith(operand_str, "0x"))
                {
                    format = "%x";
                    memmove(operand_str, operand_str + 2, MAX_INSTRUCTION_STR_LEN - 2);
                }
                else 
                {
                    format = "%d";
                }

                if (sscanf(operand_str, format, &(args[i])) != 1)
                {
                    last_line_error.line = source_line_number;
                    last_line_error.column = (int) (source_line.str - line_start);
                    last_line_error.description = "Missing argument";
                    
                    result.bytecode_size = 0;
                    return result; 
                }
                printf_yellow("ARG%zu:%d,\t", i, args[i]);
            }
            
            else // LABEL
            {
                args[i] = 0;
                result.label_args_num++;
                strcpy(result.label_args_names[i], operand_str);

                printf_yellow("ARG%zu:LABEL(%s),\t", i, operand_str);
            }        
            source_line.str += read_chars;
            source_line.len -= (unsigned)read_chars;    
        }
        else 
        {
            source_line.str += read_chars;
            source_line.len -= (unsigned)read_chars;   

            last_line_error.line = source_line_number;
            last_line_error.column = (int) (source_line.str - line_start);
            last_line_error.description = "Missing argument";
            
            result.bytecode_size = 0;
            return result; 
        }
    }

    memcpy(result.bytecode + current_inline_bytecode_offset, args, instruction.args_num * ARG_SIZE);

    result.bytecode_size = instruction.args_num * ARG_SIZE + OPCODE_SIZE;

    return result;
}
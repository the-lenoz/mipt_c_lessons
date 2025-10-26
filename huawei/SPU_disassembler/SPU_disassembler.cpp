#include <cstdint>
#include <cstdio>
#include <cstring>

#include "SPU_assembler.hpp"

#include "SPU_disassembler.hpp"

#include "debug_utilites.hpp"
#include "my_assert.hpp"



int fdisassemble_buffer(uint8_t* bytecode_buffer, size_t buffer_size, FILE* fp)
{
    assert(bytecode_buffer);
    assert(fp);

    size_t buffer_index = 0;
    int instruction_size = 0;

    char instruction_disasm[MAX_INSTRUCTION_STR_LEN + 1];

    while (buffer_index < buffer_size)
    {
        memset(instruction_disasm, '\0', MAX_INSTRUCTION_STR_LEN + 1);
        instruction_size = sdisassemble_instruction(bytecode_buffer + buffer_index, buffer_size - buffer_index, instruction_disasm);
        if (instruction_size == -1)
        {
            break;
        }
        fprintf(fp, "%s\n", instruction_disasm);
        buffer_index += instruction_size;
    }

    return 0;
}

int sdisassemble_instruction(uint8_t* bytecode, size_t size, char* disasm_array)
{
    assert(bytecode);
    assert(disasm_array); //TODO size

    uint8_t opcode = *bytecode;
    switch (opcode & (uint8_t)~(uint8_t)ARG_TYPE_OPCODE_MASK)
    {
        #define INSTRUCTION(instr_name, instr_opcode, args_number, executor) case instr_opcode: \
            {\
                strcpy(disasm_array, instr_name);\
                for (int i = 0; i < args_number; ++i)\
                {\
                    strcat(disasm_array, "\t");\
                    sprintf(disasm_array + strlen(disasm_array), "0x%08x", *(uint32_t*)(bytecode + 1 + i * ARG_SIZE));\
                } return OPCODE_SIZE + args_number * ARG_SIZE;\
            }
        EXPAND_INSTRUCTIONS()
        #undef INSTRUCTION
        default: 
            strcpy(disasm_array, "db ");
            to_hex(disasm_array + 3, bytecode, 1);
            disasm_array[5] = '\0';
            return 1;
    }   
}

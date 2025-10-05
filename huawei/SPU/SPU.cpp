#include "SPU.hpp"
#include "debug_utilites/debug_utilites.hpp"
#include "error_handling/my_assert.hpp"
#include "status.hpp"
#include "terminal_decorator.hpp"
#include <cstdint>
#include <cstdlib>
#include <stdint.h>
#include <string.h>
#include <stdint.h>


SPUInstruction parse_instruction(void* instruction_ptr)
{
    unsigned char opcode = ((unsigned char*) instruction_ptr)[0];

    SPUInstruction instruction = {};

    for (size_t i = 0; i < instructions_number; ++i)
    {
        if ((opcode & (~ARG_TYPE_OPCODE_MASK)) == instructions[i].opcode)
        {
            instruction = instructions[i];
            break;
        }
    }

    instruction.opcode = opcode;
    instruction.args_ptr = (unsigned char*)instruction_ptr + OPCODE_SIZE;

    return instruction;
}



void SPU_init(SPU* processor, uint32_t RAM_size)
{
    processor->memory_size = RAM_size;
    processor->memory = calloc(RAM_size, 1);

    if (processor->memory == NULL)
    {
        processor->last_operation_status = MAKE_ERROR_STRUCT(CANNOT_ALLOCATE_MEMORY_ERROR);
        return;
    }

    return;
}

void SPU_destroy(SPU* processor)
{
    assert(processor != NULL);
    assert(processor->memory != NULL);

    free(processor->memory);

    *processor = {};
}


void SPU_start(SPU* processor, uint32_t entrypoint)
{
    assert(processor != NULL);
    assert(processor->memory != NULL);

    SPU_write_memory_cell(processor, INSTRUCTION_POINTER_ADDR, entrypoint);


    SPU_run_loop(processor);
}


void SPU_run_loop(SPU* processor)
{
    assert(processor != NULL);
    assert(processor->memory != NULL);

    processor->is_running = 1;

    uint32_t instruction_pointer = 0;

    printf_yellow("Starting processor\n");

    while (processor->is_running)
    {
        instruction_pointer = SPU_read_memory_cell(processor, INSTRUCTION_POINTER_ADDR);

        if (SPU_is_valid_memaddr(processor, instruction_pointer))
        {
            SPU_execute_instruction(processor, instruction_pointer);

            SPU_dump(processor);
            getchar();
        }
        else
        {
            processor->is_running = 0;
        }       
    }

    printf_green("Run completed\n");
}


int SPU_is_valid_memaddr(SPU* processor, uint32_t virtual_address)
{
    return virtual_address >= 0 && virtual_address < processor->memory_size;
}


StatusData SPU_verify(SPU* processor)   //TODO
{
    UNUSED(processor);
    return MAKE_SUCCESS_STRUCT(NULL);
}


//! REQUIRES: LOGGER INITIALIZED
void SPU_dump(SPU* processor)
{
    char mem_dump_row[DUMP_ROW_SIZE];
    char mem_dump_row_str[DUMP_ROW_SIZE * 3 + 1] = {};

    printf_green("SPU {\n  memory_size = %u,\n  memory_ptr = %p,\n  is_running = %d\n}\n", processor->memory_size, processor->memory, processor->is_running);

    printf_yellow("=== RAM_DUMP_BEGIN ===\n\n");
    for (uint32_t dump_ptr = 0; dump_ptr + DUMP_ROW_SIZE <= processor->memory_size; dump_ptr += DUMP_ROW_SIZE)
    {
        SPU_read_memory(processor, mem_dump_row, dump_ptr, DUMP_ROW_SIZE);
        to_hex(mem_dump_row_str, mem_dump_row, DUMP_ROW_SIZE);
        printf_green("ADDR: %#8x\t| %s |\n", dump_ptr, mem_dump_row_str);
    }
    printf_yellow("\n=== RAM_DUMP_END ===\n");
}



void* SPU_get_real_mem_addr(SPU* processor, uint32_t virtual_addr)
{
    assert(processor != NULL);
    assert(processor->memory_size != 0);
    assert(processor->memory != NULL);

    assert(virtual_addr < processor->memory_size);

    return (char*)(processor->memory) + virtual_addr;

}

uint32_t SPU_read_memory_cell(SPU* processor, uint32_t virtual_addr)
{
    assert(processor != NULL);
    assert(processor->memory_size != 0);
    assert(processor->memory != NULL);

    assert(virtual_addr < processor->memory_size);

    uint32_t value = 0;

    SPU_read_memory(processor, &value, virtual_addr, sizeof(value));

    printf_yellow("Reading memory cell: [%u] = %u\n", virtual_addr, value);


    return value;
}

void SPU_write_memory_cell(SPU* processor, uint32_t virtual_addr, uint32_t value)
{
    assert(processor != NULL);
    assert(processor->memory_size != 0);
    assert(processor->memory != NULL);

    assert(virtual_addr < processor->memory_size);

    printf_yellow("Writing memory cell: [%u] <- %u\n", virtual_addr, value);

    SPU_write_memory(processor, virtual_addr, &value, sizeof(value));
}

void SPU_write_memory(SPU *processor, uint32_t virtual_dst, void* real_src, size_t count)
{
    assert(processor != NULL);
    assert(processor->memory_size != 0);
    assert(processor->memory != NULL);

    void* real_dst = SPU_get_real_mem_addr(processor, virtual_dst);

    memcpy(real_dst, real_src, count);
}

void SPU_read_memory(SPU *processor, void* real_dst, uint32_t virtual_src, size_t count)
{
    assert(processor != NULL);
    assert(processor->memory_size != 0);
    assert(processor->memory != NULL);

    void* real_src = SPU_get_real_mem_addr(processor, virtual_src);

    memcpy(real_dst, real_src, count);
}


void SPU_execute_instruction(SPU* processor, uint32_t virtual_instruction_ptr)
{
    assert(processor != NULL);
    assert(processor->memory_size != 0);
    assert(processor->memory != NULL);

    void* real_instruction_ptr = SPU_get_real_mem_addr(processor, virtual_instruction_ptr);

    SPUInstruction instruction = parse_instruction(real_instruction_ptr);

    printf_yellow("Executing: '%s'\n", instruction.name);

    instruction.executor(processor, instruction);

    uint32_t current_instruction_pointer = SPU_read_memory_cell(processor, INSTRUCTION_POINTER_ADDR);
    SPU_write_memory_cell(processor, INSTRUCTION_POINTER_ADDR, current_instruction_pointer + OPCODE_SIZE + ARG_SIZE * instruction.args_num);
}



void SPU_execute_O_NOP(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;
}

void SPU_execute_O_HLT(SPU* processor, SPUInstruction instr)
{
    UNUSED(instr);
    
    processor->is_running = 0;
    
    return;
}


void SPU_execute_O_INT(SPU* processor, SPUInstruction instr)
{
    assert(instr.args_ptr != NULL);
    assert(instr.args_num == 2);

    uint32_t args[2] = {};
    memcpy(args, instr.args_ptr, 2 * sizeof(args[0]));

    uint32_t interrupt_index = args[0];

    uint32_t interrupt_data_ptr = args[1];

    printf_yellow("Executing INT 0x%x 0x%x\n", interrupt_index, interrupt_data_ptr);


    if ((instr.opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)
    {
        interrupt_index = SPU_read_memory_cell(processor, interrupt_index);

        interrupt_data_ptr = SPU_read_memory_cell(processor, interrupt_data_ptr);
    }
    
    if (interrupt_index >= interrupt_handlers_number || interrupt_handlers[interrupt_index] == NULL)
    {
        return; // UNKNOWN INTERUPT
    }

    interrupt_handlers[interrupt_index](processor, interrupt_data_ptr);

    return;
}
void SPU_execute_O_MOV_CONST(SPU* processor, SPUInstruction instr)
{
    uint32_t args[2] = {};
    memcpy(args, instr.args_ptr, 2 * sizeof(args[0]));

    uint32_t destination_ptr = args[0];

    uint32_t value = args[1];


    if ((instr.opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)
    {
        destination_ptr = SPU_read_memory_cell(processor, destination_ptr);
    }

    printf_yellow("MOVC dest=%p, value=%u\n", destination_ptr, value);

    SPU_write_memory_cell(processor, destination_ptr, value);

    return;
}

void SPU_execute_O_LEA(SPU* processor, SPUInstruction instr)
{
    uint32_t args[2] = {};
    memcpy(args, instr.args_ptr, 2 * sizeof(args[0]));

    uint32_t destination_ptr = args[0];

    uint32_t local_ptr_value = args[1];


    if ((instr.opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)
    {
        destination_ptr = SPU_read_memory_cell(processor, destination_ptr);
    }

    uint32_t offset = SPU_read_memory_cell(processor, INSTRUCTION_POINTER_ADDR);


    SPU_write_memory_cell(processor, destination_ptr, offset + local_ptr_value);

    return;
}


void SPU_execute_O_MOV(SPU* processor, SPUInstruction instr)
{
    assert(instr.args_ptr != NULL);
    assert(instr.args_num == 3);

    uint32_t args[3] = {};

    memcpy(args, instr.args_ptr, 3 * sizeof(args[0]));

    uint32_t dst_ptr = args[0];

    uint32_t src_ptr = args[1];

    uint32_t count = args[2];


    if ((instr.opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)
    {
        dst_ptr = SPU_read_memory_cell(processor, dst_ptr);

        src_ptr = SPU_read_memory_cell(processor, src_ptr);

        count = SPU_read_memory_cell(processor, count);
    }
    
    void* real_dst_ptr = SPU_get_real_mem_addr(processor, dst_ptr);
    void* real_src_ptr = SPU_get_real_mem_addr(processor, src_ptr);


    memmove(real_dst_ptr, real_src_ptr, count);

    return;
}

void SPU_execute_O_INV(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;
}

void SPU_execute_O_NEG(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;
}

void SPU_execute_O_INC(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;
}
void SPU_execute_O_DEC(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;
}

void SPU_execute_O_ALL(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;;
}
void SPU_execute_O_ANY(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;;
}

void SPU_execute_O_CLEA(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;;
}



void SPU_execute_O_EQ(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;;
}
void SPU_execute_O_OR(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;;
}
void SPU_execute_O_AND(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;;
}
void SPU_execute_O_XOR(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;;
}

void SPU_execute_O_ADD(SPU* processor, SPUInstruction instr)
{
    assert(instr.args_ptr != NULL);
    assert(instr.args_num == 4);

    uint32_t args[4] = {};
    memcpy(args, instr.args_ptr, 4 * ARG_SIZE);

    uint32_t dst_ptr = args[0];

    uint32_t A_ptr = args[1];
    uint32_t B_ptr = args[2];

    uint32_t count = args[3];


    if ((instr.opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)
    {
        dst_ptr = SPU_read_memory_cell(processor, dst_ptr);

        A_ptr = SPU_read_memory_cell(processor, A_ptr);
        B_ptr = SPU_read_memory_cell(processor, B_ptr);

        count = SPU_read_memory_cell(processor, count);
    }

    uint64_t carry = 0, A = 0, B = 0;

    for (uint32_t i = 0; i < count * sizeof(uint32_t); i += sizeof(uint32_t))
    {
        A = SPU_read_memory_cell(processor, A_ptr + i);
        B = SPU_read_memory_cell(processor, B_ptr + i);
        carry = A + B + carry;
        
        SPU_write_memory_cell(processor, dst_ptr + i, carry & 0x00000000ffffffff);
        carry = carry >> 32;
    }
    
    if (carry)
    {
        SPU_write_memory_cell(processor, dst_ptr + count * sizeof(uint32_t), carry & 0x00000000ffffffff);
    }

    return;
}
void SPU_execute_O_SUB(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;;
}
void SPU_execute_O_MUL(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;;
}
void SPU_execute_O_DIV(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;;
}

void SPU_execute_O_CMOV(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;;
}
void SPU_execute_O_CALL(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;;
}


void SPU_handle_print_DWORD_number(SPU* processor, uint32_t data_ptr)
{
    int32_t number = (int32_t) SPU_read_memory_cell(processor, data_ptr);

    printf_blinking("SPU_OUTPUT: '%d'\n", number);

    return;
}

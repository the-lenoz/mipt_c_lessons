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



static SPUInstruction parse_instruction(void* instruction_ptr);


static void SPU_run_loop(SPU* processor);


static int SPU_is_valid_memaddr(SPU* processor, uint32_t virtual_address);

static int SPU_is_valid_mem_array(SPU* processor, uint32_t virtual_array_address, uint32_t array_size);


static void* SPU_get_real_mem_addr(SPU* processor, uint32_t virtual_addr);

static uint32_t SPU_read_memory_cell(SPU* processor, uint32_t virtual_addr);
static void SPU_write_memory_cell(SPU* processor, uint32_t virtual_addr, uint32_t value);



static void SPU_read_memory(SPU *processor, void* real_dst, uint32_t virtual_src, size_t count);

static uint32_t SPU_get_abs_ptr(SPU* processor, int32_t virtual_relative_ptr);


static void SPU_execute_instruction(SPU* processor, uint32_t virtual_instruction_ptr);



static SPUInstruction parse_instruction(void* instruction_ptr)
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



void SPU_init(SPU* processor, uint32_t RAM_size, int debug_mode)
{
    processor->memory_size = RAM_size;
    processor->memory = calloc(RAM_size, 1);
    processor->debug_mode = debug_mode;

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


static void SPU_run_loop(SPU* processor)
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


static int SPU_is_valid_memaddr(SPU* processor, uint32_t virtual_address)
{
    return virtual_address < processor->memory_size;
}

static int SPU_is_valid_mem_array(SPU* processor, uint32_t virtual_array_address, uint32_t array_size)
{
    return SPU_is_valid_memaddr(processor, virtual_array_address) 
        && SPU_is_valid_memaddr(processor, virtual_array_address + array_size - 1);
}


StatusData SPU_verify(SPU* processor)   //TODO
{
    UNUSED(processor);
    return MAKE_SUCCESS_STRUCT(NULL);
}


void SPU_dump(SPU* processor)
{
    char mem_dump_row[DUMP_ROW_SIZE];
    char mem_dump_row_str[DUMP_ROW_SIZE * 3 + 1] = {};

    printf_green("SPU {\n  memory_size = %u,\n  memory_ptr = %p,\n  is_running = %d\n}\n", processor->memory_size, processor->memory, processor->is_running);

    printf_yellow("=== RAM_DUMP_BEGIN ===\n\n");

    uint32_t used_memory_size = processor->memory_size;

    char b = 0;

    for (; b == 0; SPU_read_memory(processor, &b, used_memory_size - 1, 1)) used_memory_size--;

    used_memory_size = (used_memory_size / DUMP_ROW_SIZE) * DUMP_ROW_SIZE;

    for (uint32_t dump_ptr = 0; dump_ptr <= used_memory_size; dump_ptr += DUMP_ROW_SIZE)
    {
        memset(mem_dump_row, 0, DUMP_ROW_SIZE);
        if (SPU_is_valid_mem_array(processor, dump_ptr, DUMP_ROW_SIZE))
        {
            SPU_read_memory(processor, mem_dump_row, dump_ptr, DUMP_ROW_SIZE);
        }

        to_hex(mem_dump_row_str, mem_dump_row, DUMP_ROW_SIZE);
        printf_green("ADDR: %#8x\t| %s |\n", dump_ptr, mem_dump_row_str);
    }
    printf_yellow("\n=== RAM_DUMP_END ===\n");
}



static void* SPU_get_real_mem_addr(SPU* processor, uint32_t virtual_addr)
{
    assert(processor != NULL);
    assert(processor->memory_size != 0);
    assert(processor->memory != NULL);

    assert(virtual_addr < processor->memory_size);

    return (char*)(processor->memory) + virtual_addr;

}

static uint32_t SPU_read_memory_cell(SPU* processor, uint32_t virtual_addr)
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

static void SPU_write_memory_cell(SPU* processor, uint32_t virtual_addr, uint32_t value)
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

    //printf_yellow("Writing memory: %zu bytes to 0x%x\n", count, virtual_dst);

    memcpy(real_dst, real_src, count);
}

static void SPU_read_memory(SPU *processor, void* real_dst, uint32_t virtual_src, size_t count)
{
    assert(processor != NULL);
    assert(processor->memory_size != 0);
    assert(processor->memory != NULL);

    void* real_src = SPU_get_real_mem_addr(processor, virtual_src);

    //printf_yellow("Reading memory: %zu bytes from 0x%x\n", count, virtual_src);

    memcpy(real_dst, real_src, count);
}

static uint32_t SPU_get_abs_ptr(SPU* processor, int32_t virtual_relative_ptr)
{
    //uint32_t offset = SPU_read_memory_cell(processor, INSTRUCTION_POINTER_ADDR);

    return (uint32_t)(virtual_relative_ptr + (int32_t)processor->current_instruction_pointer);
}


static void SPU_execute_instruction(SPU* processor, uint32_t virtual_instruction_ptr)
{
    assert(processor != NULL);
    assert(processor->memory_size != 0);
    assert(processor->memory != NULL);

    void* real_instruction_ptr = SPU_get_real_mem_addr(processor, virtual_instruction_ptr);

    SPUInstruction instruction = parse_instruction(real_instruction_ptr);


    uint32_t current_instruction_pointer = SPU_read_memory_cell(processor, INSTRUCTION_POINTER_ADDR);

    processor->current_instruction_pointer = current_instruction_pointer;

    SPU_write_memory_cell(processor, INSTRUCTION_POINTER_ADDR, current_instruction_pointer + OPCODE_SIZE + ARG_SIZE * instruction.args_num);


    printf_yellow("Executing: '%s'\n", instruction.name);

    instruction.executor(processor, instruction);

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


void SPU_execute_O_OUT(SPU* processor, SPUInstruction instr)
{
    assert(instr.args_ptr != NULL);
    assert(instr.args_num == 2);

    uint32_t args[2] = {};
    memcpy(args, instr.args_ptr, 2 * sizeof(args[0]));

    uint32_t interrupt_index = args[0];

    int32_t relative_interrupt_data_ptr = (int32_t) args[1];

    uint32_t abs_interrupt_data_ptr = SPU_get_abs_ptr(processor, relative_interrupt_data_ptr);

    printf_yellow("Executing INT 0x%x 0x%x\n", interrupt_index, relative_interrupt_data_ptr);


    if ((instr.opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)
    {
        interrupt_index = SPU_read_memory_cell(processor, interrupt_index);

        abs_interrupt_data_ptr = SPU_read_memory_cell(processor, abs_interrupt_data_ptr);
    }
    
    if (interrupt_index >= port_out_handlers_number || port_out_handlers[interrupt_index] == NULL)
    {
        return; // UNKNOWN INTERUPT
    }

    port_out_handlers[interrupt_index](processor, abs_interrupt_data_ptr);

    return;
}
void SPU_execute_O_MOV_CONST(SPU* processor, SPUInstruction instr)
{
    uint32_t args[2] = {};
    memcpy(args, instr.args_ptr, 2 * sizeof(args[0]));

    int32_t relative_destination_ptr = (int32_t) args[0];

    uint32_t value = args[1];

    uint32_t abs_destination_ptr = SPU_get_abs_ptr(processor, relative_destination_ptr);


    if ((instr.opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)
    {
        abs_destination_ptr = SPU_read_memory_cell(processor, abs_destination_ptr);
    }

    printf_yellow("MOVC dest=%p, value=%u\n", abs_destination_ptr, value);

    SPU_write_memory_cell(processor, abs_destination_ptr, value);

    return;
}

void SPU_execute_O_LEA(SPU* processor, SPUInstruction instr)
{
    int32_t args[2] = {};
    memcpy(args, instr.args_ptr, 2 * sizeof(args[0]));

    int32_t relative_destination_ptr = args[0];

    int32_t local_ptr_value = args[1];

    uint32_t abs_destination_ptr = SPU_get_abs_ptr(processor, relative_destination_ptr);

    if ((instr.opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)
    {
        abs_destination_ptr = SPU_read_memory_cell(processor, abs_destination_ptr);
    }

    uint32_t abs_ptr_value = SPU_get_abs_ptr(processor, local_ptr_value);


    SPU_write_memory_cell(processor, abs_destination_ptr, abs_ptr_value);

    return;
}


void SPU_execute_O_MOV(SPU* processor, SPUInstruction instr)
{
    assert(instr.args_ptr != NULL);
    assert(instr.args_num == 3);

    int32_t args[3] = {};

    memcpy(args, instr.args_ptr, 3 * sizeof(args[0]));

    int32_t relative_dst_ptr = args[0];
    int32_t relative_src_ptr = args[1];

    int32_t count = args[2];

    uint32_t abs_dst_ptr = SPU_get_abs_ptr(processor, relative_dst_ptr);
    uint32_t abs_src_ptr = SPU_get_abs_ptr(processor, relative_src_ptr);

    printf_yellow("MOV 0x%x 0x%x %d\n", abs_dst_ptr, abs_src_ptr, count);


    if ((instr.opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)
    {
        abs_dst_ptr = SPU_read_memory_cell(processor, abs_dst_ptr);

        abs_src_ptr = SPU_read_memory_cell(processor, abs_src_ptr);

        count = (int32_t) SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, count));
    }
    
    void* real_dst_ptr = SPU_get_real_mem_addr(processor, abs_dst_ptr);
    void* real_src_ptr = SPU_get_real_mem_addr(processor, abs_src_ptr);


    memmove(real_dst_ptr, real_src_ptr, (uint32_t) count);

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
    return;
}
void SPU_execute_O_ANY(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;
}

void SPU_execute_O_CLEA(SPU* processor, SPUInstruction instr)
{
    int32_t args[3] = {};
    memcpy(args, instr.args_ptr, 3 * sizeof(args[0]));

    int32_t flag_ptr = args[0];


    int32_t relative_destination_ptr = args[1];

    int32_t local_ptr_value = args[2];

    uint32_t abs_destination_ptr = SPU_get_abs_ptr(processor, relative_destination_ptr);

    uint32_t flag = SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, flag_ptr));

    if ((instr.opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)
    {
        flag = SPU_read_memory_cell(processor, flag);
        abs_destination_ptr = SPU_read_memory_cell(processor, abs_destination_ptr);
    }
    if (flag)
    {
        uint32_t abs_ptr_value = SPU_get_abs_ptr(processor, local_ptr_value);

        SPU_write_memory_cell(processor, abs_destination_ptr, abs_ptr_value);
    }

    return;
}



void SPU_execute_O_EQ(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;
}
void SPU_execute_O_OR(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;
}
void SPU_execute_O_AND(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;
}
void SPU_execute_O_XOR(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;
}

void SPU_execute_O_ADD(SPU* processor, SPUInstruction instr)
{
    assert(instr.args_ptr != NULL);
    assert(instr.args_num == 4);

    int32_t args[4] = {};
    memcpy(args, instr.args_ptr, 4 * ARG_SIZE);

    int32_t relative_dst_ptr = args[0];

    int32_t relative_A_ptr = args[1];
    int32_t relative_B_ptr = args[2];

    int32_t count = args[3];


    uint32_t abs_dst_ptr = SPU_get_abs_ptr(processor, relative_dst_ptr);
    
    uint32_t abs_A_ptr = SPU_get_abs_ptr(processor, relative_A_ptr);
    uint32_t abs_B_ptr = SPU_get_abs_ptr(processor, relative_B_ptr);


    if ((instr.opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)
    {
        abs_dst_ptr = SPU_read_memory_cell(processor, abs_dst_ptr);

        abs_A_ptr = SPU_read_memory_cell(processor, abs_A_ptr);
        abs_B_ptr = SPU_read_memory_cell(processor, abs_B_ptr);

        count = (int32_t) SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, count));
    }

    uint16_t carry = 0;
    uint8_t a = 0, b = 0;

    for (uint32_t i = 0; i < (uint32_t)count; ++i)
    {
        SPU_read_memory(processor, &a, abs_A_ptr + i, 1);
        SPU_read_memory(processor, &b, abs_B_ptr + i, 1);;
        carry = (uint16_t)(a + b + carry);
        
        SPU_write_memory(processor, abs_dst_ptr + i, &carry, 1);
        carry = carry / 256;
    }

    return;
}
void SPU_execute_O_SUB(SPU* processor, SPUInstruction instr)
{
    assert(instr.args_ptr != NULL);
    assert(instr.args_num == 4);

    int32_t args[4] = {};
    memcpy(args, instr.args_ptr, 4 * ARG_SIZE);

    int32_t relative_dst_ptr = args[0];

    int32_t relative_A_ptr = args[1];
    int32_t relative_B_ptr = args[2];

    int32_t count = args[3];


    uint32_t abs_dst_ptr = SPU_get_abs_ptr(processor, relative_dst_ptr);
    
    uint32_t abs_A_ptr = SPU_get_abs_ptr(processor, relative_A_ptr);
    uint32_t abs_B_ptr = SPU_get_abs_ptr(processor, relative_B_ptr);


    if ((instr.opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)
    {
        abs_dst_ptr = SPU_read_memory_cell(processor, abs_dst_ptr);

        abs_A_ptr = SPU_read_memory_cell(processor, abs_A_ptr);
        abs_B_ptr = SPU_read_memory_cell(processor, abs_B_ptr);

        count = (int32_t) SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, count));
    }

    uint8_t carry = 0;
    int32_t result = 0;
    uint8_t a = 0, b = 0;

    for (uint32_t i = 0; i < (uint32_t)count; ++i)
    {
        SPU_read_memory(processor, &a, abs_A_ptr + i, 1);
        SPU_read_memory(processor, &b, abs_B_ptr + i, 1);;
        result = a - b - carry;
        
        SPU_write_memory(processor, abs_dst_ptr + i, &result, 1);
        carry = result >= 0 ? 0 : 1;
    }

    return;
}
void SPU_execute_O_MUL(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;
}
void SPU_execute_O_DIV(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;
}

void SPU_execute_O_CMOV(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;
}

void SPU_execute_O_LT(SPU* processor, SPUInstruction instr)
{
    assert(instr.args_ptr != NULL);
    assert(instr.args_num == 4);

    int32_t args[4] = {};
    memcpy(args, instr.args_ptr, 4 * ARG_SIZE);

    int32_t relative_flag_ptr = args[0];

    int32_t relative_A_ptr = args[1];
    int32_t relative_B_ptr = args[2];

    uint32_t count = (uint32_t) args[3];


    uint32_t abs_flag_ptr = SPU_get_abs_ptr(processor, relative_flag_ptr);
    
    uint32_t abs_A_ptr = SPU_get_abs_ptr(processor, relative_A_ptr);
    uint32_t abs_B_ptr = SPU_get_abs_ptr(processor, relative_B_ptr);


    if ((instr.opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)
    {
        abs_flag_ptr = SPU_read_memory_cell(processor, abs_flag_ptr);

        abs_A_ptr = SPU_read_memory_cell(processor, abs_A_ptr);
        abs_B_ptr = SPU_read_memory_cell(processor, abs_B_ptr);

        count = SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, (int32_t)count));
    }

    uint8_t a = 0, b = 0;

    uint8_t Ba = 0, Bb = 0;

    for (uint32_t i = 0; i < (uint32_t)count; ++i)
    {
        SPU_read_memory(processor, &a, abs_A_ptr + count - 1 - i, 1);
        SPU_read_memory(processor, &b, abs_B_ptr + count - 1 - i, 1);;
        
        if (i == 0)
        {
            Ba = (a & 0b10000000) >> 7;
            Bb = (b & 0b10000000) >> 7;
        }

        if (a < b)
        {
            SPU_write_memory_cell(processor, abs_flag_ptr, Ba == Bb);
            return;
        }
        else if (a > b)
        {
            SPU_write_memory_cell(processor, abs_flag_ptr, Ba != Bb);
            return;
        }
    }
    

    SPU_write_memory_cell(processor, abs_flag_ptr, 0);
    return;
}

void SPU_execute_O_CALL(SPU* processor, SPUInstruction instr)
{
    UNUSED(processor);
    UNUSED(instr);
    return;
}


void SPU_handle_print_DWORD_number(SPU* processor, uint32_t data_ptr)
{
    int32_t number = (int32_t) SPU_read_memory_cell(processor, data_ptr);

    printf_blinking("SPU_OUTPUT: '%d'\n", number);

    return;
}

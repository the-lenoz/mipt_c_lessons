#include "SPU.hpp"
#include "debug_utilites/debug_utilites.hpp"
#include "error_handling/my_assert.hpp"
#include "AVXXX_N.hpp"
#include "status.hpp"
#include "terminal_decorator.hpp"
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <cstdint>
#include <cstdlib>
#include <stdint.h>
#include <string.h>
#include <stdint.h>



static SDL_Window* VGA_window = NULL;


static inline SPUInstruction parse_instruction(void* instruction_ptr);


static void SPU_run_loop(SPU* processor);


static inline int SPU_is_valid_memaddr(SPU* processor, uint32_t virtual_address);

static inline int SPU_is_valid_mem_array(SPU* processor, uint32_t virtual_array_address, uint32_t array_size);


static inline void* SPU_get_real_mem_addr(SPU* processor, uint32_t virtual_addr);

static inline uint32_t SPU_read_memory_cell(SPU* processor, uint32_t virtual_addr);
static inline void SPU_write_memory_cell(SPU* processor, uint32_t virtual_addr, uint32_t value);



static inline void SPU_read_memory(SPU *processor, void* real_dst, uint32_t virtual_src, size_t count);

static inline uint32_t SPU_get_abs_ptr(SPU* processor, int32_t virtual_relative_ptr);


static inline void SPU_execute_instruction(SPU* processor, uint32_t virtual_instruction_ptr);



static inline SPUInstruction parse_instruction(void* instruction_ptr)
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

            if (processor->debug_mode)
            {
                SPU_dump(processor);
                getchar();
            }
        }
        else
        {
            processor->is_running = 0;
        }       
    }

    printf_green("Run completed\n");
}


static inline int SPU_is_valid_memaddr(SPU* processor, uint32_t virtual_address)
{
    return virtual_address < processor->memory_size;
}

static inline int SPU_is_valid_mem_array(SPU* processor, uint32_t virtual_array_address, uint32_t array_size)
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
        printf_green("ADDR: %#8x (%d)\t| %s |\n", dump_ptr, dump_ptr, mem_dump_row_str);
    }
    printf_yellow("\n=== RAM_DUMP_END ===\n");
}



static inline void* SPU_get_real_mem_addr(SPU* processor, uint32_t virtual_addr)
{
    assert(processor != NULL);
    assert(processor->memory_size != 0);
    assert(processor->memory != NULL);

    assert(virtual_addr < processor->memory_size);

    return (char*)(processor->memory) + virtual_addr;

}

static inline uint32_t SPU_read_memory_cell(SPU* processor, uint32_t virtual_addr)
{
    assert(processor != NULL);
    assert(processor->memory_size != 0);
    assert(processor->memory != NULL);

    assert(virtual_addr < processor->memory_size);

    uint32_t value = 0;

    SPU_read_memory(processor, &value, virtual_addr, sizeof(value));

    //printf_yellow("Reading memory cell: [%u] = %u\n", virtual_addr, value);


    return value;
}

static inline void SPU_write_memory_cell(SPU* processor, uint32_t virtual_addr, uint32_t value)
{
    assert(processor != NULL);
    assert(processor->memory_size != 0);
    assert(processor->memory != NULL);

    assert(virtual_addr < processor->memory_size);

    if (processor->debug_mode) printf_yellow("Writing memory cell: [%u] <- %u\n", virtual_addr, value);

    SPU_write_memory(processor, virtual_addr, &value, sizeof(value));
}

void SPU_write_memory(SPU *processor, uint32_t virtual_dst, void* real_src, size_t count)
{
    assert(processor != NULL);
    assert(processor->memory_size != 0);
    assert(processor->memory != NULL);

    void* real_dst = SPU_get_real_mem_addr(processor, virtual_dst);

    if (processor->debug_mode) printf_yellow("Writing memory: %zu bytes to 0x%x\n", count, virtual_dst);

    memcpy(real_dst, real_src, count);
}

static inline void SPU_read_memory(SPU *processor, void* real_dst, uint32_t virtual_src, size_t count)
{
    assert(processor != NULL);
    assert(processor->memory_size != 0);
    assert(processor->memory != NULL);

    void* real_src = SPU_get_real_mem_addr(processor, virtual_src);

    //printf_yellow("Reading memory: %zu bytes from 0x%x\n", count, virtual_src);

    memcpy(real_dst, real_src, count);
}

static inline uint32_t SPU_get_abs_ptr(SPU* processor, int32_t virtual_relative_ptr)
{
    //uint32_t offset = SPU_read_memory_cell(processor, INSTRUCTION_POINTER_ADDR);

    return (uint32_t)(virtual_relative_ptr + (int32_t)processor->current_instruction_pointer);
}


static inline void SPU_execute_instruction(SPU* processor, uint32_t virtual_instruction_ptr)
{
    assert(processor != NULL);
    assert(processor->memory_size != 0);
    assert(processor->memory != NULL);

    void* real_instruction_ptr = SPU_get_real_mem_addr(processor, virtual_instruction_ptr);

    uint8_t opcode = *(uint8_t*) real_instruction_ptr;
    uint32_t* args = (uint32_t*) ((uint8_t*)real_instruction_ptr + OPCODE_SIZE);


    uint32_t args_num = 0;
    switch (opcode & ARG_NUM_OPCODE_MASK) {
        case ARG_NUM_0: args_num = 0; break;
        case ARG_NUM_2: args_num = 2; break;
        case ARG_NUM_3: args_num = 3; break;
        case ARG_NUM_4: args_num = 4; break;
        default: break;
    }

    uint32_t current_instruction_pointer = SPU_read_memory_cell(processor, INSTRUCTION_POINTER_ADDR);

    processor->current_instruction_pointer = current_instruction_pointer;

    SPU_write_memory_cell(processor, INSTRUCTION_POINTER_ADDR, current_instruction_pointer + OPCODE_SIZE + ARG_SIZE * args_num);

    if (processor->debug_mode)
    {
        SPUInstruction instruction = parse_instruction(real_instruction_ptr);
        printf_yellow("DEBUG: executing %s", instruction.name);
        for (uint i = 0; i < instruction.args_num; ++i)
            printf_yellow(" %d", args[i]);
        printf_yellow("\nARG_VALUES: int32\n");
        for (uint i = 0; i < instruction.args_num; ++i)
            printf_yellow("[%d] = %d, ", i, (int32_t)SPU_read_memory_cell(processor, args[i]));
        printf("\n");
    }

    #define INSTRUCTION(instr_name, instr_opcode, args_number, executor) case instr_opcode: executor break;
    switch (opcode & (uint8_t)~(uint8_t)ARG_TYPE_OPCODE_MASK) {
    case (0b00000 | 0b00000000): {
        SPU_execute_O_NOP(processor, {.opcode=opcode, .args_ptr=(int8_t*)real_instruction_ptr + OPCODE_SIZE});
    } break;
    case (0b00001 | 0b00000000): {
      processor->is_running = 0;
    } break;
    case (0b00000 | 0b00100000): {
        SPU_execute_O_MOV_CONST(processor, {.opcode=opcode, .args_ptr=(int8_t*)real_instruction_ptr + OPCODE_SIZE});
    } break;
    case (0b00001 | 0b00100000): {
        SPU_execute_O_LEA(processor, {.opcode=opcode, .args_ptr=(int8_t*)real_instruction_ptr + OPCODE_SIZE});
    } break;
    case (0b00000 | 0b01000000): {
        SPU_execute_O_MOV(processor, {.opcode=opcode, .args_ptr=(int8_t*)real_instruction_ptr + OPCODE_SIZE});
    } break;
    case (0b00001 | 0b01000000): {
    } break;
    case (0b00010 | 0b01000000): {
    } break;
    case (0b00011 | 0b01000000): {
    } break;
    case (0b00100 | 0b01000000): {
    } break;
    case (0b00101 | 0b01000000): {
    } break;
    case (0b00110 | 0b01000000): {
    } break;
    case (0b00111 | 0b01000000): {
        SPU_execute_O_CLEA(processor, {.opcode=opcode, .args_ptr=(int8_t*)real_instruction_ptr + OPCODE_SIZE});
    } break;
    case (0b01000 | 0b01000000): {
        SPU_execute_O_OUT(processor, {.opcode=opcode, .args_ptr=(int8_t*)real_instruction_ptr + OPCODE_SIZE});
    } break;
    case (0b01001 | 0b01000000): {
        SPU_execute_O_IN(processor, {.opcode=opcode, .args_ptr=(int8_t*)real_instruction_ptr + OPCODE_SIZE});
    } break;
    case (0b00000 | 0b01100000): {
    } break;
    case (0b00001 | 0b01100000): {
    } break;
    case (0b00010 | 0b01100000): {
    } break;
    case (0b00011 | 0b01100000): {
    } break;
    case (0b00100 | 0b01100000): {
        SPU_execute_O_ADD(processor, {.opcode=opcode, .args_ptr=(int8_t*)real_instruction_ptr + OPCODE_SIZE});
    } break;
    case (0b00101 | 0b01100000): {
        SPU_execute_O_SUB(processor, {.opcode=opcode, .args_ptr=(int8_t*)real_instruction_ptr + OPCODE_SIZE});
    } break;
    case (0b00110 | 0b01100000): {
        SPU_execute_O_MUL(processor, {.opcode=opcode, .args_ptr=(int8_t*)real_instruction_ptr + OPCODE_SIZE});
    } break;
    case (0b00111 | 0b01100000): {
        SPU_execute_O_DIV(processor, {.opcode=opcode, .args_ptr=(int8_t*)real_instruction_ptr + OPCODE_SIZE});
    } break;
    case (0b01000 | 0b01100000): {
        SPU_execute_O_CMOV(processor, {.opcode=opcode, .args_ptr=(int8_t*)real_instruction_ptr + OPCODE_SIZE});
    } break;
    case (0b01001 | 0b01100000): {
        SPU_execute_O_LT(processor, {.opcode=opcode, .args_ptr=(int8_t*)real_instruction_ptr + OPCODE_SIZE});
    } break;
        default: break;
    }
    #undef INSTRUCTION

    if (processor->debug_mode)
    {
        SPUInstruction instruction = parse_instruction(real_instruction_ptr);
        printf_yellow("\nDEBUG: ARG_VALUES (after execution): int32\n");
        for (uint i = 0; i < instruction.args_num; ++i)
            printf_yellow("[%d] = %d, ", i, (int32_t)SPU_read_memory_cell(processor, args[i]));
        printf("\n");
    }
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


void SPU_execute_O_MOV_CONST(SPU* processor, SPUInstruction instr)
{
    uint32_t* args = (uint32_t*)instr.args_ptr;

    int32_t relative_destination_ptr = (int32_t) args[0];

    uint32_t value = args[1];

    uint32_t abs_destination_ptr = SPU_get_abs_ptr(processor, relative_destination_ptr);


    if ((instr.opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)
    {
        abs_destination_ptr = SPU_read_memory_cell(processor, abs_destination_ptr);
    }

    SPU_write_memory_cell(processor, abs_destination_ptr, value);

    return;
}

void SPU_execute_O_LEA(SPU* processor, SPUInstruction instr)
{
    int32_t* args = (int32_t*) instr.args_ptr;

    int32_t relative_destination_ptr = args[0];

    int32_t local_ptr_value = args[1];

    uint32_t abs_destination_ptr = SPU_get_abs_ptr(processor, relative_destination_ptr);

    //printf_yellow("LEA dest=%p, value=%u - ", abs_destination_ptr, local_ptr_value);

    if ((instr.opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)
    {
        //printf_yellow("$");
        abs_destination_ptr = SPU_read_memory_cell(processor, abs_destination_ptr);
    }
    //printf_yellow("\n");

    uint32_t abs_ptr_value = SPU_get_abs_ptr(processor, local_ptr_value);


    SPU_write_memory_cell(processor, abs_destination_ptr, abs_ptr_value);

    return;
}


void SPU_execute_O_MOV(SPU* processor, SPUInstruction instr)
{
    assert(instr.args_ptr != NULL);

    int32_t* args = (int32_t*) instr.args_ptr;

    int32_t relative_dst_ptr = args[0];
    int32_t relative_src_ptr = args[1];

    int32_t count = args[2];

    uint32_t abs_dst_ptr = SPU_get_abs_ptr(processor, relative_dst_ptr);
    uint32_t abs_src_ptr = SPU_get_abs_ptr(processor, relative_src_ptr);


    if ((instr.opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)
    {
        abs_dst_ptr = SPU_read_memory_cell(processor, abs_dst_ptr);

        abs_src_ptr = SPU_read_memory_cell(processor, abs_src_ptr);

        count = (int32_t) SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, count));
    }
    
    void* real_dst_ptr = SPU_get_real_mem_addr(processor, abs_dst_ptr);
    void* real_src_ptr = SPU_get_real_mem_addr(processor, abs_src_ptr);

    //printf_yellow("MOV %d -> %d - %d bytes\n", abs_src_ptr, abs_dst_ptr, count);

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
    int32_t* args = (int32_t*) instr.args_ptr;

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

void SPU_execute_O_OUT(SPU* processor, SPUInstruction instr)
{
    assert(instr.args_ptr != NULL);

    int32_t* args = (int32_t*) instr.args_ptr;

    int32_t port_number = args[0];
    int32_t relative_data_ptr = args[1];
    int32_t count = args[2];


    uint32_t abs_data_ptr = SPU_get_abs_ptr(processor, relative_data_ptr);

    if ((instr.opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)
    {
        port_number = (int32_t)SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, port_number));
        abs_data_ptr = SPU_read_memory_cell(processor, abs_data_ptr);
        count = (int32_t) SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, count));
    }

    if ((uint32_t)port_number >= port_out_handlers_number || port_out_handlers[port_number] == NULL)
    {
        return; // UNKNOWN PORT
    }

    port_out_handlers[port_number](processor, abs_data_ptr, (uint32_t)count);

    return;
}

void SPU_execute_O_IN(SPU* processor, SPUInstruction instr)
{
    assert(instr.args_ptr != NULL);

    int32_t* args = (int32_t*) instr.args_ptr;
    
    uint32_t port_number = (uint32_t)args[0];
    int32_t relative_data_ptr = args[1];
    int32_t count = args[2];

    uint32_t abs_data_ptr = SPU_get_abs_ptr(processor, relative_data_ptr);

    if ((instr.opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)
    {
        port_number = SPU_read_memory_cell(processor, port_number);
        abs_data_ptr = SPU_read_memory_cell(processor, abs_data_ptr);
        count = (int32_t) SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, count));
    }
    
    if (port_number >= port_in_handlers_number || port_in_handlers[port_number] == NULL)
    {
        return; /* UNKNOWN PORT */
    }

    port_out_handlers[port_number](processor, abs_data_ptr, (uint32_t)count);

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

    int32_t* args = (int32_t*) instr.args_ptr;

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

    if (SPU_is_valid_mem_array(processor, abs_A_ptr, (uint32_t)count) &&
        SPU_is_valid_mem_array(processor, abs_B_ptr, (uint32_t)count) &&
        SPU_is_valid_mem_array(processor, abs_dst_ptr, (uint32_t)count))
    {
        int8_t a_byte = 0, b_byte = 0, c_byte = 0;
        int16_t a_word = 0, b_word = 0, c_word = 0;
        int32_t a_dword = 0, b_dword = 0, c_dword = 0;
        int64_t a_qword = 0, b_qword = 0, c_qword = 0;
        switch (count) {
            case sizeof(int8_t):
                SPU_read_memory(processor, &a_byte, abs_A_ptr, sizeof(a_byte));
                SPU_read_memory(processor, &b_byte, abs_B_ptr, sizeof(b_byte));

                c_byte = a_byte + b_byte;

                SPU_write_memory(processor, abs_dst_ptr, &c_byte, sizeof(c_byte));
                break;
            case sizeof(int16_t):
                SPU_read_memory(processor, &a_word, abs_A_ptr, sizeof(a_word));
                SPU_read_memory(processor, &b_word, abs_B_ptr, sizeof(b_word));

                c_word = a_word + b_word;

                SPU_write_memory(processor, abs_dst_ptr, &c_word, sizeof(c_word));
                break;
            case sizeof(int32_t):
                SPU_read_memory(processor, &a_dword, abs_A_ptr, sizeof(a_dword));
                SPU_read_memory(processor, &b_dword, abs_B_ptr, sizeof(b_dword));

                c_dword = a_dword + b_dword;

                SPU_write_memory(processor, abs_dst_ptr, &c_dword, sizeof(c_dword));
                break;
            case sizeof(int64_t):
                SPU_read_memory(processor, &a_qword, abs_A_ptr, sizeof(a_qword));
                SPU_read_memory(processor, &b_qword, abs_B_ptr, sizeof(b_qword));

                c_qword = a_qword + b_qword;

                SPU_write_memory(processor, abs_dst_ptr, &c_qword, sizeof(c_qword));
                break;
            default:
                long_add(
                        (int8_t*)SPU_get_real_mem_addr(processor, abs_A_ptr), 
                        (int8_t*)SPU_get_real_mem_addr(processor, abs_B_ptr), 
                        (int8_t*)SPU_get_real_mem_addr(processor, abs_dst_ptr), (uint32_t)count);
                break;
        }
    }

    return;
}
void SPU_execute_O_SUB(SPU* processor, SPUInstruction instr)
{
    assert(instr.args_ptr != NULL);

    int32_t* args = (int32_t*) instr.args_ptr;

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

    if (SPU_is_valid_mem_array(processor, abs_A_ptr, (uint32_t)count) &&
        SPU_is_valid_mem_array(processor, abs_B_ptr, (uint32_t)count) &&
        SPU_is_valid_mem_array(processor, abs_dst_ptr, (uint32_t)count))
    {
        int8_t a_byte = 0, b_byte = 0, c_byte = 0;
        int16_t a_word = 0, b_word = 0, c_word = 0;
        int32_t a_dword = 0, b_dword = 0, c_dword = 0;
        int64_t a_qword = 0, b_qword = 0, c_qword = 0;
        switch (count) {
            case sizeof(int8_t):
                SPU_read_memory(processor, &a_byte, abs_A_ptr, sizeof(a_byte));
                SPU_read_memory(processor, &b_byte, abs_B_ptr, sizeof(b_byte));

                c_byte = a_byte - b_byte;

                SPU_write_memory(processor, abs_dst_ptr, &c_byte, sizeof(c_byte));
                break;
            case sizeof(int16_t):
                SPU_read_memory(processor, &a_word, abs_A_ptr, sizeof(a_word));
                SPU_read_memory(processor, &b_word, abs_B_ptr, sizeof(b_word));

                c_word = a_word - b_word;

                SPU_write_memory(processor, abs_dst_ptr, &c_word, sizeof(c_word));
                break;
            case sizeof(int32_t):
                SPU_read_memory(processor, &a_dword, abs_A_ptr, sizeof(a_dword));
                SPU_read_memory(processor, &b_dword, abs_B_ptr, sizeof(b_dword));

                c_dword = a_dword - b_dword;

                SPU_write_memory(processor, abs_dst_ptr, &c_dword, sizeof(c_dword));
                break;
            case sizeof(int64_t):
                SPU_read_memory(processor, &a_qword, abs_A_ptr, sizeof(a_qword));
                SPU_read_memory(processor, &b_qword, abs_B_ptr, sizeof(b_qword));

                c_qword = a_qword - b_qword;

                SPU_write_memory(processor, abs_dst_ptr, &c_qword, sizeof(c_qword));
                break;
            default:
                long_sub(
                        (int8_t*)SPU_get_real_mem_addr(processor, abs_A_ptr), 
                        (int8_t*)SPU_get_real_mem_addr(processor, abs_B_ptr), 
                        (int8_t*)SPU_get_real_mem_addr(processor, abs_dst_ptr), (uint32_t)count);
                break;
        }
    }

    return;
}
void SPU_execute_O_MUL(SPU* processor, SPUInstruction instr)
{
    assert(instr.args_ptr != NULL);

    int32_t* args = (int32_t*)instr.args_ptr;

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

    if (SPU_is_valid_mem_array(processor, abs_A_ptr, (uint32_t)count) &&
        SPU_is_valid_mem_array(processor, abs_B_ptr, (uint32_t)count) &&
        SPU_is_valid_mem_array(processor, abs_dst_ptr, (uint32_t)count))
    {
        int8_t a_byte = 0, b_byte = 0, c_byte = 0;
        int16_t a_word = 0, b_word = 0, c_word = 0;
        int32_t a_dword = 0, b_dword = 0, c_dword = 0;
        int64_t a_qword = 0, b_qword = 0, c_qword = 0;
        switch (count) {
            case sizeof(int8_t):
                SPU_read_memory(processor, &a_byte, abs_A_ptr, sizeof(a_byte));
                SPU_read_memory(processor, &b_byte, abs_B_ptr, sizeof(b_byte));

                c_byte = a_byte * b_byte;

                SPU_write_memory(processor, abs_dst_ptr, &c_byte, sizeof(c_byte));
                break;
            case sizeof(int16_t):
                SPU_read_memory(processor, &a_word, abs_A_ptr, sizeof(a_word));
                SPU_read_memory(processor, &b_word, abs_B_ptr, sizeof(b_word));

                c_word = a_word * b_word;

                SPU_write_memory(processor, abs_dst_ptr, &c_word, sizeof(c_word));
                break;
            case sizeof(int32_t):
                SPU_read_memory(processor, &a_dword, abs_A_ptr, sizeof(a_dword));
                SPU_read_memory(processor, &b_dword, abs_B_ptr, sizeof(b_dword));

                c_dword = a_dword * b_dword;

                SPU_write_memory(processor, abs_dst_ptr, &c_dword, sizeof(c_dword));
                break;
            case sizeof(int64_t):
                SPU_read_memory(processor, &a_qword, abs_A_ptr, sizeof(a_qword));
                SPU_read_memory(processor, &b_qword, abs_B_ptr, sizeof(b_qword));

                c_qword = a_qword * b_qword;

                SPU_write_memory(processor, abs_dst_ptr, &c_qword, sizeof(c_qword));
                break;
            default:
                long_mul(
                        (int8_t*)SPU_get_real_mem_addr(processor, abs_A_ptr), 
                        (int8_t*)SPU_get_real_mem_addr(processor, abs_B_ptr), 
                        (int8_t*)SPU_get_real_mem_addr(processor, abs_dst_ptr), (uint32_t)count);
                break;
        }
    }

    return;
}
void SPU_execute_O_DIV(SPU* processor, SPUInstruction instr)
{
    assert(instr.args_ptr != NULL);

    int32_t* args = (int32_t*)instr.args_ptr;

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

    if (SPU_is_valid_mem_array(processor, abs_A_ptr, (uint32_t)count) &&
        SPU_is_valid_mem_array(processor, abs_B_ptr, (uint32_t)count) &&
        SPU_is_valid_mem_array(processor, abs_dst_ptr, (uint32_t)count))
    {
        int8_t a_byte = 0, b_byte = 0, c_byte = 0;
        int16_t a_word = 0, b_word = 0, c_word = 0;
        int32_t a_dword = 0, b_dword = 0, c_dword = 0;
        int64_t a_qword = 0, b_qword = 0, c_qword = 0;
        switch (count) {
            case sizeof(int8_t):
                SPU_read_memory(processor, &a_byte, abs_A_ptr, sizeof(a_byte));
                SPU_read_memory(processor, &b_byte, abs_B_ptr, sizeof(b_byte));

                c_byte = a_byte / b_byte;

                SPU_write_memory(processor, abs_dst_ptr, &c_byte, sizeof(c_byte));
                break;
            case sizeof(int16_t):
                SPU_read_memory(processor, &a_word, abs_A_ptr, sizeof(a_word));
                SPU_read_memory(processor, &b_word, abs_B_ptr, sizeof(b_word));

                c_word = a_word / b_word;

                SPU_write_memory(processor, abs_dst_ptr, &c_word, sizeof(c_word));
                break;
            case sizeof(int32_t):
                SPU_read_memory(processor, &a_dword, abs_A_ptr, sizeof(a_dword));
                SPU_read_memory(processor, &b_dword, abs_B_ptr, sizeof(b_dword));

                c_dword = a_dword / b_dword;

                SPU_write_memory(processor, abs_dst_ptr, &c_dword, sizeof(c_dword));
                break;
            case sizeof(int64_t):
                SPU_read_memory(processor, &a_qword, abs_A_ptr, sizeof(a_qword));
                SPU_read_memory(processor, &b_qword, abs_B_ptr, sizeof(b_qword));

                c_qword = a_qword / b_qword;

                SPU_write_memory(processor, abs_dst_ptr, &c_qword, sizeof(c_qword));
                break;
            default:
                long_div(
                        (int8_t*)SPU_get_real_mem_addr(processor, abs_A_ptr), 
                        (int8_t*)SPU_get_real_mem_addr(processor, abs_B_ptr), 
                        (int8_t*)SPU_get_real_mem_addr(processor, abs_dst_ptr), (uint32_t)count);
                break;
        }
    }

    return;
}

void SPU_execute_O_CMOV(SPU* processor, SPUInstruction instr)
{
    int32_t* args = (int32_t*) instr.args_ptr;

    int32_t flag_ptr = args[0];
    int32_t relative_dst_ptr = args[1];
    int32_t relative_src_ptr = args[2];
    int32_t count = args[3];

    uint32_t abs_dst_ptr = SPU_get_abs_ptr(processor, relative_dst_ptr);
    uint32_t abs_src_ptr = SPU_get_abs_ptr(processor, relative_src_ptr);
    uint32_t flag = SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, flag_ptr));

    if ((instr.opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)
    {
        flag = SPU_read_memory_cell(processor, flag);
        abs_dst_ptr = SPU_read_memory_cell(processor, abs_dst_ptr);
        abs_src_ptr = SPU_read_memory_cell(processor, abs_src_ptr);
        count = (int32_t) SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, count));
    }
    
    void* real_dst_ptr = SPU_get_real_mem_addr(processor, abs_dst_ptr);
    void* real_src_ptr = SPU_get_real_mem_addr(processor, abs_src_ptr);

    if (flag)
    {
        memmove(real_dst_ptr, real_src_ptr, (uint32_t) count);
    }

    return;
}

void SPU_execute_O_LT(SPU* processor, SPUInstruction instr)
{
    assert(instr.args_ptr != NULL);

    int32_t* args = (int32_t*) instr.args_ptr;

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

void SPU_handle_out_text_VGA(SPU* processor, uint32_t data_ptr, uint32_t count)
{
    assert(processor != NULL);

    //printf_yellow("VGA_OUT - data_ptr: %d, count: %d\n", data_ptr, count);

    if (!SPU_is_valid_mem_array(processor, data_ptr, count)) return;

    int8_t c = 0;

    putc('\n', stdout);
    for(uint32_t i = 0; i < text_VGA_screen_height; ++i)
    {
        for (uint32_t j = 0; j < text_VGA_screen_width; ++j)
        {
            if (i * text_VGA_screen_width + j < count)
            {
                SPU_read_memory(processor, &c, data_ptr + i * text_VGA_screen_width + j, 1);
                if (c == '\0') c = '0';
                putc(c, stdout);
                putc(c, stdout);
            }
            else
            {
                putc('_', stdout);
                putc('_', stdout);
            }
        }
        putc('\n', stdout);
    }
}

void SPU_handle_out_VGA(SPU* processor, uint32_t data_ptr, uint32_t count)
{
    assert(processor != NULL);

    //printf_yellow("out_VGA: data=%d, count=%d\n");


    if (!SPU_is_valid_mem_array(processor, data_ptr, count)) return;

    if (VGA_window == NULL)
    {
        //printf("Creating SDL window\n");
        VGA_window = SDL_CreateWindow("SPU VGA", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, VGA_screen_view_width, VGA_screen_view_height, SDL_WINDOW_SHOWN);
        //printf("Created SDL window - %p\n", (void*)VGA_window);
        //printf("%s\n", SDL_GetError());
    }

    SDL_Surface* window_surface = SDL_GetWindowSurface(VGA_window);
    SDL_ShowWindow(VGA_window);
    SDL_LockSurface(window_surface);
    
    uint8_t* pixels_arr = (uint8_t*) (window_surface->pixels);

    uint32_t pixel = 0;
    for (int y = 0; y < VGA_screen_true_height; ++y)
    {
        for (int x = 0; x < VGA_screen_true_width; ++x)
        {
            pixel = SPU_read_memory_cell(processor, 
                data_ptr + (uint32_t)y * sizeof(uint32_t) * VGA_screen_true_width + (uint32_t)x * sizeof(uint32_t));
            for (int i = 0; i < VGA_screen_view_height / VGA_screen_true_height; ++i)
            {
                for (int j = 0; j < VGA_screen_view_width / VGA_screen_true_width; ++j)
                {
                    *(uint32_t*)(pixels_arr + (y * VGA_screen_view_height / VGA_screen_true_height + i) 
                                                * window_surface->pitch
                                            + ((uint32_t)x * VGA_screen_view_width / VGA_screen_true_width + j) * sizeof(uint32_t)) = pixel;
                }
            }
        }
        
    }

    SDL_UnlockSurface(window_surface);
    SDL_UpdateWindowSurface(VGA_window);
}

void SPU_handle_out_port_printf(SPU* processor, uint32_t data_ptr, uint32_t count)
{
    if (count != 4)
    {
        return;
    }

    int32_t number = (int32_t) SPU_read_memory_cell(processor, data_ptr);

    printf_blinking("SPU_OUTPUT: '%d'\n", number);

    return;
}

#ifndef SPU_DECLARED
#define SPU_DECLARED

#include "status.hpp"
#include <cstdint>
#include <stdlib.h>
#include <sys/types.h>


#define UNUSED(x) (void)x


#define DUMP_ROW_SIZE   32

/*******************************************************
//! Argument size == sizeof(size_t) ==  DWORD
//!
//! 1 bit for arguments type:
//! 0 - all types are AS IS
//! 1 - all types are POINTERS (ABSOLUTE)- e. g. 
//!     size_t -> size_t*; size_t* -> size_t** 
//!
//! 2 bits for arguments number
//! 00 - 0
//! 01 - 2
//! 10 - 3
//! 11 - 4
//!
//! 5 bits for opcode
********************************************************/

#define OPCODE_SIZE 1
#define ARG_SIZE    4
#define MAX_ARGS_NUMBER 4

#define ARG_TYPE_OPCODE_MASK 0b10000000
#define ARG_TYPE_ASIS        0b00000000
#define ARG_TYPE_PTR         0b10000000

#define ARG_NUM_OPCODE_MASK 0b01100000
#define ARG_NUM_0           0b00000000
//DEPRECATED BEFORE IMPLEMENTATION #define ARG_NUM_1           0b00100000
#define ARG_NUM_2           0b00100000
#define ARG_NUM_3           0b01000000
#define ARG_NUM_4           0b01100000


//! 
#define O_NOP           (0b00000 | ARG_NUM_0)
#define O_HLT           (0b00001 | ARG_NUM_0)


//! (DST: void*) (VAL: size_t)      IMPORTANT: ARG_TYPE_PTR_MODIFIER AFFECTS ONLY DST
#define O_MOV_CONST     (0b00000 | ARG_NUM_2)

//! (DST: void*) (VAL: void*)      IMPORTANT: ARG_TYPE_PTR_MODIFIER AFFECTS ONLY DST
#define O_LEA           (0b00001 | ARG_NUM_2)


//! (DST: void*) (SRC: void*) (COUNT: size_t*) 
//! DST MAY INTERSECT WITH SRC
#define O_MOV           (0b00000 | ARG_NUM_3)

//! (DST: void*) (SRC: void*) (COUNT: size_t*) 
//! DST MAY BE SRC.
#define O_INV           (0b00001 | ARG_NUM_3)

//! (DST: void*) (SRC: void*) (COUNT: size_t*) 
//! DST MAY BE SRC.
#define O_NEG           (0b00010 | ARG_NUM_3)

//! (DST: void*) (SRC: void*) (COUNT: size_t*) 
//! DST MAY BE SRC.
#define O_INC           (0b00011 | ARG_NUM_3)
#define O_DEC           (0b00100 | ARG_NUM_3)

//! (DST: size_t*) (SRC: void*) (COUNT: size_t*)
#define O_ALL           (0b00101 | ARG_NUM_3)
#define O_ANY           (0b00110 | ARG_NUM_3)

//! (FLAG: size_t*) (DST: void*) (VAL: void*)      IMPORTANT: ARG_TYPE_PTR_MODIFIER AFFECTS ONLY DST AND FLAG
#define O_CLEA           (0b00111 | ARG_NUM_3)

//! (INDEX: size_t) (SRC: void*) (COUNT: size_t*)
#define O_OUT           (0b01000 | ARG_NUM_3)

//! (INDEX: size_t) (DST: void*) (COUNT: size_t*)
#define O_IN           (0b01001 | ARG_NUM_3)


//! (DST: void*) (A: void*) (B: void*) (COUNT: size_t*) 
#define O_EQ            (0b00000 | ARG_NUM_4)
#define O_OR            (0b00001 | ARG_NUM_4)
#define O_AND           (0b00010 | ARG_NUM_4)
#define O_XOR           (0b00011 | ARG_NUM_4)

#define O_ADD           (0b00100 | ARG_NUM_4)
#define O_SUB           (0b00101 | ARG_NUM_4)
#define O_MUL           (0b00110 | ARG_NUM_4)
#define O_DIV           (0b00111 | ARG_NUM_4)


//! (FLAG: size_t*) (DST: void*) (SRC: void*) (COUNT: size_t*) 
#define O_CMOV          (0b01000 | ARG_NUM_4)

//! (FLAG: size_t*) (A: void*) (B: void*) (COUNT: size_t*) 
#define O_LT           (0b01001 | ARG_NUM_4)


#define INSTRUCTION_POINTER_ADDR    0x0000000000000000


#define MAX_INSTRUCTION_SIZE    OPCODE_SIZE + 4 * ARG_SIZE


struct SPU
{
    uint32_t memory_size;
    void* memory;
    uint32_t current_instruction_pointer;
    StatusData last_operation_status;
    int is_running;
    int debug_mode;
};


void SPU_init(SPU* processor, uint32_t RAM_size, int debug_mode = 0);

void SPU_destroy(SPU* processor);


void SPU_start(SPU* processor, uint32_t entrypoint);

StatusData SPU_verify(SPU* processor);

void SPU_dump(SPU* processor);


void SPU_write_memory(SPU *processor, uint32_t virtual_dst, void* real_src, size_t count);


struct SPUInstruction
{
    unsigned char opcode;
    void* args_ptr;
    uint32_t args_num;
    void (*executor) (SPU* processor, SPUInstruction instr);
    const char* name;
};



typedef void (*instruction_executor) (SPU* processor, SPUInstruction instr);


void SPU_execute_O_NOP(SPU* processor, SPUInstruction instr);
void SPU_execute_O_HLT(SPU* processor, SPUInstruction instr);


void SPU_execute_O_MOV_CONST(SPU* processor, SPUInstruction instr);

void SPU_execute_O_LEA(SPU* processor, SPUInstruction instr);


void SPU_execute_O_MOV(SPU* processor, SPUInstruction instr);

void SPU_execute_O_INV(SPU* processor, SPUInstruction instr);

void SPU_execute_O_NEG(SPU* processor, SPUInstruction instr);

void SPU_execute_O_INC(SPU* processor, SPUInstruction instr);
void SPU_execute_O_DEC(SPU* processor, SPUInstruction instr);

void SPU_execute_O_ALL(SPU* processor, SPUInstruction instr);
void SPU_execute_O_ANY(SPU* processor, SPUInstruction instr);

void SPU_execute_O_CLEA(SPU* processor, SPUInstruction instr);

void SPU_execute_O_OUT(SPU* processor, SPUInstruction instr);
void SPU_execute_O_IN(SPU* processor, SPUInstruction instr);


void SPU_execute_O_EQ(SPU* processor, SPUInstruction instr);
void SPU_execute_O_OR(SPU* processor, SPUInstruction instr);
void SPU_execute_O_AND(SPU* processor, SPUInstruction instr);
void SPU_execute_O_XOR(SPU* processor, SPUInstruction instr);

void SPU_execute_O_ADD(SPU* processor, SPUInstruction instr);
void SPU_execute_O_SUB(SPU* processor, SPUInstruction instr);
void SPU_execute_O_MUL(SPU* processor, SPUInstruction instr);
void SPU_execute_O_DIV(SPU* processor, SPUInstruction instr);

void SPU_execute_O_CMOV(SPU* processor, SPUInstruction instr);

void SPU_execute_O_LT(SPU* processor, SPUInstruction instr);

void SPU_execute_O_CALL(SPU* processor, SPUInstruction instr);


const SPUInstruction instructions[] = 
{
    {
        .opcode = O_NOP,
        .args_num = 0,
        .executor = SPU_execute_O_NOP,
        .name = "NOP"
    },
    {
        .opcode = O_HLT,
        .args_num = 0,
        .executor = SPU_execute_O_HLT,
        .name = "HLT"
    },


    {
        .opcode = O_MOV_CONST,
        .args_num = 2,
        .executor = SPU_execute_O_MOV_CONST,
        .name = "MOVC"
    },

    {
        .opcode = O_LEA,
        .args_num = 2,
        .executor = SPU_execute_O_LEA,
        .name = "LEA"
    },

    {
        .opcode = O_MOV,
        .args_num = 3,
        .executor = SPU_execute_O_MOV,
        .name = "MOV"
    },
    {
        .opcode = O_INV,
        .args_num = 3,
        .executor = SPU_execute_O_INV,
        .name = "INV"
    },
    {
        .opcode = O_NEG,
        .args_num = 3,
        .executor = SPU_execute_O_NEG,
        .name = "NEG"
    },
    {
        .opcode = O_INC,
        .args_num = 3,
        .executor = SPU_execute_O_INC,
        .name = "INC"
    },
    {
        .opcode = O_DEC,
        .args_num = 3,
        .executor = SPU_execute_O_DEC,
        .name = "DEC"
    },
    {
        .opcode = O_ALL,
        .args_num = 3,
        .executor = SPU_execute_O_ALL,
        .name = "ALL"
    },
    {
        .opcode = O_ANY,
        .args_num = 3,
        .executor = SPU_execute_O_ANY,
        .name = "ANY"
    },

    {
        .opcode = O_CLEA,
        .args_num = 3,
        .executor = SPU_execute_O_CLEA,
        .name = "CLEA"
    },
    {
        .opcode = O_OUT,
        .args_num = 3,
        .executor = SPU_execute_O_OUT,
        .name = "OUT"
    },
    {
        .opcode = O_IN,
        .args_num = 3,
        .executor = SPU_execute_O_IN,
        .name = "OUT"
    },


    {
        .opcode = O_EQ,
        .args_num = 4,
        .executor = SPU_execute_O_EQ,
        .name = "EQ"
    },
    {
        .opcode = O_AND,
        .args_num = 4,
        .executor = SPU_execute_O_AND,
        .name = "AND"
    },
    {
        .opcode = O_OR,
        .args_num = 4,
        .executor = SPU_execute_O_OR,
        .name = "OR"
    },
    {
        .opcode = O_XOR,
        .args_num = 4,
        .executor = SPU_execute_O_XOR,
        .name = "XOR"
    },

    {
        .opcode = O_ADD,
        .args_num = 4,
        .executor = SPU_execute_O_ADD,
        .name = "ADD"
    },
    {
        .opcode = O_SUB,
        .args_num = 4,
        .executor = SPU_execute_O_SUB,
        .name = "SUB"
    },
    {
        .opcode = O_MUL,
        .args_num = 4,
        .executor = SPU_execute_O_MUL,
        .name = "MUL"
    },
    {
        .opcode = O_DIV,
        .args_num = 4,
        .executor = SPU_execute_O_DIV,
        .name = "DIV"
    },

    {
        .opcode = O_CMOV,
        .args_num = 4,
        .executor = SPU_execute_O_CMOV,
        .name = "CMOV"
    },
    {
        .opcode = O_LT,
        .args_num = 4,
        .executor = SPU_execute_O_LT,
        .name = "LT"
    },
};

const size_t instructions_number = sizeof(instructions) / sizeof(instructions[0]);



typedef void (*port_out_handler) (SPU* processor, uint32_t data_ptr, uint32_t count);

typedef void (*port_in_handler) (SPU* processor, uint32_t dst_ptr, uint32_t count);



const int text_VGA_screen_width = 96;
const int text_VGA_screen_height = 48;
void SPU_handle_out_text_VGA(SPU* processor, uint32_t data_ptr, uint32_t count);

const int VGA_screen_width = 1280;
const int VGA_screen_height = 720;
void SPU_handle_out_VGA(SPU* processor, uint32_t data_ptr, uint32_t count);

void SPU_handle_out_port_printf(SPU* processor, uint32_t data_ptr, uint32_t count);


const port_out_handler port_out_handlers[] = 
{
    SPU_handle_out_text_VGA,
    SPU_handle_out_port_printf,
    SPU_handle_out_VGA
};

const size_t port_out_handlers_number = sizeof(port_out_handlers) / sizeof(port_out_handlers[0]);



const port_in_handler port_in_handlers[] = 
{
    SPU_handle_out_port_printf,
    SPU_handle_out_port_printf,
};

const size_t port_in_handlers_number = sizeof(port_in_handlers) / sizeof(port_in_handlers[0]);



#endif // SPU_DECLARED  

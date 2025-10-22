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

#define O_NEG           (0b00010 | ARG_NUM_3)

#define O_INC           (0b00011 | ARG_NUM_3)
#define O_DEC           (0b00100 | ARG_NUM_3)

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
    uint32_t args_num;
    const char* name;
    void* args_ptr;
};



typedef void (*instruction_executor) (SPU* processor, int32_t* args, uint8_t opcode);


void SPU_execute_O_NOP(SPU* processor, int32_t* args, uint8_t opcode);
void SPU_execute_O_HLT(SPU* processor, int32_t* args, uint8_t opcode);


void SPU_execute_O_MOV_CONST(SPU* processor, int32_t* args, uint8_t opcode);

void SPU_execute_O_LEA(SPU* processor, int32_t* args, uint8_t opcode);


void SPU_execute_O_MOV(SPU* processor, int32_t* args, uint8_t opcode);

void SPU_execute_O_INV(SPU* processor, int32_t* args, uint8_t opcode);

void SPU_execute_O_NEG(SPU* processor, int32_t* args, uint8_t opcode);

void SPU_execute_O_INC(SPU* processor, int32_t* args, uint8_t opcode);
void SPU_execute_O_DEC(SPU* processor, int32_t* args, uint8_t opcode);

void SPU_execute_O_ALL(SPU* processor, int32_t* args, uint8_t opcode);
void SPU_execute_O_ANY(SPU* processor, int32_t* args, uint8_t opcode);

void SPU_execute_O_CLEA(SPU* processor, int32_t* args, uint8_t opcode);

void SPU_execute_O_OUT(SPU* processor, int32_t* args, uint8_t opcode);
void SPU_execute_O_IN(SPU* processor, int32_t* args, uint8_t opcode);


void SPU_execute_O_EQ(SPU* processor, int32_t* args, uint8_t opcode);
void SPU_execute_O_OR(SPU* processor, int32_t* args, uint8_t opcode);
void SPU_execute_O_AND(SPU* processor, int32_t* args, uint8_t opcode);
void SPU_execute_O_XOR(SPU* processor, int32_t* args, uint8_t opcode);

void SPU_execute_O_ADD(SPU* processor, int32_t* args, uint8_t opcode);
void SPU_execute_O_SUB(SPU* processor, int32_t* args, uint8_t opcode);
void SPU_execute_O_MUL(SPU* processor, int32_t* args, uint8_t opcode);
void SPU_execute_O_DIV(SPU* processor, int32_t* args, uint8_t opcode);

void SPU_execute_O_CMOV(SPU* processor, int32_t* args, uint8_t opcode);

void SPU_execute_O_LT(SPU* processor, int32_t* args, uint8_t opcode);



#define EXPAND_INSTRUCTIONS()                                                                               /* 
! NO ARGS
*/ INSTRUCTION(                                                                                             \
    "NOP",                                                                                                  \
    (0b00000 | ARG_NUM_0),                                                                                  \
    0,                                                                                                      \
    SPU_execute_O_NOP(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);          \
)                                                                                                           \
INSTRUCTION(                                                                                                \
    "HLT",                                                                                                  \
    (0b00001 | ARG_NUM_0),                                                                                  \
    0,                                                                                                      \
    SPU_execute_O_HLT(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);          \
)                                                                                                           /*
! (DST: void*) (VAL: size_t)      IMPORTANT: ARG_TYPE_PTR_MODIFIER AFFECTS ONLY DST
*/INSTRUCTION(                                                                                              \
    "MOVC",                                                                                                 \
    (0b00000 | ARG_NUM_2),                                                                                  \
    2,                                                                                                      \
    SPU_execute_O_MOV_CONST(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);    \
)                                                                                                           /*
! (DST: void*) (VAL: void*)      IMPORTANT: ARG_TYPE_PTR_MODIFIER AFFECTS ONLY DST
*/ INSTRUCTION(                                                                                             \
    "LEA",                                                                                                  \
    (0b00001 | ARG_NUM_2),                                                                                  \
    2,                                                                                                      \
    SPU_execute_O_LEA(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);          \
)                                                                                                           /*


! (DST: void*) (SRC: void*) (COUNT: size_t*) 
! DST MAY INTERSECT WITH SRC
*/ INSTRUCTION(                                                                                             \
    "MOV",                                                                                                  \
    (0b00000 | ARG_NUM_3),                                                                                  \
    3,                                                                                                      \
    SPU_execute_O_MOV(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);          \
)                                                                                                           /* 
! (DST: void*) (SRC: void*) (COUNT: size_t*) 
! DST MAY BE SRC.
*/ INSTRUCTION(                                                                                             \
    "INV",                                                                                                  \
    (0b00001 | ARG_NUM_3),                                                                                  \
    3,                                                                                                      \
    SPU_execute_O_INV(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);          \
)                                                                                                           \
INSTRUCTION(                                                                                                \
    "NEG",                                                                                                  \
    (0b00010 | ARG_NUM_3),                                                                                  \
    3,                                                                                                      \
    SPU_execute_O_NEG(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);          \
)                                                                                                           /*

*/ INSTRUCTION(                                                                                             \
    "INC",                                                                                                  \
    (0b00011 | ARG_NUM_3),                                                                                  \
    3,                                                                                                      \
    SPU_execute_O_INC(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);          \
)                                                                                                           \
INSTRUCTION(                                                                                                \
    "DEC",                                                                                                  \
    (0b00100 | ARG_NUM_3),                                                                                  \
    3,                                                                                                      \
    SPU_execute_O_DEC(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);          \
)                                                                                                           /*


*/ INSTRUCTION(                                                                                             \
    "ALL",                                                                                                  \
    (0b00101 | ARG_NUM_3),                                                                                  \
    3,                                                                                                      \
    SPU_execute_O_ALL(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);          \
)                                                                                                           \
INSTRUCTION(                                                                                                \
    "ANY",                                                                                                  \
    (0b00110 | ARG_NUM_3),                                                                                  \
    3,                                                                                                      \
    SPU_execute_O_ANY(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);          \
)                                                                                                           /*

! (FLAG: size_t*) (DST: void*) (VAL: void*)      IMPORTANT: ARG_TYPE_PTR_MODIFIER AFFECTS ONLY DST AND FLAG
*/ INSTRUCTION(                                                                                             \
    "CLEA",                                                                                                 \
    (0b00111 | ARG_NUM_3),                                                                                  \
    3,                                                                                                      \
    SPU_execute_O_CLEA(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);         \
)                                                                                                           /*

! (INDEX: size_t) (SRC: void*) (COUNT: size_t*)
*/ INSTRUCTION(                                                                                             \
    "OUT",                                                                                                  \
    (0b01000 | ARG_NUM_3),                                                                                  \
    3,                                                                                                      \
    SPU_execute_O_OUT(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);          \
)                                                                                                           /*

! (INDEX: size_t) (DST: void*) (COUNT: size_t*)
*/ INSTRUCTION(                                                                                             \
    "IN",                                                                                                   \
    (0b01001 | ARG_NUM_3),                                                                                  \
    3,                                                                                                      \
    SPU_execute_O_IN(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);           \
)                                                                                                           /*

! (DST: void*) (A: void*) (B: void*) (COUNT: size_t*) 
*/ INSTRUCTION(                                                                                             \
    "EQ",                                                                                                   \
    (0b00000 | ARG_NUM_4),                                                                                  \
    4,                                                                                                      \
    SPU_execute_O_EQ(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);           \
)                                                                                                           \
INSTRUCTION(                                                                                                \
    "OR",                                                                                                   \
    (0b00001 | ARG_NUM_4),                                                                                  \
    4,                                                                                                      \
    SPU_execute_O_OR(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);           \
)                                                                                                           \
INSTRUCTION(                                                                                                \
    "AND",                                                                                                  \
    (0b00010 | ARG_NUM_4),                                                                                  \
    4,                                                                                                      \
    SPU_execute_O_AND(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);          \
)                                                                                                           \
INSTRUCTION(                                                                                                \
    "XOR",                                                                                                  \
    (0b00011 | ARG_NUM_4),                                                                                  \
    4,                                                                                                      \
    SPU_execute_O_XOR(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);          \
)                                                                                                           /*

*/ INSTRUCTION(                                                                                             \
    "ADD",                                                                                                  \
    (0b00100 | ARG_NUM_4),                                                                                  \
    4,                                                                                                      \
    SPU_execute_O_ADD(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);          \
)                                                                                                           \
INSTRUCTION(                                                                                                \
    "SUB",                                                                                                  \
    (0b00101 | ARG_NUM_4),                                                                                  \
    4,                                                                                                      \
    SPU_execute_O_SUB(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);          \
)                                                                                                           \
INSTRUCTION(                                                                                                \
    "MUL",                                                                                                  \
    (0b00110 | ARG_NUM_4),                                                                                  \
    4,                                                                                                      \
    SPU_execute_O_MUL(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);          \
)                                                                                                           \
INSTRUCTION(                                                                                                \
    "DIV",                                                                                                  \
    (0b00111 | ARG_NUM_4),                                                                                  \
    4,                                                                                                      \
    SPU_execute_O_DIV(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);          \
)                                                                                                           /*

! (FLAG: size_t*) (DST: void*) (SRC: void*) (COUNT: size_t*) 
*/ INSTRUCTION(                                                                                             \
    "CMOV",                                                                                                 \
    (0b01000 | ARG_NUM_4),                                                                                  \
    4,                                                                                                      \
    SPU_execute_O_CMOV(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);         \
)                                                                                                           /*

! (FLAG: size_t*) (A: void*) (B: void*) (COUNT: size_t*) 
*/ INSTRUCTION(                                                                                             \
    "LT",                                                                                                   \
    (0b01001 | ARG_NUM_4),                                                                                  \
    4,                                                                                                      \
    SPU_execute_O_LT(processor, (int32_t*)((int8_t*)real_instruction_ptr + OPCODE_SIZE), opcode);           \
)



#define INSTRUCTION(instr_name, instr_opcode, args_number, executor) {.opcode=instr_opcode, .args_num=args_number, .name=instr_name, .args_ptr=NULL},
const SPUInstruction instructions[] = {
    EXPAND_INSTRUCTIONS()
};
#undef INSTRUCTION

const size_t instructions_number = sizeof(instructions) / sizeof(instructions[0]);

typedef void (*port_out_handler) (SPU* processor, uint32_t data_ptr, uint32_t count);

typedef void (*port_in_handler) (SPU* processor, uint32_t dst_ptr, uint32_t count);



const int text_VGA_screen_width = 96;
const int text_VGA_screen_height = 48;
void SPU_handle_out_text_VGA(SPU* processor, uint32_t data_ptr, uint32_t count);

const int VGA_screen_true_width = 256;
const int VGA_screen_true_height = 144;

const int VGA_screen_view_width = 1280;
const int VGA_screen_view_height = 720;

void SPU_handle_out_VGA(SPU* processor, uint32_t data_ptr, uint32_t count);
void SPU_handle_out_port_printf(SPU* processor, uint32_t data_ptr, uint32_t count);
void SPU_handle_out_port_sound(SPU* processor, uint32_t data_ptr, uint32_t count);


const port_out_handler port_out_handlers[] = 
{
    SPU_handle_out_text_VGA,
    SPU_handle_out_port_printf,
    SPU_handle_out_VGA,
    SPU_handle_out_port_sound,
};

const size_t port_out_handlers_number = sizeof(port_out_handlers) / sizeof(port_out_handlers[0]);

void SPU_handle_in_port_timer(SPU* processor, uint32_t dst_ptr, uint32_t count);


const port_in_handler port_in_handlers[] = 
{
    SPU_handle_in_port_timer,
};

const size_t port_in_handlers_number = sizeof(port_in_handlers) / sizeof(port_in_handlers[0]);



#endif // SPU_DECLARED  

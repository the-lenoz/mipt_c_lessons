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
    void* args_ptr;
    uint32_t args_num;
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



#define EXPAND_INSTRUCTIONS() /* 
! NO ARGS
*/ INSTRUCTION(                                     \
    "NOP",                                          \
    (0b00000 | ARG_NUM_0),                          \
    0,                                              \
    {                                               \
        /*DO NOTHING*/                              \
    }                                               \
)                                                   \
INSTRUCTION(                                        \
    "HLT",                                          \
    (0b00001 | ARG_NUM_0),                          \
    0,                                              \
    {                                               \
        processor->is_running = 0;                  \
    }                                               \
)                                                   /*
! (DST: void*) (VAL: size_t)      IMPORTANT: ARG_TYPE_PTR_MODIFIER AFFECTS ONLY DST
*/INSTRUCTION(                                      \
    "MOVC",                                         \
    (0b00000 | ARG_NUM_2),                          \
    2,                                              \
    {                                               \
        int32_t relative_destination_ptr = (int32_t) args[0];                                           \
        uint32_t value = args[1];                                                                       \
                                                                                                        \
        uint32_t abs_destination_ptr = SPU_get_abs_ptr(processor, relative_destination_ptr);            \
                                                                                                        \
                                                                                                        \
        if ((opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)                                            \
        {                                                                                               \
            abs_destination_ptr = SPU_read_memory_cell(processor, abs_destination_ptr);                 \
        }                                                                                               \
                                                                                                        \
        SPU_write_memory_cell(processor, abs_destination_ptr, value);                                   \
    }                                               \
)                                                   /*
! (DST: void*) (VAL: void*)      IMPORTANT: ARG_TYPE_PTR_MODIFIER AFFECTS ONLY DST
*/ INSTRUCTION(                                     \
    "LEA",                                          \
    (0b00001 | ARG_NUM_2),                          \
    2,                                              \
    {                                               \
        int32_t relative_destination_ptr = (int32_t)args[0];                                             \
        int32_t local_ptr_value = (int32_t)args[1];                                                      \
                                                                                                \
        uint32_t abs_destination_ptr = SPU_get_abs_ptr(processor, relative_destination_ptr);    \
                                                                                                \
        if ((opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)                                    \
        {                                                                                       \
            abs_destination_ptr = SPU_read_memory_cell(processor, abs_destination_ptr);         \
        }                                                                                       \
                                                                                                \
        uint32_t abs_ptr_value = SPU_get_abs_ptr(processor, local_ptr_value);                   \
                                                                                                \
        SPU_write_memory_cell(processor, abs_destination_ptr, abs_ptr_value);                   \
    }                                                                                           \
)                                                   /*


! (DST: void*) (SRC: void*) (COUNT: size_t*) 
! DST MAY INTERSECT WITH SRC
*/ INSTRUCTION(                                     \
    "MOV",                                          \
    (0b00000 | ARG_NUM_3),                          \
    3,                                              \
    {                                               \
        int32_t relative_dst_ptr = (int32_t)args[0];                                                     \
        int32_t relative_src_ptr = (int32_t)args[1];                                                     \
        int32_t count = (int32_t)args[2];                                                                \
                                                                                                \
        uint32_t abs_dst_ptr = SPU_get_abs_ptr(processor, relative_dst_ptr);                    \
        uint32_t abs_src_ptr = SPU_get_abs_ptr(processor, relative_src_ptr);                    \
                                                                                                \
        if ((opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)                              \
        {                                                                                       \
            abs_dst_ptr = SPU_read_memory_cell(processor, abs_dst_ptr);                         \
            abs_src_ptr = SPU_read_memory_cell(processor, abs_src_ptr);                         \
            count = (int32_t)SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, count));\
        }                                                                                       \
                                                                                                \
        void* real_dst_ptr = SPU_get_real_mem_addr(processor, abs_dst_ptr);                     \
        void* real_src_ptr = SPU_get_real_mem_addr(processor, abs_src_ptr);                     \
                                                                                                \
        memmove(real_dst_ptr, real_src_ptr, (uint32_t) count);                                  \
    }                                               \
)                                                   /* 
! (DST: void*) (SRC: void*) (COUNT: size_t*) 
! DST MAY BE SRC.
*/ INSTRUCTION(                                     \
    "INV",                                          \
    (0b00001 | ARG_NUM_3),                          \
    3,                                              \
    {                                               \
        /*NOT IMPLEMENTED*/                         \
    }                                               \
)                                                   \
INSTRUCTION(                                        \
    "NEG",                                          \
    (0b00010 | ARG_NUM_3),                          \
    3,                                              \
    {                                               \
        /*NOT IMPLEMENTED*/                         \
    }                                               \
)                                                   /*

*/ INSTRUCTION(                                     \
    "INC",                                          \
    (0b00011 | ARG_NUM_3),                          \
    3,                                              \
    {                                               \
        /*NOT IMPLEMENTED*/                         \
    }                                               \
)                                                   \
INSTRUCTION(                                        \
    "DEC",                                          \
    (0b00100 | ARG_NUM_3),                          \
    3,                                              \
    {                                               \
        /*NOT IMPLEMENTED*/                         \
    }                                               \
)                                                   /*


*/ INSTRUCTION(                                     \
    "ALL",                                          \
    (0b00101 | ARG_NUM_3),                          \
    3,                                              \
    {                                               \
        /*NOT IMPLEMENTED*/                         \
    }                                               \
)                                                   \
INSTRUCTION(                                        \
    "ANY",                                          \
    (0b00110 | ARG_NUM_3),                          \
    3,                                              \
    {                                               \
        /*NOT IMPLEMENTED*/                         \
    }                                               \
)                                                   /*

! (FLAG: size_t*) (DST: void*) (VAL: void*)      IMPORTANT: ARG_TYPE_PTR_MODIFIER AFFECTS ONLY DST AND FLAG
*/ INSTRUCTION(                                     \
    "CLEA",                                         \
    (0b00111 | ARG_NUM_3),                          \
    3,                                              \
    {                                               \
        int32_t flag_ptr = (int32_t)args[0];                                                             \
        int32_t relative_destination_ptr = (int32_t)args[1];                                             \
        int32_t local_ptr_value = (int32_t)args[2];                                                      \
                                                                                                \
        uint32_t abs_destination_ptr = SPU_get_abs_ptr(processor, relative_destination_ptr);    \
        uint32_t flag = SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, flag_ptr));  \
                                                                                                \
        if ((opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)                                    \
        {                                                                                       \
            flag = SPU_read_memory_cell(processor, flag);                                       \
            abs_destination_ptr = SPU_read_memory_cell(processor, abs_destination_ptr);         \
        }                                                                                       \
        if (flag)                                                                               \
        {                                                                                       \
            uint32_t abs_ptr_value = SPU_get_abs_ptr(processor, local_ptr_value);               \
            SPU_write_memory_cell(processor, abs_destination_ptr, abs_ptr_value);               \
        }                                                                                       \
    }                                              \
)                                                  /*

! (INDEX: size_t) (SRC: void*) (COUNT: size_t*)
*/ INSTRUCTION(                                     \
    "OUT",                                          \
    (0b01000 | ARG_NUM_3),                          \
    3,                                              \
    {                                               \
        int32_t port_number = (int32_t)args[0];                                                                          \
        int32_t relative_data_ptr = (int32_t)args[1];                                                                    \
        int32_t count = (int32_t)args[2];                                                                                \
                                                                                                                \
        uint32_t abs_data_ptr = SPU_get_abs_ptr(processor, relative_data_ptr);                                  \
                                                                                                                \
        if ((opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)                                                    \
        {                                                                                                       \
            port_number = (int32_t)SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, port_number));    \
            abs_data_ptr = SPU_read_memory_cell(processor, abs_data_ptr);                                       \
            count = (int32_t) SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, count));               \
        }                                                                                                       \
                                                                                                                \
        if ((uint32_t)port_number >= port_out_handlers_number || port_out_handlers[port_number] == NULL)        \
        {                                                                                                       \
            return; /* UNKNOWN PORT */                                                                          \
        }                                                                                                       \
                                                                                                                \
        port_out_handlers[port_number](processor, abs_data_ptr, (uint32_t)count);                               \
    }                                               \
)                                                   /*

! (INDEX: size_t) (DST: void*) (COUNT: size_t*)
*/ INSTRUCTION(                                     \
    "IN",                                           \
    (0b01001 | ARG_NUM_3),                          \
    3,                                              \
    {                                               \
        int32_t port_number = (int32_t)args[0];                                                                          \
        int32_t relative_data_ptr = (int32_t)args[1];                                                                    \
        int32_t count = (int32_t)args[2];                                                                                \
                                                                                                                \
        uint32_t abs_data_ptr = SPU_get_abs_ptr(processor, relative_data_ptr);                                  \
                                                                                                                \
        if ((opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)                                                    \
        {                                                                                                       \
            port_number = (int32_t)SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, port_number));    \
            abs_data_ptr = SPU_read_memory_cell(processor, abs_data_ptr);                                       \
            count = (int32_t) SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, count));               \
        }                                                                                                       \
                                                                                                                \
        if ((uint32_t)port_number >= port_in_handlers_number || port_in_handlers[port_number] == NULL)          \
        {                                                                                                       \
            return; /* UNKNOWN PORT */                                                                          \
        }                                                                                                       \
                                                                                                                \
        port_in_handlers[port_number](processor, abs_data_ptr, (uint32_t)count);                                \
    }                                               \
)                                                   /*

! (DST: void*) (A: void*) (B: void*) (COUNT: size_t*) 
*/ INSTRUCTION(                                     \
    "EQ",                                           \
    (0b00000 | ARG_NUM_4),                          \
    4,                                              \
    {                                               \
        /*NOT IMPLEMENTED*/                         \
    }                                               \
)                                                   \
INSTRUCTION(                                        \
    "OR",                                           \
    (0b00001 | ARG_NUM_4),                          \
    4,                                              \
    {                                               \
        /*NOT IMPLEMENTED*/                         \
    }                                               \
)                                                   \
INSTRUCTION(                                        \
    "AND",                                          \
    (0b00010 | ARG_NUM_4),                          \
    4,                                              \
    {                                               \
        /*NOT IMPLEMENTED*/                         \
    }                                               \
)                                                   \
INSTRUCTION(                                        \
    "XOR",                                          \
    (0b00011 | ARG_NUM_4),                          \
    4,                                              \
    {                                               \
        /*NOT IMPLEMENTED*/                         \
    }                                               \
)                                                   /*

*/ INSTRUCTION(                                     \
    "ADD",                                          \
    (0b00100 | ARG_NUM_4),                          \
    4,                                              \
    {                                               \
        int32_t relative_dst_ptr = (int32_t)args[0];                                                                                                        \
        int32_t relative_A_ptr = (int32_t)args[1];                                                                                                        \
        int32_t relative_B_ptr = (int32_t)args[2];                                                                                                        \
        int32_t count = (int32_t)args[3];                                                                                                        \
                                                                                                        \
                                                                                                        \
        uint32_t abs_dst_ptr = SPU_get_abs_ptr(processor, relative_dst_ptr);                                                                                                        \
        uint32_t abs_A_ptr = SPU_get_abs_ptr(processor, relative_A_ptr);                                                                                                        \
        uint32_t abs_B_ptr = SPU_get_abs_ptr(processor, relative_B_ptr);                                                                                                        \
                                                                                                        \
                                                                                                        \
        if ((opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)                                                                                                        \
        {                                                                                                        \
            abs_dst_ptr = SPU_read_memory_cell(processor, abs_dst_ptr);                                                                                                        \
            abs_A_ptr = SPU_read_memory_cell(processor, abs_A_ptr);                                                                                                        \
            abs_B_ptr = SPU_read_memory_cell(processor, abs_B_ptr);                                                                                                        \
            count = (int32_t) SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, count));                                                                                                        \
        }                                                                                                        \
                                                                                                        \
        if (SPU_is_valid_mem_array(processor, abs_A_ptr, (uint32_t)count) &&                                                                                                        \
            SPU_is_valid_mem_array(processor, abs_B_ptr, (uint32_t)count) &&                                                                                                        \
            SPU_is_valid_mem_array(processor, abs_dst_ptr, (uint32_t)count))                                                                                                        \
        {                                                                                                        \
            int8_t a_byte = 0; int8_t b_byte = 0; int8_t c_byte = 0;                                                                                                        \
            int16_t a_word = 0; int16_t b_word = 0; int16_t c_word = 0;                                                                                                        \
            int32_t a_dword = 0; int32_t b_dword = 0; int32_t c_dword = 0;                                                                                                        \
            int64_t a_qword = 0; int64_t b_qword = 0; int64_t c_qword = 0;                                                                                                        \
            switch (count) {                                                                                                        \
                case sizeof(int8_t):                                                                                                        \
                    SPU_read_memory(processor, &a_byte, abs_A_ptr, sizeof(a_byte));                                                                                                        \
                    SPU_read_memory(processor, &b_byte, abs_B_ptr, sizeof(b_byte));                                                                                                        \
                                                                                                        \
                    c_byte = a_byte + b_byte;                                                                                                        \
                                                                                                        \
                    SPU_write_memory(processor, abs_dst_ptr, &c_byte, sizeof(c_byte));                                                                                                        \
                    break;                                                                                                        \
                case sizeof(int16_t):                                                                                                        \
                    SPU_read_memory(processor, &a_word, abs_A_ptr, sizeof(a_word));                                                                                                        \
                    SPU_read_memory(processor, &b_word, abs_B_ptr, sizeof(b_word));                                                                                                        \
                                                                                                        \
                    c_word = a_word + b_word;                                                                                                        \
                                                                                                        \
                    SPU_write_memory(processor, abs_dst_ptr, &c_word, sizeof(c_word));                                                                                                        \
                    break;                                                                                                        \
                case sizeof(int32_t):                                                                                                                                                                                                                \
                    SPU_read_memory(processor, &a_dword, abs_A_ptr, sizeof(a_dword));                                                                                                                                                                                                                \
                    SPU_read_memory(processor, &b_dword, abs_B_ptr, sizeof(b_dword));                                                                                                                                                                                                                \
                                                                                                        \
                    c_dword = a_dword + b_dword;                                                                                                        \
                                                                                                        \
                    SPU_write_memory(processor, abs_dst_ptr, &c_dword, sizeof(c_dword));                                                                                                        \
                    break;                                                                                                        \
                case sizeof(int64_t):                                                                                                        \
                    SPU_read_memory(processor, &a_qword, abs_A_ptr, sizeof(a_qword));                                                                                                        \
                    SPU_read_memory(processor, &b_qword, abs_B_ptr, sizeof(b_qword));                                                                                                        \
                                                                                                        \
                    c_qword = a_qword + b_qword;                                                                                                        \
                                                                                                        \
                    SPU_write_memory(processor, abs_dst_ptr, &c_qword, sizeof(c_qword));                                                                                                        \
                    break;                                                                                                        \
                default:                                                                                                        \
                    long_add(                                                                                                        \
                            (int8_t*)SPU_get_real_mem_addr(processor, abs_A_ptr),                                                                                                         \
                            (int8_t*)SPU_get_real_mem_addr(processor, abs_B_ptr),                                                                                                         \
                            (int8_t*)SPU_get_real_mem_addr(processor, abs_dst_ptr), (uint32_t)count);                                                                                                        \
                    break;                                                                                                        \
            }                                                                                                        \
        }                                                                                                        \
    }                                               \
)                                                   \
INSTRUCTION(                                        \
    "SUB",                                          \
    (0b00101 | ARG_NUM_4),                          \
    4,                                              \
    {                                               \
        int32_t relative_dst_ptr = (int32_t)args[0];                                                                                                        \
        int32_t relative_A_ptr = (int32_t)args[1];                                                                                                        \
        int32_t relative_B_ptr = (int32_t)args[2];                                                                                                        \
        int32_t count = (int32_t)args[3];                                                                                                        \
                                                                                                        \
                                                                                                        \
        uint32_t abs_dst_ptr = SPU_get_abs_ptr(processor, relative_dst_ptr);                                                                                                        \
        uint32_t abs_A_ptr = SPU_get_abs_ptr(processor, relative_A_ptr);                                                                                                        \
        uint32_t abs_B_ptr = SPU_get_abs_ptr(processor, relative_B_ptr);                                                                                                        \
                                                                                                        \
                                                                                                        \
        if ((opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)                                                                                                        \
        {                                                                                                        \
            abs_dst_ptr = SPU_read_memory_cell(processor, abs_dst_ptr);                                                                                                        \
            abs_A_ptr = SPU_read_memory_cell(processor, abs_A_ptr);                                                                                                        \
            abs_B_ptr = SPU_read_memory_cell(processor, abs_B_ptr);                                                                                                        \
            count = (int32_t) SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, count));                                                                                                        \
        }                                                                                                        \
                                                                                                        \
        if (SPU_is_valid_mem_array(processor, abs_A_ptr, (uint32_t)count) &&                                                                                                        \
            SPU_is_valid_mem_array(processor, abs_B_ptr, (uint32_t)count) &&                                                                                                        \
            SPU_is_valid_mem_array(processor, abs_dst_ptr, (uint32_t)count))                                                                                                        \
        {                                                                                                        \
            int8_t a_byte = 0; int8_t b_byte = 0; int8_t c_byte = 0;                                                                                                        \
            int16_t a_word = 0; int16_t b_word = 0; int16_t c_word = 0;                                                                                                        \
            int32_t a_dword = 0; int32_t b_dword = 0; int32_t c_dword = 0;                                                                                                        \
            int64_t a_qword = 0; int64_t b_qword = 0; int64_t c_qword = 0;                                                                                                        \
            switch (count) {                                                                                                        \
                case sizeof(int8_t):                                                                                                        \
                    SPU_read_memory(processor, &a_byte, abs_A_ptr, sizeof(a_byte));                                                                                                        \
                    SPU_read_memory(processor, &b_byte, abs_B_ptr, sizeof(b_byte));                                                                                                        \
                                                                                                        \
                    c_byte = a_byte - b_byte;                                                                                                        \
                                                                                                        \
                    SPU_write_memory(processor, abs_dst_ptr, &c_byte, sizeof(c_byte));                                                                                                        \
                    break;                                                                                                        \
                case sizeof(int16_t):                                                                                                        \
                    SPU_read_memory(processor, &a_word, abs_A_ptr, sizeof(a_word));                                                                                                        \
                    SPU_read_memory(processor, &b_word, abs_B_ptr, sizeof(b_word));                                                                                                        \
                                                                                                        \
                    c_word = a_word - b_word;                                                                                                        \
                                                                                                        \
                    SPU_write_memory(processor, abs_dst_ptr, &c_word, sizeof(c_word));                                                                                                        \
                    break;                                                                                                        \
                case sizeof(int32_t):                                                                                                                                                                                                                \
                    SPU_read_memory(processor, &a_dword, abs_A_ptr, sizeof(a_dword));                                                                                                                                                                                                                \
                    SPU_read_memory(processor, &b_dword, abs_B_ptr, sizeof(b_dword));                                                                                                                                                                                                                \
                                                                                                        \
                    c_dword = a_dword - b_dword;                                                                                                        \
                                                                                                        \
                    SPU_write_memory(processor, abs_dst_ptr, &c_dword, sizeof(c_dword));                                                                                                        \
                    break;                                                                                                        \
                case sizeof(int64_t):                                                                                                        \
                    SPU_read_memory(processor, &a_qword, abs_A_ptr, sizeof(a_qword));                                                                                                        \
                    SPU_read_memory(processor, &b_qword, abs_B_ptr, sizeof(b_qword));                                                                                                        \
                                                                                                        \
                    c_qword = a_qword - b_qword;                                                                                                        \
                                                                                                        \
                    SPU_write_memory(processor, abs_dst_ptr, &c_qword, sizeof(c_qword));                                                                                                        \
                    break;                                                                                                        \
                default:                                                                                                        \
                    long_sub(                                                                                                        \
                            (int8_t*)SPU_get_real_mem_addr(processor, abs_A_ptr),                                                                                                         \
                            (int8_t*)SPU_get_real_mem_addr(processor, abs_B_ptr),                                                                                                         \
                            (int8_t*)SPU_get_real_mem_addr(processor, abs_dst_ptr), (uint32_t)count);                                                                                                        \
                    break;                                                                                                        \
            }                                                                                                        \
        }                                                                                                        \
    }                                               \
)                                                   \
INSTRUCTION(                                        \
    "MUL",                                          \
    (0b00110 | ARG_NUM_4),                          \
    4,                                              \
    {                                               \
        int32_t relative_dst_ptr = (int32_t)args[0];                                                                                                        \
        int32_t relative_A_ptr = (int32_t)args[1];                                                                                                        \
        int32_t relative_B_ptr = (int32_t)args[2];                                                                                                        \
        int32_t count = (int32_t)args[3];                                                                                                        \
                                                                                                        \
                                                                                                        \
        uint32_t abs_dst_ptr = SPU_get_abs_ptr(processor, relative_dst_ptr);                                                                                                        \
        uint32_t abs_A_ptr = SPU_get_abs_ptr(processor, relative_A_ptr);                                                                                                        \
        uint32_t abs_B_ptr = SPU_get_abs_ptr(processor, relative_B_ptr);                                                                                                        \
                                                                                                        \
                                                                                                        \
        if ((opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)                                                                                                        \
        {                                                                                                        \
            abs_dst_ptr = SPU_read_memory_cell(processor, abs_dst_ptr);                                                                                                        \
            abs_A_ptr = SPU_read_memory_cell(processor, abs_A_ptr);                                                                                                        \
            abs_B_ptr = SPU_read_memory_cell(processor, abs_B_ptr);                                                                                                        \
            count = (int32_t) SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, count));                                                                                                        \
        }                                                                                                        \
                                                                                                        \
        if (SPU_is_valid_mem_array(processor, abs_A_ptr, (uint32_t)count) &&                                                                                                        \
            SPU_is_valid_mem_array(processor, abs_B_ptr, (uint32_t)count) &&                                                                                                        \
            SPU_is_valid_mem_array(processor, abs_dst_ptr, (uint32_t)count))                                                                                                        \
        {                                                                                                        \
            int8_t a_byte = 0; int8_t b_byte = 0; int8_t c_byte = 0;                                                                                                        \
            int16_t a_word = 0; int16_t b_word = 0; int16_t c_word = 0;                                                                                                        \
            int32_t a_dword = 0; int32_t b_dword = 0; int32_t c_dword = 0;                                                                                                        \
            int64_t a_qword = 0; int64_t b_qword = 0; int64_t c_qword = 0;                                                                                                        \
            switch (count) {                                                                                                        \
                case sizeof(int8_t):                                                                                                        \
                    SPU_read_memory(processor, &a_byte, abs_A_ptr, sizeof(a_byte));                                                                                                        \
                    SPU_read_memory(processor, &b_byte, abs_B_ptr, sizeof(b_byte));                                                                                                        \
                                                                                                        \
                    c_byte = a_byte * b_byte;                                                                                                        \
                                                                                                        \
                    SPU_write_memory(processor, abs_dst_ptr, &c_byte, sizeof(c_byte));                                                                                                        \
                    break;                                                                                                        \
                case sizeof(int16_t):                                                                                                        \
                    SPU_read_memory(processor, &a_word, abs_A_ptr, sizeof(a_word));                                                                                                        \
                    SPU_read_memory(processor, &b_word, abs_B_ptr, sizeof(b_word));                                                                                                        \
                                                                                                        \
                    c_word = a_word * b_word;                                                                                                        \
                                                                                                        \
                    SPU_write_memory(processor, abs_dst_ptr, &c_word, sizeof(c_word));                                                                                                        \
                    break;                                                                                                        \
                case sizeof(int32_t):                                                                                                                                                                                                                \
                    SPU_read_memory(processor, &a_dword, abs_A_ptr, sizeof(a_dword));                                                                                                                                                                                                                \
                    SPU_read_memory(processor, &b_dword, abs_B_ptr, sizeof(b_dword));                                                                                                                                                                                                                \
                                                                                                        \
                    c_dword = a_dword * b_dword;                                                                                                        \
                                                                                                        \
                    SPU_write_memory(processor, abs_dst_ptr, &c_dword, sizeof(c_dword));                                                                                                        \
                    break;                                                                                                        \
                case sizeof(int64_t):                                                                                                        \
                    SPU_read_memory(processor, &a_qword, abs_A_ptr, sizeof(a_qword));                                                                                                        \
                    SPU_read_memory(processor, &b_qword, abs_B_ptr, sizeof(b_qword));                                                                                                        \
                                                                                                        \
                    c_qword = a_qword * b_qword;                                                                                                        \
                                                                                                        \
                    SPU_write_memory(processor, abs_dst_ptr, &c_qword, sizeof(c_qword));                                                                                                        \
                    break;                                                                                                        \
                default:                                                                                                        \
                    long_mul(                                                                                                        \
                            (int8_t*)SPU_get_real_mem_addr(processor, abs_A_ptr),                                                                                                         \
                            (int8_t*)SPU_get_real_mem_addr(processor, abs_B_ptr),                                                                                                         \
                            (int8_t*)SPU_get_real_mem_addr(processor, abs_dst_ptr), (uint32_t)count);                                                                                                        \
                    break;                                                                                                        \
            }                                                                                                        \
        }                                                                                                        \
    }                                               \
)                                                   \
INSTRUCTION(                                        \
    "DIV",                                          \
    (0b00111 | ARG_NUM_4),                          \
    4,                                              \
    {                                               \
        int32_t relative_dst_ptr = (int32_t)args[0];                                                                                                        \
        int32_t relative_A_ptr = (int32_t)args[1];                                                                                                        \
        int32_t relative_B_ptr = (int32_t)args[2];                                                                                                        \
        int32_t count = (int32_t)args[3];                                                                                                        \
                                                                                                        \
                                                                                                        \
        uint32_t abs_dst_ptr = SPU_get_abs_ptr(processor, relative_dst_ptr);                                                                                                        \
        uint32_t abs_A_ptr = SPU_get_abs_ptr(processor, relative_A_ptr);                                                                                                        \
        uint32_t abs_B_ptr = SPU_get_abs_ptr(processor, relative_B_ptr);                                                                                                        \
                                                                                                        \
                                                                                                        \
        if ((opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)                                                                                                        \
        {                                                                                                        \
            abs_dst_ptr = SPU_read_memory_cell(processor, abs_dst_ptr);                                                                                                        \
            abs_A_ptr = SPU_read_memory_cell(processor, abs_A_ptr);                                                                                                        \
            abs_B_ptr = SPU_read_memory_cell(processor, abs_B_ptr);                                                                                                        \
            count = (int32_t) SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, count));                                                                                                        \
        }                                                                                                        \
                                                                                                        \
        if (SPU_is_valid_mem_array(processor, abs_A_ptr, (uint32_t)count) &&                                                                                                        \
            SPU_is_valid_mem_array(processor, abs_B_ptr, (uint32_t)count) &&                                                                                                        \
            SPU_is_valid_mem_array(processor, abs_dst_ptr, (uint32_t)count))                                                                                                        \
        {                                                                                                        \
            int8_t a_byte = 0; int8_t b_byte = 0; int8_t c_byte = 0;                                                                                                        \
            int16_t a_word = 0; int16_t b_word = 0; int16_t c_word = 0;                                                                                                        \
            int32_t a_dword = 0; int32_t b_dword = 0; int32_t c_dword = 0;                                                                                                        \
            int64_t a_qword = 0; int64_t b_qword = 0; int64_t c_qword = 0;                                                                                                        \
            switch (count) {                                                                                                        \
                case sizeof(int8_t):                                                                                                        \
                    SPU_read_memory(processor, &a_byte, abs_A_ptr, sizeof(a_byte));                                                                                                        \
                    SPU_read_memory(processor, &b_byte, abs_B_ptr, sizeof(b_byte));                                                                                                        \
                                                                                                        \
                    c_byte = a_byte / b_byte;                                                                                                        \
                                                                                                        \
                    SPU_write_memory(processor, abs_dst_ptr, &c_byte, sizeof(c_byte));                                                                                                        \
                    break;                                                                                                        \
                case sizeof(int16_t):                                                                                                        \
                    SPU_read_memory(processor, &a_word, abs_A_ptr, sizeof(a_word));                                                                                                        \
                    SPU_read_memory(processor, &b_word, abs_B_ptr, sizeof(b_word));                                                                                                        \
                                                                                                        \
                    c_word = a_word / b_word;                                                                                                        \
                                                                                                        \
                    SPU_write_memory(processor, abs_dst_ptr, &c_word, sizeof(c_word));                                                                                                        \
                    break;                                                                                                        \
                case sizeof(int32_t):                                                                                                                                                                                                                \
                    SPU_read_memory(processor, &a_dword, abs_A_ptr, sizeof(a_dword));                                                                                                        \
                    SPU_read_memory(processor, &b_dword, abs_B_ptr, sizeof(b_dword));                                                                                                                                                                                                                \
                                                                                                        \
                    c_dword = a_dword / b_dword;                                                                                                        \
                                                                                                        \
                    SPU_write_memory(processor, abs_dst_ptr, &c_dword, sizeof(c_dword));                                                                                                        \
                    break;                                                                                                        \
                case sizeof(int64_t):                                                                                                        \
                    SPU_read_memory(processor, &a_qword, abs_A_ptr, sizeof(a_qword));                                                                                                        \
                    SPU_read_memory(processor, &b_qword, abs_B_ptr, sizeof(b_qword));                                                                                                        \
                                                                                                        \
                    c_qword = a_qword / b_qword;                                                                                                        \
                                                                                                        \
                    SPU_write_memory(processor, abs_dst_ptr, &c_qword, sizeof(c_qword));                                                                                                        \
                    break;                                                                                                        \
                default:                                                                                                        \
                    long_div(                                                                                                        \
                            (int8_t*)SPU_get_real_mem_addr(processor, abs_A_ptr),                                                                                                         \
                            (int8_t*)SPU_get_real_mem_addr(processor, abs_B_ptr),                                                                                                         \
                            (int8_t*)SPU_get_real_mem_addr(processor, abs_dst_ptr), (uint32_t)count);                                                                                                        \
                    break;                                                                                                        \
            }                                                                                                        \
        }                                                                                                        \
    }                                               \
)                                                   /*

! (FLAG: size_t*) (DST: void*) (SRC: void*) (COUNT: size_t*) 
*/ INSTRUCTION(                                     \
    "CMOV",                                         \
    (0b01000 | ARG_NUM_4),                          \
    4,                                              \
    {                                               \
        int32_t flag_ptr = (int32_t)args[0];                                                                 \
        int32_t relative_dst_ptr = (int32_t)args[1];                                                         \
        int32_t relative_src_ptr = (int32_t)args[2];                                                         \
        int32_t count = (int32_t)args[3];                                                                    \
                                                                                                    \
        uint32_t abs_dst_ptr = SPU_get_abs_ptr(processor, relative_dst_ptr);                        \
        uint32_t abs_src_ptr = SPU_get_abs_ptr(processor, relative_src_ptr);                        \
        uint32_t flag = SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, flag_ptr));      \
                                                                                                    \
        if ((opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)                                        \
        {                                                                                           \
            flag = SPU_read_memory_cell(processor, flag);                                           \
            abs_dst_ptr = SPU_read_memory_cell(processor, abs_dst_ptr);                             \
            abs_src_ptr = SPU_read_memory_cell(processor, abs_src_ptr);                             \
            count = (int32_t) SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, count));   \
        }                                                                                           \
                                                                                                    \
        void* real_dst_ptr = SPU_get_real_mem_addr(processor, abs_dst_ptr);                         \
        void* real_src_ptr = SPU_get_real_mem_addr(processor, abs_src_ptr);                         \
                                                                                                    \
        if (flag)                                                                                   \
        {                                                                                           \
            memmove(real_dst_ptr, real_src_ptr, (uint32_t) count);                                  \
        }                                                                                           \
    }                                               \
)                                                   /*

! (FLAG: size_t*) (A: void*) (B: void*) (COUNT: size_t*) 
*/ INSTRUCTION(                                     \
    "LT",                                           \
    (0b01001 | ARG_NUM_4),                          \
    4,                                              \
    {                                               \
        int32_t relative_flag_ptr = (int32_t)args[0];                                                        \
        int32_t relative_A_ptr = (int32_t)args[1];                                                           \
        int32_t relative_B_ptr = (int32_t)args[2];                                                           \
        uint32_t count = (uint32_t) args[3];                                                        \
                                                                                                    \
        uint32_t abs_flag_ptr = SPU_get_abs_ptr(processor, relative_flag_ptr);                      \
        uint32_t abs_A_ptr = SPU_get_abs_ptr(processor, relative_A_ptr);                            \
        uint32_t abs_B_ptr = SPU_get_abs_ptr(processor, relative_B_ptr);                            \
                                                                                                    \
        if ((opcode & ARG_TYPE_OPCODE_MASK) == ARG_TYPE_PTR)                                  \
        {                                                                                           \
            abs_flag_ptr = SPU_read_memory_cell(processor, abs_flag_ptr);                           \
            abs_A_ptr = SPU_read_memory_cell(processor, abs_A_ptr);                                 \
            abs_B_ptr = SPU_read_memory_cell(processor, abs_B_ptr);                                 \
            count = SPU_read_memory_cell(processor, SPU_get_abs_ptr(processor, (int32_t)count));    \
        }                                                                                           \
                                                                                                    \
        uint8_t a = 0;                                                                              \
        uint8_t b = 0;                                                                              \
        uint8_t Ba = 0;                                                                             \
        uint8_t Bb = 0;                                                                             \
                                                                                                    \
        for (uint32_t i = 0; i < (uint32_t)count; ++i)                                              \
        {                                                                                           \
            SPU_read_memory(processor, &a, abs_A_ptr + count - 1 - i, 1);                           \
            SPU_read_memory(processor, &b, abs_B_ptr + count - 1 - i, 1);                           \
                                                                                                    \
            if (i == 0)                                                                             \
            {                                                                                       \
                Ba = (a & 0b10000000) >> 7;                                                         \
                Bb = (b & 0b10000000) >> 7;                                                         \
            }                                                                                       \
                                                                                                    \
            if (a < b)                                                                              \
            {                                                                                       \
                SPU_write_memory_cell(processor, abs_flag_ptr, Ba == Bb);                           \
                break;                                                                              \
            }                                                                                       \
            else if (a > b)                                                                         \
            {                                                                                       \
                SPU_write_memory_cell(processor, abs_flag_ptr, Ba != Bb);                           \
                break;                                                                              \
            }                                                                                       \
        }                                                                                           \
                                                                                                    \
        SPU_write_memory_cell(processor, abs_flag_ptr, 0);                                          \
    }                                               \
)



#define INSTRUCTION(instr_name, instr_opcode, args_number, executor) {.opcode=instr_opcode, .args_num=args_number, .name=instr_name},
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

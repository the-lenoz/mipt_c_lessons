#ifndef SPU_DECLARED
#define SPU_DECLARED

#include <stdlib.h>


/*******************************************************
//! 1 bit for arguments size:
//! 0 - 2 byte (WORD) argument size
//! 1 - 4 byte (DWORD) argument size
//!
//! 2 bits for arguments number
//! binary number means exactly number of arguments
//!
//! 5 bits for opcode
********************************************************/

#define ARG_SIZE_OPCODE_MASK 0b10000000
#define ARG_SIZE_WORD        0b00000000
#define ARG_SIZE_DWORD       0b10000000

#define ARG_NUM_OPCODE_MASK 0b01100000
#define ARG_NUM_0           0b00000000
//DEPRECATED BEFORE IMPLEMENTATION #define ARG_NUM_1           0b00100000
#define ARG_NUM_2           0b00100000
#define ARG_NUM_3           0b01000000
#define ARG_NUM_4           0b01100000


//! 
#define O_NOP           0b00000


//! (INDEX:1 byte) (DATA: void*) 
#define O_INT           0b00000

//! (DST: void*) (VAL: size_t) 
#define O_MOV_CONST     0b00001

//#define O_ABSPTR        0b00001


//! (DST: void*) (SRC: void*) (COUNT: size_t*) 
//! DST MAY INTERSECT WITH SRC
#define O_MOV           0b00000

//! (DST: void*) (SRC: void*) (COUNT: size_t*) 
//! DST MAY BE SRC.
#define O_INV           0b00001

//! (DST: void*) (SRC: void*) (COUNT: size_t*) 
//! DST MAY BE SRC.
#define O_NEG           0b00010

//! (DST: void*) (SRC: void*) (COUNT: size_t*) 
//! DST MAY BE SRC.
#define O_INC           0b00010
#define O_DEC           0b00011

//! (DST: size_t*) (SRC: void*) (COUNT: size_t*)
#define O_ALL           0b00100
#define O_ANY           0b00100




//! (DST: void*) (A: void*) (B: void*) (COUNT: size_t*) 
#define O_EQ            0b00000
#define O_OR            0b00001
#define O_AND           0b00010
#define O_XOR           0b00011

#define O_ADD           0b00100
#define O_SUB           0b00101
#define O_MUL           0b00110
#define O_DIV           0b00111

//! (FLAG: size_t*) (SRC: void*) (DST: void*) (COUNT: size_t*) 
#define O_CMOV          0b01000


//!  ()
#define O_CALL


size_t get_instruction_len(void* instruction_ptr);


#endif // SPU_DECLARED

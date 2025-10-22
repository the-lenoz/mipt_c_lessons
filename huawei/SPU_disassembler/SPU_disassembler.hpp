#ifndef SPU_DISASSEMBLER_DECLARED
#define SPU_DISASSEMBLER_DECLARED

#include <cstddef>
#include <cstdint>
#include <cstdio>


int fdisassemble_buffer(uint8_t* bytecode_buffer, size_t buffer_size, FILE* fp);

int sdisassemble_instruction(uint8_t* bytecode, size_t size, char* disasm_array);


#endif // SPU_DISASSEMBLER_DECLARED
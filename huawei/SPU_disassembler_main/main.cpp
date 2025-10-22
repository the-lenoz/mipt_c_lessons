#include "SPU.hpp"
#include "SPU_disassembler.hpp"
#include "file_operations.hpp"
#include "terminal_decorator.hpp"
#include <cstdint>
#include <cstdio>


int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf_red(stderr, "Usage: %s <path_to_bytecode\n", argv[0]);
        return -1;
    }

    const char* bytecode_file_path = argv[1];

    size_t file_size = get_file_size(bytecode_file_path);
    uint8_t* bytecode_buffer = (uint8_t*) calloc(file_size, sizeof(uint8_t));
    FILE* fp = fopen(bytecode_file_path, "rb");
    fread(bytecode_buffer, 1, file_size, fp);
    fclose(fp);

    fdisassemble_buffer(bytecode_buffer, file_size, stdout);
    free(bytecode_buffer);


    return 0;
}
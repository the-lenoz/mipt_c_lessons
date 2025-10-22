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

    ssize_t file_size = get_file_size(bytecode_file_path);
    if (file_size == -1)
    {
        fprintf_red(stderr, "Error: can't get file size: '%s'. Exiting\n", bytecode_file_path);
        return -1;
    }
    uint8_t* bytecode_buffer = (uint8_t*) calloc(file_size, sizeof(uint8_t));
    if (bytecode_buffer == NULL)
    {
        fprintf_red(stderr, "Error: can't allocate memory. Exiting\n");
        return -1;
    }

    FILE* fp = fopen(bytecode_file_path, "rb");

    if (fp == NULL)
    {
        fprintf_red(stderr, "Error: can't open file: '%s'. Exiting\n", bytecode_file_path);
        return -1;
    }

    if (fread(bytecode_buffer, 1, file_size, fp) != (size_t)file_size)
    {
        fprintf_red(stderr, "File: '%s' readed with error. Exiting\n", bytecode_file_path);
        return -1;
    }
    fclose(fp);

    if (fdisassemble_buffer(bytecode_buffer, file_size, stdout) != 0)
    {
        fprintf_red(stderr, "Error(s) while disassembling. Exiting\n");
        return -1;
    }
    free(bytecode_buffer);


    return 0;
}
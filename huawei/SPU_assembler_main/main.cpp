#include "my_assert.hpp"
#include "SPU_assembler/SPU_assembler.hpp"
#include "terminal_decorator.hpp"
#include <cstddef>
#include <cstdio>



int main(int argc, char** argv)
{
    assert(argv != NULL);

    if (argc != 3)
    {
        fprintf_red(stderr, "Usage: %s source.asm output.bcode\n", argv[0]);
        return -1;
    }

    const char* input_file_path = argv[1];
    const char* output_file_path = argv[2];

    return process_asm_file(input_file_path, output_file_path);
}
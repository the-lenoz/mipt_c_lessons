#include "SPU.hpp"
#include "file_operations.hpp"
#include "my_assert.hpp"
#include "terminal_decorator.hpp"



int main(int argc, char** argv)
{
    assert(argv != NULL);

    if (argc != 2)
    {
        fprintf_red(stderr, "Usage: %s file.bcode\n", argv[0]);
        return -1;
    }

    const char* input_file_path = argv[1];

    int file_size = get_file_size(input_file_path);
    char* bytecode = allocate_and_read(input_file_path);


    SPU processor = {};

    SPU_init(&processor, (uint32_t) file_size + 65536, 1);

    SPU_write_memory(&processor, 32, bytecode, (size_t) file_size);

    free(bytecode);

    SPU_start(&processor, 32);


    SPU_destroy(&processor);

    return 0;    
}
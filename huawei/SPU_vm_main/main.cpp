#include "SPU.hpp"
#include "file_operations.hpp"
#include "my_assert.hpp"
#include "terminal_decorator.hpp"
#include <cstring>
#include <SDL2/SDL.h>



int main(int argc, char** argv)
{
    assert(argv != NULL);

    if (argc < 2)
    {
        fprintf_red(stderr, "Usage: %s file.bcode\n", argv[0]);
        return -1;
    }

    const char* input_file_path = argv[1];

    int file_size = get_file_size(input_file_path);
    char* bytecode = allocate_and_read(input_file_path);


    int debug_mode = (argc == 3 && strcmp(argv[2], "-d") == 0) ? 1 : 0;

    SDL_Init(SDL_INIT_EVERYTHING);

    SPU processor = {};

    SPU_init(&processor, (uint32_t) file_size + (1 << 24), debug_mode);

    SPU_write_memory(&processor, 32, bytecode, (size_t) file_size);

    free(bytecode);

    SPU_start(&processor, 32);

    SDL_Quit();

    SPU_destroy(&processor);


    return 0;    
}
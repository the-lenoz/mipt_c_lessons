#include "SPU.hpp"




int main(int argc, char** argv)
{


    UNUSED(argc);
    UNUSED(argv);

    uint8_t bytecode[] = {
        O_NOP | ARG_TYPE_ASIS,
        O_NOP | ARG_TYPE_ASIS,
        O_NOP | ARG_TYPE_ASIS,
        O_MOV_CONST | ARG_TYPE_ASIS,    0x4, 00, 00, 00,    10, 00, 00, 00,
        O_MOV_CONST | ARG_TYPE_ASIS,    0x8, 00, 00, 00,    15, 00, 00, 00,
        O_ADD | ARG_TYPE_ASIS,          0x12,00, 00, 00,    0x4,00, 00, 00,     0x8,00, 00, 00, 0x1, 00, 00, 00,
        O_INT | ARG_TYPE_ASIS,          0x00, 00, 00, 00,   0x12, 00, 00, 00, 
        O_HLT | ARG_TYPE_ASIS
    };

    SPU processor = {};

    SPU_init(&processor, 256);

    SPU_write_memory(&processor, 32, bytecode, sizeof(bytecode));

    SPU_start(&processor, 32);


    SPU_destroy(&processor);

    return 0;    
}
#ifndef SPU_ASSEMBLER_DECLARED
#define SPU_ASSEMBLER_DECLARED


struct ASMError
{
    const char* filename;
    int line;
    int column;
    
    const char* description;
};


#endif // SPU_ASSEMBLER_DECLARED
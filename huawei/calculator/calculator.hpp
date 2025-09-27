#ifndef CALCULATOR_DECLARED
#define CALCULATOR_DECLARED

#include "stack/stack.hpp"
#include "status.hpp"
#define MAX_COMMAND_STR_LEN 16

enum CalculatorInstruction
{
    PUSH,
    OUT,
    HLT,

    ADD,
    SUB,
    MUL,
    DIV,
    SQRT
};

struct CalculatorCommand
{
    CalculatorInstruction command;
    char command_str[MAX_COMMAND_STR_LEN];

    double arg;
};

StatusData run_loop(Stack* st);


StatusData parse_command(CalculatorCommand* command);

StatusData run_command(CalculatorCommand command, Stack* st);






#endif // CALCULATOR_DECLARED
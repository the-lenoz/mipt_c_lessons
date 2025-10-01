#include "calculator.hpp"
#include "logger.hpp"
#include "stack/stack.hpp"
#include "status.hpp"
#include <cmath>
#include <cstddef>
#include <cstring>
#include <math.h>
#include <stdio.h>

StatusData run_loop(Stack* st)
{
    char* line = NULL;
    size_t n = 0;
    StatusData status = MAKE_SUCCESS_STRUCT(NULL);
    CalculatorCommand command = {};
    do 
    {
        line = NULL;
        n = 0;
        getline(&line, &n, stdin);
        command = {.arg=NAN};
        if (sscanf(line, " %15s %lg", command.command_str, &command.arg) == 0)
        {
            continue;
        }
        free(line);

        status = parse_command(&command);
        if (status.status_code != SUCCESS)
        {
            return status;
        }

        status = run_command(command, st);
        if (status.status_code != SUCCESS)
        {
            LOG_ERROR(status);
            stack_dump(st, ERROR);
        }

    }
    while (command.command != HLT);
    return MAKE_SUCCESS_STRUCT(NULL);
}

StatusData parse_command(CalculatorCommand* command)
{
    if (strcmp(command->command_str, "PUSH") == 0) command->command = PUSH;
    else if (strcmp(command->command_str, "OUT") == 0) command->command = OUT;
    else if (strcmp(command->command_str, "HLT") == 0) command->command = HLT;
    else if (strcmp(command->command_str, "ADD") == 0) command->command = ADD;
    else if (strcmp(command->command_str, "SUB") == 0) command->command = SUB;
    else if (strcmp(command->command_str, "MUL") == 0) command->command = MUL;
    else if (strcmp(command->command_str, "DIV") == 0) command->command = DIV;
    else if (strcmp(command->command_str, "SQRT") == 0) command->command = SQRT;
    else if (strcmp(command->command_str, "DUMP") == 0) command->command = DUMP;
    else return MAKE_EXTENDED_ERROR_STRUCT(INVALID_FUNCTION_PARAM, "Syntax error: unknown command");
    return MAKE_SUCCESS_STRUCT(NULL);
}

StatusData run_command(CalculatorCommand command, Stack* st)
{
    double a = NAN, b = NAN;
    StatusData status = MAKE_SUCCESS_STRUCT(NULL);
    switch (command.command)
    {
        case PUSH:
            if (isnan(command.arg))
            {
                status = MAKE_EXTENDED_ERROR_STRUCT(INVALID_FUNCTION_PARAM, "Syntax error: invalid PUSH arg");
                break;
            }
            stack_push(st, command.arg);
            status = st->last_operation_status;
            break;
        case OUT:
            a = stack_pop(st);
            status = st->last_operation_status;
            if (status.status_code != SUCCESS)
            {
                break;
            }
            printf("%lg\n", a);
            break;
        case HLT:
            break;

        case ADD:
            b = stack_pop(st);
            status = st->last_operation_status;
            if (status.status_code != SUCCESS)
            {
                break;
            }

            a = stack_pop(st);
            status = st->last_operation_status;
            if (status.status_code != SUCCESS)
            {
                break;
            }
            stack_push(st, a + b);
            status = st->last_operation_status;
            break;
        case SUB:
            b = stack_pop(st);
            status = st->last_operation_status;
            if (status.status_code != SUCCESS)
            {
                break;
            }

            a = stack_pop(st);
            status = st->last_operation_status;
            if (status.status_code != SUCCESS)
            {
                break;
            }
            stack_push(st, a - b);
            status = st->last_operation_status;
            break;
        case MUL:
            b = stack_pop(st);
            status = st->last_operation_status;
            if (status.status_code != SUCCESS)
            {
                break;
            }

            a = stack_pop(st);
            status = st->last_operation_status;
            if (status.status_code != SUCCESS)
            {
                break;
            }
            stack_push(st, a * b);
            status = st->last_operation_status;
            break;
        case DIV:
            b = stack_pop(st);
            status = st->last_operation_status;
            if (status.status_code != SUCCESS)
            {
                break;
            }

            a = stack_pop(st);
            status = st->last_operation_status;
            if (status.status_code != SUCCESS)
            {
                break;
            }
            stack_push(st, a / b);
            status = st->last_operation_status;
            break;
        case SQRT:
            a = stack_pop(st);
            status = st->last_operation_status;
            if (status.status_code != SUCCESS)
            {
                break;
            }
            stack_push(st, sqrt(a));
            status = st->last_operation_status;
            break;
        case DUMP:
            stack_dump(st, INFO);
            break;
        default:
            break;
    }
    return status;
}
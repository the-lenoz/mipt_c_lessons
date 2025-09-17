#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error_handling/my_assert.hpp"

#include "terminal_decorator.hpp"

int vprintf_escape(const char* color, const char* reset, const char* format, va_list ap)
{
    assert(color != NULL);
    assert(format != NULL);

    int result = 0;

    printf("%s", color);
    result = vprintf(format, ap);
    printf("%s", reset);

    return result;
}


int printf_red(const char* format, ...)
{
    assert(format != NULL);

    va_list ap;
    va_start(ap, format);
    return vprintf_escape(ANSI_COLOR_RED, ANSI_COLOR_RESET, format, ap);
}


int printf_green(const char* format, ...)
{
    assert(format != NULL);

    va_list ap;
    va_start(ap, format);
    return vprintf_escape(ANSI_COLOR_GREEN, ANSI_COLOR_RESET, format, ap);
}


int printf_yellow(const char* format, ...)
{
    assert(format != NULL);

    va_list ap;
    va_start(ap, format);
    return vprintf_escape(ANSI_COLOR_YELLOW, ANSI_COLOR_RESET, format, ap);
}

int printf_blinking(const char* format, ...)
{
    assert(format != NULL);

    va_list ap;
    va_start(ap, format);
    return vprintf_escape(TERMINAL_BLINK_START, TERMINAL_BLINK_RESET, format, ap);
}
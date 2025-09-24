#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error_handling/my_assert.hpp"

#include "terminal_decorator.hpp"

int vfprintf_escape(FILE* fp, const char* color, const char* reset, const char* format, va_list ap)
{
    assert(fp != NULL);
    assert(color != NULL);
    assert(format != NULL);

    int result = 0;

    fprintf(fp, "%s", color);
    result = vfprintf(fp, format, ap);
    fprintf(fp, "%s", reset);

    return result;
}


int fprintf_red(FILE* fp, const char* format, ...)
{
    assert(format != NULL);

    va_list ap;
    va_start(ap, format);
    return vfprintf_escape(fp, ANSI_COLOR_RED, ANSI_COLOR_RESET, format, ap);
}


int fprintf_green(FILE* fp, const char* format, ...)
{
    assert(format != NULL);

    va_list ap;
    va_start(ap, format);
    return vfprintf_escape(fp, ANSI_COLOR_GREEN, ANSI_COLOR_RESET, format, ap);
}


int fprintf_yellow(FILE* fp, const char* format, ...)
{
    assert(format != NULL);

    va_list ap;
    va_start(ap, format);
    return vfprintf_escape(fp, ANSI_COLOR_YELLOW, ANSI_COLOR_RESET, format, ap);
}

int fprintf_blinking(FILE* fp, const char* format, ...)
{
    assert(format != NULL);

    va_list ap;
    va_start(ap, format);
    return vfprintf_escape(fp, TERMINAL_BLINK_START, TERMINAL_BLINK_RESET, format, ap);
}


int printf_red(const char* format, ...)
{
    assert(format != NULL);

    va_list ap;
    va_start(ap, format);
    return vfprintf_escape(stdout, ANSI_COLOR_RED, ANSI_COLOR_RESET, format, ap);
}

int printf_green(const char* format, ...)
{
    assert(format != NULL);

    va_list ap;
    va_start(ap, format);
    return vfprintf_escape(stdout, ANSI_COLOR_GREEN, ANSI_COLOR_RESET, format, ap);
}

int printf_yellow(const char* format, ...)
{
    assert(format != NULL);

    va_list ap;
    va_start(ap, format);
    return vfprintf_escape(stdout, ANSI_COLOR_YELLOW, ANSI_COLOR_RESET, format, ap);
}

int printf_blinking(const char* format, ...)
{
    assert(format != NULL);

    va_list ap;
    va_start(ap, format);
    return vfprintf_escape(stdout, TERMINAL_BLINK_START, TERMINAL_BLINK_RESET, format, ap);
}
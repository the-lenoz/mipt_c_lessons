#ifndef COLORS_DECLARED
#define COLORS_DECLARED

#include <cstdio>
#include <stdarg.h>

#define ANSI_COLOR_RED      "\x1b[31m"
#define ANSI_COLOR_GREEN    "\x1b[32m"
#define ANSI_COLOR_YELLOW   "\x1b[33m"

#define ANSI_COLOR_RESET   "\x1b[0m"

#define TERMINAL_BLINK_START  "\033[5m"
#define TERMINAL_BLINK_RESET  "\033[m"


const int ANSI_COLOR_LEN = sizeof(ANSI_COLOR_RED);
const int ANSI_COLOR_RESET_LEN = sizeof(ANSI_COLOR_RESET);


//----------------------------------------------------------------------------
//! @brief int vprintf_escape(FILE* fp, const char* color, const char* format, va_list ap)
//! This function works like standard vfprintf but prints all in color
//----------------------------------------------------------------------------
int vfprintf_escape(FILE* fp, const char* color, const char* reset, const char* format, va_list ap);

//----------------------------------------------------------------------------
//! @brief int printf_red(char* format, ...)
//! This function works like standard printf but prints all in red color
//----------------------------------------------------------------------------
int printf_red(const char* format, ...);

//----------------------------------------------------------------------------
//! @brief int printf_green(char* format, ...)
//! This function works like standard printf but prints all in green color
//----------------------------------------------------------------------------
int printf_green(const char* format, ...);

//----------------------------------------------------------------------------
//! @brief int printf_yellow(char* format, ...)
//! This function works like standard printf but prints all in yellow color
//----------------------------------------------------------------------------
int printf_yellow(const char* format, ...);

//----------------------------------------------------------------------------
//! @brief int printf_blinking(char* format, ...)
//! This function works like standard printf but prints all blinking
//----------------------------------------------------------------------------
int printf_blinking(const char* format, ...);

//----------------------------------------------------------------------------
//! @brief int printf_red(FILE* fp, char* format, ...)
//! This function works like standard fprintf but prints all in red color
//----------------------------------------------------------------------------
int fprintf_red(FILE* fp, const char* format, ...);

//----------------------------------------------------------------------------
//! @brief int printf_green(FILE* fp, char* format, ...)
//! This function works like standard fprintf but prints all in green color
//----------------------------------------------------------------------------
int fprintf_green(FILE* fp, const char* format, ...);

//----------------------------------------------------------------------------
//! @brief int printf_yellow(FILE* fp, char* format, ...)
//! This function works like standard fprintf but prints all in yellow color
//----------------------------------------------------------------------------
int fprintf_yellow(FILE* fp, const char* format, ...);

//----------------------------------------------------------------------------
//! @brief int printf_blinking(FILE* fp, char* format, ...)
//! This function works like standard fprintf but prints all blinking
//----------------------------------------------------------------------------
int fprintf_blinking(FILE* fp, const char* format, ...);

#endif
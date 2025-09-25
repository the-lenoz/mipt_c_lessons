#include <cstdarg>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "my_assert.hpp"
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <execinfo.h>

#include "status.hpp"
#include "terminal_decorator.hpp"

#include "logger.hpp"


static LoggerProperties logger_properties = 
{
    .log_targets = NULL,
    .log_targets_count = 0,
    .logging_on = 0,
    .filename = NULL
};

char timestamp_buffer[MAX_LOGGER_TIMESTAMP_LEN + 1] = {0};
char logger_annotation_buffer[MAX_LOGGER_ANNOTATION_LEN + 1] = {0};


int LOG_START(const char* filename, int log_targets_count, LogTarget* log_targets)
{
    assert(filename != NULL);
    assert(log_targets != NULL);

    size_t file_path_len = 0;

    if (logger_properties.logging_on)
    {
        print_error(MAKE_ERROR_STRUCT(LOG_START_ERROR));
        return -1;
    }
    if (log_targets_count == 0)
    {
        print_error(MAKE_ERROR_STRUCT(LOG_TARGET_EMPTY_ERROR));
        return -1;
    }

    logger_properties.log_targets = log_targets;
    logger_properties.log_targets_count = log_targets_count;

    void* bt_buf[1] = {NULL};
    backtrace(bt_buf, 1); // MAKE SURE THAT LIBGCC IS LOADED

    for (int i = 0; i < log_targets_count; ++i)
    {
        if (logger_properties.log_targets[i].file_path == NULL) // STDOUT
        {
            logger_properties.log_targets[i].type = STDOUT;
            logger_properties.log_targets[i].fp = NULL;
            setbuf(stdout, NULL);
            setbuf(stderr, NULL);
            continue;
        }   

        file_path_len = strlen(logger_properties.log_targets[i].file_path);
        if (strcmp(&(logger_properties.log_targets[i].file_path[file_path_len - 1 -strlen(HTML_FILE_SUFFIX)]), 
                HTML_FILE_SUFFIX))
        {
            logger_properties.log_targets[i].type = HTML;
        }
        else
        {
            logger_properties.log_targets[i].type = TEXT;
        }

        logger_properties.log_targets[i].fp = fopen(logger_properties.log_targets[i].file_path, "a");

        if (logger_properties.log_targets[i].fp == NULL)
        {
            print_error(MAKE_ERROR_STRUCT(CANNOT_OPEN_FILE_ERROR));
            return -1;
        }

        setbuf(logger_properties.log_targets[i].fp, NULL);
    }

    atexit(LOG_STOP);

    logger_properties.filename = filename;

    logger_properties.logging_on = 1;

    LOG_MESSAGE_F(INFO, "Логгер запущен");
    return 0;
}

int LOG_MESSAGE_F(LogMessageType message_type, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    return vlog_message_f(message_type, format, args);
}

int LOG_ERROR(StatusData error_data)
{
    const char* error_description = get_error_description(error_data.status_code);
    void* backtrace_buffer[BACKTRACE_BUFFER_SIZE] = {};
    FILE* fp = NULL;

    backtrace(backtrace_buffer, BACKTRACE_BUFFER_SIZE);

    if (LOG_MESSAGE_F(ERROR, 
        "ОШИБКА: '%s'\n'%s'\nПроизошла в файле: '%s', в функции '%s', на строке номер %d\nТрассировка:", 
        error_description, error_data.error_description, error_data.filename, error_data.func_name, error_data.line_number) == -1)
    {
        return -1;
    }

    int backtrace_size = 0;
    for (;backtrace_size <= BACKTRACE_BUFFER_SIZE && backtrace_buffer[backtrace_size] != NULL; ++backtrace_size)
    {
        // do nothing
    }

    for (int i = 0; i < logger_properties.log_targets_count; ++i)
    {
        fp = logger_properties.log_targets[i].fp;
        if (logger_properties.log_targets[i].type == HTML)
        {
            if (fprintf(logger_properties.log_targets[i].fp, "<pre style=\"color:red\">\n") == -1)
            {
                return -1;
            }
        }
        else if (logger_properties.log_targets[i].type == STDOUT)
        {
            fp = stderr;
        }

        backtrace_symbols_fd(backtrace_buffer, backtrace_size, 
            fileno(fp));

        if (logger_properties.log_targets[i].type == HTML)
        {
            if (fprintf(logger_properties.log_targets[i].fp, "</pre>") == -1)
            {
                return -1;
            }
        }
    }

    return 0;
}

void LOG_STOP(void)
{
    if (!logger_properties.logging_on) return;

    LOG_MESSAGE_F(INFO, "Завершение");
    
    for (int i = 0; i < logger_properties.log_targets_count; ++i)
    {
        if (logger_properties.log_targets[i].type == HTML || logger_properties.log_targets[i].type == TEXT)
        {
            fclose(logger_properties.log_targets[i].fp);
        }
    }
    
}

const char* get_log_message_type_str(LogMessageType message_type)
{
    switch (message_type)
    {
        case INFO:
            return "INFO";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
        default:
            return "INFO";
    }
}

int write_log_annotation(LogTarget target, LogMessageType message_type)
{
    assert(logger_properties.filename != NULL);
    assert(target.type == STDOUT || target.fp != NULL);

    time_t timer = {};
    
    memset(timestamp_buffer, '\0', MAX_LOGGER_TIMESTAMP_LEN);
    memset(logger_annotation_buffer, '\0', MAX_LOGGER_ANNOTATION_LEN);

    struct tm* tm_info;

    timer = time(NULL);
    tm_info = localtime(&timer);

    strftime(timestamp_buffer, MAX_LOGGER_TIMESTAMP_LEN, " [%Y-%m-%d %H:%M:%S] - [", tm_info);

    FILE* fp = target.fp;

    if (target.type == STDOUT)
    {
        switch (message_type)
        {
            case INFO:
                fp = stdout;
                break;
            case WARNING:
                fp = stdout;
                strcat(logger_annotation_buffer, ANSI_COLOR_YELLOW);
                break;
            case ERROR:
                fp = stderr;
                strcat(logger_annotation_buffer, ANSI_COLOR_RED);
                break;
            default:
                fp = stdout;
        }
    }

    strcat(logger_annotation_buffer, logger_properties.filename);
    strncat(logger_annotation_buffer, timestamp_buffer, MAX_LOGGER_TIMESTAMP_LEN);
    strcat(logger_annotation_buffer, get_log_message_type_str(message_type));
    strcat(logger_annotation_buffer, "]: ");

    return fwrite(logger_annotation_buffer, sizeof(char), strlen(logger_annotation_buffer), fp) == 0 ? -1 : 0;
}

int vlog_to_target(LogTarget target, LogMessageType message_type, const char* format, va_list va_args)
{
    assert(format != NULL);
    assert(target.type == STDOUT || target.fp != NULL);

    FILE* fp = target.fp;

    if (target.type == STDOUT)
    {
        switch (message_type)
        {
            case INFO:
                fp = stdout;
                break;
            case WARNING:
                fp = stdout;
                strcat(logger_annotation_buffer, ANSI_COLOR_YELLOW);
                break;
            case ERROR:
                fp = stderr;
                strcat(logger_annotation_buffer, ANSI_COLOR_RED);
                break;
            default:
                fp = stdout;
        }
    }
    else if (target.type == HTML)
    {
        const char* color = "black";

        if (message_type == WARNING)
        {
            color = "yellow";
        }
        else if (message_type == ERROR)
        {
            color = "red";
        }

        if (fprintf(target.fp, "<pre><div style=\"color:") == -1)
        {
            return -1;
        }

        if (fprintf(target.fp, "%s", color) == -1)
        {
            return -1;
        }

        if (fprintf(target.fp, "\">") == -1)
        {
            return -1;
        }
    }

    if (write_log_annotation(target, message_type) == -1) 
    {
        return -1;
    }

    if (vfprintf(fp, format, va_args) == -1)
    {
        return -1;
    }

    if (target.type == STDOUT)
    {
        
    }
    switch (target.type)
    {
        case STDOUT:
            if (fprintf(fp, ANSI_COLOR_RESET) == 0)
            {
                return -1;
            }

            if (fprintf(fp, "\n") == 0)
            {
                return -1;
            }
            break;
        case TEXT:
            if (fprintf(fp, "\n") == 0)
            {
                return -1;
            }
            break;
        case HTML:
            if (fprintf(fp, "</div></pre><br>") == 0)
            {
                return -1;
            }
            break;
        default:
            break;
    }
    return 0;
}

int vlog_message_f(LogMessageType message_type, const char* format, va_list va_args)
{
    assert(format != NULL);
    va_list va_args_copy;

    int log_status_code = 0;
    
    if (!logger_properties.logging_on)
    {
        print_error(MAKE_ERROR_STRUCT(LOGGER_OFF_WRITE_ERROR));
        return -1;
    }

    for (int i = 0; i < logger_properties.log_targets_count; ++i)
    {
        va_copy(va_args_copy, va_args);
        if (vlog_to_target(logger_properties.log_targets[i], message_type, format, va_args_copy) == -1)
        {
            print_error(MAKE_ERROR_STRUCT(LOG_WRITE_ERROR));
            log_status_code = -1;
        }
    }
    return log_status_code;
}
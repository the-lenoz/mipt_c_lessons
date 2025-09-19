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
            logger_properties.log_targets[i].file_descriptor = STDOUT_FILENO;
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

        logger_properties.log_targets[i].file_descriptor = open(logger_properties.log_targets[i].file_path, O_WRONLY | O_CREAT | O_APPEND, 0640);

        if (logger_properties.log_targets[i].file_descriptor == -1)
        {
            print_error(MAKE_ERROR_STRUCT(CANNOT_OPEN_FILE_ERROR));
            return -1;
        }
    }

    atexit(&LOG_STOP);

    logger_properties.filename = filename;

    logger_properties.logging_on = 1;

    LOG_MESSAGE("Логгер запущен", INFO);
    return 0;
}

int LOG_MESSAGE(const char* message, LogMessageType message_type)
{
    assert(message != NULL);

    int log_status_code = 0;
    
    if (!logger_properties.logging_on)
    {
        print_error(MAKE_ERROR_STRUCT(LOGGER_OFF_WRITE_ERROR));
        return -1;
    }

    for (int i = 0; i < logger_properties.log_targets_count; ++i)
    {
        switch (logger_properties.log_targets[i].type)
        {
            case STDOUT:
                if (log_to_stdout(logger_properties.log_targets[i].file_descriptor, message, message_type) == -1)
                {
                    print_error(MAKE_ERROR_STRUCT(LOG_WRITE_ERROR));
                    log_status_code = -1;
                }   
                break;
            case TEXT:
                if (log_to_text_file(logger_properties.log_targets[i].file_descriptor, message, message_type) == -1)
                {
                    print_error(MAKE_ERROR_STRUCT(LOG_WRITE_ERROR));
                    log_status_code = -1;
                }
                break;
            case HTML:
                if (log_to_html_file(logger_properties.log_targets[i].file_descriptor, message, message_type) == -1)
                {
                    print_error(MAKE_ERROR_STRUCT(LOG_WRITE_ERROR));
                    log_status_code = -1;
                }
                break;
            default:
                break;
        }
    }
    return log_status_code;    
}

int LOG_ERROR(StatusData error_data)
{
    const char* error_description = get_error_description(error_data.status_code);
    char line_number_string[MAX_LINE_NUMBER_STR_LEN] = {0};
    void * backtrace_buffer[BACKTRACE_BUFFER_SIZE] = {0};

    backtrace(backtrace_buffer, BACKTRACE_BUFFER_SIZE);

    sprintf(line_number_string, "%d", error_data.line_number);

    LOG_MESSAGE_AND_EXIT_ON_ERROR("ОШИБКА:");
    LOG_MESSAGE_AND_EXIT_ON_ERROR(error_description);
    LOG_MESSAGE_AND_EXIT_ON_ERROR("Произошла в файле:");
    LOG_MESSAGE_AND_EXIT_ON_ERROR(error_data.filename);
    LOG_MESSAGE_AND_EXIT_ON_ERROR(error_data.error_description);
    LOG_MESSAGE_AND_EXIT_ON_ERROR("В функции:");
    LOG_MESSAGE_AND_EXIT_ON_ERROR(error_data.func_name);
    LOG_MESSAGE_AND_EXIT_ON_ERROR("На строке номер:");
    LOG_MESSAGE_AND_EXIT_ON_ERROR(line_number_string);
    LOG_MESSAGE_AND_EXIT_ON_ERROR("ТРАССИРОВКА (последний вызов указан последним):");

    int backtrace_size = 0;
    for (;backtrace_size <= BACKTRACE_BUFFER_SIZE && backtrace_buffer[backtrace_size] != NULL; ++backtrace_size)
    {
        // do nothing
    }

    for (int i = 0; i < logger_properties.log_targets_count; ++i)
    {
        if (logger_properties.log_targets[i].type == HTML)
        {
            if (write(logger_properties.log_targets[i].file_descriptor, "<pre style=\"color:red\">\n", 23) == -1)
            {
                return -1;
            }
        }
        backtrace_symbols_fd(backtrace_buffer, backtrace_size, logger_properties.log_targets[i].file_descriptor);
        if (logger_properties.log_targets[i].type == HTML)
        {
            if (write(logger_properties.log_targets[i].file_descriptor, "</pre>", 6) == -1)
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

    LOG_MESSAGE("Завершение", INFO);
    
    for (int i = 0; i < logger_properties.log_targets_count; ++i)
    {
        if (logger_properties.log_targets[i].type == HTML || logger_properties.log_targets[i].type == TEXT)
        {
            fsync(logger_properties.log_targets[i].file_descriptor);
            close(logger_properties.log_targets[i].file_descriptor);
        }
    }
    //free((void *)logger_properties.log_targets);
    
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

int write_log_annotation(LogMessageType message_type, int file_descriptor)
{
    assert(logger_properties.filename != NULL);

    time_t timer;
    
    memset(timestamp_buffer, '\0', MAX_LOGGER_TIMESTAMP_LEN);
    memset(logger_annotation_buffer, '\0', MAX_LOGGER_ANNOTATION_LEN);

    struct tm* tm_info;

    timer = time(NULL);
    tm_info = localtime(&timer);

    strftime(timestamp_buffer, MAX_LOGGER_TIMESTAMP_LEN, " [%Y-%m-%d %H:%M:%S] - [", tm_info);

    if (file_descriptor == STDOUT_FILENO && message_type != INFO)
    {
        if (message_type == WARNING)
        {
            strcat(logger_annotation_buffer, ANSI_COLOR_YELLOW);
        }
        else if (message_type == ERROR)
        {
            strcat(logger_annotation_buffer, ANSI_COLOR_RED);
        }
    }

    strcat(logger_annotation_buffer, logger_properties.filename);
    strncat(logger_annotation_buffer, timestamp_buffer, MAX_LOGGER_TIMESTAMP_LEN);
    strcat(logger_annotation_buffer, get_log_message_type_str(message_type));
    strcat(logger_annotation_buffer, "]: ");

    return write(file_descriptor, logger_annotation_buffer, strlen(logger_annotation_buffer)) == -1 ? -1 : 0;
}


int log_to_text_file(int file_descriptor, const char* message, LogMessageType message_type)
{
    assert(message != NULL);
    assert(file_descriptor != -1);

    size_t message_len = strlen(message);

    if (write_log_annotation(message_type, file_descriptor) == -1)
    {
        return -1;
    }

    if (write(file_descriptor, message, message_len) == -1)
    {
        return -1;
    }

    if (write(file_descriptor, "\n", 1) == -1)
    {
        return -1;
    }

    return 0;
}

int log_to_html_file(int file_descriptor, const char* message, LogMessageType message_type)
{
    assert(message != NULL);
    assert(file_descriptor != -1);

    size_t message_len = strlen(message);

    const char* color = "black";

    if (message_type == WARNING)
    {
        color = "yellow";
    }
    else if (message_type == ERROR)
    {
        color = "red";
    }

    if (write(file_descriptor, "<pre><div style=\"color:", 23) == -1)
    {
        return -1;
    }

    if (write(file_descriptor, color, strlen(color)) == -1)
    {
        return -1;
    }

    if (write(file_descriptor, "\">", 2) == -1)
    {
        return -1;
    }

    if (write_log_annotation(message_type, file_descriptor) == -1)
    {
        return -1;
    }

    if (write(file_descriptor, message, message_len) == -1)
    {
        return -1;
    }

    if (write(file_descriptor, "</div></pre><br>", 16) == -1)
    {
        return -1;
    }

    return 0;
}

int log_to_stdout(int file_descriptor, const char* message, LogMessageType message_type)
{
    assert(message != NULL);

    size_t message_len = strlen(message);    

    if (write_log_annotation(message_type, file_descriptor) == -1)
    {
        return -1;
    }

    if (write(file_descriptor, message, message_len) == -1)
    {
        return -1;
    }

    if (write(file_descriptor, ANSI_COLOR_RESET, ANSI_COLOR_RESET_LEN) == -1)
    {
        return -1;
    }

    if (write(file_descriptor, "\n", 1) == -1)
    {
        return -1;
    }

    return 0;
}

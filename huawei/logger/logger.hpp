#ifndef LOGGER_DECLARED
#define LOGGER_DECLARED

#define MAX_LOGGER_ANNOTATION_LEN   128
#define MAX_LOGGER_FILENAME_ANNOTATION_LEN  32
#define MAX_LOGGER_TIMESTAMP_LEN    31
#define MAX_LINE_NUMBER_STR_LEN     32

#define HTML_FILE_SUFFIX ".html"

#include <stdio.h>

#include "status.hpp"


enum LogMessageType
{
    INFO,
    WARNING,
    ERROR
};

enum LogTargetType
{
    HTML,
    TEXT,
    STDOUT
};

struct LogTarget
{
    const char* file_path;
    FILE* fp;
    LogTargetType type;
};

struct LoggerProperties
{
    LogTarget* log_targets;
    int log_targets_count;
    int logging_on;
    const char* filename;
};

//--------------------------------------------------------------
//! @brief int LOG_START(const char* filename, int log_targets_count, LogTarget* log_targets)
//! Function opens log file and starts logging
//! @param [in] filename - this executable name
//! @param [in] log_targets_count - len of log_targets list
//! @param [in] log_targets - log targets list
//! @return 0 if started successfully else -1
//--------------------------------------------------------------
int LOG_START(const char* filename, int log_targets_count, LogTarget* log_targets);

//--------------------------------------------------------------
//! @brief int LOG_MESSAGE_F(const char* message)
//! Function logs message
//! @param [in] message_type
//! @param [in] format
//! @param [in] ...
//! @return 0 if logged successfully else -1
//--------------------------------------------------------------
int LOG_MESSAGE_F(LogMessageType message_type, const char* format, ...);

//--------------------------------------------------------------
//! @brief int LOG_ERROR(StatusData error_data)
//! Function logs error
//! @param [in] error_data
//! @return 0 if logged successfully else -1
//--------------------------------------------------------------
int LOG_ERROR(StatusData error_data);

//--------------------------------------------------------------
//! @brief int LOG_STOP(const char* log_file_path)
//! Function opens log file and starts logging
//! @param [in] log_file_path - path to log file
//--------------------------------------------------------------
void LOG_STOP(void);

//--------------------------------------------------------------
//! @brief int get_log_message_type_str(const char* log_file_path)
//! Function to get log message type string by enum
//! @param [in] message_type
//! @return message type string
//--------------------------------------------------------------
const char* get_log_message_type_str(LogMessageType message_type);

//--------------------------------------------------------------
//! @brief int write_log_annotation(const char* log_file_path)
//! Function logs logger annotation
//! @param [in] target
//! @param [in] message_type
//! @return -1 on error else 0
//--------------------------------------------------------------
int write_log_annotation(LogTarget target, LogMessageType message_type);

int vlog_message_f(LogMessageType message_type, const char* format, va_list va_args);

int vlog_to_target(LogTarget target, LogMessageType message_type, const char* format, va_list va_args);


#endif // LOGGER_DECLARED
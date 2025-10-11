#ifndef ERRORS_DECLARED
#define ERRORS_DECLARED

#include <stdio.h>


#define MAKE_ERROR_STRUCT(stat_code) {\
            .status_code = stat_code,\
            .filename = __FILE__,\
            .func_name = __func__,\
            .line_number = __LINE__\
        }

#define MAKE_EXTENDED_ERROR_STRUCT(stat_code, description) {\
            .status_code = stat_code,\
            .filename = __FILE__,\
            .func_name = __func__,\
            .line_number = __LINE__,\
            .error_description = description,\
        }

#define MAKE_CUSTOM_ERROR_STRUCT(handler, data) {\
            .status_code = CUSTOM_DATA,\
            .filename = __FILE__,\
            .func_name = __func__,\
            .line_number = __LINE__,\
            .custom_status_data = data,\
            .custom_status_data_handler = handler\
        }
        
#define MAKE_SUCCESS_STRUCT(data) {.status_code=SUCCESS}

#define BACKTRACE_BUFFER_SIZE 512


enum StatusCode
{
    SUCCESS,
    CANNOT_OPEN_FILE_ERROR,
    CANNOT_ALLOCATE_MEMORY_ERROR,
    TEST_FAILED_ERROR,
    LOG_TARGET_EMPTY_ERROR,
    LOG_WRITE_ERROR,
    LOGGER_OFF_WRITE_ERROR,
    LOG_START_ERROR,
    CANNOT_GET_LOG_TARGETS,
    ASSERTION_FAILED,
    INVALID_FUNCTION_PARAM,
    CUSTOM_DATA
};


struct StatusData
{
    StatusCode status_code;
    const char* filename;
    const char* func_name;
    int line_number;
    const char* error_description;
    void* custom_status_data;
    int (*custom_status_data_handler) (FILE* fp, StatusData error_data);
};

typedef int (*error_dumper) (FILE* fp, StatusData error_data);

//------------------------------------------------------------------------
//! @brief void fprint_error(FILE* fp, StatusData error)
//! Function pretty prints error structure
//! @param [in] error - structure
//------------------------------------------------------------------------
void fprint_error(FILE* fp, StatusData error_data);

//------------------------------------------------------------------------
//! @brief void print_error(StatusData error)
//! Function pretty prints error structure
//! @param [in] error - structure
//------------------------------------------------------------------------
void print_error(StatusData error);



void print_back_trace(void);

//------------------------------------------------------------------------
//! @brief void get_error_description(StatusCode status_code)
//! Function to get error description by its code
//! @param [in] status_code
//! @return error description
//------------------------------------------------------------------------
const char* get_error_description(StatusCode status_code);

#endif // ERRORS_DECLARED
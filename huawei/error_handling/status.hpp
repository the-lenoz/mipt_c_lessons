#ifndef ERRORS_DECLARED
#define ERRORS_DECLARED


#define MAKE_ERROR_STRUCT(stat_code) {\
            .status_code = stat_code,\
            .filename = __FILE__,\
            .func_name = __func__,\
            .line_number = __LINE__\
        }
        
#define MAKE_SUCCESS_STRUCT(data) {.status_code=SUCCESS, .success_data=data}

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
    ASSERTION_FAILED
};


struct StatusData
{
    StatusCode status_code;
    const char* filename;
    const char* func_name;
    int line_number;
    const char* error_description;
    void* success_data;
};

//------------------------------------------------------------------------
//! @brief void print_error(StatusData error)
//! Function pretty prints error by its ERRNO
//! @param [in] error - ERRNO
//------------------------------------------------------------------------
void print_error(StatusData error);

void print_back_trace(void);

//------------------------------------------------------------------------
//! @brief void print_error(StatusData error)
//! Function to get error description by its code
//! @param [in] status_code
//! @return error description
//------------------------------------------------------------------------
const char* get_error_description(StatusCode status_code);

#endif // ERRORS_DECLARED
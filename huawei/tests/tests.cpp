#include "tests.hpp"
#include "logger/logger.hpp"
#include "status.hpp"
#include "strings/mystr.hpp"
#include "error_handling/my_assert.hpp"
#include <cstddef>


typedef int (*test_func_t)(void);

typedef struct 
{
    const char* test_name;
    test_func_t test;
} test_struct;

test_struct tests[] = {
    {
        .test_name = "Test strcmp",
        .test=test_strcmp
    },
    {
        .test_name = "Test strcat",
        .test=test_strcat
    },
};
size_t tests_len = sizeof(tests) / sizeof(tests[0]);


int main(int argc, char** argv)
{
    (void)(argc);

    assert(argv != NULL);
    assert(argv[0] != NULL);

    LogTarget log_targets[] = 
    {
        {}, // stdout
        {"/tmp/log.html"}
    };
    int log_targets_count = sizeof(log_targets) / sizeof(log_targets[0]);

    LOG_START(argv[0], log_targets_count, log_targets);
    int tests_failed = 0, current_test_result = 0;
    for (size_t i = 0; i < tests_len; ++i)
    {
        current_test_result = tests[i].test();
        tests_failed += current_test_result;
        if (current_test_result != 0)
        {
            LOG_ERROR(MAKE_EXTENDED_ERROR_STRUCT(TEST_FAILED_ERROR, tests[i].test_name));
        } 
        else
        {
            LOG_MESSAGE_F(INFO, "Test PASSED");
        }
    }
    return tests_failed;
}


int test_fgetc()
{
    return 0;
}


int test_getchar()
{
    return 0;
}

int test_putchar()
{
    return 0;
}


int test_fputc()
{
    return 0;
}


int test_fgets()
{

    return 0;
}

int test_fputs()
{
    return 0;
}


int test_puts()
{
    return 0;
}

int test_strcmp()
{
    if (mc_strcmp("Hello", "Hello")) return 1;
    if (!mc_strcmp("Hello!", "Hello-")) return 1;
    if (!mc_strcmp("Hello!", "Hello")) return 1;
    return 0;
}

int test_strchr()
{
    return 0;
}


int test_strlen()
{
    return 0;
}


int test_strcpy()
{
    return 0;
}

int test_strncpy()
{
    return 0;
}


int test_strcat()
{
    return 0;
}

int test_strncat()
{
    return 0;
}


int test_strdup()
{
    return 0;
}


int test_getline()
{
    return 0;
}

int test_strtok()
{
    return 0;
}


int test_strstr()
{
    return 0;
}


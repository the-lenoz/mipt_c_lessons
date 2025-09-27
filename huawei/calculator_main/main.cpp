#include "calculator/calculator.hpp"
#include "stack/stack.hpp"

#include "error_handling/my_assert.hpp"
#include "logger.hpp"
#include "status.hpp"

LogTarget log_targets[] = {
    {
        .file_path=NULL
    },
};
const size_t log_tergets_len = sizeof(log_targets) / sizeof(log_targets[0]);

int main(int argc, char** argv)
{
    assert(argc != 0);
    assert(argv != NULL);
    assert(argv[0] != NULL);

    LOG_START(argv[0], (int)log_tergets_len, log_targets);

    Stack st = {};
    stack_init(&st);

    StatusData status = run_loop(&st);
    if (status.status_code != SUCCESS)
    {
        LOG_ERROR(status);
    }

    stack_destroy(&st);

    return 0;
}
#ifndef TEST_TASK_UTILS_H
#define TEST_TASK_UTILS_H

#include <utility>
#include <string>

enum class error_codes {
    WrongNumberOfArguments = 1,
    UnableToOpenInputFile = 2,
    PipelineInternalError = 3,
    WrongOutputFileExt = 4,
};

[[nodiscard]] std::tuple<std::string, std::string, bool> parse_args(int argc, char** argv);

#endif //TEST_TASK_UTILS_H

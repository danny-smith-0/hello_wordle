#ifndef WORDLE_PRINT_BUCKETS_H
#define WORDLE_PRINT_BUCKETS_H

#include <string>

#include <wordle_defs.h>

namespace wordle::print
{
    void print_colored_buckets_to_json(std::map<std::string, colored_buckets_t> answers, std::string guess);

    std::string buckets_to_json_string(colored_buckets_t colored_buckets);
}

#endif //WORDLE_PRINT_BUCKETS_H

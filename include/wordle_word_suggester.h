#ifndef WORDLE_WORD_SUGGESTER
#define WORDLE_WORD_SUGGESTER

#include <map>
#include <string>
#include <vector>

#include <wordle_defs.h>
#include <wordle_inputs.h>

namespace wordle
{
    class WordSuggester
    {
    public:
        static constexpr size_t _max_index = 4;
        void print_words(int words_per_row = 1, words_t words = words_t());
        std::string print_buckets(colored_buckets_t const& results);

        std::map<std::string, colored_buckets_t> suggest(Inputs const& inputs, bool suggest_guesses = true);

        std::map<std::string, colored_buckets_t> collect_buckets(Inputs const& inputs, bool suggest_guesses = true);
        std::map<std::string, colored_buckets_t> collect_buckets(Inputs const& inputs, words_t words);
        colored_buckets_t calc_buckets(std::string guess, words_t words);

    // private:
        // functions
        static std::string find_required_letters(words_t words);
        static words_t subtract_required_letters(words_t const& words, std::string required_letters, std::string* unspecified_letters = 0);
    };
}

#endif  // WORDLE_WORD_SUGGESTER

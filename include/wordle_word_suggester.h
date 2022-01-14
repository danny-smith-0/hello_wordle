#ifndef WORDLE_WORD_SUGGESTER
#define WORDLE_WORD_SUGGESTER

#include <vector>
#include <string>

namespace wordle
{

    class WordSuggester
    {
    public:
        void load_words();
        void print_words(int words_per_row = 1, std::vector<std::string> words = std::vector<std::string>());

        size_t remove_words_with_letter(char letter);
        size_t remove_words_with_letter_position(char letter, size_t position);

        std::vector<std::string> _valid_answers;
        std::vector<std::string> _valid_guesses;

    };
}

#endif  // WORDLE_WORD_SUGGESTER

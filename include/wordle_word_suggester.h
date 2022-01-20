#ifndef WORDLE_WORD_SUGGESTER
#define WORDLE_WORD_SUGGESTER

#include <vector>
#include <string>

namespace wordle
{

    class WordSuggester
    {
    public:
        WordSuggester::WordSuggester() { this->load_words(); }
        void print_words(int words_per_row = 1, std::vector<std::string> words = std::vector<std::string>());

        void black_letter(char letter);
        void green_letter(char letter, size_t correct_position);
        void yellow_letter(char letter, size_t wrong_position);

        void suggest();

    private:
        // functions
        void load_words();

        size_t remove_words_with_letter(char letter);
        size_t remove_words_with_letter_position(char letter, size_t position);
        size_t remove_words_without_letter(char required_letter);
        size_t remove_words_without_letter_position(char required_letter, size_t position);

        // variables
        std::vector<std::string> _valid_answers;
        std::vector<std::string> _valid_guesses;

        std::string _required_letters;
    };
}

#endif  // WORDLE_WORD_SUGGESTER

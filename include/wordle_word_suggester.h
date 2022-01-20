#ifndef WORDLE_WORD_SUGGESTER
#define WORDLE_WORD_SUGGESTER

#include <vector>
#include <string>
#include <map>
#include <optional>

namespace wordle
{

    class WordSuggester
    {
    public:
        static constexpr size_t _max_index = 4;
        WordSuggester::WordSuggester() { this->load_words(); }
        void print_words(int words_per_row = 1, std::vector<std::string> words = std::vector<std::string>());

        void black_letter(char letter);
        void black_duplicate_letter(char letter);
        void green_letter(char letter, size_t correct_index);
        void yellow_letter(char letter, size_t wrong_index);

        void suggest();

        std::string required_letters()
        {
            std::string out;
            for (auto [letter, opt_pos] : _required_letters_and_positions)
                out += letter;
            return out;
        }

    // private:
        // functions
        void load_words();

        size_t remove_words_with_letter(char letter);
        size_t remove_words_with_letter_index(char letter, size_t index);
        size_t remove_words_without_letter(char required_letter);
        size_t remove_words_without_letter_index(char required_letter, size_t index);

        // variables
        std::vector<std::string> _valid_answers;
        std::vector<std::string> _valid_guesses;

        std::map<std::string, std::optional<size_t>> _required_letters_and_positions;
        std::string _required_letters;
    };
}

#endif  // WORDLE_WORD_SUGGESTER

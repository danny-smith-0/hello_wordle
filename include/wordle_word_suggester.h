#ifndef WORDLE_WORD_SUGGESTER
#define WORDLE_WORD_SUGGESTER

#include <map>
#include <string>
#include <vector>

namespace wordle
{

    class WordSuggester
    {
    public:
        static constexpr size_t _max_index = 4;
        WordSuggester::WordSuggester() { this->load_words(); }
        void print_words(int words_per_row = 1, std::vector<std::string> words = std::vector<std::string>());

        void black_letter(char letter);
        void green_letter(char letter, size_t correct_index);
        void yellow_letter(char letter, size_t wrong_index);

        void suggest();

        double how_many_words_remain_after_guess(std::string guess, std::vector<std::string> words);

    // private:
        // functions
        void load_words();

        size_t remove_words_with_letter(char letter);
        size_t remove_words_with_letter_index(char letter, size_t index);
        size_t remove_words_without_letter(char required_letter);
        size_t remove_words_without_letter_index(char required_letter, size_t index);

        // for suggesting
        void subtract_required_letters(std::vector<std::string> const& words, std::string required_letters, std::string* unspecified_letters, std::vector<std::string>* unspecified_letters_by_word);
        void score_words_by_letter_scores(std::vector<std::string> const& unspecified_letters_by_word, std::vector<std::string> const& original_words, std::map<char, size_t> const& letter_count);

        // variables
        std::vector<std::string> _valid_answers_orig;    // As loaded from file
        std::vector<std::string> _valid_answers_trimmed;         // Allows removing
        std::vector<std::string> _valid_guesses_orig;         // As loaded from file
        std::vector<std::string> _valid_guesses_trimmed; // Allows removing

        std::string _required_letters;
        std::string _excluded_letters;
    };
}

#endif  // WORDLE_WORD_SUGGESTER

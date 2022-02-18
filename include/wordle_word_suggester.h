#ifndef WORDLE_WORD_SUGGESTER
#define WORDLE_WORD_SUGGESTER

#include <map>
#include <string>
#include <vector>

namespace wordle
{
    typedef std::vector<std::string> bucket_t;
    typedef std::vector<std::string> words_t;
    // Each bucket of possible answers is painted with five ordered colors, the key-string of this map
    typedef std::map<std::string, bucket_t> colored_buckets_t;

    class WordSuggester
    {
    public:
        static constexpr size_t _max_index = 4;
        WordSuggester::WordSuggester() { this->load_words(); }
        void print_words(int words_per_row = 1, words_t words = words_t());
        std::string print_buckets(colored_buckets_t const& results);

        void black_letter(char letter);
        void black_duplicate_letter(char letter, size_t green_index);
        void green_letter(char letter, size_t correct_index);
        void yellow_letter(char letter, size_t wrong_index, bool is_duplicate = false);

        void suggest();

        void collect_buckets();
        std::map<std::string, colored_buckets_t> collect_buckets(words_t words);
        colored_buckets_t calc_buckets(std::string guess, words_t words);

    // private:
        // functions
        void load_words();

        size_t remove_words_with_letter(char letter);
        size_t remove_words_with_letter_index(char letter, size_t index);
        size_t remove_words_without_letter(char required_letter, bool duplicate = false);
        size_t remove_words_without_letter_index(char required_letter, size_t index);

        // for suggesting
        void subtract_required_letters(words_t const& words, std::string required_letters, std::string* unspecified_letters, words_t* unspecified_letters_by_word);
        void score_words_by_letter_scores(words_t const& unspecified_letters_by_word, words_t const& original_words, std::map<char, size_t> const& letter_count);

        // variables
        words_t _valid_answers_orig;    // As loaded from file
        words_t _valid_answers_trimmed; // Allows removing
        words_t _valid_guesses_orig;    // As loaded from file
        words_t _valid_guesses_trimmed; // Allows removing

        std::string _required_letters;
        std::string _excluded_letters;
    };
}

#endif  // WORDLE_WORD_SUGGESTER

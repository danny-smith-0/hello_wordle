#ifndef WORDLE_INPUTS_H
#define WORDLE_INPUTS_H

#include <wordle_defs.h>

namespace wordle
{
    enum class GameType : int
    {
        wordle,     // english
        quordle,    // english
        wordle_es,  // spanish
    };

    class Inputs
    {
        public:
        Inputs::Inputs(GameType game_type = GameType::wordle) { this->load_words(game_type); }
        void load_words(GameType game_type = GameType::wordle);

        void B(char letter);
        void black_duplicate_letter(char letter, size_t green_index);
        void G(char letter, size_t correct_index);
        void Y(char letter, size_t wrong_index, bool is_duplicate = false);

        size_t remove_words_with_letter(char letter);
        size_t remove_words_with_letter_index(char letter, size_t index);
        size_t remove_words_without_letter(char required_letter, bool duplicate = false);
        size_t remove_words_without_letter_index(char required_letter, size_t index);

        // variables
        words_t _valid_answers_orig;    // As loaded from file
        words_t _valid_answers_trimmed; // Allows removing
        words_t _valid_guesses_orig;    // As loaded from file
        words_t _valid_guesses_trimmed; // Allows removing
        //

        std::string _required_letters;
        std::string _excluded_letters;
    };
}

#endif  // WORDLE_INPUTS_H

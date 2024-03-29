#include <wordle_inputs.h>

#include <string>    // string and getline
#include <fstream>   // std::ifstream (and ofstream)
#include <iostream>  // cout and cin
#include <sstream>   // stringstream
#include <wordle_defs.h>


#define HARD_MODE 0
#define SPANISH 0

using namespace wordle;


void Inputs::load_words(GameType game_type)
{
    // Get valid answers
    std::ifstream file_stream;
    if (game_type == GameType::wordle_es)
        file_stream.open("../include/wordle_es.txt");
    else if (game_type == GameType::nerdle)
        file_stream.open("../nerdle/solutions.txt");
    else
        file_stream.open("../include/valid_answers.txt");

    std::string line;
    while (std::getline(file_stream, line))
        _valid_answers_orig.push_back(line);
    file_stream.close();

    // Get valid guesses
    if (game_type == GameType::wordle_es || game_type == GameType::nerdle)
        _valid_guesses_orig = _valid_answers_orig;
    else
    {
        file_stream.open("../include/valid_guesses.txt");
        while (std::getline(file_stream, line))
            _valid_guesses_orig.push_back(line);
        file_stream.close();
    }



    // Move previous from valid answers to valid guesses

    // bool move_previous_answers = false;
    // std::string bool_str;
    // std::cout << "--Move previous answers from valid answers to valid guesses? (0 = no, 1 = yes)\n";
    // std::getline (std::cin, bool_str);
    // std::stringstream(bool_str) >> move_previous_answers;

    // if (move_previous_answers)
    if (game_type == GameType::wordle)
    {
        file_stream.open("../include/previous_answers.txt");
        std::vector<std::string> prev_answers;
        std::string prev_answer;
        while (std::getline(file_stream, prev_answer))
            prev_answers.push_back(prev_answer);

        // Also remove the words before I started playing wordle
        file_stream.close();
        file_stream.open("../include/before_i_played_wordle.txt");
        while (std::getline(file_stream, prev_answer))
            prev_answers.push_back(prev_answer);

        for (auto prev_answer : prev_answers)
        {
            // If the previous answer actually is in the answer set (it should be), remove from the answer set and add it to guess set
            auto prev_answer_itr = std::find(_valid_answers_orig.begin(), _valid_answers_orig.end(), prev_answer);
            if (prev_answer_itr != _valid_answers_orig.end())
            {
                _valid_answers_orig.erase(prev_answer_itr);
                _valid_guesses_orig.push_back(prev_answer);
            }
        }
        file_stream.close();
    }

    _valid_answers_trimmed = _valid_answers_orig;

    #if HARD_MODE
    _valid_guesses_trimmed = _valid_guesses_orig;
    #endif  // HARD_MODE
}

void Inputs::B(char letter)
{
    this->remove_words_with_letter(letter);
}

void Inputs::black_duplicate_letter(char letter, size_t green_index)
{
    if (green_index != 0) this->remove_words_with_letter_index(letter, 0);
    if (green_index != 1) this->remove_words_with_letter_index(letter, 1);
    if (green_index != 2) this->remove_words_with_letter_index(letter, 2);
    if (green_index != 3) this->remove_words_with_letter_index(letter, 3);
    if (green_index != 4) this->remove_words_with_letter_index(letter, 4);
}

void Inputs::G(char letter, size_t correct_index)
{
    this->remove_words_without_letter_index(letter, correct_index);
}

void Inputs::Y(char letter, size_t wrong_index, bool is_duplicate)
{
    this->remove_words_without_letter(letter, is_duplicate);
    this->remove_words_with_letter_index(letter, wrong_index);
}

size_t Inputs::remove_words_with_letter(char letter)
{
    if (_excluded_letters.find(letter) == std::string::npos)
        _excluded_letters += letter;

    for (auto itr = _valid_answers_trimmed.begin(); itr != _valid_answers_trimmed.end(); )
    {
        if (itr->find(letter) != std::string::npos)
            itr = _valid_answers_trimmed.erase(itr);
        else
            ++itr;
    }

    #if HARD_MODE
    for (auto itr = _valid_guesses_trimmed.begin(); itr != _valid_guesses_trimmed.end(); )
    {
        if (itr->find(letter) != std::string::npos)
            itr = _valid_guesses_trimmed.erase(itr);
        else
            ++itr;
    }
    #endif  // HARD_MODE

    return _valid_answers_trimmed.size();
}

size_t Inputs::remove_words_with_letter_index(char letter, size_t index)
{
    for (auto itr = _valid_answers_trimmed.begin(); itr != _valid_answers_trimmed.end(); )
    {
        if (itr->find(letter) != std::string::npos && itr->at(index) == letter)
            itr = _valid_answers_trimmed.erase(itr);
        else
            ++itr;
    }

    #if HARD_MODE
    for (auto itr = _valid_guesses_trimmed.begin(); itr != _valid_guesses_trimmed.end(); )
    {
        if (itr->find(letter) != std::string::npos && itr->at(index) == letter)
            itr = _valid_guesses_trimmed.erase(itr);
        else
            ++itr;
    }
    #endif  // HARD_MODE

    return _valid_answers_trimmed.size();
}


size_t Inputs::remove_words_without_letter(char required_letter, bool duplicate)
{
    if (_required_letters.find(required_letter) == std::string::npos)
        _required_letters += required_letter;

    for (auto itr = _valid_answers_trimmed.begin(); itr != _valid_answers_trimmed.end(); )
    {
        size_t pos =      itr->find(required_letter);
        pos = duplicate ? itr->find(required_letter, pos + 1) : pos;
        if (pos == std::string::npos)
            itr = _valid_answers_trimmed.erase(itr);
        else
            ++itr;
    }

    #if HARD_MODE
    for (auto itr = _valid_guesses_trimmed.begin(); itr != _valid_guesses_trimmed.end(); )
    {
        size_t pos =      itr->find(required_letter);
        pos = duplicate ? itr->find(required_letter, pos + 1) : pos;
        if (pos == std::string::npos)
            itr = _valid_guesses_trimmed.erase(itr);
        else
            ++itr;
    }
    #endif  // HARD_MODE

    return _valid_answers_trimmed.size();
}

size_t Inputs::remove_words_without_letter_index(char required_letter, size_t index)
{
    if (_required_letters.find(required_letter) == std::string::npos)
        _required_letters += required_letter;

    for (auto itr = _valid_answers_trimmed.begin(); itr != _valid_answers_trimmed.end(); )
    {
        if (itr->find(required_letter) == std::string::npos || itr->at(index) != required_letter)
            itr = _valid_answers_trimmed.erase(itr);
        else
            ++itr;
    }

    #if HARD_MODE
    for (auto itr = _valid_guesses_trimmed.begin(); itr != _valid_guesses_trimmed.end(); )
    {
        if (itr->find(required_letter) == std::string::npos || itr->at(index) != required_letter)
            itr = _valid_guesses_trimmed.erase(itr);
        else
            ++itr;
    }
    #endif  // HARD_MODE

    return _valid_answers_trimmed.size();
}

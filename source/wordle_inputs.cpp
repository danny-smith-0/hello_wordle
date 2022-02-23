#include <wordle_inputs.h>

#include <string>    // string and getline
#include <fstream>   // std::ifstream (and ofstream)

#include <wordle_defs.h>


#define HARD_MODE 0

using namespace wordle;

void Inputs::load_words()
{
    std::string file_path = "../include/valid_answers.txt";
    std::ifstream file_stream1 (file_path);
    std::string line;
    while (std::getline(file_stream1, line))
        _valid_answers_orig.push_back(line);
    _valid_answers_trimmed = _valid_answers_orig;

    file_path = "../include/valid_guesses.txt";
    std::ifstream file_stream2 (file_path);
    while (std::getline(file_stream2, line))
        _valid_guesses_orig.push_back(line);

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

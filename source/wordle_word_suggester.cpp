#include <wordle_word_suggester.h>

#include <iostream>  // std::cout
#include <fstream>   // ifstream (and ofstream)
#include <algorithm> // unique

using namespace wordle;

void WordSuggester::load_words()
{
    std::string file_path = "../include/valid_answers.txt";
    std::ifstream file_stream1 (file_path);
    std::string line;
    while (std::getline(file_stream1, line))
        _valid_answers.push_back(line);

    file_path = "../include/valid_guesses.txt";
    std::ifstream file_stream2 (file_path);
    while (std::getline(file_stream2, line))
        _valid_guesses.push_back(line);
}

size_t WordSuggester::remove_words_with_letter(char letter)
{
    for (auto itr = _valid_answers.begin(); itr != _valid_answers.end(); )
    {
        if (itr->find(letter) != std::string::npos)
            itr = _valid_answers.erase(itr);
        else
            ++itr;
    }
    return _valid_answers.size();
}

size_t WordSuggester::remove_words_with_letter_position(char letter, size_t position)
{
    for (auto itr = _valid_answers.begin(); itr != _valid_answers.end(); )
    {
        if (itr->find(letter) == position)
            itr = _valid_answers.erase(itr);
        else
            ++itr;
    }
    return _valid_answers.size();
}

size_t WordSuggester::remove_words_without_letter(char required_letter)
{
    for (auto itr = _valid_answers.begin(); itr != _valid_answers.end(); )
    {
        if (itr->find(required_letter) == std::string::npos)
            itr = _valid_answers.erase(itr);
        else
            ++itr;
    }
    return _valid_answers.size();
}

void WordSuggester::print_words(int words_per_row, std::vector<std::string> words)
{
    // Default to printing the valid answers
    if (words.empty())
        words = this->_valid_answers;

    int count = 1;
    for (auto word : words)
    {
        std::cout << word << " ";
        if (++count > words_per_row)
        {
            std::cout << "\n";
            count = 1;
        }
    }
    std::cout << "\n";
}

int main()
{
    WordSuggester word_suggester;
    word_suggester.load_words();

    // std::string letters_not_included = "storplinfcd";

    // for (auto remove_char : letters_not_included)
    //     word_suggester.remove_words_with_letter(remove_char);

    // word_suggester.remove_words_with_letter_position('a', 1);
    // word_suggester.remove_words_with_letter_position('a', 2);
    // word_suggester.remove_words_with_letter_position('e', 4);

    // {
    //     word_suggester.remove_words_with_letter_position('e', 0);
    //     word_suggester.remove_words_with_letter_position('e', 1);
    //     word_suggester.remove_words_with_letter_position('e', 2);
    // }


    // TODO add the ability to get the unspecified letters of the possible words

    word_suggester.print_words(1);

    int c = 0;
    c++;
}

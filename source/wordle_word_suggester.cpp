#include <wordle_word_suggester.h>

#include <iostream>  // std::cout
#include <fstream>   // ifstream (and ofstream)
#include <algorithm> // unique

using namespace wordle;

void WordSuggester::load_words()
{
    std::string file_path = "../include/words.txt";
    std::ifstream file_stream (file_path);
    std::string line;
    while (std::getline(file_stream, line))
        _words.push_back(line);

    // TEMP until I get a better word list
    std::sort(_words.begin(), _words.end());
    auto last = std::unique(_words.begin(), _words.end());
    _words.erase(last, _words.end());
}

size_t WordSuggester::remove_words_with_letter(char letter)
{
    for (auto itr = _words.begin(); itr != _words.end(); )
    {
        if (itr->find(letter) != std::string::npos)
            itr = _words.erase(itr);
        else
            ++itr;
    }
    return _words.size();
}

size_t WordSuggester::remove_words_with_letter_position(char letter, size_t position)
{
    for (auto itr = _words.begin(); itr != _words.end(); )
    {
        if (itr->find(letter) == position)
            itr = _words.erase(itr);
        else
            ++itr;
    }
    return _words.size();
}

void WordSuggester::print_words(int words_per_row)
{
    int count = 1;
    for (auto word : _words)
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

    std::string letters_not_included = "storplinfcd";

    for (auto remove_char : letters_not_included)
        word_suggester.remove_words_with_letter(remove_char);

    word_suggester.remove_words_with_letter_position('a', 1);
    word_suggester.remove_words_with_letter_position('a', 2);
    word_suggester.remove_words_with_letter_position('e', 4);

    {
        word_suggester.remove_words_with_letter_position('e', 0);
        word_suggester.remove_words_with_letter_position('e', 1);
        word_suggester.remove_words_with_letter_position('e', 2);
    }

    // I need a much better library of words

    // Once I don't have to remove garbage words, have the ability to get the unspecified letters of the possible words

    // std::vector<std::string> new_ending (word_suggester._words.begin() + 495, word_suggester._words.end());
    word_suggester.print_words(3);
    int c = 0;
    c++;
}

#include <wordle_word_suggester.h>

#include <iostream>  // std::cout
#include <fstream>   // ifstream (and ofstream)

using namespace wordle;

std::vector<std::string> get_strings_from_file(const std::string& file_path)
{
    std::vector<std::string> strings;
    std::ifstream file_stream (file_path);
    std::string line;
    while (std::getline(file_stream, line))
        strings.push_back(line);
    return strings;
}


void WordSuggester::load_words()
{
    std::string file_path = "../include/words.txt";
    std::ifstream file_stream (file_path);
    std::string line;
    std::vector<std::string> strings;
    while (std::getline(file_stream, line))
        strings.push_back(line);
    _words = strings;
}

int main()
{
    WordSuggester word_suggester;
    word_suggester.load_words();
}

#ifndef WORDLE_WORD_SUGGESTER
#define WORDLE_WORD_SUGGESTER

#include <vector>
#include <string>

namespace wordle
{

    class WordSuggester
    {
    public:
        void load_words();
        std::vector<std::string> _words;

    };
}

#endif  // WORDLE_WORD_SUGGESTER

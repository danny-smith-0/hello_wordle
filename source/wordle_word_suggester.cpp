#include <wordle_word_suggester.h>

#include <algorithm> // std::unique
#include <iostream>  // std::cout
#include <iomanip>   // std::setw
#include <fstream>   // std::ifstream (and ofstream)
#include <sstream>   // std::stringstream

#include <set>

using namespace wordle;

#define HARD_MODE 0

template <typename T>
void sort_and_remove_non_unique_elements(T* vector_or_string)
{
    std::sort(vector_or_string->begin(), vector_or_string->end());
    auto itr_last = std::unique(vector_or_string->begin(), vector_or_string->end());
    vector_or_string->erase(itr_last, vector_or_string->end());
}

void WordSuggester::load_words()
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

size_t WordSuggester::remove_words_with_letter(char letter)
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

size_t WordSuggester::remove_words_with_letter_index(char letter, size_t index)
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

void WordSuggester::black_letter(char letter)
{
    this->remove_words_with_letter(letter);
}

void WordSuggester::black_duplicate_letter(char letter, size_t green_index)
{
    if (green_index != 0) this->remove_words_with_letter_index(letter, 0);
    if (green_index != 1) this->remove_words_with_letter_index(letter, 1);
    if (green_index != 2) this->remove_words_with_letter_index(letter, 2);
    if (green_index != 3) this->remove_words_with_letter_index(letter, 3);
    if (green_index != 4) this->remove_words_with_letter_index(letter, 4);
}

void WordSuggester::green_letter(char letter, size_t correct_index)
{
    this->remove_words_without_letter_index(letter, correct_index);
}

void WordSuggester::yellow_letter(char letter, size_t wrong_index, bool is_duplicate)
{
    this->remove_words_without_letter(letter, is_duplicate);
    this->remove_words_with_letter_index(letter, wrong_index);
}

size_t WordSuggester::remove_words_without_letter(char required_letter, bool duplicate)
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

size_t WordSuggester::remove_words_without_letter_index(char required_letter, size_t index)
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


void WordSuggester::print_words(int words_per_row, std::vector<std::string> words)
{
    // Default to printing the valid answers
    if (words.empty())
        words = this->_valid_answers_trimmed;

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

void WordSuggester::subtract_required_letters(std::vector<std::string> const& words, std::string required_letters, std::string* unspecified_letters, std::vector<std::string>* unspecified_letters_by_word)
{
    for (auto word : words)
    {
        std::string reqs = required_letters;  // Make a copy so we can edit
        std::string remaining_letters;
        for (auto mychar : word)
        {
            if (reqs.find(mychar) != std::string::npos)
                reqs.erase(reqs.find(mychar), 1);
            else
                remaining_letters += mychar;
        }
        if (unspecified_letters)
            *unspecified_letters += remaining_letters;
        if (unspecified_letters_by_word && !remaining_letters.empty())
            unspecified_letters_by_word->push_back(remaining_letters);
    }
}

void WordSuggester::score_words_by_letter_scores(std::vector<std::string> const& unspecified_letters_by_word, std::vector<std::string> const& original_words, std::map<char, size_t> const& letter_count)
{
    if (this->_valid_answers_trimmed.size() == 1)
    {
        std::cout << "Final answer: " << this->_valid_answers_trimmed[0] << std::endl;
        return;
    }

    // std::map<std::string, size_t> scoring_words;
    std::map<size_t, std::string> scoring_words;
    for (size_t ii = 0; ii != unspecified_letters_by_word.size(); ++ii)
    {
        // Don't score a word for the same letter multiple times.
        auto one_words_unspecifieds = unspecified_letters_by_word[ii];  // Make a copy so we can edit
        sort_and_remove_non_unique_elements(&one_words_unspecifieds);

        size_t score = 0;
        for (auto mychar : one_words_unspecifieds)
        {
            if (letter_count.find(mychar) == letter_count.end())
                continue;
            score += letter_count.at(mychar);
        }

        // scoring_words[this->_valid_answers_trimmed[ii]] = score;
        scoring_words[score] += original_words[ii];
    }

    // Print out the scored by score
    size_t count = 0;
    size_t word_print_cutoff = 25;
    for (auto ritr = scoring_words.rbegin(); ritr != scoring_words.rend(); ++ritr)
    {
        std::string words = ritr->second;
        while (count < word_print_cutoff && !words.empty())
        {
            std::string word (words.begin(), words.begin() + 5);
            words.erase(words.begin(), words.begin() + 5);
            std::cout << word << " : " << ritr->first << "\n";
            ++count;
        }
    }
    if ( count >= word_print_cutoff)
        std::cout << " Printed only " << word_print_cutoff << " of " << original_words.size() << " words\n";
    std::cout << "\n";
}

void WordSuggester::suggest()
{
    if (this->_valid_answers_trimmed.size() == 1)
    {
        std::cout << "\nFinal answer: " << this->_valid_answers_trimmed[0] << "\n\n";
        return;
    }

    if (this->_valid_answers_trimmed.size() == 2)
    {
        std::cout << "\nFlip a coin!\n" << this->_valid_answers_trimmed[0] << "\n" << this->_valid_answers_trimmed[1] << "\n\n";
        return;
    }

    // Get all the letters in the words that weren't required, grouped all together and by word
    std::string unspecified_letters = "";
    std::vector<std::string> unspecified_letters_by_word;
    std::vector<std::string> unspecified_letters_by_word__guesses;

    subtract_required_letters(this->_valid_answers_trimmed, this->_required_letters, &unspecified_letters, &unspecified_letters_by_word);
    subtract_required_letters(this->_valid_guesses_orig,    this->_required_letters,                    0, &unspecified_letters_by_word__guesses);

    // Count (score) the unspecified letters
    std::sort(unspecified_letters.begin(), unspecified_letters.end());
    std::map<char, size_t> letter_count;
    if (!unspecified_letters.empty())
    {
        letter_count[unspecified_letters[0]] = 1;
        for (auto itr = unspecified_letters.begin(); itr != unspecified_letters.end() - 1; ++itr)
        {
            if (*(itr + 1) == *itr)
                ++letter_count[*itr];
            else
                letter_count[*(itr + 1)] = 1;
        }
    }
    // Remove non-unique letters
    sort_and_remove_non_unique_elements(&unspecified_letters);

    // // Print the letter counts
    // std::cout << "Counts of the unspecified letters in the remaining words (alphabetical & by count)\n ";
    // for (auto letter : unspecified_letters)
    //     std::cout << "  " << letter << " ";
    // std::cout << "\n";
    // for (auto letter : unspecified_letters)
    //     std::cout << std::fixed << std::setw(4) << letter_count[letter];
    // std::cout << "\n\n";

    // Backwards letter count, then print in order
    std::map<size_t, std::string> backwards_letter_count;
    for (auto [letter, score] : letter_count)
        backwards_letter_count[score] += letter;

    std::stringstream ss;
    std::cout << "Scoring the remaining words by the count of their unique unspecified letters\n ";
    for (auto ritr = backwards_letter_count.rbegin(); ritr != backwards_letter_count.rend(); ++ritr)
    {
        std::string letters = ritr->second;
        while (!letters.empty())
        {
            std::cout << "  " << letters[0] << " ";
            letters.erase(0, 1);
            ss << std::fixed << std::setw(4) << ritr->first;
        }
    }
    std::cout << "\n" << ss.str() << "\n\n";

    // Score the remaining words by popular letter - easy first pass for a score
    // std::cout << "valid answers: \n";
    // score_words_by_letter_scores(unspecified_letters_by_word, this->_valid_answers_trimmed, letter_count);
    // std::cout << "valid guesses (invalid answers): \n";
    // score_words_by_letter_scores(unspecified_letters_by_word__guesses, this->_valid_guesses_orig, letter_count);

    /*
    Idea of next pass on score - what word, if guessed could give us the most information about the remaining words?
    Which touches the most words, but also leads us closer to a final answer.
    It's tricky, because there are a lot of types of information. It'll be fun to work through

    So far I haven't been considering location of the letters after the initial removing, before this function. That will affect how to get info.
    Compare all guess words, see which have the min, max, and average number of remaining words across the guess and all remaining valid answers
    */

    this->collect_buckets();
}

colored_buckets_t WordSuggester::calc_buckets(std::string guess, std::vector<std::string> words)
{

    // Get the words that match with each color result
    colored_buckets_t colored_buckets;

    for (auto word : words)
    {
        std::string color_res;
        for (size_t guess_index = 0; guess_index < guess.size(); ++guess_index)
        {
            char guess_letter = guess[guess_index];

            if (word.find(guess_letter) == std::string::npos) // not in the word
                color_res += "B";
            else
            {
                if (word[guess_index] == guess_letter)  // in the exact same spot
                    color_res += "G";
                else
                {
                    // Handle duplicates
                    if (std::count(guess.begin(), guess.end(), guess_letter) > 1)
                    {
                        //if I'm first, and the other one is not green, I get to be yellow
                        //if I'm first, and the second one is green, I'm yellow if there are two, or black if there is one
                        //if I'm second, there must be duplicates in the word.

                        //am I first?
                        bool i_am_first = guess_index == guess.find_first_of(guess_letter);
                        bool word_has_duplicates = std::count(word.begin(), word.end(), guess_letter) > 1;
                        bool my_duplicate_is_green = word[guess.find_last_of(guess_letter)] == guess_letter;
                        if (i_am_first && !my_duplicate_is_green)
                        {
                            color_res += "Y"; // in the wrong spot
                        }
                        else
                        {
                            if (word_has_duplicates)
                                color_res += "Y"; // in the wrong spot
                            else
                                color_res += "B";
                        }
                    }
                    else
                    {
                        color_res += "Y"; // in the wrong spot
                    }
                }
            }
        }
        colored_buckets[color_res].push_back(word);
    }

    //average
    double average_words_remaining = static_cast<double>(words.size()) / static_cast<double>(colored_buckets.size());

    return colored_buckets;
}

double get_average_bucket_size(colored_buckets_t in)
{
    double total_elements = 0;
    for (auto row : in)
        total_elements += row.second.size();
    return total_elements / static_cast<double>(in.size());
}

size_t max_bucket_depth(colored_buckets_t in)
{
    size_t max_bucket_depth = 0;
    for (auto row : in)
        max_bucket_depth = std::max(row.second.size(), max_bucket_depth);
    return max_bucket_depth;
}

std::string bucket_depth_str(colored_buckets_t in)
{
    size_t max_buckets = 30;
    std::stringstream out;
    if (in.size() <= max_buckets)
    {
        std::vector<size_t> bucket_depths;
        for (auto row : in)
            bucket_depths.push_back(row.second.size());

        std::sort(bucket_depths.begin(), bucket_depths.end(), std::greater<size_t>());
        for (auto itr_depth = bucket_depths.begin(); itr_depth != bucket_depths.end(); ++itr_depth)
        {
            out << *itr_depth;
            if (itr_depth != bucket_depths.end() - 1)
                out << ",";
        }
    }
    return out.str();
}


// TODO make this a tuple so I can include the results
struct comparator
{
    template <typename T>
    bool operator()(T const& l, T const& r) const
    {
        if (l.second != r.second)
            return l.second < r.second;
        return l.first < r.first;
    }
};

std::map<std::string, colored_buckets_t> WordSuggester::collect_buckets(words_t guess_words)
{
    int num_answers = static_cast<int>(this->_valid_answers_trimmed.size());

    std::map<std::string, double> avg_bucket_size;
    std::map<std::string, colored_buckets_t> all_buckets;
    for (auto guess : guess_words)
    {
        colored_buckets_t colored_buckets = this->calc_buckets(guess, this->_valid_answers_trimmed);
        avg_bucket_size[guess] = get_average_bucket_size(colored_buckets);
        if (num_answers < 20 && num_answers == guess_words.size())
            std::cout << guess << "\n" << this->print_buckets(colored_buckets);// << "\n";
        if (avg_bucket_size[guess] < 21)
            all_buckets[guess] = colored_buckets;
    }

    std::set<std::pair<std::string, double>, comparator> ordered_guesses(avg_bucket_size.begin(), avg_bucket_size.end());
    int count = 0;
    int count_cutoff = num_answers > 30 ? 20 : num_answers;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(4);
    std::cout << "                avg    max\n";
    std::cout << "       total  bucket bucket\n";
    std::cout << "     buckets   depth  depth\n";
    for (auto [guess, average_words_remaining] : ordered_guesses)
    {
        ss << "" << guess << ",  "
            << std::setw(3) <<  all_buckets[guess].size()
            << ",  " << average_words_remaining
            << ",  " << std::setw(3) << max_bucket_depth(all_buckets[guess])
            << "   " << bucket_depth_str(all_buckets[guess]) << "\n";
        if (count++ >= count_cutoff)
            break;
    }
    std::cout << ss.str();
    return all_buckets;
}

void WordSuggester::collect_buckets()
{
    std::cout  << "\n\nhow_many_words_remain_after_guess\nanswers: (out of " << this->_valid_answers_trimmed.size() << ")\n";
    std::map<std::string, colored_buckets_t> answer_buckets_trim = collect_buckets(this->_valid_answers_trimmed);

    // Quordle
    // std::cout  << "\n\nuntrimmed answers:\n";
    // words_t these_words;
    // these_words.push_back("daily");
    // these_words.push_back("dally");
    // these_words.push_back("valid");
    // these_words.push_back("villa");
    // these_words.push_back("imply");
    // these_words.push_back("lymph");
    // std::map<std::string, colored_buckets_t> answer_buckets_untrimmed = collect_buckets(these_words);

    #if HARD_MODE
    // For hard mode
    std::cout  << "\n\nguesses (trimmed):\n";
    std::map<std::string, colored_buckets_t> guess_buckets_trim = collect_buckets(this->_valid_guesses_trimmed);
    #endif  // HARD_MODE

    std::cout  << "\n\nguesses (all):\n";
    std::map<std::string, colored_buckets_t> guess_buckets_orig = collect_buckets(this->_valid_guesses_orig);

    // A place for a break point
    int c = 0;
    c++;
}

std::string WordSuggester::print_buckets(colored_buckets_t const& colored_buckets)
{
    std::stringstream ss;
    for ( auto [color, bucket] : colored_buckets)
    {
        ss << color;
        for (auto word : bucket)
            ss << "," << word;
        ss << "\n";
    }
    return ss.str();
}

int main()
{
    std::cout << "Hello Wordle!\n";
    WordSuggester word_suggester;
    std::cout << "Libraries loaded.\n";

    bool duplicate = true;
    // word_suggester.black_letter( '');
    // word_suggester.green_letter( '', );
    // word_suggester.yellow_letter('', );



    word_suggester.suggest();

    int c = 0;
    c++;
}

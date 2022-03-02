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

void WordSuggester::print_words(int words_per_row, std::vector<std::string> words)
{
    // Default to printing the valid answers
    // if (words.empty())
    //     words = this->_valid_answers_trimmed;

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

void WordSuggester::subtract_required_letters(std::vector<std::string> const& words, std::string required_letters, std::string* unspecified_letters)
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
    }
}

std::map<std::string, colored_buckets_t> WordSuggester::suggest(Inputs const& inputs, bool suggest_guesses)
{
    if (inputs._valid_answers_trimmed.size() == 1)
    {
        std::cout << "\nFinal answer: " << inputs._valid_answers_trimmed[0] << "\n\n";
        return std::map<std::string, colored_buckets_t>();
    }

    if (inputs._valid_answers_trimmed.size() == 2)
    {
        std::cout << "\nFlip a coin!\n" << inputs._valid_answers_trimmed[0] << "\n" << inputs._valid_answers_trimmed[1] << "\n\n";
        return std::map<std::string, colored_buckets_t>();
    }

    // Get all the letters in the words that weren't required, grouped all together and by word
    std::string unspecified_letters = "";

    subtract_required_letters(inputs._valid_answers_trimmed, inputs._required_letters, &unspecified_letters);

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


    /*
    Idea of next pass on score - what word, if guessed could give us the most information about the remaining words?
    Which touches the most words, but also leads us closer to a final answer.
    It's tricky, because there are a lot of types of information. It'll be fun to work through

    So far I haven't been considering location of the letters after the initial removing, before this function. That will affect how to get info.
    Compare all guess words, see which have the min, max, and average number of remaining words across the guess and all remaining valid answers
    */

    std::map<std::string, colored_buckets_t> answers = this->collect_buckets(inputs, suggest_guesses);
    return answers;
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

std::map<std::string, colored_buckets_t> WordSuggester::collect_buckets(Inputs const& inputs, words_t guess_words)
{
    int num_answers = static_cast<int>(inputs._valid_answers_trimmed.size());
    bool guess_words_are_answers = num_answers == guess_words.size();
    std::map<std::string, double> avg_bucket_size;
    std::map<std::string, colored_buckets_t> all_buckets;
    for (auto guess : guess_words)
    {
        colored_buckets_t colored_buckets = this->calc_buckets(guess, inputs._valid_answers_trimmed);
        avg_bucket_size[guess] = get_average_bucket_size(colored_buckets);
        if (num_answers < 20 && guess_words_are_answers)
            std::cout << guess << "\n" << this->print_buckets(colored_buckets);// << "\n";
        // if (avg_bucket_size[guess] < 21) // For optimization, but breaks the new method  // For Spanish, use 85 for now.
            all_buckets[guess] = colored_buckets;
    }

    std::set<std::pair<std::string, double>, comparator> ordered_guesses(avg_bucket_size.begin(), avg_bucket_size.end());
    int count = 0;
    int count_cutoff = (!guess_words_are_answers || num_answers > 30) ? 20 : num_answers;
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

std::map<std::string, colored_buckets_t> WordSuggester::collect_buckets(Inputs const& inputs, bool suggest_guesses)
{
    std::cout  << "how_many_words_remain_after_guess\nanswers: (out of " << inputs._valid_answers_trimmed.size() << ")\n";
    std::map<std::string, colored_buckets_t> answer_buckets_trim = collect_buckets(inputs, inputs._valid_answers_trimmed);

    // // Quordle
    // std::cout  << "\n\nuntrimmed answers:\n";
    // words_t remaining_words;
    // // remaining_words.push_back("");
    // std::map<std::string, colored_buckets_t> answer_buckets_untrimmed = collect_buckets(inputs, remaining_words);

    if (suggest_guesses)
    {
        #if HARD_MODE
        // For hard mode
        std::cout  << "\n\nguesses (trimmed):\n";
        std::map<std::string, colored_buckets_t> guess_buckets_trim = collect_buckets(inputs, inputs._valid_guesses_trimmed);
        #endif  // HARD_MODE

        std::cout  << "\nall guesses & answers:\n";
        auto all_words = inputs._valid_guesses_orig;
        all_words.insert(all_words.end(), inputs._valid_answers_orig.begin(), inputs._valid_answers_orig.end());
        std::map<std::string, colored_buckets_t> guess_buckets_orig = collect_buckets(inputs, all_words);
    }
    // A place for a break point
    int c = 0;
    c++;
    return answer_buckets_trim;
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

words_t words_list_intersection(words_t const& w1, words_t const& w2)
{
    words_t intersection;
    for (auto word : w1) {
        if (std::find(w2.begin(), w2.end(), word) != w2.end())
            intersection.push_back(word); }
    return intersection;
}

words_t trim_words_by_user_inputs(std::map<std::string, colored_buckets_t>& answers)
{
    words_t words;
    std::string guess_str, result_str, bool_str;
    bool more_guesses = false;
    bool first_guess  = true;
    int count = 0;
    do {
        std::cout << "--Enter your guess and color result on two separate lines \n";
        std::getline (std::cin, guess_str);
        std::getline (std::cin, result_str);
        std::cout << "Guess: " << guess_str << ", result: " << result_str << "\n";
        if (first_guess)
            words = answers[guess_str][result_str];
        else
            words = words_list_intersection(words, answers[guess_str][result_str]);
        first_guess = false;
        if (++count < 6)
        {
            std::cout << "--Another guess or proceed to suggestions? (0 = proceed. 1: more guesses)\n";
            std::getline (std::cin, bool_str);
            std::stringstream(bool_str) >> more_guesses;
        }
        else
            more_guesses = false;
    }
    while (more_guesses);
    std::cout << std::endl;
    return words;
}

bool user_says_to_suggest_guesses()
{
    bool suggest_guesses = false;

    // cin suggest_guesses
    std::string bool_str;
    std::cout << "--Enter preferred type of suggestions: (0 = valid answers only, 1 = valid answers and guesses)\n";
    std::getline (std::cin, bool_str);
    std::stringstream(bool_str) >> suggest_guesses;

    return suggest_guesses;
}

int main()
{
    std::cout << "Hello Wordle!\n";
    Inputs inputs;
    std::cout << "Libraries loaded.\n";

    bool duplicate = true;
    // inputs.B('');
    // inputs.G('', );
    // inputs.Y('', );

    WordSuggester word_suggester;
    bool suggest_guesses = false;
    // inputs._valid_answers_trimmed.push_back("when_using_guess_words_put_them_here");
    std::map<std::string, colored_buckets_t> answers = word_suggester.suggest(inputs, suggest_guesses);

    std::cout << "\n--------\n";
    words_t words;
    #define TOPLEFT     0
    #define TOPRIGHT    0
    #define BOTTOMLEFT  0
    #define BOTTOMRIGHT 0

    words = trim_words_by_user_inputs(answers);

    #if TOPLEFT
        words =                                answers[guess_str][result_str];
        // words = words_list_intersection(words, answers["round"]["BBBBB"]);
    #elif TOPRIGHT
        words =                                answers["slate"]["BBYYB"];
        // words = words_list_intersection(words, answers["round"]["BBYBB"]);
    #elif BOTTOMLEFT
        words =                                answers["slate"]["GBBBB"];
        // words = words_list_intersection(words, answers["round"]["BBBBB"]);
    #elif BOTTOMRIGHT
        words =                                answers["slate"]["BBBBB"];
        // words = words_list_intersection(words, answers["round"]["YYBBB"]);
    #endif
    inputs._valid_answers_trimmed = words;
    suggest_guesses = user_says_to_suggest_guesses();
    std::map<std::string, colored_buckets_t> answersB = word_suggester.suggest(inputs, suggest_guesses);
    int c = 0;
    c++;
}

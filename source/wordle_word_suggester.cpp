#include <wordle_word_suggester.h>

#include <algorithm> // std::unique
#include <iostream>  // std::cout
#include <iomanip>   // std::setw
#include <fstream>   // std::ifstream (and ofstream)
#include <sstream>   // std::stringstream
#include <random>    // std::random_device

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

bool flip_a_coin(bool print_results = false)
{
    std::random_device rd;
    std::random_device::result_type random_number = rd();
    std::random_device::result_type random_max = rd.max();
    double result = static_cast<double>(random_number) / static_cast<double>(random_max);
    bool heads =  result < 0.5 ? false : true;
    if (print_results)
        std::cout << random_number << "/" << random_max << " = " << result << "\n\n";
    return heads;
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

        // Now pretend to flip a coin
        bool heads = flip_a_coin();
        int selected_index =  heads ? 0 : 1;
        std::cout << "A random number generator chose " << inputs._valid_answers_trimmed[selected_index] << "\n\n";

        return std::map<std::string, colored_buckets_t>();
    }

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
                    // Handle duplicates - TODO Handle triplicates
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
        if (inputs._game_type != GameType::wordle_es || (inputs._game_type == GameType::wordle_es && avg_bucket_size[guess] < 85))
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

bool get_boolean_from_user_0_or_1(std::string print_message)
{
    bool result;

    bool bad_input = true;
    while (bad_input)
    {
        std::cout << print_message;
        std::string input_string;
        std::getline (std::cin, input_string);
        if (input_string == "0" || input_string == "1")
        {
            std::stringstream(input_string) >> result;
            bad_input = false;
        }
        else
            std::cout << "\nBad input. Try again\n\n";
    }

    return result;
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

        // Make sure the inputs are the correct case to work with this function (lower for guess, upper for result)
        std::transform(guess_str.begin(), guess_str.end(), guess_str.begin(), std::tolower);
        std::transform(result_str.begin(), result_str.end(), result_str.begin(), std::toupper);

        words_t trimmed_words;
        if (first_guess)
        {
            if (guess_str.empty())
                guess_str = "parse"; // Assume the word input is "parse" if the first line is blank
            trimmed_words = answers[guess_str][result_str];
        }
        else
        {
            // For every guess after the first, find the intersection between the remaining possible words and the possible words based on the current guess
            trimmed_words = words_list_intersection(words, answers[guess_str][result_str]);
        }

        if (trimmed_words.empty())
        {
            // If the intersection is zero (probably because of bad input), try again!
            std::cout << "\nBad input! No solutions remaining. Try last entry again\n\n";
            more_guesses = true;
        }
        else
        {
            words = trimmed_words;
            std::cout << "Guess: " << guess_str << ", result: " << result_str << ". Words remaining: " << words.size() << "\n";
            if (words.size() < 9)
            {
                std::cout << "\n";
                for (auto word : words)
                    std::cout << word << ", ";
                std::cout << "\n\n";
            }

            first_guess = false;

            if (words.size() > 1 && ++count < 6)
                more_guesses = get_boolean_from_user_0_or_1("--Another guess or proceed to suggestions? (0 = proceed. 1: more guesses)\n");
            else
                more_guesses = false;
        }
    }
    while (more_guesses);

    std::cout << std::endl;
    return words;
}

GameType what_game()
{
    std::string input_str;
    std::cout << "What game would you like to play? ("
        << static_cast<int>(GameType::wordle)    << " = wordle, "
        << static_cast<int>(GameType::quordle)   << " = quordle, "
        << static_cast<int>(GameType::wordle_es) << " = wordle en español), "
        << static_cast<int>(GameType::nerdle)    << " = nerdle\n";
    std::getline (std::cin, input_str);
    int game_type_int = 0;
    std::stringstream(input_str) >> game_type_int;
    GameType game_type = static_cast<GameType>(game_type_int);

    return game_type;
}

int main()
{
    std::cout << "Hello Wordle!\n\n";
    GameType game_type = what_game();
    Inputs inputs(game_type);
    std::cout << "Libraries loaded.\n";

    WordSuggester word_suggester;
    bool suggest_guesses = false;
    // inputs._valid_answers_trimmed.push_back("when_using_guess_words_put_them_here");
    std::map<std::string, colored_buckets_t> answers = word_suggester.suggest(inputs, suggest_guesses);

    std::cout << "\n--------\n";

    bool success = false;
    while (!success)
    {
        words_t words = trim_words_by_user_inputs(answers);
        inputs._valid_answers_trimmed = words;

        // Allow user to request guesses if there are more than 2 remaining possibilities
        if (words.size() > 2)
            suggest_guesses = get_boolean_from_user_0_or_1("--Enter preferred type of suggestions: (0 = valid answers only, 1 = valid answers and guesses)\n");

        word_suggester.suggest(inputs, suggest_guesses);

        success = words.size() < 2;
    }

    return 0;
}

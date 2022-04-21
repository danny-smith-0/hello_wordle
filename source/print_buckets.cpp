#include <wordle_defs.h>

#include <string>
#include <sstream> // std::stringstream
#include <fstream> // std::ofstream

namespace wordle::print
{
    void print_colored_buckets_to_json(std::map<std::string, colored_buckets_t> answers, std::string guess)
    {
        std::string filename = guess + ".json";
        std::ofstream outfile(filename);
        outfile << buckets_to_json_string(answers[guess]);
        outfile.close();
    }

    std::string buckets_to_json_string(colored_buckets_t colored_buckets)
    {
        std::stringstream ss;
        ss << "{";

        bool first_color_string = true;
        for (auto [color_string, buckets] : colored_buckets)
        {
            ss  << (first_color_string ? "\n" : ",\n" );
            first_color_string = false;
            ss  << "    \"" << color_string << "\":\n"
                << "    [";

            bool first_word = true;
            for (std::string word : buckets)
            {
                ss  << (first_word ? "\n" : ",\n" );
                first_word = false;
                ss << "        \"" << word << "\"";
            }
            ss << "\n    ]";
        }
        ss << "\n}\n";

        return ss.str();
    }

}

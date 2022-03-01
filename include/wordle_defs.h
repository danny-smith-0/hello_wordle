#ifndef WORDLE_DEFS_H
#define WORDLE_DEFS_H

#include <vector>
#include <map>
#include <string>

typedef std::vector<std::string> bucket_t;
typedef std::vector<std::string> words_t;
// Each bucket of possible answers is painted with five ordered colors, the key-string of this map
typedef std::map<std::string, bucket_t> colored_buckets_t;

#endif  // WORDLE_DEFS_H

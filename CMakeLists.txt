cmake_minimum_required (VERSION 3.14)
project (wordle_word_suggester)

add_executable ( wordle_word_suggester
	source/wordle_word_suggester.cpp
	source/wordle_inputs.cpp
	source/print_buckets.cpp
)

target_include_directories( wordle_word_suggester
    PUBLIC
    include
)

target_compile_features( wordle_word_suggester
	PRIVATE
	cxx_std_17
)

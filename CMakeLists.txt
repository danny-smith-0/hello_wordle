cmake_minimum_required (VERSION 3.14)
project (hello_wordle)

add_executable ( hello_wordle
	source/wordle_word_suggester.cpp
	source/wordle_inputs.cpp
)

target_include_directories( hello_wordle
    PUBLIC
    include
)

target_compile_features( hello_wordle
	PRIVATE
	cxx_std_17
)

add_custom_command(
    TARGET hello_wordle
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_LIST_DIR}/utils/play_hello_wordle_4.bat
        ${CMAKE_CURRENT_LIST_DIR}/utils/play_hello_wordle.bat
        ${CMAKE_BINARY_DIR}
)

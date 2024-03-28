#pragma once

#include <string_view>

namespace rain::lang::lex {

struct State {
    // Pointer to the current character in the source.
    const char* it = nullptr;

    // Line and column of the current character.
    int line   = 0;
    int column = 0;

    // Index of the current token, with 0 being the first token.
    int index = 0;

    std::string_view source;
    std::string_view file_name;
};

}  // namespace rain::lang::lex

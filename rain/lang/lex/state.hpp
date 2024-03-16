#pragma once

namespace rain::lang::lex {

struct State {
    // Pointer to the current character in the source.
    const char* it = nullptr;

    // Line and column of the current character.
    int line   = 0;
    int column = 0;

    // Index of the current token, with 0 being the first token.
    int index = 0;
};

}  // namespace rain::lang::lex

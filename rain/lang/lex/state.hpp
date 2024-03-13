#pragma once

namespace rain::lang::lex {

struct State {
    // Pointer to the current character in the source.
    const char* it;

    // Line and column of the current character.
    int line;
    int column;

    // Index of the current token, with 0 being the first token.
    int index;
};

}  // namespace rain::lang::lex

#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "rain/lang/token.hpp"

namespace rain::lang {

class Lexer {
    size_t             _next_token = 0;
    std::vector<Token> _tokens;

    // These refer to the next character yet to be tokenized. NOT the current token.
    const char* _it     = nullptr;
    int         _line   = 1;
    int         _column = 1;

    std::string_view _source;
    std::string_view _file_name = "<unknown>";

  public:
    Lexer() = default;

    static Lexer from_memory(const std::string_view file_name, const std::string_view source) {
        Lexer lexer;
        lexer._source    = source;
        lexer._file_name = file_name;
        lexer._it        = source.data();
        return lexer;
    }

    [[nodiscard]] constexpr std::string_view file_name() const noexcept { return _file_name; }

    using LexerState = size_t;
    LexerState save_state() const { return _next_token; }
    void       restore_state(LexerState state) { _next_token = state; }

    Token next() {
        // If we have tokens left, return the next one.
        if (_next_token < _tokens.size()) {
            return _tokens[_next_token++];
        }

        if (_it != nullptr) {
            // Try to get the next token.
            // If we're at the end of the file, no new tokens will be added.
            _tokenize();

            // Check to ensure that a new token was added before incrementing the next token index.
            if (_next_token < _tokens.size()) {
                return _tokens[_next_token++];
            }
        }

        return _tokens.back();
    }

    Token peek() {
        if (_next_token < _tokens.size()) {
            return _tokens[_next_token];
        }
        _tokenize();
        return _tokens.back();
    }

  private:
    /** Load the next token into the list of all tokens. */
    void _tokenize();

    /** Completely tokenize the entire source file. */
    void _tokenize_all();
};

}  // namespace rain::lang

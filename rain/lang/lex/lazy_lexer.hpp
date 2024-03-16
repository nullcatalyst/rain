#pragma once

#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/lex/state.hpp"
#include "rain/lang/lex/token.hpp"

namespace rain::lang::lex {

/**
 * A lexer that lazily tokenizes a source string.
 *
 * The benefit of this lexer is that it does not spend any time tokenizing additional tokens if an
 * unrecoverable error occurs during the parsing of the tokens.
 */
class LazyLexer : public Lexer {
    State _state;

    std::string_view _source;
    std::string_view _file_name = "<unknown>";

  public:
    ~LazyLexer() override = default;

    static LazyLexer using_source(std::string_view source,
                                  std::string_view file_name = "<unknown>") {
        LazyLexer new_lexer;
        new_lexer._state = State{
            .it     = source.data(),
            .line   = 1,
            .column = 1,
            .index  = 0,
        };
        new_lexer._source    = source;
        new_lexer._file_name = file_name;
        return new_lexer;
    }

    [[nodiscard]] constexpr std::string_view source() const noexcept override { return _source; }
    [[nodiscard]] constexpr std::string_view file_name() const noexcept override {
        return _file_name;
    }

    [[nodiscard]] constexpr State save_state() const noexcept override { return _state; }
    void                          restore_state(State state) noexcept override { _state = state; }

    Token next() override;
    Token peek() override;
};

}  // namespace rain::lang::lex

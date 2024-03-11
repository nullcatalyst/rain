#pragma once

#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/token.hpp"

namespace rain::lang {

/**
 * A lexer that lazily tokenizes a source string.
 *
 * The benefit of this lexer is that it does not spend any time tokenizing additional tokens if an
 * unrecoverable error occurs during the parsing of the tokens.
 **/
class LazyLexer : public Lexer {
    const char* _it     = nullptr;
    int         _line   = 1;
    int         _column = 1;

    std::string_view _source;
    std::string_view _file_name = "<unknown>";

  public:
    ~LazyLexer() override = default;

    [[nodiscard]] constexpr std::string_view source() const noexcept override { return _source; }
    [[nodiscard]] constexpr std::string_view file_name() const noexcept override {
        return _file_name;
    }

    Token next() override;
    Token peek() override;
};

}  // namespace rain::lang

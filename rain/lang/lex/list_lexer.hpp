#pragma once

#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/token.hpp"

namespace rain::lang {

class ListLexer : public Lexer {
    size_t             _next_token = 0;
    std::vector<Token> _tokens;

    std::string_view _source;
    std::string_view _file_name = "<unknown>";

  public:
    ListLexer()           = default;
    ~ListLexer() override = default;

    static ListLexer from_lexer(Lexer& lexer) {
        ListLexer new_lexer;
        new_lexer._source    = lexer.source();
        new_lexer._file_name = lexer.file_name();

        do {
            new_lexer._tokens.push_back(lexer.next());
        } while (new_lexer._tokens.back().kind != TokenKind::Eof);

        return new_lexer;
    }

    [[nodiscard]] constexpr std::string_view source() const noexcept override { return _source; }
    [[nodiscard]] constexpr std::string_view file_name() const noexcept override {
        return _file_name;
    }

    Token next() override {
        if (_next_token < _tokens.size()) {
            return _tokens[_next_token++];
        }

        return _tokens.back();
    }

    Token peek() override {
        if (_next_token < _tokens.size()) {
            return _tokens[_next_token];
        }

        return _tokens.back();
    }
};

}  // namespace rain::lang

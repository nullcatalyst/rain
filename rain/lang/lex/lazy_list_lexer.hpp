#pragma once

#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/token.hpp"

namespace rain::lang {

class LazyListLexer : public Lexer {
    size_t             _next_token = 0;
    std::vector<Token> _tokens;
    Lexer*             _lexer = nullptr;

  public:
    ~LazyListLexer() override = default;

    /**
     * Wrap the lexer in a lexer that saves all tokens in a list.
     *
     * This does NOT take ownership of the lexer. The lexer MUST outlive the LazyListLexer.
     **/
    static LazyListLexer using_lexer(Lexer& lexer) {
        LazyListLexer new_lexer;
        new_lexer._lexer = &lexer;
        return new_lexer;
    }

    [[nodiscard]] constexpr std::string_view source() const noexcept override {
        return _lexer->source();
    }
    [[nodiscard]] constexpr std::string_view file_name() const noexcept override {
        return _lexer->file_name();
    }

    Token next() override {
        if (_next_token < _tokens.size()) {
            return _tokens[_next_token++];
        }

        if (_tokens.size() > 0 && _tokens.back().kind == TokenKind::Eof) {
            return _tokens.back();
        }

        Token token = _lexer->next();
        _tokens.push_back(token);
        return token;
    }

    Token peek() override {
        if (_next_token < _tokens.size()) {
            return _tokens[_next_token];
        }

        if (_tokens.size() > 0 && _tokens.back().kind == TokenKind::Eof) {
            return _tokens.back();
        }

        Token token = _lexer->next();
        _tokens.push_back(token);
        return token;
    }
};

}  // namespace rain::lang

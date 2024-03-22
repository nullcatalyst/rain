#pragma once

#include "rain/lang/lex/lazy.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/lex/token.hpp"

namespace rain::lang::lex {

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
        } while (new_lexer._tokens.back().kind != TokenKind::EndOfFile);

        return new_lexer;
    }

    static ListLexer from_tokens(std::vector<Token> tokens,
                                 std::string_view   file_name = "<unknown>") {
        ListLexer new_lexer;
        new_lexer._tokens    = std::move(tokens);
        new_lexer._file_name = file_name;
        return new_lexer;
    }

    static ListLexer using_source(std::string_view source,
                                  std::string_view file_name = "<unknown>") {
        LazyLexer lazy_lexer = LazyLexer::using_source(source, file_name);
        return from_lexer(lazy_lexer);
    }

    [[nodiscard]] constexpr std::string_view source() const noexcept override { return _source; }
    [[nodiscard]] constexpr std::string_view file_name() const noexcept override {
        return _file_name;
    }

    [[nodiscard]] State save_state() const noexcept override {
        return State{.index = static_cast<int>(_next_token)};
    }
    void restore_state(State state) noexcept override { _next_token = state.index; }

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

}  // namespace rain::lang::lex

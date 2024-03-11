#pragma once

#include <algorithm>
#include <array>
#include <cctype>
#include <tuple>

#include "rain/lang/token.hpp"

namespace rain::lang {

struct LexerState {
    const char* it;
    int         line;
    int         column;
};

[[nodiscard]] LexerState skip_whitespace(const std::string_view source, LexerState state);
[[nodiscard]] TokenKind  find_keyword(std::string_view source);
[[nodiscard]] TokenKind  find_operator(const std::string_view source, LexerState state);
[[nodiscard]] int        operator_length(TokenKind kind);

[[nodiscard]] std::tuple<Token, LexerState> next_token(const std::string_view source,
                                                       LexerState             state);

}  // namespace rain::lang

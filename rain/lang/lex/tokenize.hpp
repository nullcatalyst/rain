#pragma once

#include <string_view>
#include <tuple>

#include "rain/lang/lex/state.hpp"
#include "rain/lang/lex/token.hpp"

namespace rain::lang::lex {

[[nodiscard]] State     skip_whitespace(State state);
[[nodiscard]] TokenKind find_keyword(const std::string_view word);
[[nodiscard]] TokenKind find_operator(State state);
[[nodiscard]] int       operator_length(TokenKind kind);

[[nodiscard]] std::tuple<Token, State> next_token(State state);

}  // namespace rain::lang::lex

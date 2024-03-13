#pragma once

#include <string_view>
#include <tuple>

#include "rain/lang/lex/state.hpp"
#include "rain/lang/lex/token.hpp"

namespace rain::lang::lex {

[[nodiscard]] State     skip_whitespace(const std::string_view source, State state);
[[nodiscard]] TokenKind find_keyword(std::string_view word);
[[nodiscard]] TokenKind find_operator(State state);
[[nodiscard]] int       operator_length(TokenKind kind);

[[nodiscard]] std::tuple<Token, State> next_token(const std::string_view source, State state);

}  // namespace rain::lang::lex
